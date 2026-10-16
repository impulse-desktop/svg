#include "svg.h"

#include <math.h>
#include <string.h>

namespace {
    using u8 = unsigned char;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i64 = int64_t;

    // The containers, the sort and the object pool below are the parts of
    // libstd (https://github.com/pg83/std, MIT) that the renderer needs,
    // copied in so that svg.h and svg.cpp depend on nothing but the C library,
    // and brought down to C++17.

    template <typename T>
    struct RemoveReference {
        using Type = T;
    };

    template <typename T>
    struct RemoveReference<T&> {
        using Type = T;
    };

    template <typename T>
    struct RemoveReference<T&&> {
        using Type = T;
    };

    template <typename T>
    using rem_ref = typename RemoveReference<T>::Type;

    template <typename T>
    constexpr rem_ref<T>&& move(T&& t) noexcept {
        return static_cast<rem_ref<T>&&>(t);
    }

    template <typename T>
    constexpr T&& forward(rem_ref<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr T&& forward(rem_ref<T>&& t) noexcept {
        return static_cast<T&&>(t);
    }

    template <typename T, typename N>
    T exchange(T& o, N n) {
        auto r = o;

        o = n;

        return r;
    }

    static void xchg(void*& l, void*& r) noexcept {
        void* t = l;

        l = r;
        r = t;
    }

    static void xchg(float& l, float& r) noexcept {
        float t = l;

        l = r;
        r = t;
    }

    template <typename T>
    auto destruct(T* t) {
        return (t->~T(), t);
    }

    template <typename T>
    inline T* advancePtr(T* ptr, size_t len) noexcept {
        return (T*)(len + (const u8*)ptr);
    }

    // The least power of two at or past v, which is at least 2.
    template <class T>
    inline T clp2(T v) noexcept {
        if constexpr (sizeof(T) == 4) {
            return T(1) << (32 - __builtin_clz(v - 1));
        } else {
            return T(1) << (64 - __builtin_clzll(v - 1));
        }
    }

    template <typename T>
    T max(T l, T r) {
        return l < r ? r : l;
    }

    // Running out of memory throws bad_alloc, as new does.
    static void* allocateMemory(size_t len) {
        return ::operator new(len);
    }

    static void freeMemory(void* ptr) noexcept {
        ::operator delete(ptr);
    }

    static int memCmp(const void* l, const void* r, size_t len) noexcept {
        if (len == 0) {
            return 0;
        }

        return memcmp(l, r, len);
    }

    static void* memCpy(void* to, const void* from, size_t len) noexcept {
        memcpy(to, from, len);

        return len + (u8*)to;
    }

    static size_t strLen(const u8* s) noexcept {
        return strlen((const char*)s);
    }

    static void memZero(void* from, void* to) noexcept {
        memset(from, 0, (u8*)to - (u8*)from);
    }

    static u64 splitMix64(u64 x) noexcept {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9U;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebU;
        x ^= x >> 31;

        return x;
    }

    static u64 shash64(const void* data, size_t len) noexcept {
        u64 h = 14695981039346656037ull;
        const u8* p = (const u8*)data;

        for (size_t i = 0; i < len; ++i) {
            h ^= p[i];
            h *= 1099511628211ull;
        }

        return splitMix64(h);
    }

    struct Newable {
        static void* operator new(size_t, void* ptr) noexcept;
    };

    class StringView;

    class Buffer {
        void* data_;

    public:
        struct Header: public Newable {
            size_t used;
            size_t size;
        };

    private:
        Header* header() const noexcept;
        void appendUnsafe(const void* data, size_t len);

    public:
        ~Buffer() noexcept;

        Buffer(size_t len);
        Buffer(const void* data, size_t len);

        Buffer() noexcept;
        Buffer(const Buffer& buf);
        Buffer(Buffer&& buf) noexcept;

        void* data() const noexcept;
        void* mutData() noexcept;
        void* current() const noexcept;
        void* mutCurrent() noexcept;
        size_t capacity() const noexcept;
        size_t used() const noexcept;
        size_t length() const noexcept;
        bool empty() const noexcept;
        void xchg(Buffer& buf) noexcept;
        void reset() noexcept;
        void seekAbsolute(size_t pos) noexcept;
        void seekAbsolute(const void* ptr) noexcept;
        size_t offsetOf(const void* ptr) noexcept;
        void grow(size_t size);
        void append(const void* data, size_t len);
        void growDelta(size_t len);
        void zero(size_t len);
    };

    alignas(max_align_t) static const char emptyHeader[sizeof(Buffer::Header)] = {};

    static Buffer::Header* nullHeader() noexcept {
        return (Buffer::Header*)emptyHeader;
    }

    static Buffer::Header* allocHeader(size_t len) {
        if (len) {
            auto flen = max<size_t>(clp2(len + sizeof(Buffer::Header)), 256);
            auto blen = flen - sizeof(Buffer::Header);

            return new (allocateMemory(flen)) Buffer::Header{{}, 0, blen};
        }

        return nullHeader();
    }

    static void freeHeader(Buffer::Header* ptr) noexcept {
        if (ptr != nullHeader()) {
            freeMemory(ptr);
        }
    }

    template <typename T>
    class Vector {
        Buffer buf_;

    public:
        static_assert(__is_trivially_destructible(T));

        Vector() = default;
        Vector(Vector&&) = default;
        Vector(const Vector&) = default;

        auto data() const noexcept {
            return (const T*)buf_.data();
        }

        auto begin() const noexcept {
            return data();
        }

        auto end() const noexcept {
            return (const T*)buf_.current();
        }

        auto mutData() noexcept {
            return const_cast<T*>(data());
        }

        auto mutBegin() noexcept {
            return const_cast<T*>(begin());
        }

        auto mutEnd() noexcept {
            return const_cast<T*>(end());
        }

        size_t length() const noexcept {
            return end() - begin();
        }

        bool empty() const noexcept {
            return buf_.empty();
        }

        auto& mut(size_t i) noexcept {
            return *(mutBegin() + i);
        }

        const auto& operator[](size_t i) const noexcept {
            return *(begin() + i);
        }

        const auto& back() const noexcept {
            return *(end() - 1);
        }

        auto& mutBack() noexcept {
            return *(mutEnd() - 1);
        }

        void clear() noexcept {
            buf_.reset();
        }

        void pushBack(const T& t) {
            buf_.append(&t, sizeof(t));
        }

        auto popBack() noexcept {
            auto res = back();

            buf_.seekAbsolute(end() - 1);

            return res;
        }

        void append(const T* b, const T* e) {
            buf_.append((const u8*)b, (const u8*)e - (const u8*)b);
        }

        void append(const T* b, size_t len) {
            append(b, b + len);
        }

        void xchg(Vector& v) noexcept {
            buf_.xchg(v.buf_);
        }

        void zero(size_t cnt) {
            buf_.zero(cnt * sizeof(T));
        }
    };

    class StringView {
        const u8* ptr_;
        size_t len_;

    public:
        StringView() noexcept;

        StringView(const u8* ptr, size_t len) noexcept;
        StringView(const u8* b, const u8* e) noexcept;
        explicit StringView(const char* s) noexcept;
        StringView(const Buffer& b) noexcept;

        const u8* data() const noexcept;
        size_t length() const noexcept;
        const u8* begin() const noexcept;
        const u8* end() const noexcept;
        const u8& operator[](size_t i) const noexcept;
        bool empty() const noexcept;
        const u8& back() const noexcept;
        u64 hash64() const noexcept;
    };

    static bool operator==(StringView l, StringView r) noexcept {
        return l.length() == r.length() && memCmp(l.data(), r.data(), l.length()) == 0;
    }

    static bool operator!=(StringView l, StringView r) noexcept {
        return !(l == r);
    }

    class PCG32 {
        u64 state_;
        u64 seq_;

    public:
        PCG32(u64 seq) noexcept;
        PCG32(const void* seq) noexcept;
        PCG32(u64 state, u64 seq) noexcept;

        u32 nextU32() noexcept;
        u32 uniformBiased(u32 n) noexcept;
    };

    static u32 xorShift(u64 v) noexcept {
        const u32 xorshifted = ((v >> 18u) ^ v) >> 27u;
        const u32 rot = v >> 59u;

        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    namespace QSP {
        template <typename I, typename C>
        struct Context {
            struct WorkItem {
                I b;
                I e;
            };

            C& f;
            Vector<WorkItem> w;
            PCG32 r;

            Context(C& _f, void* seed) noexcept
                : f(_f)
                , r(seed)
            {
            }

            void insertionSort(I b, I e) {
                for (auto i = b + 1; i != e; ++i) {
                    for (auto j = i; j != b && f(*j, *(j - 1)); --j) {
                        xchg(*j, *(j - 1));
                    }
                }
            }

            auto chooseRandom(I b, I e) noexcept {
                return b + r.uniformBiased(e - b);
            }

            // called as partitionHoare(b, l) where l = e-1 (pivot is at *l).
            // p captures l; the second while pre-decrements e, so e never reaches p.
            // *p is never swapped — pivot stays in place throughout.
            auto partitionHoare(I b, I e) {
                for (auto p = e;; ++b) {
                    while (b != e && f(*b, *p)) {
                        ++b;
                    }

                    while (b != e && f(*p, *--e)) {
                    }

                    if (b == e) {
                        return b;
                    }

                    xchg(*b, *e);
                }
            }

            // assume b < e
            bool alreadySorted(I b, I e) {
                for (++b; b != e; ++b) {
                    if (f(*b, *(b - 1))) {
                        return false;
                    }
                }

                return true;
            }

            void qSortStep(I b, I e) {
                if (e - b < 16) {
                    return;
                }

                if (alreadySorted(b, e)) {
                    return;
                }

                auto l = e - 1;

                // pivot to last
                xchg(*chooseRandom(b, e), *l);

                // place for pivot
                auto p = partitionHoare(b, l);

                // move pivot form last to proper place
                xchg(*p, *l);

                // recurse
                w.pushBack(WorkItem{p + 1, e});
                w.pushBack(WorkItem{b, p});
            }

            void qSortLoop() {
                while (!w.empty()) {
                    auto item = w.popBack();

                    qSortStep(item.b, item.e);
                }
            }

            void qSort(I b, I e) {
                qSortStep(b, e);
                qSortLoop();
                insertionSort(b, e);
            }
        };
    }

    // Sorts a range that is not empty.
    template <typename I, typename C>
    void quickSort(I b, I e, C&& f) {
        QSP::Context<I, C>(f, (void*)&*b).qSort(b, e);
    }

    struct Disposable {
        Disposable* prev = 0;

        virtual ~Disposable() noexcept;
    };

    class Disposer {
        Disposable* end = 0;

    public:
        ~Disposer() noexcept;

        void dispose() noexcept;
        void submit(Disposable* d) noexcept;
    };

    constexpr size_t initialChunk = 128;

    class MemoryPool {
        u8* currentChunk;
        u8* currentChunkEnd;
        Disposer* ds;
        Vector<void*> large;
        size_t nextChunk = initialChunk;

        void allocateNewChunk(size_t minSize);

    public:
        MemoryPool(void* buf, size_t len) noexcept;
        ~MemoryPool() noexcept;

        void* allocate(size_t len);
    };

    constexpr size_t largeBlock = 1 << 16;
    constexpr size_t chunkAlignment = alignof(max_align_t);

    struct alignas(chunkAlignment) ChunkDestructor: public Disposable, public Disposer, public Newable {};

    struct alignas(chunkAlignment) Chunk: public Disposable, public Newable {
        ~Chunk() noexcept override;
    };

    static_assert(sizeof(Chunk) % chunkAlignment == 0);
    static_assert(sizeof(ChunkDestructor) % chunkAlignment == 0);

    template <typename T>
    struct Embed {
        T t;

        template <typename... A>
        Embed(A&&... a)
            : t(forward<A>(a)...)
        {
        }
    };

    // Every object made in a pool lives until the pool is deleted; the ones
    // with a destructor are then destroyed in reverse order of creation.
    class ObjPool {
        alignas(max_align_t) u8 buf_[256 - sizeof(MemoryPool) - sizeof(Disposer)];
        MemoryPool mp_;
        Disposer ds_;

        template <size_t Size, size_t Align>
        void* allocFor() {
            static_assert(Align <= alignof(max_align_t));

            return allocate(Size);
        }

        template <typename T, typename... A>
        T* makeImpl(A&&... a) {
            return new (allocFor<sizeof(T), alignof(T)>()) T(forward<A>(a)...);
        }

    public:
        ObjPool() noexcept;

        void* allocate(size_t len);
        void submit(Disposable* d) noexcept;

        template <typename T, typename... A>
        T* make(A&&... a) {
            struct Wrapper1: public Embed<T>, public Newable {
                using Embed<T>::Embed;
            };

            if constexpr (__is_trivially_destructible(T)) {
                static_assert(sizeof(Wrapper1) == sizeof(T));

                return &makeImpl<Wrapper1>(forward<A>(a)...)->t;
            } else {
                struct Wrapper2: public Disposable, public Wrapper1 {
                    using Wrapper1::Wrapper1;
                };

                auto res = makeImpl<Wrapper2>(forward<A>(a)...);

                submit(res);

                return &res->t;
            }
        }
    };

    static_assert(sizeof(ObjPool) == 256);
}

void* Newable::operator new(size_t, void* ptr) noexcept {
    return ptr;
}

Buffer::Header* Buffer::header() const noexcept {
    return (Header*)data_ - 1;
}

Buffer::~Buffer() noexcept {
    freeHeader(header());
}

Buffer::Buffer() noexcept
    : Buffer(0)
{
}

Buffer::Buffer(size_t len)
    : data_(allocHeader(len) + 1)
{
}

Buffer::Buffer(const Buffer& buf)
    : Buffer(buf.data(), buf.used())
{
}

Buffer::Buffer(const void* ptr, size_t len)
    : Buffer(len)
{
    appendUnsafe(ptr, len);
}

Buffer::Buffer(Buffer&& buf) noexcept
    : Buffer()
{
    buf.xchg(*this);
}

void* Buffer::data() const noexcept {
    return data_;
}

void* Buffer::mutData() noexcept {
    return data_;
}

void* Buffer::current() const noexcept {
    return advancePtr(data(), used());
}

void* Buffer::mutCurrent() noexcept {
    return current();
}

size_t Buffer::capacity() const noexcept {
    return header()->size;
}

size_t Buffer::used() const noexcept {
    return header()->used;
}

size_t Buffer::length() const noexcept {
    return used();
}

bool Buffer::empty() const noexcept {
    return used() == 0;
}

void Buffer::xchg(Buffer& buf) noexcept {
    ::xchg(data_, buf.data_);
}

void Buffer::reset() noexcept {
    seekAbsolute((size_t)0);
}

void Buffer::seekAbsolute(size_t pos) noexcept {
    if (header()->used != pos) {
        header()->used = pos;
    }
}

void Buffer::seekAbsolute(const void* ptr) noexcept {
    seekAbsolute(offsetOf(ptr));
}

size_t Buffer::offsetOf(const void* ptr) noexcept {
    return (const u8*)ptr - (const u8*)data();
}

void Buffer::grow(size_t size) {
    if (size > capacity()) {
        Buffer buf(size);

        buf.appendUnsafe(data(), used());
        buf.xchg(*this);
    }
}

void Buffer::append(const void* ptr, size_t len) {
    growDelta(len);
    appendUnsafe(ptr, len);
}

void Buffer::appendUnsafe(const void* ptr, size_t len) {
    auto cur = (u8*)mutCurrent();

    if (len == 1) {
        *cur = *(const u8*)ptr;
        header()->used += 1;
    } else if (len) {
        memCpy(cur, ptr, len);
        header()->used += len;
    }
}

void Buffer::growDelta(size_t len) {
    grow(used() + len);
}

void Buffer::zero(size_t len) {
    grow(len);
    seekAbsolute(len);
    memZero(mutData(), mutCurrent());
}

StringView::StringView() noexcept
    : StringView(nullptr, (size_t)0)
{
}

StringView::StringView(const u8* ptr, size_t len) noexcept
    : ptr_(ptr)
    , len_(len)
{
}

StringView::StringView(const u8* b, const u8* e) noexcept
    : StringView(b, e - b)
{
}

StringView::StringView(const char* s) noexcept
    : StringView((const u8*)s, strLen((const u8*)s))
{
}

StringView::StringView(const Buffer& b) noexcept
    : StringView((const u8*)b.data(), b.length())
{
}

const u8* StringView::data() const noexcept {
    return ptr_;
}

size_t StringView::length() const noexcept {
    return len_;
}

const u8* StringView::begin() const noexcept {
    return data();
}

const u8* StringView::end() const noexcept {
    return begin() + length();
}

const u8& StringView::operator[](size_t i) const noexcept {
    return *(begin() + i);
}

bool StringView::empty() const noexcept {
    return length() == 0;
}

const u8& StringView::back() const noexcept {
    return *(end() - 1);
}

u64 StringView::hash64() const noexcept {
    return shash64(data(), length());
}

PCG32::PCG32(u64 seq) noexcept
    : PCG32(splitMix64((size_t)this), seq)
{
}

PCG32::PCG32(const void* seq) noexcept
    : PCG32(splitMix64((size_t)seq))
{
}

PCG32::PCG32(u64 state, u64 seq) noexcept
    : state_(0)
    , seq_(2ULL * seq + 1ULL)
{
    nextU32();
    state_ += state;
    nextU32();
}

u32 PCG32::nextU32() noexcept {
    return xorShift(exchange(state_, state_ * 6364136223846793005ULL + seq_));
}

u32 PCG32::uniformBiased(u32 n) noexcept {
    // https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
    return ((u64)nextU32() * (u64)n) >> 32;
}

Disposable::~Disposable() noexcept {
}

Disposer::~Disposer() noexcept {
    dispose();
}

void Disposer::dispose() noexcept {
    auto cur = exchange(end, (Disposable*)0);

    while (cur) {
        destruct(exchange(cur, cur->prev));
    }
}

void Disposer::submit(Disposable* d) noexcept {
    d->prev = end;
    end = d;
}

Chunk::~Chunk() noexcept {
    freeMemory(this);
}

MemoryPool::MemoryPool(void* buf, size_t len) noexcept
    : currentChunk((u8*)buf)
    , currentChunkEnd(currentChunk + len)
    , ds(new (allocate(sizeof(ChunkDestructor))) ChunkDestructor())
{
    ds->submit((ChunkDestructor*)ds);
}

MemoryPool::~MemoryPool() noexcept {
    ds->dispose();

    for (size_t i = 0; i < large.length(); i++) {
        freeMemory(large[i]);
    }
}

void* MemoryPool::allocate(size_t len) {
    // A large block is allocated to its size on its own, outside the
    // chunks, whose sizes double.
    if (len >= largeBlock) {
        void* block = allocateMemory(len);

        large.pushBack(block);

        return block;
    }

    const size_t alignedLen = (len + chunkAlignment - 1) & ~(chunkAlignment - 1);

    if (currentChunk + alignedLen > currentChunkEnd) {
        allocateNewChunk(alignedLen + sizeof(Chunk));
    }

    return exchange(currentChunk, currentChunk + alignedLen);
}

// The chunks grow by half each time.
void MemoryPool::allocateNewChunk(size_t minSize) {
    while (nextChunk < minSize) {
        nextChunk += nextChunk / 2;
    }

    size_t size = nextChunk;
    auto newChunk = new (allocateMemory(size)) Chunk();

    ds->submit(newChunk);
    nextChunk += nextChunk / 2;
    currentChunkEnd = (u8*)newChunk + size;
    currentChunk = (u8*)(newChunk + 1);
}

ObjPool::ObjPool() noexcept
    : mp_(buf_, sizeof(buf_))
{
}

void* ObjPool::allocate(size_t len) {
    return mp_.allocate(len);
}

void ObjPool::submit(Disposable* d) noexcept {
    ds_.submit(d);
}

namespace {
    struct Document;
}

// Each public object is made in its own pool; destroying it deletes the pool.
// The document holds the file parsed and cascaded, which a render reads and
// never writes.
struct SvgDocument {
    ObjPool* pool;
    SvgDecode decode;
    SvgFail fail;
    void* context;
    Document* doc;
    float width;
    float height;

    SvgDocument(ObjPool* pool, SvgDecode decode, SvgFail fail, void* context, Document* doc, float width, float height);
};

struct SvgLibrary {
    ObjPool* pool;
    SvgDecode decode;
    SvgFail fail;
    void* context;

    SvgLibrary(ObjPool* pool, SvgDecode decode, SvgFail fail, void* context);
};

SvgDocument::SvgDocument(ObjPool* pool_, SvgDecode decode_, SvgFail fail_, void* context_, Document* doc_, float width_, float height_)
    : pool(pool_)
    , decode(decode_)
    , fail(fail_)
    , context(context_)
    , doc(doc_)
    , width(width_)
    , height(height_)
{
}

SvgLibrary::SvgLibrary(ObjPool* pool_, SvgDecode decode_, SvgFail fail_, void* context_)
    : pool(pool_)
    , decode(decode_)
    , fail(fail_)
    , context(context_)
{
}

// A string literal as a StringView with its length known at compile time;
// the concatenation with "" rejects anything that is not a literal.
#define SV(text) StringView((const u8*)("" text), sizeof("" text) - 1)

#define SVG_ATOMS(ATOM)                                            \
    ATOM(Svg, "svg")                                               \
    ATOM(G, "g")                                                   \
    ATOM(Path, "path")                                             \
    ATOM(Rect, "rect")                                             \
    ATOM(Circle, "circle")                                         \
    ATOM(Ellipse, "ellipse")                                       \
    ATOM(Line, "line")                                             \
    ATOM(Polyline, "polyline")                                     \
    ATOM(Polygon, "polygon")                                       \
    ATOM(Use, "use")                                               \
    ATOM(Defs, "defs")                                             \
    ATOM(Symbol, "symbol")                                         \
    ATOM(LinearGradient, "linearGradient")                         \
    ATOM(RadialGradient, "radialGradient")                         \
    ATOM(MeshGradient, "meshgradient")                             \
    ATOM(MeshRow, "meshrow")                                       \
    ATOM(MeshPatch, "meshpatch")                                   \
    ATOM(Stop, "stop")                                             \
    ATOM(ClipPath, "clipPath")                                     \
    ATOM(Pattern, "pattern")                                       \
    ATOM(Marker, "marker")                                         \
    ATOM(MarkerWidth, "markerWidth")                               \
    ATOM(MarkerHeight, "markerHeight")                             \
    ATOM(MarkerUnits, "markerUnits")                               \
    ATOM(RefX, "refX")                                             \
    ATOM(RefY, "refY")                                             \
    ATOM(Orient, "orient")                                         \
    ATOM(Style, "style")                                           \
    ATOM(A, "a")                                                   \
    ATOM(Switch, "switch")                                         \
    ATOM(Image, "image")                                           \
    ATOM(Id, "id")                                                 \
    ATOM(Class, "class")                                           \
    ATOM(X, "x")                                                   \
    ATOM(Y, "y")                                                   \
    ATOM(Width, "width")                                           \
    ATOM(Height, "height")                                         \
    ATOM(Rx, "rx")                                                 \
    ATOM(Ry, "ry")                                                 \
    ATOM(Cx, "cx")                                                 \
    ATOM(Cy, "cy")                                                 \
    ATOM(R, "r")                                                   \
    ATOM(Fx, "fx")                                                 \
    ATOM(Fy, "fy")                                                 \
    ATOM(Fr, "fr")                                                 \
    ATOM(X1, "x1")                                                 \
    ATOM(Y1, "y1")                                                 \
    ATOM(X2, "x2")                                                 \
    ATOM(Y2, "y2")                                                 \
    ATOM(Points, "points")                                         \
    ATOM(D, "d")                                                   \
    ATOM(PathLength, "pathLength")                                 \
    ATOM(Transform, "transform")                                   \
    ATOM(ViewBox, "viewBox")                                       \
    ATOM(View, "view")                                             \
    ATOM(PreserveAspectRatio, "preserveAspectRatio")               \
    ATOM(Href, "href")                                             \
    ATOM(Offset, "offset")                                         \
    ATOM(GradientUnits, "gradientUnits")                           \
    ATOM(GradientTransform, "gradientTransform")                   \
    ATOM(SpreadMethod, "spreadMethod")                             \
    ATOM(ClipPathUnits, "clipPathUnits")                           \
    ATOM(PatternUnits, "patternUnits")                             \
    ATOM(PatternContentUnits, "patternContentUnits")               \
    ATOM(PatternTransform, "patternTransform")                     \
    ATOM(TransformOrigin, "transform-origin")                      \
    ATOM(TransformBox, "transform-box")                            \
    ATOM(FilterUnits, "filterUnits")                               \
    ATOM(PrimitiveUnits, "primitiveUnits")                         \
    ATOM(In, "in")                                                 \
    ATOM(In2, "in2")                                               \
    ATOM(Result, "result")                                         \
    ATOM(StdDeviation, "stdDeviation")                             \
    ATOM(Dx, "dx")                                                 \
    ATOM(Dy, "dy")                                                 \
    ATOM(Mode, "mode")                                             \
    ATOM(Values, "values")                                         \
    ATOM(Operator, "operator")                                     \
    ATOM(K1, "k1")                                                 \
    ATOM(K2, "k2")                                                 \
    ATOM(K3, "k3")                                                 \
    ATOM(K4, "k4")                                                 \
    ATOM(Order, "order")                                           \
    ATOM(KernelMatrix, "kernelMatrix")                             \
    ATOM(Divisor, "divisor")                                       \
    ATOM(Bias, "bias")                                             \
    ATOM(TargetX, "targetX")                                       \
    ATOM(TargetY, "targetY")                                       \
    ATOM(EdgeMode, "edgeMode")                                     \
    ATOM(PreserveAlpha, "preserveAlpha")                           \
    ATOM(SurfaceScale, "surfaceScale")                             \
    ATOM(DiffuseConstant, "diffuseConstant")                       \
    ATOM(SpecularConstant, "specularConstant")                     \
    ATOM(SpecularExponent, "specularExponent")                     \
    ATOM(Azimuth, "azimuth")                                       \
    ATOM(Elevation, "elevation")                                   \
    ATOM(Z, "z")                                                   \
    ATOM(PointsAtX, "pointsAtX")                                   \
    ATOM(PointsAtY, "pointsAtY")                                   \
    ATOM(PointsAtZ, "pointsAtZ")                                   \
    ATOM(LimitingConeAngle, "limitingConeAngle")                   \
    ATOM(Scale, "scale")                                           \
    ATOM(XChannelSelector, "xChannelSelector")                     \
    ATOM(YChannelSelector, "yChannelSelector")                     \
    ATOM(BaseFrequency, "baseFrequency")                           \
    ATOM(NumOctaves, "numOctaves")                                 \
    ATOM(Seed, "seed")                                             \
    ATOM(StitchTiles, "stitchTiles")                               \
    ATOM(Radius, "radius")                                         \
    ATOM(TableValues, "tableValues")                               \
    ATOM(Slope, "slope")                                           \
    ATOM(Intercept, "intercept")                                   \
    ATOM(Amplitude, "amplitude")                                   \
    ATOM(Exponent, "exponent")                                     \
    ATOM(FeBlend, "feBlend")                                       \
    ATOM(FeColorMatrix, "feColorMatrix")                           \
    ATOM(FeComponentTransfer, "feComponentTransfer")               \
    ATOM(FeComposite, "feComposite")                               \
    ATOM(FeConvolveMatrix, "feConvolveMatrix")                     \
    ATOM(FeDiffuseLighting, "feDiffuseLighting")                   \
    ATOM(FeDisplacementMap, "feDisplacementMap")                   \
    ATOM(FeDistantLight, "feDistantLight")                         \
    ATOM(FeDropShadow, "feDropShadow")                             \
    ATOM(FeFlood, "feFlood")                                       \
    ATOM(FeFuncA, "feFuncA")                                       \
    ATOM(FeFuncB, "feFuncB")                                       \
    ATOM(FeFuncG, "feFuncG")                                       \
    ATOM(FeFuncR, "feFuncR")                                       \
    ATOM(FeGaussianBlur, "feGaussianBlur")                         \
    ATOM(FeImage, "feImage")                                       \
    ATOM(FeMerge, "feMerge")                                       \
    ATOM(FeMergeNode, "feMergeNode")                               \
    ATOM(FeMorphology, "feMorphology")                             \
    ATOM(FeOffset, "feOffset")                                     \
    ATOM(FePointLight, "fePointLight")                             \
    ATOM(FeSpecularLighting, "feSpecularLighting")                 \
    ATOM(FeSpotLight, "feSpotLight")                               \
    ATOM(FeTile, "feTile")                                         \
    ATOM(FeTurbulence, "feTurbulence")                             \
    ATOM(MaskUnits, "maskUnits")                                   \
    ATOM(MaskContentUnits, "maskContentUnits")                     \
    ATOM(SystemLanguage, "systemLanguage")                         \
    ATOM(RequiredExtensions, "requiredExtensions")                 \
    ATOM(Type, "type")                                             \
    ATOM(Fill, "fill")                                             \
    ATOM(FillOpacity, "fill-opacity")                              \
    ATOM(FillRule, "fill-rule")                                    \
    ATOM(Stroke, "stroke")                                         \
    ATOM(StrokeWidth, "stroke-width")                              \
    ATOM(StrokeOpacity, "stroke-opacity")                          \
    ATOM(StrokeLinecap, "stroke-linecap")                          \
    ATOM(StrokeLinejoin, "stroke-linejoin")                        \
    ATOM(StrokeMiterlimit, "stroke-miterlimit")                    \
    ATOM(StrokeDasharray, "stroke-dasharray")                      \
    ATOM(StrokeDashoffset, "stroke-dashoffset")                    \
    ATOM(Opacity, "opacity")                                       \
    ATOM(Display, "display")                                       \
    ATOM(Visibility, "visibility")                                 \
    ATOM(Color, "color")                                           \
    ATOM(StopColor, "stop-color")                                  \
    ATOM(StopOpacity, "stop-opacity")                              \
    ATOM(ClipPathProperty, "clip-path")                            \
    ATOM(ClipRule, "clip-rule")                                    \
    ATOM(FontSize, "font-size")                                    \
    ATOM(Mask, "mask")                                             \
    ATOM(MaskType, "mask-type")                                    \
    ATOM(FloodColor, "flood-color")                                \
    ATOM(FloodOpacity, "flood-opacity")                            \
    ATOM(LightingColor, "lighting-color")                          \
    ATOM(ColorInterpolationFilters, "color-interpolation-filters") \
    ATOM(Filter, "filter")                                         \
    ATOM(BackgroundColor, "background-color")                      \
    ATOM(Overflow, "overflow")                                     \
    ATOM(MarkerStart, "marker-start")                              \
    ATOM(MarkerMid, "marker-mid")                                  \
    ATOM(MarkerEnd, "marker-end")                                  \
    ATOM(VectorEffect, "vector-effect")                            \
    ATOM(PaintOrder, "paint-order")                                \
    ATOM(MixBlendMode, "mix-blend-mode")                           \
    ATOM(EnableBackground, "enable-background")                    \
    ATOM(Isolation, "isolation")                                   \
    ATOM(ColorInterpolation, "color-interpolation")                \
    ATOM(ShapeRendering, "shape-rendering")                        \
    ATOM(ImageRendering, "image-rendering")                        \
    ATOM(Zoom, "zoom")                                             \
    ATOM(Outline, "outline")

namespace {
    enum Atom : u32 {
        AtomNone,
#define SVG_ATOM_ENUM(name, text) Atom##name,
        SVG_ATOMS(SVG_ATOM_ENUM)
#undef SVG_ATOM_ENUM
        AtomKnown,
    };

    const char* const knownAtoms[] = {
        "",
#define SVG_ATOM_TEXT(name, text) text,
        SVG_ATOMS(SVG_ATOM_TEXT)
#undef SVG_ATOM_TEXT
    };

    constexpr u32 firstProperty = AtomFill;
    constexpr u32 propertyCount = AtomOutline - AtomFill + 1;
    constexpr float pi = 3.14159265358979323846f;
    constexpr double piDouble = 3.14159265358979323846;
    constexpr float tolerance = 0.025f;
    constexpr u32 maxDepth = 512;
    constexpr double maxDashes = 1 << 20;
    constexpr double maxOutline = 1 << 23;
    constexpr u32 maxEntityDepth = 16;
    constexpr size_t maxExpansion = 1 << 22;

    const StringView svgNamespace = SV("http://www.w3.org/2000/svg");
    const StringView xlinkNamespace = SV("http://www.w3.org/1999/xlink");
    const StringView xmlNamespace = SV("http://www.w3.org/XML/1998/namespace");
    const StringView xincludeNamespace = SV("http://www.w3.org/2001/XInclude");

    struct Point {
        float x;
        float y;
    };

    struct Matrix {
        float a;
        float b;
        float c;
        float d;
        float e;
        float f;
    };

    struct Box {
        float x0;
        float y0;
        float x1;
        float y1;
    };

    struct Color {
        float r;
        float g;
        float b;
        float a;
    };

    struct Viewport {
        float width;
        float height;
        float screenWidth;
        float screenHeight;
        float rootFontSize;
    };

    struct Interner {
        Vector<StringView> texts;
        Vector<u32> slots;
    };

    struct Attribute {
        u32 name;
        u32 value;
    };

    struct Node {
        u32 name;
        u32 index;
        Node* parent;
        Node* first;
        Node* last;
        Node* next;
        Node* prev;
        Attribute* attributes;
        u32 count;
        StringView text;
        u32* style;
        u32* classes;
        u32 classCount;
        bool classesReady;
    };

    struct Rule;

    struct Document {
        ObjPool* pool;
        Interner atoms;
        Node* root;
        u32 nodeCount;
        Vector<Node*> byId;
        Rule* rules;
        u32 ruleCount;
        bool structural;
    };

    struct Entity {
        StringView name;
        StringView value;
        bool external;
    };

    struct Scope {
        StringView prefix;
        StringView uri;
    };

    struct RawAttribute {
        StringView name;
        StringView value;
    };

    struct Cursor {
        const u8* p;
        const u8* end;
    };

    struct Xml {
        Document* doc;
        SvgFail fail;
        Vector<Entity> entities;
        Vector<Scope> scopes;
        Vector<RawAttribute> raw;
        size_t expanded;
        u32 entityDepth;
    };

    enum class Combinator : u8 {
        None,
        Descendant,
        Child,
        Adjacent,
        Sibling,
    };

    enum class Test : u8 {
        Id,
        Class,
        Exists,
        Equals,
        Includes,
        Dash,
        Prefix,
        Suffix,
        Substring,
        FirstChild,
        LastChild,
        OnlyChild,
        Root,
        Never,
        Not,
    };

    struct Compound;

    struct Condition {
        Test test;
        u32 name;
        StringView value;
        Compound* inner;
        u32 innerCount;
    };

    struct Compound {
        u32 type;
        Combinator combinator;
        Condition* conditions;
        u32 count;
    };

    struct Selector {
        Compound* compounds;
        u32 count;
        u32 specificity;
    };

    struct Declaration {
        u32 property;
        u32 value;
        bool important;
    };

    struct Rule {
        Selector* selectors;
        u32 selectorCount;
        Declaration* declarations;
        u32 declarationCount;
    };

    struct Candidate {
        u32 value;
        u64 priority;
    };

    enum class Unit : u8 {
        Number,
        Px,
        Em,
        Ex,
        In,
        Cm,
        Mm,
        Pt,
        Pc,
        Q,
        Rem,
        Vw,
        Vh,
        Vmin,
        Vmax,
        Ch,
        Lh,
        Ic,
        Percent,
    };

    struct Length {
        float value;
        Unit unit;
    };

    enum class Axis : u8 {
        X,
        Y,
        Other,
    };

    enum class PaintKind : u8 {
        None,
        Color,
        CurrentColor,
        Server,
    };

    struct PaintValue {
        PaintKind kind;
        Color color;
        u32 server;
        PaintKind fallback;
        Color fallbackColor;
        bool context;
    };

    enum Verb : u8 {
        VerbMove,
        VerbLine,
        VerbCubic,
        VerbClose,
        VerbArcCubic,
    };

    struct Path {
        Vector<u8> verbs;
        Vector<Point> points;
    };

    struct Flat {
        Vector<Point> points;
        Vector<u32> ends;
        Vector<u8> closed;
    };

    enum Cap : u8 {
        CapButt,
        CapRound,
        CapSquare,
    };

    enum Join : u8 {
        JoinMiter,
        JoinRound,
        JoinBevel,
    };

    enum Winding : u8 {
        WindingNonZero,
        WindingEvenOdd,
    };

    enum class FilterFunction : u8 {
        Reference,
        Matrix,
        Blur,
        DropShadow,
    };

    struct FilterItem {
        FilterFunction kind;
        u32 reference;
        float values[20];
        Color color;
    };

    enum class ShapeKind : u8 {
        None,
        Circle,
        Ellipse,
        Inset,
        Polygon,
    };

    enum class GeometryBox : u8 {
        Fill,
        Stroke,
        View,
    };

    struct BasicShape {
        ShapeKind kind;
        GeometryBox box;
        u8 rule;
        Length values[4];
        Length* points;
        u32 pointCount;
    };

    struct ComputedStyle {
        PaintValue fill;
        PaintValue stroke;
        Color color;
        Color stopColor;
        float fillOpacity;
        float strokeOpacity;
        float strokeWidth;
        float miterLimit;
        float dashOffset;
        float opacity;
        float stopOpacity;
        float fontSize;
        float* dashes;
        u32 dashCount;
        u32 clipPath;
        BasicShape* clipShape;
        u32 mask;
        u32 markers[3];
        bool nonScaling;
        u8 blend;
        bool isolate;
        bool newBackground;
        bool backgroundClipped;
        Box backgroundBox;
        bool linearColors;
        bool aliased;
        u8 imageRendering;
        float zoom;
        float outlineWidth;
        Color outlineColor;
        u8 paintOrder[3];
        u8 maskType;
        Color floodColor;
        float floodOpacity;
        Color lightingColor;
        bool linearFilters;
        FilterItem* filters;
        u32 filterCount;
        Color background;
        bool overflowVisible;
        u8 fillRule;
        u8 clipRule;
        u8 cap;
        u8 join;
        bool visible;
        bool displayNone;
    };

    struct Edge {
        float x0;
        float y0;
        float x1;
        float y1;
    };

    enum class PaintType : u8 {
        Solid,
        Linear,
        Radial,
        Image,
        Pattern,
        Mesh,
    };

    enum Spread : u8 {
        SpreadPad,
        SpreadReflect,
        SpreadRepeat,
    };

    struct GradientStop {
        float offset;
        Color color;
    };

    struct DevicePaint {
        PaintType type;
        Color color;
        Matrix inverse;
        float x1;
        float y1;
        float x2;
        float y2;
        float cx;
        float cy;
        float r;
        float fx;
        float fy;
        float fr;
        u8 spread;
        GradientStop* stops;
        u32 stopCount;
        const u8* image;
        u32 imageWidth;
        u32 imageHeight;
        bool smooth;
        u32 programFirst;
        u32 programCount;
        float* tile;
        float opacity;
        bool linear;
    };

    struct ClipShape {
        u32 first;
        u32 count;
        u8 rule;
    };

    struct IntBox {
        int x0;
        int y0;
        int x1;
        int y1;
    };

    enum FilterInputs : int {
        InputSourceGraphic = -1,
        InputSourceAlpha = -2,
        InputBackgroundImage = -4,
        InputBackgroundAlpha = -5,
        InputFillPaint = -6,
        InputStrokePaint = -7,
    };

    struct FilterPrimitive {
        Node* node;
        u32 kind;
        float params[20];
        int in1;
        int in2;
        int* inputs;
        u32 inputCount;
        Box user;
        IntBox box;
        bool linear;
        Color flood;
        Color lighting;
        u32 programFirst;
        u32 programCount;
    };

    struct Filter {
        Document* doc;
        Matrix ctm;
        Box bbox;
        bool boxPrimitives;
        IntBox region;
        FilterPrimitive* primitives;
        u32 count;
        Filter* next;
        Box device;
        DevicePaint* fillPaint;
        DevicePaint* strokePaint;
    };

    enum class OpKind : u8 {
        Fill,
        Begin,
        End,
        Layer,
    };

    struct Op {
        OpKind kind;
        u8 rule;
        bool aliased;
        u32 first;
        u32 count;
        Box bounds;
        DevicePaint* paint;
        float opacity;
        bool clipped;
        u32 clipFirst;
        u32 clipCount;
        bool clipProgram;
        bool masked;
        u8 blend;
        u8 maskType;
        u32 maskFirst;
        u32 maskCount;
        u32 maskRegion;
        Filter* filter;
        u32 layerWidth;
        u32 layerHeight;
        Matrix layerMatrix;
        bool newBackground;
        bool backgroundOnly;
        bool resetBackground;
        bool backgroundClipped;
        Box backgroundBox;
        Matrix backgroundMatrix;
    };

    // What a pass over the document carries: the document and the pool of
    // the pass, the styles computed so far, the resources and use targets
    // entered, the instance of a use, the context paints of a marker, how
    // deep in nested images it is, and how to decode an image and to fail.
    struct Pass {
        Document* doc;
        ObjPool* pool;
        Vector<Node*> active;
        Vector<Node*> hosts;
        Vector<ComputedStyle*> documentStyles;
        SvgDecode decode;
        SvgFail fail;
        void* context;
        u32 imageDepth;
        float rootFontSize;
        bool hasContext;
        PaintValue contextFill;
        PaintValue contextStroke;
        u32** instance;
        Vector<Node*> instanceTargets;
        Vector<u32**> instanceMaps;
    };

    // The pass that lowers the document to draw commands over width by
    // height pixels; the document itself is docWidth by docHeight.
    struct Lowering: Pass {
        Vector<Op> ops;
        Vector<Op> programs;
        Vector<Op>* out;
        Vector<Edge> edges;
        Vector<ClipShape> clips;
        Matrix screen;
        Matrix host;
        Vector<u64> broken;
        u32 width;
        u32 height;
        float docWidth;
        float docHeight;
        u32 depth;
        bool backgroundInputs;
        Node* sized;
        float sizedWidth;
        float sizedHeight;
        Matrix contextMatrix;
        Box contextBox;
    };

    // A buffer of float RGBA pixels over a box of the raster, zero at first:
    // a pixel outside the box reads as zero and is never written.
    struct Plane {
        float* data;
        IntBox box;
    };

    // The layers of a drawing in progress: the base, and one for every open
    // group, over the box its group can draw.
    struct Raster {
        u32 width;
        u32 height;
        Vector<Plane> layers;
        Vector<IntBox> dirty;
        Vector<ObjPool*> pools;
        Vector<const Op*> begins;
        Vector<float> accumulation;
        Plane backdrop{nullptr, IntBox{0, 0, 0, 0}};
        size_t backgroundBase = 0;
    };

    struct NamedColor {
        const char* name;
        u32 rgb;
    };

    const NamedColor namedColors[] = {
        {"aliceblue", 0xf0f8ff},
        {"antiquewhite", 0xfaebd7},
        {"aqua", 0x00ffff},
        {"aquamarine", 0x7fffd4},
        {"azure", 0xf0ffff},
        {"beige", 0xf5f5dc},
        {"bisque", 0xffe4c4},
        {"black", 0x000000},
        {"blanchedalmond", 0xffebcd},
        {"blue", 0x0000ff},
        {"blueviolet", 0x8a2be2},
        {"brown", 0xa52a2a},
        {"burlywood", 0xdeb887},
        {"cadetblue", 0x5f9ea0},
        {"chartreuse", 0x7fff00},
        {"chocolate", 0xd2691e},
        {"coral", 0xff7f50},
        {"cornflowerblue", 0x6495ed},
        {"cornsilk", 0xfff8dc},
        {"crimson", 0xdc143c},
        {"cyan", 0x00ffff},
        {"darkblue", 0x00008b},
        {"darkcyan", 0x008b8b},
        {"darkgoldenrod", 0xb8860b},
        {"darkgray", 0xa9a9a9},
        {"darkgreen", 0x006400},
        {"darkgrey", 0xa9a9a9},
        {"darkkhaki", 0xbdb76b},
        {"darkmagenta", 0x8b008b},
        {"darkolivegreen", 0x556b2f},
        {"darkorange", 0xff8c00},
        {"darkorchid", 0x9932cc},
        {"darkred", 0x8b0000},
        {"darksalmon", 0xe9967a},
        {"darkseagreen", 0x8fbc8f},
        {"darkslateblue", 0x483d8b},
        {"darkslategray", 0x2f4f4f},
        {"darkslategrey", 0x2f4f4f},
        {"darkturquoise", 0x00ced1},
        {"darkviolet", 0x9400d3},
        {"deeppink", 0xff1493},
        {"deepskyblue", 0x00bfff},
        {"dimgray", 0x696969},
        {"dimgrey", 0x696969},
        {"dodgerblue", 0x1e90ff},
        {"firebrick", 0xb22222},
        {"floralwhite", 0xfffaf0},
        {"forestgreen", 0x228b22},
        {"fuchsia", 0xff00ff},
        {"gainsboro", 0xdcdcdc},
        {"ghostwhite", 0xf8f8ff},
        {"gold", 0xffd700},
        {"goldenrod", 0xdaa520},
        {"gray", 0x808080},
        {"green", 0x008000},
        {"greenyellow", 0xadff2f},
        {"grey", 0x808080},
        {"honeydew", 0xf0fff0},
        {"hotpink", 0xff69b4},
        {"indianred", 0xcd5c5c},
        {"indigo", 0x4b0082},
        {"ivory", 0xfffff0},
        {"khaki", 0xf0e68c},
        {"lavender", 0xe6e6fa},
        {"lavenderblush", 0xfff0f5},
        {"lawngreen", 0x7cfc00},
        {"lemonchiffon", 0xfffacd},
        {"lightblue", 0xadd8e6},
        {"lightcoral", 0xf08080},
        {"lightcyan", 0xe0ffff},
        {"lightgoldenrodyellow", 0xfafad2},
        {"lightgray", 0xd3d3d3},
        {"lightgreen", 0x90ee90},
        {"lightgrey", 0xd3d3d3},
        {"lightpink", 0xffb6c1},
        {"lightsalmon", 0xffa07a},
        {"lightseagreen", 0x20b2aa},
        {"lightskyblue", 0x87cefa},
        {"lightslategray", 0x778899},
        {"lightslategrey", 0x778899},
        {"lightsteelblue", 0xb0c4de},
        {"lightyellow", 0xffffe0},
        {"lime", 0x00ff00},
        {"limegreen", 0x32cd32},
        {"linen", 0xfaf0e6},
        {"magenta", 0xff00ff},
        {"maroon", 0x800000},
        {"mediumaquamarine", 0x66cdaa},
        {"mediumblue", 0x0000cd},
        {"mediumorchid", 0xba55d3},
        {"mediumpurple", 0x9370db},
        {"mediumseagreen", 0x3cb371},
        {"mediumslateblue", 0x7b68ee},
        {"mediumspringgreen", 0x00fa9a},
        {"mediumturquoise", 0x48d1cc},
        {"mediumvioletred", 0xc71585},
        {"midnightblue", 0x191970},
        {"mintcream", 0xf5fffa},
        {"mistyrose", 0xffe4e1},
        {"moccasin", 0xffe4b5},
        {"navajowhite", 0xffdead},
        {"navy", 0x000080},
        {"oldlace", 0xfdf5e6},
        {"olive", 0x808000},
        {"olivedrab", 0x6b8e23},
        {"orange", 0xffa500},
        {"orangered", 0xff4500},
        {"orchid", 0xda70d6},
        {"palegoldenrod", 0xeee8aa},
        {"palegreen", 0x98fb98},
        {"paleturquoise", 0xafeeee},
        {"palevioletred", 0xdb7093},
        {"papayawhip", 0xffefd5},
        {"peachpuff", 0xffdab9},
        {"peru", 0xcd853f},
        {"pink", 0xffc0cb},
        {"plum", 0xdda0dd},
        {"powderblue", 0xb0e0e6},
        {"purple", 0x800080},
        {"rebeccapurple", 0x663399},
        {"red", 0xff0000},
        {"rosybrown", 0xbc8f8f},
        {"royalblue", 0x4169e1},
        {"saddlebrown", 0x8b4513},
        {"salmon", 0xfa8072},
        {"sandybrown", 0xf4a460},
        {"seagreen", 0x2e8b57},
        {"seashell", 0xfff5ee},
        {"sienna", 0xa0522d},
        {"silver", 0xc0c0c0},
        {"skyblue", 0x87ceeb},
        {"slateblue", 0x6a5acd},
        {"slategray", 0x708090},
        {"slategrey", 0x708090},
        {"snow", 0xfffafa},
        {"springgreen", 0x00ff7f},
        {"steelblue", 0x4682b4},
        {"tan", 0xd2b48c},
        {"teal", 0x008080},
        {"thistle", 0xd8bfd8},
        {"tomato", 0xff6347},
        {"turquoise", 0x40e0d0},
        {"violet", 0xee82ee},
        {"wheat", 0xf5deb3},
        {"white", 0xffffff},
        {"whitesmoke", 0xf5f5f5},
        {"yellow", 0xffff00},
        {"yellowgreen", 0x9acd32},
    };

    const NamedColor systemColors[] = {
        {"accentcolor", 0x0075ffff},
        {"accentcolortext", 0xffffffff},
        {"activeborder", 0x000000ff},
        {"activecaption", 0xffffffff},
        {"activetext", 0xff0000ff},
        {"appworkspace", 0xffffffff},
        {"background", 0xffffffff},
        {"buttonborder", 0x000000ff},
        {"buttonface", 0xefefefff},
        {"buttonhighlight", 0xefefefff},
        {"buttonshadow", 0xefefefff},
        {"buttontext", 0x000000ff},
        {"canvas", 0xffffffff},
        {"canvastext", 0x000000ff},
        {"captiontext", 0x000000ff},
        {"field", 0xffffffff},
        {"fieldtext", 0x000000ff},
        {"graytext", 0x808080ff},
        {"highlight", 0x0041c6cc},
        {"highlighttext", 0xffffffff},
        {"inactiveborder", 0x000000ff},
        {"inactivecaption", 0xffffffff},
        {"inactivecaptiontext", 0x808080ff},
        {"infobackground", 0xffffffff},
        {"infotext", 0x000000ff},
        {"linktext", 0x0000eeff},
        {"mark", 0xffff00ff},
        {"marktext", 0x000000ff},
        {"menu", 0xffffffff},
        {"menutext", 0x000000ff},
        {"scrollbar", 0xffffffff},
        {"selecteditem", 0x1967d2ff},
        {"selecteditemtext", 0xffffffff},
        {"threeddarkshadow", 0x000000ff},
        {"threedface", 0xefefefff},
        {"threedhighlight", 0x000000ff},
        {"threedlightshadow", 0x000000ff},
        {"threedshadow", 0x000000ff},
        {"visitedtext", 0x551a8bff},
        {"window", 0xffffffff},
        {"windowframe", 0x000000ff},
        {"windowtext", 0x000000ff},
    };

    static float clampUnit(float value) {
        return value < 0.f ? 0.f : (value > 1.f ? 1.f : value);
    }

    static float minf(float a, float b) {
        return a < b ? a : b;
    }

    static float maxf(float a, float b) {
        return a > b ? a : b;
    }

    static Matrix identity() {
        return Matrix{1.f, 0.f, 0.f, 1.f, 0.f, 0.f};
    }

    static Matrix translation(float x, float y) {
        return Matrix{1.f, 0.f, 0.f, 1.f, x, y};
    }

    static Matrix scaling(float x, float y) {
        return Matrix{x, 0.f, 0.f, y, 0.f, 0.f};
    }

    static Matrix multiply(const Matrix& m, const Matrix& n) {
        return Matrix{
            m.a * n.a + m.c * n.b,
            m.b * n.a + m.d * n.b,
            m.a * n.c + m.c * n.d,
            m.b * n.c + m.d * n.d,
            m.a * n.e + m.c * n.f + m.e,
            m.b * n.e + m.d * n.f + m.f,
        };
    }

    static Point apply(const Matrix& m, Point p) {
        return Point{m.a * p.x + m.c * p.y + m.e, m.b * p.x + m.d * p.y + m.f};
    }

    static bool invert(const Matrix& m, Matrix& out) {
        double det = (double)m.a * m.d - (double)m.b * m.c;

        if (det == 0.0 || !isfinite(det)) {
            return false;
        }

        double inv = 1.0 / det;

        out.a = (float)(m.d * inv);
        out.b = (float)(-m.b * inv);
        out.c = (float)(-m.c * inv);
        out.d = (float)(m.a * inv);
        out.e = (float)((m.c * (double)m.f - m.d * (double)m.e) * inv);
        out.f = (float)((m.b * (double)m.e - m.a * (double)m.f) * inv);

        return true;
    }

    static float matrixScale(const Matrix& m) {
        float sx = sqrtf(m.a * m.a + m.b * m.b);
        float sy = sqrtf(m.c * m.c + m.d * m.d);

        return maxf(sx, sy);
    }

    static Box emptyBox() {
        return Box{INFINITY, INFINITY, -INFINITY, -INFINITY};
    }

    static void extend(Box& box, Point p) {
        box.x0 = minf(box.x0, p.x);
        box.y0 = minf(box.y0, p.y);
        box.x1 = maxf(box.x1, p.x);
        box.y1 = maxf(box.y1, p.y);
    }

    // A box grows by its points, so it is empty in both axes or in neither.
    static bool boxValid(const Box& box) {
        return box.x0 <= box.x1;
    }

    static u8 lowerAscii(u8 c) {
        return c >= 'A' && c <= 'Z' ? (u8)(c + 32) : c;
    }

    static bool equalsIgnoreCase(StringView a, StringView b) {
        if (a.length() != b.length()) {
            return false;
        }

        for (size_t i = 0; i < a.length(); i++) {
            if (lowerAscii(a[i]) != lowerAscii(b[i])) {
                return false;
            }
        }

        return true;
    }

    static bool startsWithIgnoreCase(StringView text, StringView prefix) {
        return text.length() >= prefix.length() && equalsIgnoreCase(StringView(text.begin(), text.begin() + prefix.length()), prefix);
    }

    static bool isSpace(u8 c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
    }

    // Whether the text that runs from p to end starts with c.
    static bool at(const u8* p, const u8* end, u8 c) {
        return p < end && *p == c;
    }

    static void skipSpaces(const u8*& p, const u8* end) {
        while (p < end && isSpace(*p)) {
            p++;
        }
    }

    static StringView trim(StringView text) {
        const u8* b = text.begin();
        const u8* e = text.end();

        skipSpaces(b, e);

        while (e > b && isSpace(e[-1])) {
            e--;
        }

        return StringView(b, e);
    }

    static StringView keep(ObjPool& pool, const void* data, size_t length) {
        u8* copy = (u8*)pool.allocate(length ? length : 1);

        memcpy(copy, data, length);

        return StringView(copy, length);
    }

    static u32 hashOf(StringView text) {
        return (u32)text.hash64();
    }

    static void rehash(Interner& interner) {
        size_t size = interner.slots.length() ? interner.slots.length() * 2 : 1024;
        Vector<u32> slots;

        slots.zero(size);

        for (u32 atom = 1; atom < interner.texts.length(); atom++) {
            size_t i = hashOf(interner.texts[atom]) & (size - 1);

            while (slots[i]) {
                i = (i + 1) & (size - 1);
            }

            slots.mut(i) = atom;
        }

        interner.slots.xchg(slots);
    }

    static u32 intern(Interner& interner, StringView text) {
        if ((interner.texts.length() + 1) * 2 > interner.slots.length()) {
            rehash(interner);
        }

        size_t mask = interner.slots.length() - 1;
        size_t i = hashOf(text) & mask;

        while (u32 atom = interner.slots[i]) {
            if (interner.texts[atom] == text) {
                return atom;
            }

            i = (i + 1) & mask;
        }

        u32 atom = (u32)interner.texts.length();

        interner.texts.pushBack(text);
        interner.slots.mut(i) = atom;

        return atom;
    }

    static u32 lookup(const Interner& interner, StringView text) {
        size_t mask = interner.slots.length() - 1;
        size_t i = hashOf(text) & mask;

        while (u32 atom = interner.slots[i]) {
            if (interner.texts[atom] == text) {
                return atom;
            }

            i = (i + 1) & mask;
        }

        return 0;
    }

    static void initAtoms(Interner& interner) {
        interner.texts.pushBack(StringView());
        rehash(interner);

        for (u32 i = 1; i < AtomKnown; i++) {
            intern(interner, StringView(knownAtoms[i]));
        }
    }

    static StringView textOf(const Document& doc, u32 atom) {
        return doc.atoms.texts[atom];
    }

    static u32 attribute(const Node* node, u32 name) {
        for (u32 i = 0; i < node->count; i++) {
            if (node->attributes[i].name == name) {
                return node->attributes[i].value;
            }
        }

        return 0;
    }

    static Node* elementById(const Document& doc, u32 atom) {
        return atom && atom < doc.byId.length() ? doc.byId[atom] : nullptr;
    }

    static int hexDigit(u8 c);

    static u32 fragmentAtom(const Document& doc, StringView fragment) {
        u32 atom = lookup(doc.atoms, fragment);

        if (atom || !memchr(fragment.data(), '%', fragment.length())) {
            return atom;
        }

        Buffer decoded;
        const u8* p = fragment.begin();

        while (p < fragment.end()) {
            int high = p + 2 < fragment.end() && *p == '%' ? hexDigit(p[1]) : -1;
            int low = high >= 0 ? hexDigit(p[2]) : -1;

            if (low >= 0) {
                u8 c = (u8)(high * 16 + low);

                decoded.append(&c, 1);
                p += 3;
            } else {
                decoded.append(p, 1);
                p++;
            }
        }

        return lookup(doc.atoms, StringView(decoded));
    }

    static Node* hrefTarget(const Document& doc, const Node* node) {
        u32 value = attribute(node, AtomHref);

        if (!value) {
            return nullptr;
        }

        StringView text = trim(textOf(doc, value));

        if (text.length() < 2 || text[0] != '#') {
            return nullptr;
        }

        return elementById(doc, fragmentAtom(doc, StringView(text.begin() + 1, text.end())));
    }

    // The fail of the library does not return; one that does stops the
    // program.
    [[noreturn]] static void fail(SvgFail callback, SvgError error) {
        callback(error);
        __builtin_trap();
    }

    static bool startsWith(const Cursor& c, StringView text) {
        return (size_t)(c.end - c.p) >= text.length() && memcmp(c.p, text.data(), text.length()) == 0;
    }

    static bool skipSpaces(Cursor& c) {
        const u8* start = c.p;

        while (c.p < c.end && isSpace(*c.p)) {
            c.p++;
        }

        return c.p != start;
    }

    static bool isNameStart(u8 c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':' || c >= 0x80;
    }

    static bool isNameChar(u8 c) {
        return isNameStart(c) || (c >= '0' && c <= '9') || c == '-' || c == '.';
    }

    static StringView readName(const Xml& xml, Cursor& c) {
        const u8* start = c.p;

        if (c.p >= c.end || !isNameStart(*c.p)) {
            fail(xml.fail, SVG_XML_NAME);
        }

        while (c.p < c.end && isNameChar(*c.p)) {
            c.p++;
        }

        return StringView(start, c.p);
    }

    static void skipPast(const Xml& xml, Cursor& c, StringView terminator, SvgError unterminated) {
        while ((size_t)(c.end - c.p) >= terminator.length()) {
            if (memcmp(c.p, terminator.data(), terminator.length()) == 0) {
                c.p += terminator.length();

                return;
            }

            c.p++;
        }

        fail(xml.fail, unterminated);
    }

    static StringView readQuoted(const Xml& xml, Cursor& c) {
        if (c.p >= c.end || (*c.p != '"' && *c.p != '\'')) {
            fail(xml.fail, SVG_XML_QUOTE_EXPECTED);
        }

        u8 quote = *c.p++;
        const u8* start = c.p;

        while (c.p < c.end && *c.p != quote) {
            c.p++;
        }

        if (c.p >= c.end) {
            fail(xml.fail, SVG_XML_UNTERMINATED_LITERAL);
        }

        return StringView(start, c.p++);
    }

    static void skipDeclaration(const Xml& xml, Cursor& c) {
        while (c.p < c.end && *c.p != '>') {
            if (*c.p == '"' || *c.p == '\'') {
                readQuoted(xml, c);
            } else {
                c.p++;
            }
        }

        if (c.p >= c.end) {
            fail(xml.fail, SVG_XML_UNTERMINATED_DECLARATION);
        }

        c.p++;
    }

    static void appendUtf8(Buffer& out, u32 code) {
        u8 bytes[4];
        size_t length;

        if (code < 0x80) {
            bytes[0] = (u8)code;
            length = 1;
        } else if (code < 0x800) {
            bytes[0] = (u8)(0xc0 | (code >> 6));
            bytes[1] = (u8)(0x80 | (code & 0x3f));
            length = 2;
        } else if (code < 0x10000) {
            bytes[0] = (u8)(0xe0 | (code >> 12));
            bytes[1] = (u8)(0x80 | ((code >> 6) & 0x3f));
            bytes[2] = (u8)(0x80 | (code & 0x3f));
            length = 3;
        } else {
            bytes[0] = (u8)(0xf0 | (code >> 18));
            bytes[1] = (u8)(0x80 | ((code >> 12) & 0x3f));
            bytes[2] = (u8)(0x80 | ((code >> 6) & 0x3f));
            bytes[3] = (u8)(0x80 | (code & 0x3f));
            length = 4;
        }

        out.append(bytes, length);
    }

    static u32 characterReference(const Xml& xml, StringView body) {
        u32 code = 0;
        size_t i = 1;
        bool hex = body.length() > 1 && body[1] == 'x';

        if (hex) {
            i = 2;
        }

        if (i >= body.length()) {
            fail(xml.fail, SVG_XML_REFERENCE_EMPTY);
        }

        for (; i < body.length(); i++) {
            u8 c = body[i];
            u32 digit;

            if (c >= '0' && c <= '9') {
                digit = c - '0';
            } else if (hex && lowerAscii(c) >= 'a' && lowerAscii(c) <= 'f') {
                digit = lowerAscii(c) - 'a' + 10;
            } else {
                fail(xml.fail, SVG_XML_REFERENCE_DIGIT);
            }

            code = code * (hex ? 16 : 10) + digit;

            if (code > 0x10ffff) {
                fail(xml.fail, SVG_XML_REFERENCE_CHARACTER);
            }
        }

        if (code == 0 || (code >= 0xd800 && code <= 0xdfff)) {
            fail(xml.fail, SVG_XML_REFERENCE_CHARACTER);
        }

        return code;
    }

    static int predefinedEntity(StringView name) {
        if (name == SV("lt")) {
            return '<';
        }

        if (name == SV("gt")) {
            return '>';
        }

        if (name == SV("amp")) {
            return '&';
        }

        if (name == SV("apos")) {
            return '\'';
        }

        if (name == SV("quot")) {
            return '"';
        }

        return -1;
    }

    static const Entity* findEntity(const Xml& xml, StringView name) {
        for (const Entity& entity : xml.entities) {
            if (entity.name == name) {
                return &entity;
            }
        }

        return nullptr;
    }

    static const u8* referenceEnd(const Xml& xml, const u8* p, const u8* end) {
        const u8* limit = end - p > 64 ? p + 64 : end;

        for (const u8* q = p; q < limit; q++) {
            if (*q == ';') {
                return q;
            }
        }

        fail(xml.fail, SVG_XML_UNTERMINATED_REFERENCE);
    }

    static void enterEntity(Xml& xml, const Entity& entity) {
        xml.expanded += entity.value.length();
        xml.entityDepth++;

        if (xml.expanded > maxExpansion || xml.entityDepth > maxEntityDepth) {
            fail(xml.fail, SVG_XML_ENTITY_LIMIT);
        }
    }

    static void decodeAttribute(Xml& xml, StringView raw, Buffer& out) {
        const u8* p = raw.begin();
        const u8* end = raw.end();

        while (p < end) {
            u8 c = *p;

            if (c == '<') {
                fail(xml.fail, SVG_XML_ATTRIBUTE_LT);
            }

            if (c == '&') {
                const u8* semicolon = referenceEnd(xml, p + 1, end);
                StringView name(p + 1, semicolon);

                if (!name.empty() && name[0] == '#') {
                    appendUtf8(out, characterReference(xml, name));
                } else if (int value = predefinedEntity(name); value >= 0) {
                    u8 byte = (u8)value;

                    out.append(&byte, 1);
                } else {
                    const Entity* entity = findEntity(xml, name);

                    if (!entity) {
                        fail(xml.fail, SVG_XML_UNKNOWN_ENTITY);
                    }

                    if (entity->external) {
                        fail(xml.fail, SVG_XML_EXTERNAL_ENTITY);
                    }

                    enterEntity(xml, *entity);
                    decodeAttribute(xml, entity->value, out);
                    xml.entityDepth--;
                }

                p = semicolon + 1;
                continue;
            }

            if (c == '\r' && p + 1 < end && p[1] == '\n') {
                p++;
                continue;
            }

            u8 byte = c == '\t' || c == '\n' || c == '\r' ? (u8)' ' : c;

            out.append(&byte, 1);
            p++;
        }
    }

    static u32 internAttribute(Xml& xml, StringView raw) {
        bool plain = true;

        for (u8 c : raw) {
            if (c == '&' || c == '<' || c == '\t' || c == '\n' || c == '\r') {
                plain = false;
                break;
            }
        }

        if (plain) {
            return intern(xml.doc->atoms, raw);
        }

        Buffer out;

        decodeAttribute(xml, raw, out);

        return intern(xml.doc->atoms, keep(*xml.doc->pool, out.data(), out.length()));
    }

    static StringView expandCharacterReferences(Xml& xml, StringView literal) {
        bool plain = true;

        for (u8 c : literal) {
            if (c == '&') {
                plain = false;
                break;
            }
        }

        if (plain) {
            return literal;
        }

        Buffer out;
        const u8* p = literal.begin();

        while (p < literal.end()) {
            if (*p == '&' && p + 1 < literal.end() && p[1] == '#') {
                const u8* semicolon = referenceEnd(xml, p + 1, literal.end());

                appendUtf8(out, characterReference(xml, StringView(p + 1, semicolon)));
                p = semicolon + 1;
            } else {
                out.append(p, 1);
                p++;
            }
        }

        return keep(*xml.doc->pool, out.data(), out.length());
    }

    static void parseEntity(Xml& xml, Cursor& c) {
        c.p += 8;
        skipSpaces(c);

        bool parameter = false;

        if (c.p < c.end && *c.p == '%') {
            parameter = true;
            c.p++;
            skipSpaces(c);
        }

        Entity entity{readName(xml, c), StringView(), false};

        skipSpaces(c);

        if (c.p < c.end && (*c.p == '"' || *c.p == '\'')) {
            entity.value = expandCharacterReferences(xml, readQuoted(xml, c));
        } else {
            entity.external = true;
        }

        skipDeclaration(xml, c);

        if (!parameter && !findEntity(xml, entity.name)) {
            xml.entities.pushBack(entity);
        }
    }

    static void parseDoctype(Xml& xml, Cursor& c) {
        c.p += 9;
        skipSpaces(c);
        readName(xml, c);
        skipSpaces(c);

        if (startsWith(c, SV("SYSTEM"))) {
            c.p += 6;
            skipSpaces(c);
            readQuoted(xml, c);
            skipSpaces(c);
        } else if (startsWith(c, SV("PUBLIC"))) {
            c.p += 6;
            skipSpaces(c);
            readQuoted(xml, c);
            skipSpaces(c);
            readQuoted(xml, c);
            skipSpaces(c);
        }

        if (c.p < c.end && *c.p == '[') {
            c.p++;

            for (;;) {
                skipSpaces(c);

                if (c.p >= c.end) {
                    fail(xml.fail, SVG_XML_UNTERMINATED_DOCTYPE);
                }

                if (*c.p == ']') {
                    c.p++;
                    break;
                }

                if (startsWith(c, SV("<!ENTITY"))) {
                    parseEntity(xml, c);
                } else if (startsWith(c, SV("<!--"))) {
                    c.p += 4;
                    skipPast(xml, c, SV("-->"), SVG_XML_UNTERMINATED_COMMENT);
                } else if (startsWith(c, SV("<?"))) {
                    skipPast(xml, c, SV("?>"), SVG_XML_UNTERMINATED_PI);
                } else if (startsWith(c, SV("<!"))) {
                    skipDeclaration(xml, c);
                } else if (*c.p == '%') {
                    skipPast(xml, c, SV(";"), SVG_XML_UNTERMINATED_REFERENCE);
                } else {
                    fail(xml.fail, SVG_XML_DOCTYPE_MARKUP);
                }
            }

            skipSpaces(c);
        }

        if (c.p >= c.end || *c.p != '>') {
            fail(xml.fail, SVG_XML_DOCTYPE_CLOSE);
        }

        c.p++;
    }

    static void splitName(StringView name, StringView& prefix, StringView& local) {
        for (const u8* p = name.begin(); p < name.end(); p++) {
            if (*p == ':') {
                prefix = StringView(name.begin(), p);
                local = StringView(p + 1, name.end());

                return;
            }
        }

        prefix = StringView();
        local = name;
    }

    static bool resolvePrefix(const Xml& xml, StringView prefix, StringView& uri) {
        if (prefix == SV("xml")) {
            uri = xmlNamespace;

            return true;
        }

        for (size_t i = xml.scopes.length(); i > 0; i--) {
            if (xml.scopes[i - 1].prefix == prefix) {
                uri = xml.scopes[i - 1].uri;

                return true;
            }
        }

        uri = StringView();

        return prefix.empty();
    }

    static void attach(Node* parent, Node* node) {
        node->parent = parent;
        node->prev = parent->last;

        if (parent->last) {
            parent->last->next = node;
        } else {
            parent->first = node;
        }

        parent->last = node;
    }

    // A failed xi:include, whose xi:fallback stands in for it: the text the
    // include's parent keeps, if any.
    struct Fallback {
        Buffer* text;
    };

    static void parseElement(Xml& xml, Cursor& c, Node* parent, bool foreign, u32 depth, Buffer* text, const Fallback* fallback);

    // The content of an element: its elements are its parent's children, its
    // characters go to text if the parent keeps any. A fallback says the
    // content is a failed xi:include's.
    static void parseContent(Xml& xml, Cursor& c, Node* parent, bool foreign, Buffer* text, u32 depth, bool inEntity, const Fallback* fallback) {
        for (;;) {
            if (c.p >= c.end) {
                if (inEntity) {
                    return;
                }

                fail(xml.fail, SVG_XML_UNTERMINATED_ELEMENT);
            }

            u8 ch = *c.p;

            if (ch == '<') {
                if (startsWith(c, SV("</"))) {
                    if (inEntity) {
                        fail(xml.fail, SVG_XML_ENTITY_END_TAG);
                    }

                    return;
                }

                if (startsWith(c, SV("<!--"))) {
                    c.p += 4;
                    skipPast(xml, c, SV("-->"), SVG_XML_UNTERMINATED_COMMENT);
                    continue;
                }

                if (startsWith(c, SV("<![CDATA["))) {
                    c.p += 9;

                    const u8* start = c.p;

                    skipPast(xml, c, SV("]]>"), SVG_XML_UNTERMINATED_CDATA);

                    if (text) {
                        text->append(start, c.p - 3 - start);
                    }

                    continue;
                }

                if (startsWith(c, SV("<?"))) {
                    skipPast(xml, c, SV("?>"), SVG_XML_UNTERMINATED_PI);
                    continue;
                }

                if (startsWith(c, SV("<!"))) {
                    fail(xml.fail, SVG_XML_CONTENT_DECLARATION);
                }

                parseElement(xml, c, parent, foreign, depth + 1, text, fallback);
                continue;
            }

            if (ch == '&') {
                const u8* semicolon = referenceEnd(xml, c.p + 1, c.end);
                StringView name(c.p + 1, semicolon);

                if (!name.empty() && name[0] == '#') {
                    u32 code = characterReference(xml, name);

                    if (text) {
                        appendUtf8(*text, code);
                    }
                } else if (int value = predefinedEntity(name); value >= 0) {
                    if (text) {
                        u8 byte = (u8)value;

                        text->append(&byte, 1);
                    }
                } else {
                    const Entity* entity = findEntity(xml, name);

                    if (!entity) {
                        fail(xml.fail, SVG_XML_UNKNOWN_ENTITY);
                    }

                    if (!entity->external) {
                        Cursor inner{entity->value.begin(), entity->value.end()};

                        enterEntity(xml, *entity);
                        parseContent(xml, inner, parent, foreign, text, depth, true, fallback);
                        xml.entityDepth--;
                    }
                }

                c.p = semicolon + 1;
                continue;
            }

            const u8* start = c.p;

            while (c.p < c.end && *c.p != '<' && *c.p != '&') {
                c.p++;
            }

            if (text) {
                text->append(start, c.p - start);
            }
        }
    }

    static bool include(Xml& xml, size_t rawBase, Node* parent, u32 depth, Buffer* text);
    static bool decodeDataUri(StringView href, StringView& mime, Buffer& out);

    // An element and its content. Text is its parent's text, which an
    // xi:include of text or an xi:fallback adds to.
    static void parseElement(Xml& xml, Cursor& c, Node* parent, bool foreign, u32 depth, Buffer* text, const Fallback* fallback) {
        if (depth > maxDepth) {
            fail(xml.fail, SVG_XML_DEPTH);
        }

        c.p++;

        StringView name = readName(xml, c);
        size_t rawBase = xml.raw.length();
        bool empty = false;

        for (;;) {
            bool spaced = skipSpaces(c);

            if (c.p >= c.end) {
                fail(xml.fail, SVG_XML_UNTERMINATED_TAG);
            }

            if (*c.p == '/') {
                c.p++;

                if (c.p >= c.end || *c.p != '>') {
                    fail(xml.fail, SVG_XML_SLASH);
                }

                c.p++;
                empty = true;
                break;
            }

            if (*c.p == '>') {
                c.p++;
                break;
            }

            if (!spaced) {
                fail(xml.fail, SVG_XML_ATTRIBUTE_SPACE);
            }

            StringView attributeName = readName(xml, c);

            skipSpaces(c);

            if (c.p >= c.end || *c.p != '=') {
                fail(xml.fail, SVG_XML_ATTRIBUTE_EQUALS);
            }

            c.p++;
            skipSpaces(c);

            StringView value = readQuoted(xml, c);

            for (size_t i = rawBase; i < xml.raw.length(); i++) {
                if (xml.raw[i].name == attributeName) {
                    fail(xml.fail, SVG_XML_ATTRIBUTE_DUPLICATE);
                }
            }

            xml.raw.pushBack(RawAttribute{attributeName, value});
        }

        size_t scopeBase = xml.scopes.length();

        for (size_t i = rawBase; i < xml.raw.length(); i++) {
            StringView attributeName = xml.raw[i].name;

            if (attributeName == SV("xmlns")) {
                xml.scopes.pushBack(Scope{StringView(), textOf(*xml.doc, internAttribute(xml, xml.raw[i].value))});
            } else if (attributeName.length() > 6 && memcmp(attributeName.data(), "xmlns:", 6) == 0) {
                xml.scopes.pushBack(Scope{StringView(attributeName.begin() + 6, attributeName.end()), textOf(*xml.doc, internAttribute(xml, xml.raw[i].value))});
            }
        }

        StringView prefix;
        StringView local;
        StringView uri;

        splitName(name, prefix, local);

        if (!resolvePrefix(xml, prefix, uri)) {
            fail(xml.fail, SVG_XML_UNDECLARED_PREFIX);
        }

        // An xi:include stands for what it includes, put in its place; its own
        // content is ignored but for an xi:fallback, which stands for it when
        // the include fails.
        bool xinclude = uri == xincludeNamespace;
        bool includes = xinclude && !foreign && local == SV("include");
        bool falls = xinclude && fallback && local == SV("fallback");
        bool included = includes && include(xml, rawBase, parent, depth, text);
        bool svg = !foreign && uri == svgNamespace;
        Node* node = nullptr;

        // Only the first element of the file has no parent: a foreign
        // element's children are foreign.
        if (svg) {
            node = xml.doc->pool->make<Node>();
            node->name = intern(xml.doc->atoms, local);
            node->index = xml.doc->nodeCount++;

            if (parent) {
                attach(parent, node);
            } else {
                xml.doc->root = node;
            }

            Vector<Attribute> attributes;
            bool plainHref = false;

            for (size_t i = rawBase; i < xml.raw.length(); i++) {
                StringView attributeName = xml.raw[i].name;
                StringView attributePrefix;
                StringView attributeLocal;
                StringView attributeUri;

                splitName(attributeName, attributePrefix, attributeLocal);

                if (attributeName == SV("xmlns") || attributePrefix == SV("xmlns")) {
                    continue;
                }

                u32 atom;

                if (attributePrefix.empty()) {
                    atom = intern(xml.doc->atoms, attributeLocal);
                } else {
                    if (!resolvePrefix(xml, attributePrefix, attributeUri)) {
                        fail(xml.fail, SVG_XML_UNDECLARED_PREFIX);
                    }

                    if (attributeUri != xlinkNamespace || attributeLocal != SV("href")) {
                        continue;
                    }

                    if (plainHref) {
                        continue;
                    }

                    atom = AtomHref;
                }

                u32 value = internAttribute(xml, xml.raw[i].value);

                if (atom == AtomHref) {
                    bool replaced = false;

                    for (size_t k = 0; k < attributes.length(); k++) {
                        if (attributes[k].name == AtomHref) {
                            if (attributePrefix.empty()) {
                                attributes.mut(k).value = value;
                            }

                            replaced = true;
                        }
                    }

                    if (attributePrefix.empty()) {
                        plainHref = true;
                    }

                    if (replaced) {
                        continue;
                    }
                }

                attributes.pushBack(Attribute{atom, value});
            }

            node->count = (u32)attributes.length();
            node->attributes = (Attribute*)xml.doc->pool->allocate(sizeof(Attribute) * (attributes.length() + 1));

            if (node->count) {
                memcpy(node->attributes, attributes.data(), sizeof(Attribute) * attributes.length());
            }
        }

        while (xml.raw.length() > rawBase) {
            xml.raw.popBack();
        }

        if (!empty) {
            Buffer own;
            bool collect = node && node->name == AtomStyle;

            Fallback failed{text};

            if (includes) {
                parseContent(xml, c, parent, true, nullptr, depth, false, included ? nullptr : &failed);
            } else if (falls) {
                parseContent(xml, c, parent, false, fallback->text, depth, false, nullptr);
            } else {
                parseContent(xml, c, node, !node, collect ? &own : nullptr, depth, false, nullptr);
            }

            c.p += 2;

            StringView endName = readName(xml, c);

            if (endName != name) {
                fail(xml.fail, SVG_XML_END_TAG_MISMATCH);
            }

            skipSpaces(c);

            if (c.p >= c.end || *c.p != '>') {
                fail(xml.fail, SVG_XML_END_TAG_CLOSE);
            }

            c.p++;

            if (collect) {
                node->text = keep(*xml.doc->pool, own.data(), own.length());
            }
        }

        while (xml.scopes.length() > scopeBase) {
            xml.scopes.popBack();
        }
    }

    static void skipMisc(const Xml& xml, Cursor& c) {
        for (;;) {
            skipSpaces(c);

            if (startsWith(c, SV("<!--"))) {
                c.p += 4;
                skipPast(xml, c, SV("-->"), SVG_XML_UNTERMINATED_COMMENT);
            } else if (startsWith(c, SV("<?"))) {
                skipPast(xml, c, SV("?>"), SVG_XML_UNTERMINATED_PI);
            } else {
                return;
            }
        }
    }

    static Node* nextNode(Node* node, Node* top) {
        if (node->first) {
            return node->first;
        }

        while (node != top) {
            if (node->next) {
                return node->next;
            }

            node = node->parent;
        }

        return nullptr;
    }

    // A whole XML document: its root element is the parent's child, or the
    // document's root when there is no parent.
    static void parseDocument(Xml& xml, Cursor& c, Node* parent, u32 depth) {
        if (startsWith(c, SV("\xef\xbb\xbf"))) {
            c.p += 3;
        }

        for (;;) {
            skipMisc(xml, c);

            if (startsWith(c, SV("<!DOCTYPE"))) {
                parseDoctype(xml, c);
            } else {
                break;
            }
        }

        if (c.p >= c.end || *c.p != '<') {
            fail(xml.fail, SVG_XML_NO_ROOT);
        }

        parseElement(xml, c, parent, false, depth, nullptr, nullptr);
        skipMisc(xml, c);

        if (c.p != c.end) {
            fail(xml.fail, SVG_XML_TRAILING);
        }
    }

    // What an xi:include names, put in its place, as librsvg does: a
    // document's root element as a child of the include's parent, or text,
    // with parse="text", into the parent's text if it keeps any. Without an
    // href nothing is included. The resources are data: URLs alone; another
    // href, or a data: URL that does not decode, cannot be had, and the
    // include's xi:fallback stands in: false then. An included document that
    // is no XML document fails as the file would, and so does a parse that
    // is neither xml nor text.
    static bool include(Xml& xml, size_t rawBase, Node* parent, u32 depth, Buffer* text) {
        StringView href;
        StringView parse = SV("xml");
        bool linked = false;

        for (size_t i = rawBase; i < xml.raw.length(); i++) {
            const RawAttribute& attribute = xml.raw[i];

            if (attribute.name == SV("href")) {
                href = textOf(*xml.doc, internAttribute(xml, attribute.value));
                linked = true;
            } else if (attribute.name == SV("parse")) {
                parse = textOf(*xml.doc, internAttribute(xml, attribute.value));
            }
        }

        if (parse != SV("xml") && parse != SV("text")) {
            fail(xml.fail, SVG_XML_INCLUDE_PARSE);
        }

        StringView mime;
        Buffer bytes;

        if (!linked) {
            return true;
        }

        if (!decodeDataUri(href, mime, bytes)) {
            return false;
        }

        StringView data = keep(*xml.doc->pool, bytes.data(), bytes.length());

        if (parse == SV("text")) {
            if (text) {
                text->append(data.data(), data.length());
            }

            return true;
        }

        // The included document has its own entities and namespaces.
        Xml inner;
        Cursor c{data.begin(), data.end()};

        inner.doc = xml.doc;
        inner.fail = xml.fail;
        inner.expanded = 0;
        inner.entityDepth = 0;
        parseDocument(inner, c, parent, depth + 1);

        return true;
    }

    static void parseXml(Document& doc, StringView text, SvgFail callback) {
        Xml xml;

        xml.doc = &doc;
        xml.fail = callback;
        xml.expanded = 0;
        xml.entityDepth = 0;

        Cursor c{text.begin(), text.end()};

        parseDocument(xml, c, nullptr, 0);

        doc.byId.zero(doc.atoms.texts.length());

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            u32 id = attribute(node, AtomId);

            if (id && !doc.byId[id]) {
                doc.byId.mut(id) = node;
            }
        }
    }

    static bool isIdentStart(u8 c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-' || c >= 0x80;
    }

    static bool isIdentChar(u8 c) {
        return isIdentStart(c) || (c >= '0' && c <= '9');
    }

    static StringView readIdent(const u8*& p, const u8* end) {
        const u8* start = p;

        if (at(p, end, '-')) {
            p++;
        }

        if (p >= end || !isIdentStart(*p) || *p == '-') {
            p = start;

            return StringView();
        }

        while (p < end && isIdentChar(*p)) {
            p++;
        }

        return StringView(start, p);
    }

    static const u8* skipCssString(const u8* p, const u8* end) {
        u8 quote = *p++;

        while (p < end && *p != quote) {
            if (*p == '\\' && p + 1 < end) {
                p++;
            }

            p++;
        }

        return p < end ? p + 1 : end;
    }

    static void stripComments(StringView text, Buffer& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        while (p < end) {
            if (*p == '"' || *p == '\'') {
                const u8* next = skipCssString(p, end);

                out.append(p, next - p);
                p = next;
            } else if (*p == '/' && p + 1 < end && p[1] == '*') {
                p += 2;

                while (p + 1 < end && !(p[0] == '*' && p[1] == '/')) {
                    p++;
                }

                p = p + 1 < end ? p + 2 : end;
                out.append(" ", 1);
            } else {
                out.append(p, 1);
                p++;
            }
        }
    }

    static const u8* findOutside(const u8* p, const u8* end, u8 target) {
        int depth = 0;

        while (p < end) {
            u8 c = *p;

            if (c == '"' || c == '\'') {
                p = skipCssString(p, end);
                continue;
            }

            if (depth == 0 && c == target) {
                return p;
            }

            if (c == '(' || c == '[') {
                depth++;
            } else if ((c == ')' || c == ']') && depth > 0) {
                depth--;
            }

            p++;
        }

        return nullptr;
    }

    static const u8* matchingBrace(const u8* p, const u8* end) {
        int depth = 1;

        while (p < end) {
            u8 c = *p;

            if (c == '"' || c == '\'') {
                p = skipCssString(p, end);
                continue;
            }

            if (c == '{') {
                depth++;
            } else if (c == '}') {
                depth--;

                if (depth == 0) {
                    return p;
                }
            }

            p++;
        }

        return end;
    }

    // Specificity holds the counts of ids, classes and types in its three low
    // bytes; each count saturates at 255 instead of carrying into the next one.
    static u32 addSpecificity(u32 specificity, u32 weight) {
        u32 sum = 0;

        for (u32 shift = 0; shift < 24; shift += 8) {
            u32 count = ((specificity >> shift) & 0xff) + ((weight >> shift) & 0xff);

            sum |= (count < 0xff ? count : 0xff) << shift;
        }

        return sum;
    }

    // A selector closes every bracket and parenthesis it opens, as parseSheet
    // splits selectors only where they are closed, so the text runs on past
    // the parts of an attribute test and of a :not.
    static bool parseCompound(Document& doc, const u8*& p, const u8* end, Compound& compound, u32& specificity) {
        Vector<Condition> conditions;
        bool any = false;

        compound.type = 0;

        if (at(p, end, '*')) {
            p++;
            any = true;
        } else {
            StringView type = readIdent(p, end);

            if (!type.empty()) {
                compound.type = intern(doc.atoms, type);
                specificity = addSpecificity(specificity, 1);
                any = true;
            }
        }

        while (p < end) {
            u8 c = *p;
            Condition condition{Test::Never, 0, StringView(), nullptr, 0};

            if (c == '#') {
                p++;

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                condition.test = Test::Id;
                condition.name = intern(doc.atoms, name);
                specificity = addSpecificity(specificity, 1 << 16);
            } else if (c == '.') {
                p++;

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                condition.test = Test::Class;
                condition.name = intern(doc.atoms, name);
                specificity = addSpecificity(specificity, 1 << 8);
            } else if (c == '[') {
                p++;

                skipSpaces(p, end);

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                skipSpaces(p, end);

                condition.name = intern(doc.atoms, name);
                condition.test = Test::Exists;

                if (*p != ']') {
                    if (*p == '=') {
                        condition.test = Test::Equals;
                        p++;
                    } else if (p[1] == '=') {
                        switch (*p) {
                            case '~':
                                condition.test = Test::Includes;
                                break;
                            case '|':
                                condition.test = Test::Dash;
                                break;
                            case '^':
                                condition.test = Test::Prefix;
                                break;
                            case '$':
                                condition.test = Test::Suffix;
                                break;
                            case '*':
                                condition.test = Test::Substring;
                                break;
                            default:
                                return false;
                        }

                        p += 2;
                    } else {
                        return false;
                    }

                    skipSpaces(p, end);

                    if (*p == '"' || *p == '\'') {
                        const u8* next = skipCssString(p, end);

                        condition.value = StringView(p + 1, next - 1);
                        p = next;
                    } else {
                        condition.value = readIdent(p, end);

                        if (condition.value.empty()) {
                            return false;
                        }
                    }

                    skipSpaces(p, end);
                }

                if (*p != ']') {
                    return false;
                }

                p++;
                specificity = addSpecificity(specificity, 1 << 8);
            } else if (c == ':') {
                p++;

                if (at(p, end, ':')) {
                    p++;
                    readIdent(p, end);
                    condition.test = Test::Never;
                    specificity = addSpecificity(specificity, 1);
                } else {
                    StringView name = readIdent(p, end);

                    if (!name.empty() && equalsIgnoreCase(name, SV("not")) && at(p, end, '(')) {
                        Vector<Compound> inner;
                        u32 best = 0;

                        p++;

                        for (;;) {
                            Compound item;
                            u32 weight = 0;

                            skipSpaces(p, end);

                            item.combinator = Combinator::None;

                            if (!parseCompound(doc, p, end, item, weight)) {
                                return false;
                            }

                            inner.pushBack(item);
                            best = weight > best ? weight : best;

                            skipSpaces(p, end);

                            if (at(p, end, ',')) {
                                p++;
                                continue;
                            }

                            break;
                        }

                        if (*p != ')') {
                            return false;
                        }

                        p++;
                        condition.test = Test::Not;
                        condition.innerCount = (u32)inner.length();
                        condition.inner = (Compound*)doc.pool->allocate(sizeof(Compound) * inner.length());
                        memcpy(condition.inner, inner.data(), sizeof(Compound) * inner.length());
                        specificity = addSpecificity(specificity, best);
                        conditions.pushBack(condition);
                        any = true;
                        continue;
                    }

                    if (name.empty() || at(p, end, '(')) {
                        return false;
                    }

                    specificity = addSpecificity(specificity, 1 << 8);

                    if (equalsIgnoreCase(name, SV("first-child"))) {
                        condition.test = Test::FirstChild;
                    } else if (equalsIgnoreCase(name, SV("last-child"))) {
                        condition.test = Test::LastChild;
                    } else if (equalsIgnoreCase(name, SV("only-child"))) {
                        condition.test = Test::OnlyChild;
                    } else if (equalsIgnoreCase(name, SV("root"))) {
                        condition.test = Test::Root;
                    } else if (equalsIgnoreCase(name, SV("hover")) || equalsIgnoreCase(name, SV("active")) || equalsIgnoreCase(name, SV("focus")) || equalsIgnoreCase(name, SV("visited")) || equalsIgnoreCase(name, SV("target"))) {
                        condition.test = Test::Never;
                    } else {
                        return false;
                    }
                }
            } else {
                break;
            }

            conditions.pushBack(condition);
            any = true;
        }

        if (!any) {
            return false;
        }

        compound.count = (u32)conditions.length();
        compound.conditions = (Condition*)doc.pool->allocate(sizeof(Condition) * (conditions.length() + 1));

        if (compound.count) {
            memcpy(compound.conditions, conditions.data(), sizeof(Condition) * conditions.length());
        }

        return true;
    }

    // Parses a trimmed selector.
    static bool parseSelector(Document& doc, StringView text, Selector& selector) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Vector<Compound> compounds;
        u32 specificity = 0;

        for (;;) {
            Compound compound;

            compound.combinator = Combinator::None;

            if (!parseCompound(doc, p, end, compound, specificity)) {
                return false;
            }

            compounds.pushBack(compound);

            bool spaced = false;

            while (p < end && isSpace(*p)) {
                p++;
                spaced = true;
            }

            if (p >= end) {
                break;
            }

            Combinator combinator = Combinator::Descendant;

            if (*p == '>' || *p == '+' || *p == '~') {
                combinator = *p == '>' ? Combinator::Child : (*p == '+' ? Combinator::Adjacent : Combinator::Sibling);
                p++;

                skipSpaces(p, end);
            } else if (!spaced) {
                return false;
            }

            compounds.mutBack().combinator = combinator;
        }

        selector.count = (u32)compounds.length();
        selector.specificity = specificity;
        selector.compounds = (Compound*)doc.pool->allocate(sizeof(Compound) * compounds.length());

        for (u32 i = 0; i < selector.count; i++) {
            Compound compound = compounds[selector.count - 1 - i];

            compound.combinator = i + 1 < selector.count ? compounds[selector.count - 2 - i].combinator : Combinator::None;
            selector.compounds[i] = compound;
        }

        return true;
    }

    static bool geometryProperty(u32 atom) {
        switch (atom) {
            case AtomX:
            case AtomY:
            case AtomWidth:
            case AtomHeight:
            case AtomCx:
            case AtomCy:
            case AtomR:
            case AtomRx:
            case AtomRy:
            case AtomD:
            case AtomTransform:
            case AtomTransformOrigin:
            case AtomTransformBox:
                return true;
            default:
                return false;
        }
    }

    static bool geometryApplies(u32 element, u32 property) {
        switch (property) {
            case AtomX:
            case AtomY:
                return element == AtomRect || element == AtomImage || element == AtomSvg || element == AtomUse;
            case AtomWidth:
            case AtomHeight:
                return element == AtomRect || element == AtomImage || element == AtomSvg || element == AtomUse || element == AtomSymbol;
            case AtomCx:
            case AtomCy:
                return element == AtomCircle || element == AtomEllipse;
            case AtomR:
                return element == AtomCircle;
            case AtomRx:
            case AtomRy:
                return element == AtomRect || element == AtomEllipse;
            case AtomD:
                return element == AtomPath;
            default:
                return true;
        }
    }

    static const u8* findFunction(StringView text, StringView name) {
        for (const u8* p = text.begin(); p + name.length() <= text.end(); p++) {
            if (equalsIgnoreCase(StringView(p, p + name.length()), name)) {
                return p;
            }
        }

        return nullptr;
    }

    static bool substituteFallbacks(StringView text, Buffer& out, u32 depth) {
        const u8* p = text.begin();

        while (p < text.end()) {
            const u8* var = findFunction(StringView(p, text.end()), SV("var("));
            const u8* env = findFunction(StringView(p, text.end()), SV("env("));
            const u8* at = var && (!env || var < env) ? var : env;

            if (!at) {
                out.append(p, (size_t)(text.end() - p));

                return true;
            }

            out.append(p, (size_t)(at - p));

            const u8* q = at + 4;
            const u8* comma = nullptr;
            int level = 0;

            while (q < text.end() && (level > 0 || *q != ')')) {
                if (*q == '(') {
                    level++;
                } else if (*q == ')') {
                    level--;
                } else if (*q == ',' && level == 0 && !comma) {
                    comma = q;
                }

                q++;
            }

            if (q >= text.end() || !comma || depth > 8) {
                return false;
            }

            if (!substituteFallbacks(trim(StringView(comma + 1, q)), out, depth + 1)) {
                return false;
            }

            p = q + 1;
        }

        return true;
    }

    static void parseDeclarations(Document& doc, StringView block, Vector<Declaration>& out) {
        const u8* p = block.begin();
        const u8* end = block.end();

        while (p < end) {
            const u8* stop = findOutside(p, end, ';');

            if (!stop) {
                stop = end;
            }

            StringView item(p, stop);
            const u8* colon = findOutside(item.begin(), item.end(), ':');

            p = stop < end ? stop + 1 : end;

            if (!colon) {
                continue;
            }

            StringView name = trim(StringView(item.begin(), colon));
            StringView value = trim(StringView(colon + 1, item.end()));
            bool important = false;

            if (name.length() == 0 || name.length() > 64) {
                continue;
            }

            for (const u8* bang = value.end(); bang > value.begin(); bang--) {
                if (bang[-1] == '!') {
                    if (equalsIgnoreCase(trim(StringView(bang, value.end())), SV("important"))) {
                        important = true;
                        value = trim(StringView(value.begin(), bang - 1));
                    }

                    break;
                }
            }

            u8 lowered[64];

            for (size_t i = 0; i < name.length(); i++) {
                lowered[i] = lowerAscii(name[i]);
            }

            u32 property = lookup(doc.atoms, StringView(lowered, name.length()));

            if (StringView(lowered, name.length()) == SV("-webkit-clip-path")) {
                property = AtomClipPathProperty;
            }

            if (!(property >= firstProperty && property < firstProperty + propertyCount) && !geometryProperty(property) && property != AtomMarker) {
                continue;
            }

            if (value.empty()) {
                continue;
            }

            Buffer substituted;

            if (findFunction(value, SV("var(")) || findFunction(value, SV("env("))) {
                if (!substituteFallbacks(value, substituted, 0)) {
                    continue;
                }

                value = trim(keep(*doc.pool, substituted.data(), substituted.length()));
            }

            if (property == AtomMarker) {
                if (!value.empty()) {
                    u32 atom = intern(doc.atoms, value);

                    out.pushBack(Declaration{AtomMarkerStart, atom, important});
                    out.pushBack(Declaration{AtomMarkerMid, atom, important});
                    out.pushBack(Declaration{AtomMarkerEnd, atom, important});
                }

                continue;
            }

            if (property == AtomD && startsWithIgnoreCase(value, SV("path(")) && value.back() == ')') {
                StringView inner = trim(StringView(value.begin() + 5, value.end() - 1));

                if (inner.length() >= 2 && (inner[0] == '"' || inner[0] == '\'') && inner.back() == inner[0]) {
                    value = StringView(inner.begin() + 1, inner.end() - 1);
                }
            }

            out.pushBack(Declaration{property, intern(doc.atoms, value), important});
        }
    }

    static void parseSheet(Document& doc, Vector<Rule>& rules, StringView source) {
        Buffer cleaned;

        stripComments(source, cleaned);

        StringView text = keep(*doc.pool, cleaned.data(), cleaned.length());
        const u8* p = text.begin();
        const u8* end = text.end();

        while (p < end) {
            if (isSpace(*p)) {
                p++;
                continue;
            }

            if (end - p >= 4 && memcmp(p, "<!--", 4) == 0) {
                p += 4;
                continue;
            }

            if (end - p >= 3 && memcmp(p, "-->", 3) == 0) {
                p += 3;
                continue;
            }

            if (*p == '@') {
                const u8* q = p;

                while (q < end && *q != ';' && *q != '{') {
                    q = *q == '"' || *q == '\'' ? skipCssString(q, end) : q + 1;
                }

                if (at(q, end, '{')) {
                    q = matchingBrace(q + 1, end);
                }

                p = q < end ? q + 1 : end;
                continue;
            }

            const u8* open = findOutside(p, end, '{');

            if (!open) {
                break;
            }

            const u8* close = matchingBrace(open + 1, end);
            StringView selectors(p, open);
            StringView block(open + 1, close);

            p = close < end ? close + 1 : end;

            Vector<Selector> parsed;
            bool valid = true;
            const u8* s = selectors.begin();

            while (valid) {
                const u8* comma = findOutside(s, selectors.end(), ',');
                const u8* stop = comma ? comma : selectors.end();
                Selector selector;

                valid = parseSelector(doc, trim(StringView(s, stop)), selector);

                if (valid) {
                    parsed.pushBack(selector);
                }

                if (!comma) {
                    break;
                }

                s = comma + 1;
            }

            if (!valid) {
                continue;
            }

            Vector<Declaration> declarations;

            parseDeclarations(doc, block, declarations);

            Rule rule;

            rule.selectorCount = (u32)parsed.length();
            rule.selectors = (Selector*)doc.pool->allocate(sizeof(Selector) * parsed.length());
            memcpy(rule.selectors, parsed.data(), sizeof(Selector) * parsed.length());
            rule.declarationCount = (u32)declarations.length();
            rule.declarations = (Declaration*)doc.pool->allocate(sizeof(Declaration) * (declarations.length() + 1));

            if (rule.declarationCount) {
                memcpy(rule.declarations, declarations.data(), sizeof(Declaration) * declarations.length());
            }

            rules.pushBack(rule);
        }
    }

    static void prepareClasses(Document& doc, Node* node) {
        if (node->classesReady) {
            return;
        }

        node->classesReady = true;

        u32 value = attribute(node, AtomClass);

        if (!value) {
            return;
        }

        StringView text = textOf(doc, value);
        Vector<u32> classes;
        const u8* p = text.begin();

        while (p < text.end()) {
            skipSpaces(p, text.end());

            const u8* start = p;

            while (p < text.end() && !isSpace(*p)) {
                p++;
            }

            if (p > start) {
                classes.pushBack(intern(doc.atoms, StringView(start, p)));
            }
        }

        node->classCount = (u32)classes.length();
        node->classes = (u32*)doc.pool->allocate(sizeof(u32) * (classes.length() + 1));

        if (node->classCount) {
            memcpy(node->classes, classes.data(), sizeof(u32) * classes.length());
        }
    }

    static bool containsWord(StringView list, StringView word, bool dash) {
        if (dash) {
            return list == word || (list.length() > word.length() && memcmp(list.data(), word.data(), word.length()) == 0 && list[word.length()] == '-');
        }

        const u8* p = list.begin();

        while (p < list.end()) {
            skipSpaces(p, list.end());

            const u8* start = p;

            while (p < list.end() && !isSpace(*p)) {
                p++;
            }

            if (p > start && StringView(start, p) == word) {
                return true;
            }
        }

        return false;
    }

    static bool matchCondition(Document& doc, Node* node, const Condition& condition) {
        switch (condition.test) {
            case Test::Id:
                return attribute(node, AtomId) == condition.name;
            case Test::Class:
                prepareClasses(doc, node);

                for (u32 i = 0; i < node->classCount; i++) {
                    if (node->classes[i] == condition.name) {
                        return true;
                    }
                }

                return false;
            case Test::FirstChild:
                return !node->prev;
            case Test::LastChild:
                return !node->next;
            case Test::OnlyChild:
                return !node->prev && !node->next;
            case Test::Root:
                // An instance's nodes are never the root.
                return node == doc.root;
            case Test::Never:
                return false;
            default:
                break;
        }

        u32 value = attribute(node, condition.name);

        if (!value) {
            return false;
        }

        StringView text = textOf(doc, value);
        StringView expected = condition.value;

        switch (condition.test) {
            case Test::Exists:
                return true;
            case Test::Equals:
                return text == expected;
            case Test::Includes:
                return containsWord(text, expected, false);
            case Test::Dash:
                return containsWord(text, expected, true);
            case Test::Prefix:
                return !expected.empty() && text.length() >= expected.length() && memcmp(text.data(), expected.data(), expected.length()) == 0;
            case Test::Suffix:
                return !expected.empty() && text.length() >= expected.length() && memcmp(text.end() - expected.length(), expected.data(), expected.length()) == 0;
            default:
                // Test::Substring, the last test of a value.
                if (expected.empty() || text.length() < expected.length()) {
                    return false;
                }

                for (size_t i = 0; i + expected.length() <= text.length(); i++) {
                    if (memcmp(text.data() + i, expected.data(), expected.length()) == 0) {
                        return true;
                    }
                }

                return false;
        }
    }

    static bool matchCompound(Document& doc, Node* node, const Compound& compound, const Node* boundary) {
        if (compound.type && node->name != compound.type) {
            return false;
        }

        for (u32 i = 0; i < compound.count; i++) {
            const Condition& condition = compound.conditions[i];

            if (condition.test == Test::Not) {
                for (u32 k = 0; k < condition.innerCount; k++) {
                    if (matchCompound(doc, node, condition.inner[k], boundary)) {
                        return false;
                    }
                }

                continue;
            }

            if (!matchCondition(doc, node, condition)) {
                return false;
            }
        }

        return true;
    }

    static bool matchFrom(Document& doc, Node* node, const Selector& selector, u32 index, const Node* boundary) {
        if (!matchCompound(doc, node, selector.compounds[index], boundary)) {
            return false;
        }

        if (index + 1 == selector.count) {
            return true;
        }

        switch (selector.compounds[index].combinator) {
            case Combinator::Descendant:
                for (Node* up = node == boundary ? nullptr : node->parent; up; up = up == boundary ? nullptr : up->parent) {
                    if (matchFrom(doc, up, selector, index + 1, boundary)) {
                        return true;
                    }
                }

                return false;
            case Combinator::Child:
                return node != boundary && node->parent && matchFrom(doc, node->parent, selector, index + 1, boundary);
            case Combinator::Adjacent:
                return node != boundary && node->prev && matchFrom(doc, node->prev, selector, index + 1, boundary);
            default:
                // Combinator::Sibling: a compound before the last has a combinator.
                for (Node* sibling = node == boundary ? nullptr : node->prev; sibling; sibling = sibling->prev) {
                    if (matchFrom(doc, sibling, selector, index + 1, boundary)) {
                        return true;
                    }
                }

                return false;
        }
    }

    static void setAttribute(Document& doc, Node* node, u32 name, u32 value) {
        for (u32 i = 0; i < node->count; i++) {
            if (node->attributes[i].name == name) {
                node->attributes[i].value = value;

                return;
            }
        }

        Attribute* attributes = (Attribute*)doc.pool->allocate(sizeof(Attribute) * (node->count + 1));

        if (node->count) {
            memcpy(attributes, node->attributes, sizeof(Attribute) * node->count);
        }

        attributes[node->count] = Attribute{name, value};
        node->attributes = attributes;
        node->count++;
    }

    static void considerGeometry(Vector<Declaration>& geometry, Vector<u64>& priorities, const Declaration& declaration, u64 priority) {
        for (size_t i = 0; i < geometry.length(); i++) {
            if (geometry[i].property == declaration.property) {
                if (priority >= priorities[i]) {
                    geometry.mut(i) = declaration;
                    priorities.mut(i) = priority;
                }

                return;
            }
        }

        geometry.pushBack(declaration);
        priorities.pushBack(priority);
    }

    // A declaration of a property the cascade computes.
    static void consider(Candidate* best, u32 property, u32 value, u64 priority) {
        Candidate& slot = best[property - firstProperty];

        if (priority >= slot.priority) {
            slot.value = value;
            slot.priority = priority;
        }
    }

    static bool parsePaint(const Document& doc, StringView source, PaintValue& out);
    static bool parseColor(StringView source, Color& out);
    static bool readLength(const u8*& p, const u8* end, Length& out);
    static bool parseLength(StringView text, Length& out);
    static bool readNumber(const u8*& p, const u8* end, float& out);

    static bool validDashes(StringView text) {
        const u8* p = text.begin();
        const u8* end = text.end();
        u32 count = 0;

        // The text is trimmed, and spaces after a length are skipped with it.
        for (;;) {
            Length length;

            if (p >= end) {
                return count > 0;
            }

            if (!readLength(p, end, length) || length.value < 0.f) {
                return false;
            }

            count++;

            skipSpaces(p, end);

            if (at(p, end, ',')) {
                p++;

                skipSpaces(p, end);

                if (p >= end || *p == ',') {
                    return false;
                }
            }
        }
    }

    static bool validDeclaration(const Document& doc, u32 property, StringView value) {
        StringView text = trim(value);
        PaintValue paint;
        Color color;
        Length length;

        if (equalsIgnoreCase(text, SV("inherit")) || equalsIgnoreCase(text, SV("initial")) || equalsIgnoreCase(text, SV("unset"))) {
            return true;
        }

        switch (property) {
            case AtomFill:
            case AtomStroke:
                return parsePaint(doc, text, paint) || equalsIgnoreCase(text, SV("context-fill")) || equalsIgnoreCase(text, SV("context-stroke"));
            case AtomColor:
            case AtomStopColor:
            case AtomFloodColor:
            case AtomLightingColor:
                return parseColor(text, color) || equalsIgnoreCase(text, SV("currentcolor"));
            case AtomStrokeDasharray:
                return equalsIgnoreCase(text, SV("none")) || validDashes(text);
            case AtomStrokeWidth:
                return parseLength(text, length) && length.value >= 0.f;
            case AtomShapeRendering:
                return equalsIgnoreCase(text, SV("auto")) || equalsIgnoreCase(text, SV("optimizespeed")) || equalsIgnoreCase(text, SV("crispedges")) || equalsIgnoreCase(text, SV("geometricprecision"));
            default:
                return true;
        }
    }

    static u32* cascadeNode(Document& doc, Node* node, const Node* boundary) {
        Candidate best[propertyCount];
        Vector<Declaration> geometry;
        Vector<u64> geometryPriorities;

        memset(best, 0, sizeof(best));

        for (u32 i = 0; i < node->count; i++) {
            u32 name = node->attributes[i].name;

            if (name >= firstProperty && name < firstProperty + propertyCount && name != AtomMixBlendMode && name != AtomIsolation && name != AtomOutline && name != AtomZoom) {
                u32 value = node->attributes[i].value;
                StringView text = textOf(doc, value);

                if (findFunction(text, SV("env("))) {
                    Buffer substituted;

                    value = substituteFallbacks(text, substituted, 0) ? intern(doc.atoms, trim(keep(*doc.pool, substituted.data(), substituted.length()))) : 0;
                }

                if (value && validDeclaration(doc, name, textOf(doc, value))) {
                    consider(best, name, value, 1);
                }
            }
        }

        for (u32 r = 0; r < doc.ruleCount; r++) {
            const Rule& rule = doc.rules[r];
            u32 specificity = 0;
            bool matched = false;

            for (u32 s = 0; s < rule.selectorCount; s++) {
                if (matchFrom(doc, node, rule.selectors[s], 0, boundary)) {
                    matched = true;

                    if (rule.selectors[s].specificity > specificity) {
                        specificity = rule.selectors[s].specificity;
                    }
                }
            }

            if (!matched) {
                continue;
            }

            for (u32 d = 0; d < rule.declarationCount; d++) {
                const Declaration& declaration = rule.declarations[d];
                u64 level = declaration.important ? 3 : 1;

                // Declarations come in source order and a later one wins a tie,
                // so the priority carries no position that could overflow.
                u64 priority = (level << 56) | ((u64)specificity << 32);

                if (geometryProperty(declaration.property)) {
                    considerGeometry(geometry, geometryPriorities, declaration, priority);
                } else if (validDeclaration(doc, declaration.property, textOf(doc, declaration.value))) {
                    consider(best, declaration.property, declaration.value, priority);
                }
            }
        }

        if (u32 inlineStyle = attribute(node, AtomStyle)) {
            Vector<Declaration> declarations;
            Buffer cleaned;

            stripComments(textOf(doc, inlineStyle), cleaned);
            parseDeclarations(doc, keep(*doc.pool, cleaned.data(), cleaned.length()), declarations);

            for (u32 d = 0; d < declarations.length(); d++) {
                u64 level = declarations[d].important ? 4 : 2;
                u64 priority = level << 56;

                if (geometryProperty(declarations[d].property)) {
                    considerGeometry(geometry, geometryPriorities, declarations[d], priority);
                } else if (validDeclaration(doc, declarations[d].property, textOf(doc, declarations[d].value))) {
                    consider(best, declarations[d].property, declarations[d].value, priority);
                }
            }
        }

        for (const Declaration& declaration : geometry) {
            if (boundary) {
                break;
            }

            bool outer = node == doc.root && declaration.property != AtomTransform && declaration.property != AtomTransformOrigin;

            if ((geometryApplies(node->name, declaration.property) || declaration.property == AtomD) && !outer) {
                u32 property = declaration.property;

                if (property == AtomTransform && (node->name == AtomLinearGradient || node->name == AtomRadialGradient)) {
                    property = AtomGradientTransform;
                } else if (property == AtomTransform && node->name == AtomPattern) {
                    property = AtomPatternTransform;
                }

                setAttribute(doc, node, property, declaration.value);
            }
        }

        u32* style = (u32*)doc.pool->allocate(sizeof(u32) * propertyCount);

        for (u32 i = 0; i < propertyCount; i++) {
            style[i] = best[i].value;
        }

        return style;
    }

    static void cascade(Document& doc) {
        Vector<Rule> rules;

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            if (node->name == AtomStyle && !node->text.empty()) {
                u32 type = attribute(node, AtomType);

                if (!type || textOf(doc, type).empty() || equalsIgnoreCase(trim(textOf(doc, type)), SV("text/css"))) {
                    parseSheet(doc, rules, node->text);
                }
            }
        }

        doc.ruleCount = (u32)rules.length();
        doc.rules = (Rule*)doc.pool->allocate(sizeof(Rule) * (rules.length() + 1));
        doc.structural = false;

        if (rules.length()) {
            memcpy(doc.rules, rules.data(), sizeof(Rule) * rules.length());
        }

        for (u32 r = 0; r < doc.ruleCount; r++) {
            for (u32 k = 0; k < doc.rules[r].selectorCount; k++) {
                const Selector& selector = doc.rules[r].selectors[k];

                doc.structural = doc.structural || selector.count > 1;

                for (u32 c = 0; c < selector.count; c++) {
                    for (u32 d = 0; d < selector.compounds[c].count; d++) {
                        Test test = selector.compounds[c].conditions[d].test;

                        doc.structural = doc.structural || test == Test::Root || test == Test::FirstChild || test == Test::LastChild || test == Test::OnlyChild || test == Test::Not;
                    }
                }
            }
        }

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            node->style = cascadeNode(doc, node, nullptr);
        }
    }

    static bool readNumber(const u8*& p, const u8* end, float& out) {
        const u8* s = p;
        double sign = 1.0;
        double value = 0.0;
        bool digits = false;

        if (s < end && (*s == '+' || *s == '-')) {
            sign = *s == '-' ? -1.0 : 1.0;
            s++;
        }

        while (s < end && *s >= '0' && *s <= '9') {
            value = value * 10.0 + (*s - '0');
            digits = true;
            s++;
        }

        if (at(s, end, '.')) {
            double scale = 0.1;

            s++;

            while (s < end && *s >= '0' && *s <= '9') {
                value += (*s - '0') * scale;
                scale *= 0.1;
                digits = true;
                s++;
            }
        }

        if (!digits) {
            return false;
        }

        if (s < end && (*s == 'e' || *s == 'E')) {
            const u8* e = s + 1;
            int exponentSign = 1;
            int exponent = 0;
            bool exponentDigits = false;

            if (e < end && (*e == '+' || *e == '-')) {
                exponentSign = *e == '-' ? -1 : 1;
                e++;
            }

            while (e < end && *e >= '0' && *e <= '9') {
                exponent = exponent < 10000 ? exponent * 10 + (*e - '0') : exponent;
                exponentDigits = true;
                e++;
            }

            if (exponentDigits) {
                value *= pow(10.0, exponentSign * exponent);
                s = e;
            }
        }

        double result = sign * value;

        if (!isfinite(result) || fabs(result) > 3.0e38) {
            return false;
        }

        out = (float)result;
        p = s;

        return true;
    }

    static void skipSeparator(const u8*& p, const u8* end) {
        skipSpaces(p, end);

        if (at(p, end, ',')) {
            p++;

            skipSpaces(p, end);
        }
    }

    static bool parseNumberText(StringView text, float& out) {
        StringView body = trim(text);
        const u8* p = body.begin();

        return readNumber(p, body.end(), out) && p == body.end();
    }

    static bool readLength(const u8*& p, const u8* end, Length& out) {
        const u8* s = p;

        if (!readNumber(s, end, out.value)) {
            return false;
        }

        out.unit = Unit::Number;

        if (at(s, end, '%')) {
            out.unit = Unit::Percent;
            s++;
        } else if (s < end && isIdentStart(*s) && *s != '-') {
            const u8* unitEnd = s;

            while (unitEnd < end && ((*unitEnd >= 'a' && *unitEnd <= 'z') || (*unitEnd >= 'A' && *unitEnd <= 'Z'))) {
                unitEnd++;
            }

            StringView unit(s, unitEnd);

            static const struct {
                const char* name;
                Unit unit;
            } units[] = {
                {"px", Unit::Px},
                {"em", Unit::Em},
                {"ex", Unit::Ex},
                {"in", Unit::In},
                {"cm", Unit::Cm},
                {"mm", Unit::Mm},
                {"pt", Unit::Pt},
                {"pc", Unit::Pc},
                {"q", Unit::Q},
                {"rem", Unit::Rem},
                {"vw", Unit::Vw},
                {"vh", Unit::Vh},
                {"vi", Unit::Vw},
                {"vb", Unit::Vh},
                {"vmin", Unit::Vmin},
                {"vmax", Unit::Vmax},
                {"ch", Unit::Ch},
                {"lh", Unit::Lh},
                {"rlh", Unit::Lh},
                {"ic", Unit::Ic},
            };

            bool known = false;

            for (const auto& candidate : units) {
                if (equalsIgnoreCase(unit, StringView(candidate.name))) {
                    out.unit = candidate.unit;
                    known = true;
                    break;
                }
            }

            if (!known || (unitEnd < end && isIdentChar(*unitEnd))) {
                return false;
            }

            s = unitEnd;
        }

        p = s;

        return true;
    }

    static bool parseLength(StringView text, Length& out) {
        StringView body = trim(text);
        const u8* p = body.begin();

        return readLength(p, body.end(), out) && p == body.end();
    }

    static float resolveLength(Length length, Axis axis, Viewport vp, float fontSize) {
        switch (length.unit) {
            case Unit::Number:
            case Unit::Px:
                return length.value;
            case Unit::Em:
                return length.value * fontSize;
            case Unit::Ex:
                return length.value * fontSize * 0.459f;
            case Unit::In:
                return length.value * 96.f;
            case Unit::Cm:
                return length.value * 96.f / 2.54f;
            case Unit::Mm:
                return length.value * 96.f / 25.4f;
            case Unit::Pt:
                return length.value * 4.f / 3.f;
            case Unit::Pc:
                return length.value * 16.f;
            case Unit::Q:
                return length.value * 96.f / 101.6f;
            case Unit::Rem:
                return length.value * (vp.rootFontSize > 0.f ? vp.rootFontSize : 16.f);
            case Unit::Vw:
                return length.value * vp.screenWidth / 100.f;
            case Unit::Vh:
                return length.value * vp.screenHeight / 100.f;
            case Unit::Vmin:
                return length.value * minf(vp.screenWidth, vp.screenHeight) / 100.f;
            case Unit::Vmax:
                return length.value * maxf(vp.screenWidth, vp.screenHeight) / 100.f;
            case Unit::Ch:
                return length.value * fontSize * 0.5f;
            case Unit::Lh:
                return length.value * fontSize * 1.149f;
            case Unit::Ic:
                return length.value * fontSize;
            default:
                // Unit::Percent.
                if (axis == Axis::X) {
                    return length.value * vp.width / 100.f;
                }

                if (axis == Axis::Y) {
                    return length.value * vp.height / 100.f;
                }

                return length.value * sqrtf((vp.width * vp.width + vp.height * vp.height) / 2.f) / 100.f;
        }
    }

    static bool lengthAttribute(const Document& doc, const Node* node, u32 name, Axis axis, Viewport vp, float fontSize, float& out) {
        u32 value = attribute(node, name);
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return false;
        }

        out = resolveLength(length, axis, vp, fontSize);

        return true;
    }

    static float lengthOr(const Document& doc, const Node* node, u32 name, Axis axis, Viewport vp, float fontSize, float fallback) {
        float value;

        return lengthAttribute(doc, node, name, axis, vp, fontSize, value) ? value : fallback;
    }

    static int hexDigit(u8 c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }

        c = lowerAscii(c);

        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }

        return -1;
    }

    // A component of a colour function, where it starts.
    static bool readComponent(const u8*& p, const u8* end, float scale, float& out) {
        float value;

        if (!readNumber(p, end, value)) {
            return false;
        }

        if (at(p, end, '%')) {
            p++;
            out = value / 100.f;
        } else {
            out = value / scale;
        }

        return true;
    }

    static float hueToRgb(float m1, float m2, float h) {
        if (h < 0.f) {
            h += 1.f;
        }

        if (h > 1.f) {
            h -= 1.f;
        }

        if (h * 6.f < 1.f) {
            return m1 + (m2 - m1) * h * 6.f;
        }

        if (h * 2.f < 1.f) {
            return m2;
        }

        if (h * 3.f < 2.f) {
            return m1 + (m2 - m1) * (2.f / 3.f - h) * 6.f;
        }

        return m1;
    }

    static bool parseFunctionColor(StringView text, Color& out) {
        const u8* open = text.begin();

        while (open < text.end() && *open != '(') {
            open++;
        }

        // parseColor sends the texts that end with ')'.
        if (open >= text.end()) {
            return false;
        }

        StringView name = trim(StringView(text.begin(), open));
        bool hsl = equalsIgnoreCase(name, SV("hsl")) || equalsIgnoreCase(name, SV("hsla"));
        bool rgb = equalsIgnoreCase(name, SV("rgb")) || equalsIgnoreCase(name, SV("rgba"));

        if (!hsl && !rgb) {
            return false;
        }

        const u8* p = open + 1;
        const u8* end = text.end() - 1;
        float values[4] = {0.f, 0.f, 0.f, 1.f};
        u32 count = 0;
        u32 percents = 0;

        while (count < 4) {
            skipSpaces(p, end);

            if (p >= end) {
                break;
            }

            if (count > 0 && (*p == ',' || *p == '/')) {
                p++;
            }

            float scale = 255.f;

            if (hsl) {
                scale = count == 0 ? 360.f : 100.f;
            }

            if (count == 3) {
                scale = 1.f;
            }

            if (hsl && count == 0) {
                float hue;

                if (!readNumber(p, end, hue)) {
                    return false;
                }

                if (end - p >= 3 && equalsIgnoreCase(StringView(p, p + 3), SV("deg"))) {
                    p += 3;
                } else if (end - p >= 4 && equalsIgnoreCase(StringView(p, p + 4), SV("turn"))) {
                    p += 4;
                    hue *= 360.f;
                } else if (end - p >= 3 && equalsIgnoreCase(StringView(p, p + 3), SV("rad"))) {
                    p += 3;
                    hue *= 180.f / pi;
                }

                values[0] = hue / 360.f;
            } else {
                skipSpaces(p, end);

                if (!readComponent(p, end, scale, values[count])) {
                    return false;
                }

                if (rgb && count < 3 && p[-1] == '%') {
                    percents++;
                }
            }

            count++;
        }

        skipSpaces(p, end);

        if (p != end || count < 3 || (percents != 0 && percents != 3)) {
            return false;
        }

        if (hsl) {
            float h = values[0] - floorf(values[0]);
            float s = clampUnit(values[1]);
            float l = clampUnit(values[2]);
            float m2 = l <= 0.5f ? l * (s + 1.f) : l + s - l * s;
            float m1 = l * 2.f - m2;

            out = Color{hueToRgb(m1, m2, h + 1.f / 3.f), hueToRgb(m1, m2, h), hueToRgb(m1, m2, h - 1.f / 3.f), clampUnit(values[3])};
        } else {
            out = Color{clampUnit(values[0]), clampUnit(values[1]), clampUnit(values[2]), clampUnit(values[3])};
        }

        return true;
    }

    static bool parseColor(StringView source, Color& out) {
        StringView text = trim(source);

        if (text.empty()) {
            return false;
        }

        if (text[0] == '#') {
            size_t length = text.length() - 1;
            int digits[8];

            if (length != 3 && length != 4 && length != 6 && length != 8) {
                return false;
            }

            for (size_t i = 0; i < length; i++) {
                digits[i] = hexDigit(text[i + 1]);

                if (digits[i] < 0) {
                    return false;
                }
            }

            if (length <= 4) {
                out = Color{digits[0] * 17 / 255.f, digits[1] * 17 / 255.f, digits[2] * 17 / 255.f, length == 4 ? digits[3] * 17 / 255.f : 1.f};
            } else {
                out = Color{(digits[0] * 16 + digits[1]) / 255.f, (digits[2] * 16 + digits[3]) / 255.f, (digits[4] * 16 + digits[5]) / 255.f, length == 8 ? (digits[6] * 16 + digits[7]) / 255.f : 1.f};
            }

            return true;
        }

        if (equalsIgnoreCase(text, SV("transparent"))) {
            out = Color{0.f, 0.f, 0.f, 0.f};

            return true;
        }

        if (text.back() == ')') {
            return parseFunctionColor(text, out);
        }

        for (const NamedColor& named : namedColors) {
            if (equalsIgnoreCase(text, StringView(named.name))) {
                out = Color{((named.rgb >> 16) & 0xff) / 255.f, ((named.rgb >> 8) & 0xff) / 255.f, (named.rgb & 0xff) / 255.f, 1.f};

                return true;
            }
        }

        for (const NamedColor& named : systemColors) {
            if (equalsIgnoreCase(text, StringView(named.name))) {
                out = Color{((named.rgb >> 24) & 0xff) / 255.f, ((named.rgb >> 16) & 0xff) / 255.f, ((named.rgb >> 8) & 0xff) / 255.f, (named.rgb & 0xff) / 255.f};

                return true;
            }
        }

        return false;
    }

    static bool parsePaint(const Document& doc, StringView source, PaintValue& out) {
        StringView text = trim(source);

        out.server = 0;
        out.fallback = PaintKind::None;
        out.context = false;

        if (equalsIgnoreCase(text, SV("none"))) {
            out.kind = PaintKind::None;

            return true;
        }

        if (equalsIgnoreCase(text, SV("currentcolor"))) {
            out.kind = PaintKind::CurrentColor;

            return true;
        }

        if (startsWithIgnoreCase(text, SV("url("))) {
            const u8* close = text.begin() + 4;

            skipSpaces(close, text.end());

            if (close < text.end() && (*close == '"' || *close == '\'')) {
                u8 quote = *close++;

                while (close < text.end() && *close != quote) {
                    close++;
                }
            }

            while (close < text.end() && *close != ')') {
                close++;
            }

            StringView reference = trim(StringView(text.begin() + 4, close));

            if (!reference.empty() && (reference[0] == '"' || reference[0] == '\'')) {
                reference = StringView(reference.begin() + 1, reference.length() >= 2 && reference.back() == reference[0] ? reference.end() - 1 : reference.end());
            }

            if (close >= text.end()) {
                close = text.end() - 1;
            }

            out.kind = PaintKind::Server;

            if (!reference.empty() && reference[0] == '#') {
                out.server = fragmentAtom(doc, StringView(reference.begin() + 1, reference.end()));
            }

            StringView fallback = trim(StringView(close + 1, text.end()));

            if (!fallback.empty()) {
                if (equalsIgnoreCase(fallback, SV("none"))) {
                    out.fallback = PaintKind::None;
                } else if (equalsIgnoreCase(fallback, SV("currentcolor"))) {
                    out.fallback = PaintKind::CurrentColor;
                } else if (parseColor(fallback, out.fallbackColor)) {
                    out.fallback = PaintKind::Color;
                } else {
                    return false;
                }
            }

            return true;
        }

        if (parseColor(text, out.color)) {
            out.kind = PaintKind::Color;

            return true;
        }

        return false;
    }

    static bool parseTransform(StringView text, Matrix& out) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Matrix result = identity();

        if (equalsIgnoreCase(trim(text), SV("none"))) {
            out = result;

            return true;
        }

        bool first = true;

        for (;;) {
            bool separator = false;

            skipSpaces(p, end);

            if (at(p, end, ',')) {
                if (first) {
                    return false;
                }

                separator = true;
                p++;

                skipSpaces(p, end);
            }

            if (p >= end) {
                if (separator) {
                    return false;
                }

                break;
            }

            first = false;

            const u8* start = p;

            while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
                p++;
            }

            StringView name(start, p);

            skipSpaces(p, end);

            if (p >= end || *p != '(') {
                return false;
            }

            p++;

            float args[6];
            u32 count = 0;

            for (;;) {
                skipSpaces(p, end);

                if (at(p, end, ')')) {
                    p++;
                    break;
                }

                if (count == 6 || !readNumber(p, end, args[count])) {
                    return false;
                }

                bool comma = false;

                const u8* unitStart = p;

                while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
                    p++;
                }

                StringView unit(unitStart, p);

                if (equalsIgnoreCase(unit, SV("rad"))) {
                    args[count] *= 180.f / pi;
                } else if (equalsIgnoreCase(unit, SV("grad"))) {
                    args[count] *= 0.9f;
                } else if (equalsIgnoreCase(unit, SV("turn"))) {
                    args[count] *= 360.f;
                } else if (!unit.empty() && !equalsIgnoreCase(unit, SV("deg")) && !equalsIgnoreCase(unit, SV("px"))) {
                    return false;
                }

                count++;

                skipSpaces(p, end);

                if (at(p, end, ',')) {
                    comma = true;
                    p++;
                }

                skipSpaces(p, end);

                if (comma && (p >= end || *p == ')' || *p == ',')) {
                    return false;
                }
            }

            Matrix m;

            if (name == SV("translateX") && count == 1) {
                name = SV("translate");
            } else if (name == SV("translateY") && count == 1) {
                name = SV("translate");
                args[1] = args[0];
                args[0] = 0.f;
                count = 2;
            } else if (name == SV("scaleX") && count == 1) {
                name = SV("scale");
                args[1] = 1.f;
                count = 2;
            } else if (name == SV("scaleY") && count == 1) {
                name = SV("scale");
                args[1] = args[0];
                args[0] = 1.f;
                count = 2;
            } else if (name == SV("skew") && (count == 1 || count == 2)) {
                float ax = tanf(args[0] * pi / 180.f);
                float ay = count == 2 ? tanf(args[1] * pi / 180.f) : 0.f;

                result = multiply(result, Matrix{1.f, ay, ax, 1.f, 0.f, 0.f});
                continue;
            }

            if (name == SV("matrix") && count == 6) {
                m = Matrix{args[0], args[1], args[2], args[3], args[4], args[5]};
            } else if (name == SV("translate") && (count == 1 || count == 2)) {
                m = translation(args[0], count == 2 ? args[1] : 0.f);
            } else if (name == SV("scale") && (count == 1 || count == 2)) {
                m = scaling(args[0], count == 2 ? args[1] : args[0]);
            } else if (name == SV("rotate") && (count == 1 || count == 3)) {
                float angle = args[0] * pi / 180.f;
                float cs = cosf(angle);
                float sn = sinf(angle);

                m = Matrix{cs, sn, -sn, cs, 0.f, 0.f};

                if (count == 3) {
                    m = multiply(multiply(translation(args[1], args[2]), m), translation(-args[1], -args[2]));
                }
            } else if (name == SV("skewX") && count == 1) {
                m = Matrix{1.f, 0.f, tanf(args[0] * pi / 180.f), 1.f, 0.f, 0.f};
            } else if (name == SV("skewY") && count == 1) {
                m = Matrix{1.f, tanf(args[0] * pi / 180.f), 0.f, 1.f, 0.f, 0.f};
            } else {
                return false;
            }

            result = multiply(result, m);
        }

        out = result;

        return true;
    }

    static bool readOrigin(const u8*& p, const u8* end, float extentX, float extentY, float& x, float& y, int& axis) {
        skipSpaces(p, end);

        if (p >= end) {
            return false;
        }

        static const struct {
            const char* name;
            int axis;
            float fraction;
        } keywords[] = {
            {"left", 0, 0.f},
            {"right", 0, 1.f},
            {"top", 1, 0.f},
            {"bottom", 1, 1.f},
            {"center", 2, 0.5f},
        };

        for (const auto& keyword : keywords) {
            size_t n = strlen(keyword.name);

            if ((size_t)(end - p) >= n && equalsIgnoreCase(StringView(p, p + n), StringView(keyword.name))) {
                p += n;
                axis = keyword.axis;
                x = keyword.fraction * extentX;
                y = keyword.fraction * extentY;

                return true;
            }
        }

        Length length;

        if (!readLength(p, end, length)) {
            return false;
        }

        axis = 3;
        x = length.unit == Unit::Percent ? length.value * extentX / 100.f : resolveLength(length, Axis::X, Viewport{extentX, extentY, extentX, extentY, 16.f}, 16.f);
        y = length.unit == Unit::Percent ? length.value * extentY / 100.f : x;

        return true;
    }

    static Point transformOrigin(StringView text, float extentX, float extentY) {
        const u8* p = text.begin();
        const u8* end = text.end();
        float x1;
        float y1;
        float x2;
        float y2;
        int a1;
        int a2;

        if (!readOrigin(p, end, extentX, extentY, x1, y1, a1)) {
            return Point{0.f, 0.f};
        }

        if (!readOrigin(p, end, extentX, extentY, x2, y2, a2)) {
            if (a1 == 1) {
                return Point{extentX / 2.f, y1};
            }

            return Point{x1, extentY / 2.f};
        }

        if (a1 == 1 || a2 == 0) {
            return Point{x2, y1};
        }

        return Point{x1, y2};
    }

    static Matrix originTransform(const Document& doc, const Node* node, u32 name, Viewport vp) {
        u32 value = attribute(node, name);
        Matrix m;

        if (!value || !parseTransform(textOf(doc, value), m)) {
            return identity();
        }

        u32 origin = attribute(node, AtomTransformOrigin);

        if (!origin && node != doc.root) {
            return m;
        }

        Point o = origin ? transformOrigin(textOf(doc, origin), vp.width, vp.height) : Point{vp.width / 2.f, vp.height / 2.f};

        return multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
    }

    static Matrix transformOf(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);
        Matrix m;

        if (value && parseTransform(textOf(doc, value), m)) {
            return m;
        }

        return identity();
    }

    static bool parseViewBox(const Document& doc, const Node* node, Box& out) {
        u32 value = attribute(node, AtomViewBox);

        if (!value) {
            return false;
        }

        StringView text = textOf(doc, value);
        const u8* p = text.begin();
        const u8* end = text.end();
        float numbers[4];

        for (u32 i = 0; i < 4; i++) {
            skipSpaces(p, end);

            if (!readNumber(p, end, numbers[i])) {
                return false;
            }

            if (i < 3) {
                skipSeparator(p, end);
            }
        }

        skipSpaces(p, end);

        if (p != end || numbers[2] < 0.f || numbers[3] < 0.f) {
            return false;
        }

        out = Box{numbers[0], numbers[1], numbers[0] + numbers[2], numbers[1] + numbers[3]};

        return true;
    }

    static Matrix viewBoxTransform(const Document& doc, const Node* node, const Box& viewBox, float width, float height) {
        float vw = viewBox.x1 - viewBox.x0;
        float vh = viewBox.y1 - viewBox.y0;
        float sx = width / vw;
        float sy = height / vh;
        int alignX = 1;
        int alignY = 1;
        bool slice = false;
        bool none = false;
        u32 value = attribute(node, AtomPreserveAspectRatio);

        if (value) {
            StringView text = trim(textOf(doc, value));
            const u8* p = text.begin();
            const u8* end = text.end();
            const u8* start = p;

            while (p < end && !isSpace(*p)) {
                p++;
            }

            StringView align(start, p);

            if (align == SV("defer")) {
                skipSpaces(p, end);

                start = p;

                while (p < end && !isSpace(*p)) {
                    p++;
                }

                align = StringView(start, p);
            }

            if (align == SV("none")) {
                none = true;
            } else if (align.length() == 8 && align[0] == 'x' && align[4] == 'Y') {
                StringView ax(align.begin() + 1, align.begin() + 4);
                StringView ay(align.begin() + 5, align.end());

                alignX = ax == SV("Min") ? 0 : (ax == SV("Mid") ? 1 : 2);
                alignY = ay == SV("Min") ? 0 : (ay == SV("Mid") ? 1 : 2);
            }

            skipSpaces(p, end);

            slice = StringView(p, end) == SV("slice");
        }

        if (none) {
            return multiply(scaling(sx, sy), translation(-viewBox.x0, -viewBox.y0));
        }

        float s = slice ? maxf(sx, sy) : minf(sx, sy);
        float tx = -viewBox.x0 * s;
        float ty = -viewBox.y0 * s;

        tx += (width - vw * s) * alignX * 0.5f;
        ty += (height - vh * s) * alignY * 0.5f;

        return Matrix{s, 0.f, 0.f, s, tx, ty};
    }

    static void moveTo(Path& path, Point p) {
        path.verbs.pushBack(VerbMove);
        path.points.pushBack(p);
    }

    static void lineTo(Path& path, Point p) {
        path.verbs.pushBack(VerbLine);
        path.points.pushBack(p);
    }

    static void cubicTo(Path& path, Point c1, Point c2, Point p) {
        path.verbs.pushBack(VerbCubic);
        path.points.pushBack(c1);
        path.points.pushBack(c2);
        path.points.pushBack(p);
    }

    static void closePath(Path& path) {
        path.verbs.pushBack(VerbClose);
    }

    static void arcTo(Path& path, Point p0, float rxIn, float ryIn, float angle, bool large, bool sweep, Point p1) {
        if (p0.x == p1.x && p0.y == p1.y) {
            lineTo(path, p1);

            return;
        }

        double rx = fabs((double)rxIn);
        double ry = fabs((double)ryIn);

        if (rx == 0.0 || ry == 0.0) {
            lineTo(path, p1);

            return;
        }

        double phi = fmod((double)angle, 360.0) * piDouble / 180.0;
        double cs = cos(phi);
        double sn = sin(phi);
        double dx2 = (p0.x - p1.x) / 2.0;
        double dy2 = (p0.y - p1.y) / 2.0;
        double x1p = cs * dx2 + sn * dy2;
        double y1p = -sn * dx2 + cs * dy2;
        double lambda = (x1p * x1p) / (rx * rx) + (y1p * y1p) / (ry * ry);

        if (lambda > 1.0) {
            rx *= sqrt(lambda);
            ry *= sqrt(lambda);
        }

        double num = rx * rx * ry * ry - rx * rx * y1p * y1p - ry * ry * x1p * x1p;
        double den = rx * rx * y1p * y1p + ry * ry * x1p * x1p;
        double coef = den > 0.0 ? sqrt(num / den > 0.0 ? num / den : 0.0) : 0.0;

        if (large == sweep) {
            coef = -coef;
        }

        double cxp = coef * rx * y1p / ry;
        double cyp = -coef * ry * x1p / rx;
        double cx = cs * cxp - sn * cyp + (p0.x + p1.x) / 2.0;
        double cy = sn * cxp + cs * cyp + (p0.y + p1.y) / 2.0;
        double ux = (x1p - cxp) / rx;
        double uy = (y1p - cyp) / ry;
        double vx = (-x1p - cxp) / rx;
        double vy = (-y1p - cyp) / ry;
        double theta = atan2(uy, ux);
        double delta = atan2(ux * vy - uy * vx, ux * vx + uy * vy);

        if (!sweep && delta > 0.0) {
            delta -= 2.0 * piDouble;
        } else if (sweep && delta < 0.0) {
            delta += 2.0 * piDouble;
        }

        // Ends that are not finite leave the arc no angle: it is a line.
        if (delta != delta) {
            lineTo(path, p1);

            return;
        }

        int segments = (int)ceil(fabs(delta) / (piDouble / 2.0) - 1e-7);

        if (segments < 1) {
            segments = 1;
        }

        double step = delta / segments;
        double k = 4.0 / 3.0 * tan(step / 4.0);

        for (int i = 0; i < segments; i++) {
            double a0 = theta + step * i;
            double a1 = a0 + step;
            double c0 = cos(a0);
            double s0 = sin(a0);
            double c1 = cos(a1);
            double s1 = sin(a1);
            double e1x = c0 - k * s0;
            double e1y = s0 + k * c0;
            double e2x = c1 + k * s1;
            double e2y = s1 - k * c1;
            Point q1{(float)(cx + rx * cs * e1x - ry * sn * e1y), (float)(cy + rx * sn * e1x + ry * cs * e1y)};
            Point q2{(float)(cx + rx * cs * e2x - ry * sn * e2y), (float)(cy + rx * sn * e2x + ry * cs * e2y)};
            Point q3{(float)(cx + rx * cs * c1 - ry * sn * s1), (float)(cy + rx * sn * c1 + ry * cs * s1)};

            if (i + 1 == segments) {
                q3 = p1;
            }

            cubicTo(path, q1, q2, q3);

            if (i + 1 < segments) {
                path.verbs.mutData()[path.verbs.length() - 1] = VerbArcCubic;
            }
        }
    }

    // The flag of an arc, where its argument starts.
    static bool readFlag(const u8*& p, const u8* end, bool& out) {
        if (p >= end || (*p != '0' && *p != '1')) {
            return false;
        }

        out = *p == '1';
        p++;
        skipSeparator(p, end);

        return true;
    }

    static void parsePathData(StringView text, Path& path) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Point current{0.f, 0.f};
        Point start{0.f, 0.f};
        Point control{0.f, 0.f};
        u8 command = 0;
        u8 previous = 0;
        bool needMove = false;

        for (;;) {
            skipSpaces(p, end);

            if (p >= end) {
                break;
            }

            u8 c = *p;

            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                command = c;
                p++;
            } else if (command == 0 || command == 'z' || command == 'Z') {
                break;
            } else if (command == 'M') {
                command = 'L';
            } else if (command == 'm') {
                command = 'l';
            }

            if (path.verbs.length() == 0 && command != 'M' && command != 'm') {
                break;
            }

            bool relative = command >= 'a';
            u8 upper = relative ? (u8)(command - 32) : command;
            Point base = relative ? current : Point{0.f, 0.f};

            if (upper == 'Z') {
                closePath(path);
                current = start;
                needMove = true;
                previous = upper;
                skipSeparator(p, end);
                continue;
            }

            if (needMove && upper != 'M') {
                moveTo(path, current);
            }

            needMove = false;

            float v[7];
            u32 count;

            switch (upper) {
                case 'M':
                case 'L':
                case 'T':
                    count = 2;
                    break;
                case 'H':
                case 'V':
                    count = 1;
                    break;
                case 'C':
                    count = 6;
                    break;
                case 'S':
                case 'Q':
                    count = 4;
                    break;
                case 'A':
                    count = 7;
                    break;
                default:
                    return;
            }

            bool ok = true;
            bool largeArc = false;
            bool sweepArc = false;

            for (u32 i = 0; i < count && ok; i++) {
                skipSpaces(p, end);

                if (upper == 'A' && i == 3) {
                    ok = readFlag(p, end, largeArc);
                } else if (upper == 'A' && i == 4) {
                    ok = readFlag(p, end, sweepArc);
                } else {
                    ok = readNumber(p, end, v[i]);

                    if (ok) {
                        skipSeparator(p, end);
                    }
                }
            }

            if (!ok) {
                return;
            }

            switch (upper) {
                case 'M':
                    current = Point{base.x + v[0], base.y + v[1]};
                    start = current;
                    moveTo(path, current);
                    break;
                case 'L':
                    current = Point{base.x + v[0], base.y + v[1]};
                    lineTo(path, current);
                    break;
                case 'H':
                    current = Point{(relative ? current.x : 0.f) + v[0], current.y};
                    lineTo(path, current);
                    break;
                case 'V':
                    current = Point{current.x, (relative ? current.y : 0.f) + v[0]};
                    lineTo(path, current);
                    break;
                case 'C': {
                    Point c1{base.x + v[0], base.y + v[1]};
                    Point c2{base.x + v[2], base.y + v[3]};
                    Point to{base.x + v[4], base.y + v[5]};

                    cubicTo(path, c1, c2, to);
                    control = c2;
                    current = to;
                } break;
                case 'S': {
                    Point c1 = previous == 'C' || previous == 'S' ? Point{2.f * current.x - control.x, 2.f * current.y - control.y} : current;
                    Point c2{base.x + v[0], base.y + v[1]};
                    Point to{base.x + v[2], base.y + v[3]};

                    cubicTo(path, c1, c2, to);
                    control = c2;
                    current = to;
                } break;
                case 'Q':
                case 'T': {
                    Point q = upper == 'Q' ? Point{base.x + v[0], base.y + v[1]} : (previous == 'Q' || previous == 'T' ? Point{2.f * current.x - control.x, 2.f * current.y - control.y} : current);
                    Point to = upper == 'Q' ? Point{base.x + v[2], base.y + v[3]} : Point{base.x + v[0], base.y + v[1]};
                    Point c1{current.x + 2.f / 3.f * (q.x - current.x), current.y + 2.f / 3.f * (q.y - current.y)};
                    Point c2{to.x + 2.f / 3.f * (q.x - to.x), to.y + 2.f / 3.f * (q.y - to.y)};

                    cubicTo(path, c1, c2, to);
                    control = q;
                    current = to;
                    break;
                }
                default: {
                    // 'A', the last command of the count switch above.
                    Point to{base.x + v[5], base.y + v[6]};

                    arcTo(path, current, v[0], v[1], v[2], largeArc, sweepArc, to);
                    current = to;
                    break;
                }
            }

            previous = upper;
        }
    }

    static void parsePoints(StringView text, Vector<Point>& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        for (;;) {
            Point point;

            skipSpaces(p, end);

            if (p >= end) {
                return;
            }

            if (!readNumber(p, end, point.x)) {
                out.clear();

                return;
            }

            skipSeparator(p, end);

            skipSpaces(p, end);

            if (p >= end) {
                return;
            }

            if (!readNumber(p, end, point.y)) {
                out.clear();

                return;
            }

            skipSeparator(p, end);
            out.pushBack(point);
        }
    }

    static void ellipsePath(Path& path, float cx, float cy, float rx, float ry) {
        const float k = 0.5522847498f;

        moveTo(path, Point{cx + rx, cy});
        cubicTo(path, Point{cx + rx, cy + ry * k}, Point{cx + rx * k, cy + ry}, Point{cx, cy + ry});
        cubicTo(path, Point{cx - rx * k, cy + ry}, Point{cx - rx, cy + ry * k}, Point{cx - rx, cy});
        cubicTo(path, Point{cx - rx, cy - ry * k}, Point{cx - rx * k, cy - ry}, Point{cx, cy - ry});
        cubicTo(path, Point{cx + rx * k, cy - ry}, Point{cx + rx, cy - ry * k}, Point{cx + rx, cy});
        closePath(path);
    }

    static bool isKeyword(StringView text, StringView keyword);

    static bool buildShape(const Pass& l, const Node* node, const ComputedStyle& style, Viewport vp, Path& path) {
        const Document& doc = *l.doc;
        float fs = style.fontSize;

        switch (node->name) {
            case AtomPath: {
                u32 d = attribute(node, AtomD);
                const Node* at = node;

                for (u32 hops = 0; d && hops < 64 && isKeyword(textOf(doc, d), SV("inherit")); hops++) {
                    const Node* parent = at->parent;

                    for (size_t k = l.hosts.length(); k >= 2; k -= 2) {
                        if (l.hosts[k - 2] == at) {
                            parent = l.hosts[k - 1];
                            break;
                        }
                    }

                    at = parent;
                    d = at ? attribute(at, AtomD) : 0;
                }

                if (d) {
                    parsePathData(textOf(doc, d), path);
                }
            } break;
            case AtomRect: {
                float x = lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f);
                float y = lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f);
                float w = lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
                float h = lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);
                float rx;
                float ry;
                bool hasRx = lengthAttribute(doc, node, AtomRx, Axis::X, vp, fs, rx) && rx >= 0.f;
                bool hasRy = lengthAttribute(doc, node, AtomRy, Axis::Y, vp, fs, ry) && ry >= 0.f;

                if (w <= 0.f || h <= 0.f) {
                    return false;
                }

                if (!hasRx) {
                    rx = hasRy ? ry : 0.f;
                }

                if (!hasRy) {
                    ry = hasRx ? rx : 0.f;
                }

                rx = minf(rx, w / 2.f);
                ry = minf(ry, h / 2.f);

                if (rx > 0.f && ry > 0.f) {
                    const float k = 0.5522847498f;

                    moveTo(path, Point{x + rx, y});
                    lineTo(path, Point{x + w - rx, y});
                    cubicTo(path, Point{x + w - rx + rx * k, y}, Point{x + w, y + ry - ry * k}, Point{x + w, y + ry});
                    lineTo(path, Point{x + w, y + h - ry});
                    cubicTo(path, Point{x + w, y + h - ry + ry * k}, Point{x + w - rx + rx * k, y + h}, Point{x + w - rx, y + h});
                    lineTo(path, Point{x + rx, y + h});
                    cubicTo(path, Point{x + rx - rx * k, y + h}, Point{x, y + h - ry + ry * k}, Point{x, y + h - ry});
                    lineTo(path, Point{x, y + ry});
                    cubicTo(path, Point{x, y + ry - ry * k}, Point{x + rx - rx * k, y}, Point{x + rx, y});
                    closePath(path);
                } else {
                    moveTo(path, Point{x, y});
                    lineTo(path, Point{x + w, y});
                    lineTo(path, Point{x + w, y + h});
                    lineTo(path, Point{x, y + h});
                    closePath(path);
                }

                break;
            }
            case AtomCircle: {
                float r = lengthOr(doc, node, AtomR, Axis::Other, vp, fs, 0.f);

                if (r <= 0.f) {
                    return false;
                }

                ellipsePath(path, lengthOr(doc, node, AtomCx, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomCy, Axis::Y, vp, fs, 0.f), r, r);
                break;
            }
            case AtomEllipse: {
                float rx;
                float ry;
                bool hasRx = lengthAttribute(doc, node, AtomRx, Axis::X, vp, fs, rx) && rx >= 0.f;
                bool hasRy = lengthAttribute(doc, node, AtomRy, Axis::Y, vp, fs, ry) && ry >= 0.f;

                if (!hasRx) {
                    rx = hasRy ? ry : 0.f;
                }

                if (!hasRy) {
                    ry = hasRx ? rx : 0.f;
                }

                if (rx <= 0.f || ry <= 0.f) {
                    return false;
                }

                ellipsePath(path, lengthOr(doc, node, AtomCx, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomCy, Axis::Y, vp, fs, 0.f), rx, ry);
                break;
            }
            case AtomLine:
                moveTo(path, Point{lengthOr(doc, node, AtomX1, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomY1, Axis::Y, vp, fs, 0.f)});
                lineTo(path, Point{lengthOr(doc, node, AtomX2, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomY2, Axis::Y, vp, fs, 0.f)});
                break;
            default: {
                // AtomPolyline or AtomPolygon: the callers build shapes alone.
                u32 value = attribute(node, AtomPoints);
                Vector<Point> points;

                if (value) {
                    parsePoints(textOf(doc, value), points);
                }

                if (points.length() == 0) {
                    return false;
                }

                moveTo(path, points[0]);

                for (size_t i = 1; i < points.length(); i++) {
                    lineTo(path, points[i]);
                }

                if (node->name == AtomPolygon) {
                    closePath(path);
                }

                break;
            }
        }

        return path.verbs.length() > 0;
    }

    static Point cubicAt(Point p0, Point p1, Point p2, Point p3, float t) {
        float u = 1.f - t;
        float b = 3.f * u * u * t;
        float c = 3.f * u * t * t;
        float d = t * t * t;

        return Point{p0.x + b * (p1.x - p0.x) + c * (p2.x - p0.x) + d * (p3.x - p0.x), p0.y + b * (p1.y - p0.y) + c * (p2.y - p0.y) + d * (p3.y - p0.y)};
    }

    static float deviation(Point a, Point b) {
        return fabsf(atan2f(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y));
    }

    static Point endTangent(Point a, Point b, Point c) {
        Point t{b.x - a.x, b.y - a.y};

        // A cubic with both controls at an end is straight and not refined.
        if (t.x == 0.f && t.y == 0.f) {
            t = Point{c.x - a.x, c.y - a.y};
        }

        return t;
    }

    static int refinements(Point p0, Point p1, Point p2, Point p3, float step, bool end, float turn, float tol) {
        Point anchor = end ? p3 : p0;
        Point tangent = end ? endTangent(p3, p2, p1) : endTangent(p0, p1, p2);
        int k = 0;

        while (k < 12) {
            Point q = cubicAt(p0, p1, p2, p3, end ? 1.f - step : step);
            Point chord{q.x - anchor.x, q.y - anchor.y};

            if (chord.x * chord.x + chord.y * chord.y < tol * tol || deviation(tangent, chord) <= turn) {
                break;
            }

            step *= 0.5f;
            k++;
        }

        return k;
    }

    static bool straightCubic(Point p0, Point p1, Point p2, Point p3) {
        double dx = (double)p3.x - p0.x;
        double dy = (double)p3.y - p0.y;
        double length = dx * dx + dy * dy;

        if (length == 0.0) {
            return p1.x == p0.x && p1.y == p0.y && p2.x == p0.x && p2.y == p0.y;
        }

        double x1 = (double)p1.x - p0.x;
        double y1 = (double)p1.y - p0.y;
        double x2 = (double)p2.x - p0.x;
        double y2 = (double)p2.y - p0.y;
        double along1 = dx * x1 + dy * y1;
        double along2 = dx * x2 + dy * y2;

        return dx * y1 == dy * x1 && dx * y2 == dy * x2 && along1 >= 0.0 && along2 >= along1 && along2 <= length;
    }

    static void flatten(const Path& path, const Matrix& m, float tol, float turn, Flat& out) {
        const Point* pts = path.points.data();
        size_t pi = 0;
        bool open = false;
        Point start{0.f, 0.f};
        Point last{0.f, 0.f};

        auto finish = [&](bool closed) {
            if (open) {
                out.ends.pushBack((u32)out.points.length());
                out.closed.pushBack(closed ? 1 : 0);
                open = false;
            }
        };

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    finish(false);
                    start = apply(m, pts[pi++]);
                    last = start;
                    out.points.pushBack(start);
                    open = true;
                    break;
                case VerbLine:
                    last = apply(m, pts[pi++]);
                    out.points.pushBack(last);
                    break;
                case VerbCubic:
                case VerbArcCubic: {
                    Point p0 = last;
                    Point p1 = apply(m, pts[pi]);
                    Point p2 = apply(m, pts[pi + 1]);
                    Point p3 = apply(m, pts[pi + 2]);

                    // Subdividing a straight cubic with a stationary endpoint
                    // can round its final chord into a false stroke-join tangent.
                    if (straightCubic(p0, p1, p2, p3)) {
                        out.points.pushBack(p3);
                        last = p3;
                        pi += 3;
                        break;
                    }

                    float ddx = maxf(fabsf(p0.x - 2.f * p1.x + p2.x), fabsf(p1.x - 2.f * p2.x + p3.x));
                    float ddy = maxf(fabsf(p0.y - 2.f * p1.y + p2.y), fabsf(p1.y - 2.f * p2.y + p3.y));
                    float dd = sqrtf(ddx * ddx + ddy * ddy);
                    float steps = ceilf(sqrtf(0.75f * dd / tol));
                    int n = steps >= 1.f ? (steps < 512.f ? (int)steps : 512) : 1;

                    int head = turn > 0.f ? refinements(p0, p1, p2, p3, 1.f / n, false, turn, tol) : 0;
                    int tail = turn > 0.f ? refinements(p0, p1, p2, p3, 1.f / n, true, turn, tol) : 0;

                    if (n == 1 && head && tail) {
                        tail--;
                    }

                    for (int k = head; k >= 1; k--) {
                        out.points.pushBack(cubicAt(p0, p1, p2, p3, ldexpf(1.f / n, -k)));
                    }

                    for (int i = 1; i < n; i++) {
                        out.points.pushBack(cubicAt(p0, p1, p2, p3, (float)i / n));
                    }

                    for (int k = 1; k <= tail; k++) {
                        out.points.pushBack(cubicAt(p0, p1, p2, p3, 1.f - ldexpf(1.f / n, -k)));
                    }

                    out.points.pushBack(p3);

                    last = p3;
                    pi += 3;
                    break;
                }
                default:
                    // VerbClose.
                    if (open) {
                        size_t count = out.points.length() - (out.ends.length() ? out.ends.back() : 0);

                        if (count == 1) {
                            out.points.pushBack(start);
                        }
                    }

                    finish(true);
                    last = start;
                    break;
            }
        }

        finish(false);
    }

    static void flatten(const Path& path, const Matrix& m, float tol, Flat& out) {
        flatten(path, m, tol, 0.f, out);
    }

    // Widens a range by the extremes of a cubic's coordinate between its
    // ends: where its derivative, a quadratic in t, is zero inside (0, 1).
    static void cubicExtremes(float p0, float p1, float p2, float p3, float& lo, float& hi) {
        double a = -(double)p0 + 3.0 * p1 - 3.0 * p2 + p3;
        double b = 2.0 * ((double)p0 - 2.0 * p1 + p2);
        double c = (double)p1 - p0;
        double roots[2];
        int count = 0;

        if (fabs(a) < 1e-12) {
            if (fabs(b) >= 1e-12) {
                roots[count++] = -c / b;
            }
        } else {
            double discriminant = b * b - 4.0 * a * c;

            if (discriminant >= 0.0) {
                double root = sqrt(discriminant);

                roots[count++] = (-b - root) / (2.0 * a);
                roots[count++] = (-b + root) / (2.0 * a);
            }
        }

        for (int i = 0; i < count; i++) {
            double t = roots[i];

            if (t > 0.0 && t < 1.0) {
                double u = 1.0 - t;
                float at = (float)(u * u * u * p0 + 3.0 * u * u * t * p1 + 3.0 * u * t * t * p2 + t * t * t * p3);

                lo = minf(lo, at);
                hi = maxf(hi, at);
            }
        }
    }

    // The box of a path under a matrix, exactly: the ends of its segments
    // and the extremes of its cubics, which an affine map keeps cubics.
    static Box pathBox(const Path& path, const Matrix& m) {
        Box box = emptyBox();
        const Point* pts = path.points.data();
        size_t pi = 0;
        Point start{0.f, 0.f};
        Point last{0.f, 0.f};

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    start = apply(m, pts[pi++]);
                    last = start;
                    extend(box, last);
                    break;
                case VerbLine:
                    last = apply(m, pts[pi++]);
                    extend(box, last);
                    break;
                case VerbCubic:
                case VerbArcCubic: {
                    Point p1 = apply(m, pts[pi]);
                    Point p2 = apply(m, pts[pi + 1]);
                    Point p3 = apply(m, pts[pi + 2]);

                    extend(box, p3);
                    cubicExtremes(last.x, p1.x, p2.x, p3.x, box.x0, box.x1);
                    cubicExtremes(last.y, p1.y, p2.y, p3.y, box.y0, box.y1);
                    last = p3;
                    pi += 3;
                    break;
                }
                default:
                    // VerbClose
                    last = start;
                    break;
            }
        }

        return box;
    }

    static float signedArea(const Point* points, size_t count) {
        double area = 0.0;
        double ox = points[0].x;
        double oy = points[0].y;

        for (size_t i = 0; i < count; i++) {
            const Point& a = points[i];
            const Point& b = points[(i + 1) % count];

            area += (a.x - ox) * (b.y - oy) - (b.x - ox) * (a.y - oy);
        }

        return (float)(area * 0.5);
    }

    // Adds a fan, quad or triangle of the stroke.
    static void addPolygon(Flat& out, const Point* points, size_t count) {
        bool reverse = signedArea(points, count) < 0.f;

        for (size_t i = 0; i < count; i++) {
            out.points.pushBack(points[reverse ? count - 1 - i : i]);
        }

        out.ends.pushBack((u32)out.points.length());
        out.closed.pushBack(1);
    }

    static int arcSegments(float angle, float radius, float tol) {
        if (radius <= tol) {
            return 4;
        }

        float step = 2.f * acosf(1.f - tol / radius);
        int n = (int)ceilf(fabsf(angle) / maxf(step, 0.01f));

        return n < 2 ? 2 : (n > 256 ? 256 : n);
    }

    static void addFan(Flat& out, Point center, float radius, float from, float sweep, float tol) {
        int n = arcSegments(sweep, radius, tol);
        Vector<Point> fan;

        fan.pushBack(center);

        for (int i = 0; i <= n; i++) {
            float a = from + sweep * i / n;

            fan.pushBack(Point{center.x + radius * cosf(a), center.y + radius * sinf(a)});
        }

        addPolygon(out, fan.data(), fan.length());
    }

    struct StrokeParams {
        float half;
        u8 cap;
        u8 join;
        float miterLimit;
        float tol;
    };

    static void addJoin(Flat& out, const StrokeParams& s, Point p, Point d0, Point d1) {
        float cross = d0.x * d1.y - d0.y * d1.x;
        float dot = d0.x * d1.x + d0.y * d1.y;

        if (fabsf(cross) < 1e-6f && dot > 0.f) {
            return;
        }

        float side = cross > 0.f ? -1.f : 1.f;
        Point n0{-d0.y * side, d0.x * side};
        Point n1{-d1.y * side, d1.x * side};
        Point a{p.x + n0.x * s.half, p.y + n0.y * s.half};
        Point b{p.x + n1.x * s.half, p.y + n1.y * s.half};

        if (s.join == JoinRound) {
            float from = atan2f(n0.y, n0.x);
            float to = atan2f(n1.y, n1.x);
            float sweep = to - from;

            while (sweep > pi) {
                sweep -= 2.f * pi;
            }

            while (sweep < -pi) {
                sweep += 2.f * pi;
            }

            float middle = from + sweep / 2.f;

            // Only a reversal is ambiguous, and its normals sweep half a turn
            // from the cut of atan2 the wrong way.
            if (cosf(middle) * (d0.x - d1.x) + sinf(middle) * (d0.y - d1.y) < 0.f) {
                sweep -= 2.f * pi;
            }

            addFan(out, p, s.half, from, sweep, s.tol);

            return;
        }

        if (s.join == JoinMiter) {
            float cosHalf = sqrtf(maxf(0.f, (1.f + dot) / 2.f));

            // The normals do not cancel when the turn leaves a half angle.
            if (cosHalf > 1e-6f && 1.f / cosHalf <= s.miterLimit) {
                Point m{n0.x + n1.x, n0.y + n1.y};
                float length = sqrtf(m.x * m.x + m.y * m.y);
                float reach = s.half / cosHalf;
                Point tip{p.x + m.x / length * reach, p.y + m.y / length * reach};
                Point quad[4] = {p, a, tip, b};

                addPolygon(out, quad, 4);

                return;
            }
        }

        Point triangle[3] = {p, a, b};

        addPolygon(out, triangle, 3);
    }

    static void addCap(Flat& out, const StrokeParams& s, Point p, Point d) {
        Point n{-d.y, d.x};

        if (s.cap == CapSquare) {
            Point quad[4] = {
                Point{p.x + n.x * s.half, p.y + n.y * s.half},
                Point{p.x + n.x * s.half + d.x * s.half, p.y + n.y * s.half + d.y * s.half},
                Point{p.x - n.x * s.half + d.x * s.half, p.y - n.y * s.half + d.y * s.half},
                Point{p.x - n.x * s.half, p.y - n.y * s.half},
            };

            addPolygon(out, quad, 4);
        } else if (s.cap == CapRound) {
            addFan(out, p, s.half, atan2f(n.y, n.x), -pi, s.tol);
        }
    }

    static void strokePolyline(Flat& out, const StrokeParams& s, const Point* points, size_t count, bool closed) {
        Vector<Point> pts;

        for (size_t i = 0; i < count; i++) {
            if (pts.length() == 0 || pts.back().x != points[i].x || pts.back().y != points[i].y) {
                pts.pushBack(points[i]);
            }
        }

        if (closed && pts.length() > 1 && pts.back().x == pts[0].x && pts.back().y == pts[0].y) {
            pts.popBack();
        }

        // A run of two or more points at one place: its caps make a dot.
        if (pts.length() == 1) {
            if (s.cap == CapRound) {
                addFan(out, pts[0], s.half, 0.f, 2.f * pi, s.tol);
            } else if (s.cap == CapSquare) {
                Point p = pts[0];
                Point quad[4] = {Point{p.x - s.half, p.y - s.half}, Point{p.x + s.half, p.y - s.half}, Point{p.x + s.half, p.y + s.half}, Point{p.x - s.half, p.y + s.half}};

                addPolygon(out, quad, 4);
            }

            return;
        }

        size_t n = pts.length();
        size_t segments = closed ? n : n - 1;
        Vector<Point> dirs;

        for (size_t i = 0; i < segments; i++) {
            Point a = pts[i];
            Point b = pts[(i + 1) % n];
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float length = (float)sqrt((double)dx * dx + (double)dy * dy);

            dirs.pushBack(Point{dx / length, dy / length});

            Point normal{-dy / length * s.half, dx / length * s.half};
            Point quad[4] = {
                Point{a.x + normal.x, a.y + normal.y},
                Point{b.x + normal.x, b.y + normal.y},
                Point{b.x - normal.x, b.y - normal.y},
                Point{a.x - normal.x, a.y - normal.y},
            };

            addPolygon(out, quad, 4);
        }

        for (size_t i = 1; i < segments; i++) {
            addJoin(out, s, pts[i], dirs[i - 1], dirs[i]);
        }

        if (closed) {
            addJoin(out, s, pts[0], dirs[segments - 1], dirs[0]);
        } else {
            addCap(out, s, pts[0], Point{-dirs[0].x, -dirs[0].y});
            addCap(out, s, pts[n - 1], dirs[segments - 1]);
        }
    }

    // Whether a stroke of these points, grown by the reach of its joins and
    // caps, lies wholly outside the box: its outline then covers nothing of
    // the raster and can go unbuilt.
    static bool strokeOutside(const StrokeParams& s, const Point* points, size_t count, const Box* cull) {
        if (!cull) {
            return false;
        }

        Box box = emptyBox();

        for (size_t i = 0; i < count; i++) {
            extend(box, points[i]);
        }

        float reach = s.half * maxf(s.miterLimit, 1.f) + s.tol;

        return box.x1 + reach < cull->x0 || box.x0 - reach > cull->x1 || box.y1 + reach < cull->y0 || box.y0 - reach > cull->y1;
    }

    static void dashPolyline(Flat& out, const StrokeParams& s, const Point* points, size_t count, bool closed, const float* dashes, u32 dashCount, float offset, const Box* cull) {
        Vector<Point> loop;

        loop.append(points, count);

        if (closed) {
            loop.pushBack(points[0]);
        }

        float total = 0.f;

        for (u32 i = 0; i < dashCount; i++) {
            total += dashes[i];
        }

        double extent = 0.0;

        for (size_t i = 0; i + 1 < loop.length(); i++) {
            double dx = loop[i + 1].x - loop[i].x;
            double dy = loop[i + 1].y - loop[i].y;

            extent += sqrt(dx * dx + dy * dy);
        }

        // Skia gives up dashing past a million dashes and draws the stroke
        // solid; so do we, instead of an outline that fits in no memory.
        if (!(extent / total <= maxDashes)) {
            strokePolyline(out, s, points, count, closed);

            return;
        }

        // The first pass counts the dashes inside the raster, the second
        // builds them; past the outline budget the stroke is solid too.
        float capPoints = s.cap == CapRound ? (float)(arcSegments(pi, s.half, s.tol) + 2) : (s.cap == CapSquare ? 4.f : 0.f);
        double visible = 0.0;

        for (int pass = 0; pass < 2; pass++) {
            float phase = fmodf(offset, total);

            if (phase < 0.f) {
                phase += total;
            }

            u32 index = 0;

            while (phase > dashes[index] || (phase == dashes[index] && dashes[index] > 0.f)) {
                phase -= dashes[index];
                index = (index + 1) % dashCount;
            }

            float remaining = dashes[index] - phase;
            bool on = index % 2 == 0;
            Vector<Point> piece;

            if (on) {
                piece.pushBack(loop[0]);
            }

            for (size_t i = 0; i + 1 < loop.length(); i++) {
                Point a = loop[i];
                Point b = loop[i + 1];
                float dx = b.x - a.x;
                float dy = b.y - a.y;
                float length = (float)sqrt((double)dx * dx + (double)dy * dy);
                float travelled = 0.f;

                while (length - travelled > remaining) {
                    travelled += remaining;

                    Point cut{a.x + dx * travelled / length, a.y + dy * travelled / length};

                    if (on) {
                        piece.pushBack(cut);

                        if (!strokeOutside(s, piece.data(), piece.length(), cull)) {
                            if (pass) {
                                strokePolyline(out, s, piece.data(), piece.length(), false);
                            } else {
                                visible += 1.0;
                            }
                        }

                        piece.clear();
                    } else {
                        piece.pushBack(cut);
                    }

                    on = !on;
                    index = (index + 1) % dashCount;
                    remaining = dashes[index];
                }

                remaining -= length - travelled;

                if (on) {
                    piece.pushBack(b);
                }
            }

            // A dash on at the end holds its start and the end.
            if (on && !strokeOutside(s, piece.data(), piece.length(), cull)) {
                if (pass) {
                    strokePolyline(out, s, piece.data(), piece.length(), false);
                } else {
                    visible += 1.0;
                }
            }

            if (pass == 0 && visible * (4.0 + 2.0 * capPoints) > maxOutline) {
                strokePolyline(out, s, points, count, closed);

                return;
            }
        }
    }

    static Path transformedPath(const Path& path, const Matrix& m) {
        Path out;

        out.verbs.append(path.verbs.data(), path.verbs.length());

        for (Point p : path.points) {
            out.points.pushBack(apply(m, p));
        }

        return out;
    }

    // The stroke of the path, in the path's space; a dash outside the box, the
    // raster in that space, is left out.
    static void strokePath(const Path& path, const ComputedStyle& style, float scale, const Box* cull, Flat& out) {
        if (style.cap == CapButt && style.dashCount) {
            bool visible = false;

            for (u32 i = 0; i < style.dashCount; i += 2) {
                visible |= style.dashes[i] > 0.f;
            }

            // Zero-length dashes with butt caps have no area, regardless of their spacing.
            if (!visible) {
                return;
            }
        }

        Flat flat;
        StrokeParams s{style.strokeWidth / 2.f, style.cap, style.join, style.miterLimit, tolerance / maxf(scale, 1e-6f)};

        flatten(path, identity(), s.tol, maxf(s.tol / s.half, 1.f / 1024.f), flat);

        u32 begin = 0;

        for (size_t i = 0; i < flat.ends.length(); i++) {
            u32 end = flat.ends[i];
            const Point* points = flat.points.data() + begin;
            size_t count = end - begin;

            if (count >= 2) {
                if (style.dashCount) {
                    dashPolyline(out, s, points, count, flat.closed[i], style.dashes, style.dashCount, style.dashOffset, cull);
                } else {
                    strokePolyline(out, s, points, count, flat.closed[i]);
                }
            }

            begin = end;
        }
    }

    static bool isKeyword(StringView text, StringView keyword) {
        return equalsIgnoreCase(trim(text), keyword);
    }

    static bool parseOpacity(StringView text, float& out) {
        StringView body = trim(text);
        const u8* p = body.begin();
        float value;

        if (!readNumber(p, body.end(), value)) {
            return false;
        }

        if (at(p, body.end(), '%')) {
            value /= 100.f;
            p++;
        }

        if (p != body.end()) {
            return false;
        }

        out = clampUnit(value);

        return true;
    }

    static ComputedStyle initialStyle() {
        ComputedStyle style;

        memset(&style, 0, sizeof(style));
        style.fill.kind = PaintKind::Color;
        style.fill.color = Color{0.f, 0.f, 0.f, 1.f};
        style.stroke.kind = PaintKind::None;
        style.color = Color{0.f, 0.f, 0.f, 1.f};
        style.stopColor = Color{0.f, 0.f, 0.f, 1.f};
        style.fillOpacity = 1.f;
        style.strokeOpacity = 1.f;
        style.strokeWidth = 1.f;
        style.miterLimit = 4.f;
        style.paintOrder[0] = 0;
        style.paintOrder[1] = 1;
        style.paintOrder[2] = 2;
        style.opacity = 1.f;
        style.stopOpacity = 1.f;
        style.fontSize = 16.f;
        style.fillRule = WindingNonZero;
        style.clipRule = WindingNonZero;
        style.cap = CapButt;
        style.join = JoinMiter;
        style.visible = true;
        style.floodColor = Color{0.f, 0.f, 0.f, 1.f};
        style.floodOpacity = 1.f;
        style.lightingColor = Color{1.f, 1.f, 1.f, 1.f};
        style.linearFilters = true;
        style.zoom = 1.f;

        return style;
    }

    static StringView cascaded(const Document& doc, const Node* node, u32 property) {
        u32 value = node->style[property - firstProperty];

        return value ? textOf(doc, value) : StringView();
    }

    static bool readFilterAmount(StringView args, float& out) {
        const u8* p = args.begin();
        const u8* end = args.end();

        if (args.empty()) {
            out = 1.f;

            return true;
        }

        if (!readNumber(p, end, out)) {
            return false;
        }

        if (at(p, end, '%')) {
            out /= 100.f;
            p++;
        }

        return p == end && out >= 0.f;
    }

    static bool readFilterAngle(StringView args, float& out) {
        const u8* p = args.begin();
        const u8* end = args.end();

        out = 0.f;

        if (args.empty()) {
            return true;
        }

        if (!readNumber(p, end, out)) {
            return false;
        }

        StringView unit(p, end);

        if (equalsIgnoreCase(unit, SV("deg"))) {
            return true;
        }

        if (equalsIgnoreCase(unit, SV("rad"))) {
            out *= 180.f / pi;

            return true;
        }

        if (equalsIgnoreCase(unit, SV("grad"))) {
            out *= 0.9f;

            return true;
        }

        if (equalsIgnoreCase(unit, SV("turn"))) {
            out *= 360.f;

            return true;
        }

        return unit.empty() && out == 0.f;
    }

    static bool readFilterLength(StringView text, bool unitless, const ComputedStyle& style, Viewport vp, float& out) {
        Length length;

        if (!parseLength(text, length) || length.unit == Unit::Percent) {
            return false;
        }

        if (length.unit == Unit::Number && !unitless && length.value != 0.f) {
            return false;
        }

        out = resolveLength(length, Axis::Other, vp, style.fontSize);

        return true;
    }

    static void amountMatrix(StringView name, float a, float* m) {
        float b = 1.f - a;
        float identity[20] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};

        memcpy(m, identity, sizeof(identity));

        if (name == SV("grayscale")) {
            float t[20] = {0.2126f + 0.7874f * b, 0.7152f - 0.7152f * b, 0.0722f - 0.0722f * b, 0, 0, 0.2126f - 0.2126f * b, 0.7152f + 0.2848f * b, 0.0722f - 0.0722f * b, 0, 0, 0.2126f - 0.2126f * b, 0.7152f - 0.7152f * b, 0.0722f + 0.9278f * b, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == SV("sepia")) {
            float t[20] = {0.393f + 0.607f * b, 0.769f - 0.769f * b, 0.189f - 0.189f * b, 0, 0, 0.349f - 0.349f * b, 0.686f + 0.314f * b, 0.168f - 0.168f * b, 0, 0, 0.272f - 0.272f * b, 0.534f - 0.534f * b, 0.131f + 0.869f * b, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == SV("saturate")) {
            float t[20] = {0.213f + 0.787f * a, 0.715f - 0.715f * a, 0.072f - 0.072f * a, 0, 0, 0.213f - 0.213f * a, 0.715f + 0.285f * a, 0.072f - 0.072f * a, 0, 0, 0.213f - 0.213f * a, 0.715f - 0.715f * a, 0.072f + 0.928f * a, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == SV("invert")) {
            for (int k = 0; k < 3; k++) {
                m[k * 6] = 1.f - 2.f * a;
                m[k * 5 + 4] = a;
            }
        } else if (name == SV("opacity")) {
            m[18] = a;
        } else if (name == SV("brightness")) {
            for (int k = 0; k < 3; k++) {
                m[k * 6] = a;
            }
        } else {
            // contrast, the last of the amounts.
            for (int k = 0; k < 3; k++) {
                m[k * 6] = a;
                m[k * 5 + 4] = 0.5f - 0.5f * a;
            }
        }
    }

    static void hueMatrix(float degrees, float* m) {
        float angle = degrees * pi / 180.f;
        float cs = cosf(angle);
        float sn = sinf(angle);
        float t[20] = {
            0.213f + cs * 0.787f - sn * 0.213f,
            0.715f - cs * 0.715f - sn * 0.715f,
            0.072f - cs * 0.072f + sn * 0.928f,
            0,
            0,
            0.213f - cs * 0.213f + sn * 0.143f,
            0.715f + cs * 0.285f + sn * 0.140f,
            0.072f - cs * 0.072f - sn * 0.283f,
            0,
            0,
            0.213f - cs * 0.213f - sn * 0.787f,
            0.715f - cs * 0.715f + sn * 0.715f,
            0.072f + cs * 0.928f + sn * 0.072f,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
        };

        memcpy(m, t, sizeof(t));
    }

    static bool parseDropShadow(StringView args, const ComputedStyle& style, Viewport vp, FilterItem& item) {
        const u8* p = args.begin();
        const u8* end = args.end();
        StringView tokens[5];
        u32 count = 0;

        for (;;) {
            skipSpaces(p, end);

            if (p >= end) {
                break;
            }

            const u8* start = p;
            int depth = 0;

            while (p < end && (depth > 0 || !isSpace(*p))) {
                if (*p == '(') {
                    depth++;
                } else if (*p == ')') {
                    depth--;
                } else if (*p == ',' && depth == 0) {
                    return false;
                }

                p++;
            }

            if (count == 5) {
                return false;
            }

            tokens[count++] = StringView(start, p);
        }

        float lengths[4];
        u32 lengthCount = 0;
        bool hasColor = false;

        item.color = style.color;

        for (u32 k = 0; k < count; k++) {
            float value;

            if (readFilterLength(tokens[k], true, style, vp, value)) {
                // A colour first leaves the lengths after it.
                if (lengthCount == 3) {
                    return false;
                }

                lengths[lengthCount++] = value;

                continue;
            }

            if (hasColor || (k != 0 && k != count - 1)) {
                return false;
            }

            if (!equalsIgnoreCase(tokens[k], SV("currentcolor")) && !parseColor(tokens[k], item.color)) {
                return false;
            }

            hasColor = true;
        }

        if (lengthCount < 2 || (lengthCount == 3 && lengths[2] < 0.f)) {
            return false;
        }

        item.values[0] = lengths[0];
        item.values[1] = lengths[1];
        item.values[2] = lengthCount == 3 ? lengths[2] : 0.f;

        return true;
    }

    static bool parseFilterList(const Document& doc, StringView text, const ComputedStyle& style, Viewport vp, Vector<FilterItem>& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        for (;;) {
            skipSpaces(p, end);

            if (p >= end) {
                break;
            }

            const u8* start = p;

            while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '-')) {
                p++;
            }

            StringView name(start, p);

            if (p >= end || *p != '(' || name.empty()) {
                return false;
            }

            p++;

            const u8* open = p;
            int depth = 0;

            while (p < end && (depth > 0 || *p != ')')) {
                if (*p == '(') {
                    depth++;
                } else if (*p == ')') {
                    depth--;
                }

                p++;
            }

            if (p >= end) {
                return false;
            }

            StringView args = trim(StringView(open, p));
            FilterItem item;

            p++;
            memset(&item, 0, sizeof(item));

            if (equalsIgnoreCase(name, SV("url"))) {
                if (args.length() >= 2 && (args[0] == '"' || args[0] == '\'') && args.back() == args[0]) {
                    args = trim(StringView(args.begin() + 1, args.end() - 1));
                }

                item.kind = FilterFunction::Reference;

                if (!args.empty() && args[0] == '#') {
                    item.reference = fragmentAtom(doc, StringView(args.begin() + 1, args.end()));
                }
            } else if (equalsIgnoreCase(name, SV("blur"))) {
                float sigma = 0.f;

                if (!args.empty() && (!readFilterLength(args, false, style, vp, sigma) || sigma < 0.f)) {
                    return false;
                }

                item.kind = FilterFunction::Blur;
                item.values[0] = sigma;
                item.values[1] = sigma;
            } else if (equalsIgnoreCase(name, SV("drop-shadow"))) {
                item.kind = FilterFunction::DropShadow;

                if (!parseDropShadow(args, style, vp, item)) {
                    return false;
                }
            } else if (equalsIgnoreCase(name, SV("hue-rotate"))) {
                float angle;

                if (!readFilterAngle(args, angle)) {
                    return false;
                }

                item.kind = FilterFunction::Matrix;
                hueMatrix(angle, item.values);
            } else {
                static const char* const amounts[] = {"grayscale", "sepia", "saturate", "invert", "opacity", "brightness", "contrast"};
                StringView known;
                float amount;

                for (const char* candidate : amounts) {
                    if (equalsIgnoreCase(name, StringView(candidate))) {
                        known = StringView(candidate);
                    }
                }

                if (known.empty() || !readFilterAmount(args, amount)) {
                    return false;
                }

                if (known != SV("saturate") && known != SV("brightness") && known != SV("contrast")) {
                    amount = amount > 1.f ? 1.f : amount;
                }

                item.kind = FilterFunction::Matrix;
                amountMatrix(known, amount, item.values);
            }

            out.pushBack(item);
        }

        return out.length() > 0;
    }

    static StringView shapeWord(const u8*& p, const u8* end) {
        while (p < end && (isSpace(*p) || *p == ',')) {
            p++;
        }

        const u8* start = p;

        while (p < end && !isSpace(*p) && *p != ',') {
            p++;
        }

        return StringView(start, p);
    }

    static bool parsePosition(StringView text, Length& x, Length& y) {
        const u8* p = text.begin();
        StringView words[2];
        u32 count = 0;

        x = Length{50.f, Unit::Percent};
        y = Length{50.f, Unit::Percent};

        for (;;) {
            StringView word = shapeWord(p, text.end());

            if (word.empty()) {
                break;
            }

            if (count == 2) {
                return false;
            }

            words[count++] = word;
        }

        bool xSet = false;
        bool ySet = false;

        for (u32 i = 0; i < count; i++) {
            StringView word = words[i];
            Length length;

            if (equalsIgnoreCase(word, SV("left")) || equalsIgnoreCase(word, SV("right"))) {
                if (xSet) {
                    return false;
                }

                x = Length{equalsIgnoreCase(word, SV("left")) ? 0.f : 100.f, Unit::Percent};
                xSet = true;
            } else if (equalsIgnoreCase(word, SV("top")) || equalsIgnoreCase(word, SV("bottom"))) {
                if (ySet) {
                    return false;
                }

                y = Length{equalsIgnoreCase(word, SV("top")) ? 0.f : 100.f, Unit::Percent};
                ySet = true;
            } else if (equalsIgnoreCase(word, SV("center"))) {
                // The first word finds nothing set.
                if (i == 0) {
                    xSet = count == 1;
                }
            } else if (parseLength(word, length)) {
                if (i == 0) {
                    x = length;
                    xSet = true;
                } else if (!ySet) {
                    y = length;
                    ySet = true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        return true;
    }

    static bool parseShapeFunction(ObjPool& pool, StringView name, StringView args, BasicShape& out);

    static bool parseBasicShape(ObjPool& pool, StringView text, BasicShape& out) {
        const u8* p = trim(text).begin();
        const u8* end = trim(text).end();
        StringView function;

        out.kind = ShapeKind::None;
        out.box = GeometryBox::Stroke;
        out.rule = WindingNonZero;
        out.points = nullptr;
        out.pointCount = 0;

        // The text is trimmed, so a word follows every run of spaces.
        while (p < end) {
            skipSpaces(p, end);

            const u8* start = p;

            while (p < end && !isSpace(*p) && *p != '(') {
                p++;
            }

            StringView name(start, p);

            if (at(p, end, '(')) {
                if (!function.empty()) {
                    return false;
                }

                const u8* open = ++p;
                int depth = 0;

                while (p < end && (depth > 0 || *p != ')')) {
                    depth += *p == '(' ? 1 : (*p == ')' ? -1 : 0);
                    p++;
                }

                if (p >= end) {
                    return false;
                }

                function = name;

                StringView args = trim(StringView(open, p));

                p++;

                if (!parseShapeFunction(pool, name, args, out)) {
                    return false;
                }

                continue;
            }

            if (equalsIgnoreCase(name, SV("fill-box")) || equalsIgnoreCase(name, SV("content-box")) || equalsIgnoreCase(name, SV("padding-box"))) {
                out.box = GeometryBox::Fill;
            } else if (equalsIgnoreCase(name, SV("stroke-box")) || equalsIgnoreCase(name, SV("border-box")) || equalsIgnoreCase(name, SV("margin-box"))) {
                out.box = GeometryBox::Stroke;
            } else if (equalsIgnoreCase(name, SV("view-box"))) {
                out.box = GeometryBox::View;
            } else {
                return false;
            }
        }

        return out.kind != ShapeKind::None;
    }

    static bool parseShapeFunction(ObjPool& pool, StringView name, StringView args, BasicShape& out) {
        const u8* end = args.end();

        if (equalsIgnoreCase(name, SV("circle")) || equalsIgnoreCase(name, SV("ellipse"))) {
            bool circle = equalsIgnoreCase(name, SV("circle"));
            const u8* p = args.begin();
            u32 radii = 0;

            out.kind = circle ? ShapeKind::Circle : ShapeKind::Ellipse;
            out.values[0] = Length{-1.f, Unit::Number};
            out.values[1] = Length{-1.f, Unit::Number};

            for (;;) {
                StringView word = shapeWord(p, end);

                if (word.empty()) {
                    break;
                }

                if (equalsIgnoreCase(word, SV("at"))) {
                    return parsePosition(StringView(p, end), out.values[2], out.values[3]);
                }

                if (radii == (circle ? 1u : 2u)) {
                    return false;
                }

                Length radius;

                if (equalsIgnoreCase(word, SV("closest-side"))) {
                    radius = Length{-1.f, Unit::Number};
                } else if (equalsIgnoreCase(word, SV("farthest-side"))) {
                    radius = Length{-2.f, Unit::Number};
                } else if (!parseLength(word, radius) || radius.value < 0.f) {
                    return false;
                }

                out.values[radii++] = radius;
            }

            if (!circle && radii == 1) {
                return false;
            }

            out.values[2] = Length{50.f, Unit::Percent};
            out.values[3] = Length{50.f, Unit::Percent};

            return true;
        }

        if (equalsIgnoreCase(name, SV("inset"))) {
            const u8* p = args.begin();
            u32 count = 0;

            out.kind = ShapeKind::Inset;

            for (;;) {
                StringView word = shapeWord(p, end);

                if (word.empty() || equalsIgnoreCase(word, SV("round"))) {
                    break;
                }

                if (count == 4 || !parseLength(word, out.values[count])) {
                    return false;
                }

                count++;
            }

            if (count == 0) {
                return false;
            }

            out.values[1] = count > 1 ? out.values[1] : out.values[0];
            out.values[2] = count > 2 ? out.values[2] : out.values[0];
            out.values[3] = count > 3 ? out.values[3] : out.values[1];

            return true;
        }

        if (equalsIgnoreCase(name, SV("polygon"))) {
            const u8* p = args.begin();
            Vector<Length> points;

            out.kind = ShapeKind::Polygon;

            skipSpaces(p, end);

            const u8* comma = p;

            while (comma < end && *comma != ',') {
                comma++;
            }

            StringView first = trim(StringView(p, comma));

            if (equalsIgnoreCase(first, SV("evenodd")) || equalsIgnoreCase(first, SV("nonzero"))) {
                out.rule = equalsIgnoreCase(first, SV("evenodd")) ? WindingEvenOdd : WindingNonZero;
                p = comma < end ? comma + 1 : end;
            }

            while (p < end) {
                const u8* stop = p;

                while (stop < end && *stop != ',') {
                    stop++;
                }

                const u8* q = p;
                StringView xWord = shapeWord(q, stop);
                StringView yWord = shapeWord(q, stop);
                Length x;
                Length y;

                if (!parseLength(xWord, x) || !parseLength(yWord, y) || !trim(StringView(q, stop)).empty()) {
                    return false;
                }

                points.pushBack(x);
                points.pushBack(y);
                p = stop < end ? stop + 1 : end;
            }

            out.pointCount = (u32)points.length() / 2;
            out.points = (Length*)pool.allocate(sizeof(Length) * (points.length() + 1));

            if (points.length()) {
                memcpy(out.points, points.data(), sizeof(Length) * points.length());
            }

            return out.pointCount > 0;
        }

        return false;
    }

    static float shapeLength(const Length& length, float reference, Viewport vp, float fontSize) {
        return length.unit == Unit::Percent ? length.value * reference / 100.f : resolveLength(length, Axis::Other, vp, fontSize);
    }

    static ComputedStyle computeStyle(Pass& l, const Node* node, const ComputedStyle& parent, Viewport vp) {
        const Document& doc = *l.doc;
        ComputedStyle initial = initialStyle();
        ComputedStyle style = parent;

        style.opacity = 1.f;
        style.displayNone = false;
        style.stopColor = initial.stopColor;
        style.stopOpacity = 1.f;
        style.clipPath = 0;
        style.clipShape = nullptr;
        style.nonScaling = false;
        style.blend = 0;
        style.isolate = false;
        style.newBackground = false;
        style.backgroundClipped = false;
        style.outlineWidth = 0.f;
        style.zoom = 1.f;
        style.mask = 0;
        style.maskType = 0;
        style.floodColor = Color{0.f, 0.f, 0.f, 1.f};
        style.floodOpacity = 1.f;
        style.lightingColor = Color{1.f, 1.f, 1.f, 1.f};
        style.filters = nullptr;
        style.filterCount = 0;
        style.background = Color{0.f, 0.f, 0.f, 0.f};
        style.overflowVisible = !((node->name == AtomSvg && node != doc.root) || node->name == AtomSymbol || node->name == AtomImage || node->name == AtomMarker || node->name == AtomPattern);

        for (u32 property = firstProperty; property < firstProperty + propertyCount; property++) {
            StringView value = l.instance && l.instance[node->index] ? (l.instance[node->index][property - firstProperty] ? textOf(doc, l.instance[node->index][property - firstProperty]) : StringView()) : cascaded(doc, node, property);

            if (value.empty()) {
                continue;
            }

            bool inherit = isKeyword(value, SV("inherit"));
            bool reset = isKeyword(value, SV("initial"));

            if (isKeyword(value, SV("unset"))) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomMask && property != AtomMaskType && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation && property != AtomEnableBackground && property != AtomOutline && property != AtomZoom;

                inherit = inherited;
                reset = !inherited;
            }

            const ComputedStyle& source = inherit ? parent : initial;

            switch (property) {
                case AtomFontSize:
                    if (inherit || reset) {
                        style.fontSize = source.fontSize;
                    } else {
                        Length length;

                        if (parseLength(value, length) && length.value >= 0.f) {
                            style.fontSize = length.unit == Unit::Percent ? parent.fontSize * length.value / 100.f : resolveLength(length, Axis::Other, vp, parent.fontSize);
                        }
                    }

                    break;
                default:
                    break;
            }
        }

        for (u32 property = firstProperty; property < firstProperty + propertyCount; property++) {
            StringView value = l.instance && l.instance[node->index] ? (l.instance[node->index][property - firstProperty] ? textOf(doc, l.instance[node->index][property - firstProperty]) : StringView()) : cascaded(doc, node, property);

            if (value.empty() || property == AtomFontSize) {
                continue;
            }

            bool inherit = isKeyword(value, SV("inherit"));
            bool reset = isKeyword(value, SV("initial"));

            if (isKeyword(value, SV("unset"))) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomMask && property != AtomMaskType && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation && property != AtomEnableBackground && property != AtomOutline && property != AtomZoom;

                inherit = inherited;
                reset = !inherited;
            }

            const ComputedStyle& source = inherit ? parent : initial;
            bool keyword = inherit || reset;

            switch (property) {
                case AtomFill:
                case AtomStroke: {
                    PaintValue& target = property == AtomFill ? style.fill : style.stroke;

                    bool contextFill = isKeyword(value, SV("context-fill"));

                    if (keyword) {
                        target = property == AtomFill ? source.fill : source.stroke;
                    } else if (contextFill || isKeyword(value, SV("context-stroke"))) {
                        target.kind = PaintKind::None;

                        if (l.hasContext) {
                            target = contextFill ? l.contextFill : l.contextStroke;
                            target.context = true;
                        }
                    } else {
                        // The cascade keeps the paints that parse.
                        parsePaint(doc, value, target);
                    }

                    break;
                }
                case AtomColor:
                    if (keyword) {
                        style.color = source.color;
                    } else if (!isKeyword(value, SV("currentcolor"))) {
                        parseColor(value, style.color);
                    }

                    break;
                case AtomFillOpacity:
                    if (keyword) {
                        style.fillOpacity = source.fillOpacity;
                    } else {
                        parseOpacity(value, style.fillOpacity);
                    }

                    break;
                case AtomStrokeOpacity:
                    if (keyword) {
                        style.strokeOpacity = source.strokeOpacity;
                    } else {
                        parseOpacity(value, style.strokeOpacity);
                    }

                    break;
                case AtomOpacity:
                    if (keyword) {
                        style.opacity = source.opacity;
                    } else {
                        parseOpacity(value, style.opacity);
                    }

                    break;
                case AtomStopOpacity:
                    if (keyword) {
                        style.stopOpacity = source.stopOpacity;
                    } else {
                        parseOpacity(value, style.stopOpacity);
                    }

                    break;
                case AtomFillRule:
                case AtomClipRule: {
                    u8& target = property == AtomFillRule ? style.fillRule : style.clipRule;

                    if (keyword) {
                        target = property == AtomFillRule ? source.fillRule : source.clipRule;
                    } else if (isKeyword(value, SV("nonzero"))) {
                        target = WindingNonZero;
                    } else if (isKeyword(value, SV("evenodd"))) {
                        target = WindingEvenOdd;
                    }

                    break;
                }
                case AtomStrokeWidth:
                    if (keyword) {
                        style.strokeWidth = source.strokeWidth;
                    } else {
                        // The cascade keeps the widths that parse, of no less than zero.
                        Length length;

                        parseLength(value, length);
                        style.strokeWidth = resolveLength(length, Axis::Other, vp, style.fontSize);
                    }

                    break;
                case AtomStrokeLinecap:
                    if (keyword) {
                        style.cap = source.cap;
                    } else if (isKeyword(value, SV("butt"))) {
                        style.cap = CapButt;
                    } else if (isKeyword(value, SV("round"))) {
                        style.cap = CapRound;
                    } else if (isKeyword(value, SV("square"))) {
                        style.cap = CapSquare;
                    }

                    break;
                case AtomStrokeLinejoin:
                    if (keyword) {
                        style.join = source.join;
                    } else if (isKeyword(value, SV("miter")) || isKeyword(value, SV("miter-clip")) || isKeyword(value, SV("arcs"))) {
                        style.join = JoinMiter;
                    } else if (isKeyword(value, SV("round"))) {
                        style.join = JoinRound;
                    } else if (isKeyword(value, SV("bevel"))) {
                        style.join = JoinBevel;
                    }

                    break;
                case AtomStrokeMiterlimit:
                    if (keyword) {
                        style.miterLimit = source.miterLimit;
                    } else {
                        float limit;

                        if (parseNumberText(value, limit) && limit >= 1.f) {
                            style.miterLimit = limit;
                        }
                    }

                    break;
                case AtomStrokeDasharray:
                    if (keyword) {
                        style.dashes = source.dashes;
                        style.dashCount = source.dashCount;
                    } else if (isKeyword(value, SV("none"))) {
                        style.dashes = nullptr;
                        style.dashCount = 0;
                    } else {
                        // The cascade keeps the arrays of one length or more
                        // that parse, of no less than zero each.
                        Vector<float> dashes;
                        const u8* p = value.begin();
                        const u8* end = value.end();
                        float sum = 0.f;

                        for (;;) {
                            Length length;

                            skipSpaces(p, end);

                            if (p >= end) {
                                break;
                            }

                            readLength(p, end, length);

                            float dash = resolveLength(length, Axis::Other, vp, style.fontSize);

                            sum += dash;
                            dashes.pushBack(dash);
                            skipSeparator(p, end);
                        }

                        if (sum <= 0.f) {
                            style.dashes = nullptr;
                            style.dashCount = 0;
                            break;
                        }

                        size_t count = dashes.length() % 2 ? dashes.length() * 2 : dashes.length();

                        style.dashes = (float*)l.pool->allocate(sizeof(float) * count);

                        for (size_t i = 0; i < count; i++) {
                            style.dashes[i] = dashes[i % dashes.length()];
                        }

                        style.dashCount = (u32)count;
                    }

                    break;
                case AtomStrokeDashoffset:
                    if (keyword) {
                        style.dashOffset = source.dashOffset;
                    } else {
                        Length length;

                        if (parseLength(value, length)) {
                            style.dashOffset = resolveLength(length, Axis::Other, vp, style.fontSize);
                        }
                    }

                    break;
                case AtomDisplay:
                    style.displayNone = !keyword && isKeyword(value, SV("none"));
                    break;
                case AtomVisibility:
                    if (keyword) {
                        style.visible = source.visible;
                    } else if (isKeyword(value, SV("visible"))) {
                        style.visible = true;
                    } else if (isKeyword(value, SV("hidden")) || isKeyword(value, SV("collapse"))) {
                        style.visible = false;
                    }

                    break;
                case AtomStopColor:
                    if (keyword) {
                        style.stopColor = source.stopColor;
                    } else if (isKeyword(value, SV("currentcolor"))) {
                        style.stopColor = style.color;
                    } else {
                        // The cascade keeps the colours that parse.
                        parseColor(value, style.stopColor);
                    }

                    break;
                case AtomClipPathProperty:
                case AtomMask: {
                    u32& target = property == AtomMask ? style.mask : style.clipPath;

                    if (keyword) {
                        target = property == AtomMask ? source.mask : source.clipPath;

                        if (property != AtomMask) {
                            style.clipShape = source.clipShape;
                        }

                        break;
                    }

                    PaintValue reference;
                    BasicShape shape;

                    if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        target = reference.server;
                    } else if (isKeyword(value, SV("none"))) {
                        target = 0;
                    } else if (property != AtomMask && parseBasicShape(*l.pool, value, shape)) {
                        style.clipShape = l.pool->make<BasicShape>(shape);
                    }

                    break;
                }
                case AtomPaintOrder: {
                    if (keyword) {
                        memcpy(style.paintOrder, source.paintOrder, sizeof(style.paintOrder));
                        break;
                    }

                    u8 order[3];
                    u32 count = 0;
                    bool valid = true;
                    const u8* p = value.begin();

                    if (isKeyword(value, SV("normal"))) {
                        order[0] = 0;
                        order[1] = 1;
                        order[2] = 2;
                        memcpy(style.paintOrder, order, sizeof(order));
                        break;
                    }

                    while (valid) {
                        StringView word = shapeWord(p, value.end());

                        if (word.empty()) {
                            break;
                        }

                        u8 layer = equalsIgnoreCase(word, SV("fill")) ? 0 : (equalsIgnoreCase(word, SV("stroke")) ? 1 : (equalsIgnoreCase(word, SV("markers")) ? 2 : 3));

                        for (u32 k = 0; k < count; k++) {
                            valid = valid && order[k] != layer;
                        }

                        // A fourth layer repeats one or is none.
                        valid = valid && layer < 3;

                        if (valid) {
                            order[count++] = layer;
                        }
                    }

                    if (!valid || count == 0) {
                        break;
                    }

                    for (u8 layer = 0; layer < 3; layer++) {
                        bool present = false;

                        for (u32 k = 0; k < count; k++) {
                            present = present || order[k] == layer;
                        }

                        if (!present) {
                            order[count++] = layer;
                        }
                    }

                    memcpy(style.paintOrder, order, sizeof(order));
                    break;
                }
                case AtomMixBlendMode: {
                    static const char* const modes[] = {"normal", "multiply", "screen", "overlay", "darken", "lighten", "color-dodge", "color-burn", "hard-light", "soft-light", "difference", "exclusion", "hue", "saturation", "color", "luminosity"};

                    if (keyword) {
                        style.blend = source.blend;
                        break;
                    }

                    for (u8 k = 0; k < sizeof(modes) / sizeof(modes[0]); k++) {
                        if (isKeyword(value, StringView(modes[k]))) {
                            style.blend = k;
                        }
                    }

                    break;
                }
                case AtomOutline: {
                    if (keyword) {
                        style.outlineWidth = source.outlineWidth;
                        style.outlineColor = source.outlineColor;
                        break;
                    }

                    const u8* p = value.begin();
                    float width = 3.f;
                    Color color = style.color;
                    bool solid = false;

                    for (;;) {
                        skipSpaces(p, value.end());

                        const u8* start = p;
                        int depth = 0;

                        while (p < value.end() && (depth > 0 || !isSpace(*p))) {
                            depth += *p == '(' ? 1 : (*p == ')' ? -1 : 0);
                            p++;
                        }

                        StringView word(start, p);
                        Length length;

                        if (word.empty()) {
                            break;
                        }

                        if (equalsIgnoreCase(word, SV("solid"))) {
                            solid = true;
                        } else if (equalsIgnoreCase(word, SV("thin"))) {
                            width = 1.f;
                        } else if (equalsIgnoreCase(word, SV("medium"))) {
                            width = 3.f;
                        } else if (equalsIgnoreCase(word, SV("thick"))) {
                            width = 5.f;
                        } else if (parseLength(word, length) && length.unit != Unit::Percent) {
                            width = resolveLength(length, Axis::Other, vp, style.fontSize);
                        } else if (!parseColor(word, color)) {
                            solid = false;
                            break;
                        }
                    }

                    style.outlineWidth = solid ? width : 0.f;
                    style.outlineColor = color;
                    break;
                }
                case AtomZoom:
                    if (keyword) {
                        style.zoom = source.zoom;
                    } else if (isKeyword(value, SV("normal")) || isKeyword(value, SV("reset"))) {
                        style.zoom = 1.f;
                    } else {
                        Length length;

                        if (parseLength(value, length) && length.value > 0.f && (length.unit == Unit::Number || length.unit == Unit::Percent)) {
                            style.zoom = length.unit == Unit::Percent ? length.value / 100.f : length.value;
                        }
                    }

                    break;
                case AtomImageRendering:
                    if (keyword) {
                        style.imageRendering = source.imageRendering;
                    } else if (isKeyword(value, SV("crisp-edges")) || isKeyword(value, SV("optimizespeed"))) {
                        style.imageRendering = 1;
                    } else if (isKeyword(value, SV("pixelated"))) {
                        style.imageRendering = 2;
                    } else if (isKeyword(value, SV("auto")) || isKeyword(value, SV("smooth")) || isKeyword(value, SV("high-quality")) || isKeyword(value, SV("optimizequality"))) {
                        style.imageRendering = 0;
                    }

                    break;
                case AtomShapeRendering:
                    if (keyword) {
                        style.aliased = source.aliased;
                    } else {
                        // The cascade keeps the four values.
                        style.aliased = isKeyword(value, SV("crispedges")) || isKeyword(value, SV("optimizespeed"));
                    }

                    break;
                case AtomColorInterpolation:
                    if (keyword) {
                        style.linearColors = source.linearColors;
                    } else if (isKeyword(value, SV("linearrgb"))) {
                        style.linearColors = true;
                    } else if (isKeyword(value, SV("srgb")) || isKeyword(value, SV("auto"))) {
                        style.linearColors = false;
                    }

                    break;
                case AtomEnableBackground: {
                    if (keyword) {
                        style.newBackground = source.newBackground;
                        style.backgroundClipped = source.backgroundClipped;
                        style.backgroundBox = source.backgroundBox;
                        break;
                    }

                    const u8* p = value.begin();
                    const u8* end = value.end();
                    skipSpaces(p, end);
                    StringView word = readIdent(p, end);

                    if (isKeyword(word, SV("accumulate"))) {
                        style.newBackground = false;
                    } else if (isKeyword(word, SV("new"))) {
                        style.newBackground = true;
                        skipSpaces(p, end);
                        style.backgroundClipped = p < end;

                        if (p < end) {
                            float values[4];
                            bool valid = true;

                            for (float& v : values) {
                                while (p < end && (isSpace(*p) || *p == ',')) {
                                    p++;
                                }
                                valid = valid && readNumber(p, end, v);
                            }

                            skipSpaces(p, end);
                            style.backgroundBox = valid && p == end && values[2] > 0.f && values[3] > 0.f ? Box{values[0], values[1], values[0] + values[2], values[1] + values[3]} : Box{0.f, 0.f, 0.f, 0.f};
                        }
                    }

                    break;
                }
                case AtomIsolation:
                    if (keyword) {
                        style.isolate = source.isolate;
                    } else if (isKeyword(value, SV("isolate"))) {
                        style.isolate = true;
                    } else if (isKeyword(value, SV("auto"))) {
                        style.isolate = false;
                    }

                    break;
                case AtomVectorEffect:
                    if (keyword) {
                        style.nonScaling = source.nonScaling;
                    } else if (isKeyword(value, SV("non-scaling-stroke"))) {
                        style.nonScaling = true;
                    } else if (isKeyword(value, SV("none"))) {
                        style.nonScaling = false;
                    }

                    break;
                case AtomMarkerStart:
                case AtomMarkerMid:
                case AtomMarkerEnd: {
                    u32& target = style.markers[property - AtomMarkerStart];
                    PaintValue reference;

                    if (keyword) {
                        target = source.markers[property - AtomMarkerStart];
                    } else if (isKeyword(value, SV("none"))) {
                        target = 0;
                    } else if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        target = reference.server;
                    }

                    break;
                }
                case AtomFloodColor:
                case AtomLightingColor: {
                    Color& target = property == AtomFloodColor ? style.floodColor : style.lightingColor;

                    if (keyword) {
                        target = property == AtomFloodColor ? source.floodColor : source.lightingColor;
                    } else if (isKeyword(value, SV("currentcolor"))) {
                        target = style.color;
                    } else {
                        // The cascade keeps the colours that parse.
                        parseColor(value, target);
                    }

                    break;
                }
                case AtomFloodOpacity:
                    if (keyword) {
                        style.floodOpacity = source.floodOpacity;
                    } else {
                        parseOpacity(value, style.floodOpacity);
                    }

                    break;
                case AtomColorInterpolationFilters:
                    if (keyword) {
                        style.linearFilters = source.linearFilters;
                    } else if (isKeyword(value, SV("linearrgb"))) {
                        style.linearFilters = true;
                    } else if (isKeyword(value, SV("srgb")) || isKeyword(value, SV("auto"))) {
                        style.linearFilters = false;
                    }

                    break;
                case AtomFilter: {
                    if (keyword) {
                        style.filters = source.filters;
                        style.filterCount = source.filterCount;
                        break;
                    }

                    if (isKeyword(value, SV("none"))) {
                        style.filters = nullptr;
                        style.filterCount = 0;
                        break;
                    }

                    Vector<FilterItem> items;

                    if (parseFilterList(doc, value, style, vp, items)) {
                        style.filterCount = (u32)items.length();
                        style.filters = (FilterItem*)l.pool->allocate(sizeof(FilterItem) * items.length());
                        memcpy(style.filters, items.data(), sizeof(FilterItem) * items.length());
                    }

                    break;
                }
                case AtomBackgroundColor:
                    if (keyword) {
                        style.background = source.background;
                    } else if (isKeyword(value, SV("currentcolor"))) {
                        style.background = style.color;
                    } else {
                        Color color;

                        if (parseColor(value, color)) {
                            style.background = color;
                        }
                    }

                    break;
                case AtomOverflow:
                    if (keyword) {
                        style.overflowVisible = source.overflowVisible;
                    } else {
                        style.overflowVisible = isKeyword(value, SV("visible")) || isKeyword(value, SV("auto"));
                    }

                    break;
                default:
                    // AtomMaskType: every property but font-size has its case.
                    if (keyword) {
                        style.maskType = source.maskType;
                    } else if (isKeyword(value, SV("alpha"))) {
                        style.maskType = 1;
                    } else if (isKeyword(value, SV("luminance"))) {
                        style.maskType = 0;
                    }

                    break;
            }
        }

        return style;
    }

    static const ComputedStyle& documentStyle(Pass& l, Node* node, Viewport vp) {
        if (ComputedStyle* known = l.documentStyles[node->index]) {
            return *known;
        }

        ComputedStyle parent = node->parent ? documentStyle(l, node->parent, vp) : initialStyle();
        ComputedStyle* style = l.pool->make<ComputedStyle>(computeStyle(l, node, parent, vp));

        l.documentStyles.mut(node->index) = style;

        return *style;
    }

    static bool passesConditions(const Document& doc, const Node* node);

    static bool resourceUsable(Pass& l, Node* node, Viewport vp) {
        for (Node* up = node; up != l.doc->root; up = up->parent) {
            if (documentStyle(l, up, vp).displayNone || (up != node && !passesConditions(*l.doc, up))) {
                return false;
            }
        }

        return true;
    }

    static void emitEdges(Lowering& l, const Flat& flat, Box& bounds) {
        u32 begin = 0;

        for (u32 end : flat.ends) {
            if (end - begin >= 2) {
                for (u32 i = begin; i < end; i++) {
                    Point a = flat.points[i];
                    Point b = flat.points[i + 1 < end ? i + 1 : begin];

                    extend(bounds, a);

                    if (a.y != b.y) {
                        l.edges.pushBack(Edge{a.x, a.y, b.x, b.y});
                    }
                }
            }

            begin = end;
        }
    }

    static void emitFill(Lowering& l, const Flat& flat, u8 rule, DevicePaint* paint, bool aliased) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Fill;
        op.rule = rule;
        op.aliased = aliased;
        op.paint = paint;
        op.first = (u32)l.edges.length();
        op.bounds = emptyBox();
        emitEdges(l, flat, op.bounds);
        op.count = (u32)l.edges.length() - op.first;

        if (op.count && boxValid(op.bounds)) {
            l.out->pushBack(op);
        }
    }

    static DevicePaint* solidPaint(Lowering& l, Color color, float opacity) {
        float alpha = color.a * opacity;

        if (alpha <= 0.f) {
            return nullptr;
        }

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Solid;
        paint->color = Color{color.r * alpha, color.g * alpha, color.b * alpha, alpha};

        return paint;
    }

    static u32 patternAttribute(const Document& doc, Node* pattern, u32 name, Node** owner) {
        Node* node = pattern;

        for (u32 hops = 0; hops < 16; hops++) {
            if (u32 value = attribute(node, name); value && !trim(textOf(doc, value)).empty()) {
                if (owner) {
                    *owner = node;
                }

                return value;
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomPattern || next == node) {
                break;
            }

            node = next;
        }

        return 0;
    }

    static bool renderable(u32 name);

    static Node* patternContent(const Document& doc, Node* pattern) {
        Node* node = pattern;

        for (u32 hops = 0; hops < 16; hops++) {
            for (Node* child = node->first; child; child = child->next) {
                if (renderable(child->name)) {
                    return node;
                }
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomPattern || next == node) {
                break;
            }

            node = next;
        }

        return nullptr;
    }

    static float patternLength(Lowering& l, Node* pattern, u32 name, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        u32 value = patternAttribute(*l.doc, pattern, name, nullptr);
        Length length;

        if (!value || !parseLength(textOf(*l.doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static bool linkBroken(const Lowering& l, const Node* node, u32 property) {
        u64 key = ((u64)node->index << 32) | property;

        for (u64 link : l.broken) {
            if (link == key) {
                return true;
            }
        }

        return false;
    }

    static bool closesCycle(Lowering& l, const Node* node, u32 property, const Node* resource) {
        for (Node* active : l.active) {
            if (active == resource) {
                l.broken.pushBack(((u64)node->index << 32) | property);

                return true;
            }
        }

        return linkBroken(l, node, property);
    }

    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp);

    static DevicePaint* patternPaint(Lowering& l, Node* pattern, float opacity, Box bbox, const Matrix& ctm, Viewport vp) {
        const Document& doc = *l.doc;

        u32 units = patternAttribute(doc, pattern, AtomPatternUnits, nullptr);
        u32 contentUnits = patternAttribute(doc, pattern, AtomPatternContentUnits, nullptr);
        bool boxUnits = !units || textOf(doc, units) != SV("userSpaceOnUse");
        bool boxContent = contentUnits && textOf(doc, contentUnits) == SV("objectBoundingBox");
        float bw = bbox.x1 - bbox.x0;
        float bh = bbox.y1 - bbox.y0;

        if ((boxUnits || boxContent) && (!boxValid(bbox) || bw <= 0.f || bh <= 0.f)) {
            return nullptr;
        }

        float x = patternLength(l, pattern, AtomX, boxUnits, Axis::X, vp, 0.f);
        float y = patternLength(l, pattern, AtomY, boxUnits, Axis::Y, vp, 0.f);
        float w = patternLength(l, pattern, AtomWidth, boxUnits, Axis::X, vp, 0.f);
        float h = patternLength(l, pattern, AtomHeight, boxUnits, Axis::Y, vp, 0.f);

        if (boxUnits) {
            x = bbox.x0 + x * bw;
            y = bbox.y0 + y * bh;
            w *= bw;
            h *= bh;
        }

        Node* content = patternContent(doc, pattern);

        if (w <= 0.f || h <= 0.f || !content) {
            return nullptr;
        }

        Matrix space = ctm;
        u32 transform = patternAttribute(doc, pattern, AtomPatternTransform, nullptr);
        Matrix local;

        if (transform && parseTransform(textOf(doc, transform), local)) {
            if (u32 origin = attribute(pattern, AtomTransformOrigin)) {
                Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

                local = multiply(multiply(translation(o.x, o.y), local), translation(-o.x, -o.y));
            }

            space = multiply(space, local);
        }

        space = multiply(space, translation(x, y));

        Matrix inner = identity();
        Node* viewBoxOwner = nullptr;
        u32 viewBoxValue = patternAttribute(doc, pattern, AtomViewBox, &viewBoxOwner);
        Box viewBox;

        if (viewBoxValue && parseViewBox(doc, viewBoxOwner, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return nullptr;
            }

            Node* ratioOwner = pattern;

            patternAttribute(doc, pattern, AtomPreserveAspectRatio, &ratioOwner);
            inner = viewBoxTransform(doc, ratioOwner, viewBox, w, h);
        } else if (boxContent) {
            inner = scaling(bw, bh);
        }

        float sx = sqrtf(space.a * space.a + space.b * space.b);
        float sy = sqrtf(space.c * space.c + space.d * space.d);
        float tw = ceilf(w * sx);
        float th = ceilf(h * sy);

        if (!(tw >= 1.f) || !(th >= 1.f)) {
            tw = maxf(tw, 1.f);
            th = maxf(th, 1.f);
        }

        tw = minf(tw, 2048.f);
        th = minf(th, 2048.f);

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Pattern;
        paint->imageWidth = (u32)tw;
        paint->imageHeight = (u32)th;
        paint->opacity = opacity;

        if (!invert(multiply(space, scaling(w / tw, h / th)), paint->inverse)) {
            return nullptr;
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;
        const ComputedStyle& contentStyle = documentStyle(l, content, vp);

        Matrix outerHost = l.host;
        Matrix screenInverse;

        u32 outerWidth = l.width;
        u32 outerHeight = l.height;

        l.out = &program;
        l.active.pushBack(pattern);
        l.host = invert(l.screen, screenInverse) ? multiply(multiply(scaling(tw / w, th / h), inner), screenInverse) : identity();
        l.width = paint->imageWidth;
        l.height = paint->imageHeight;
        lowerChildren(l, content, contentStyle, multiply(scaling(tw / w, th / h), inner), vp);
        l.width = outerWidth;
        l.height = outerHeight;
        l.host = outerHost;
        l.active.popBack();
        l.out = saved;
        paint->programFirst = (u32)l.programs.length();
        paint->programCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return paint;
    }

    static u32 gradientAttribute(const Document& doc, Node* gradient, u32 name) {
        Node* node = gradient;
        bool specific = name == AtomX1 || name == AtomY1 || name == AtomX2 || name == AtomY2 || name == AtomCx || name == AtomCy || name == AtomR || name == AtomFx || name == AtomFy || name == AtomFr;

        for (u32 hops = 0; hops < 16; hops++) {
            if (u32 value = attribute(node, name); value && !trim(textOf(doc, value)).empty() && (!specific || node->name == gradient->name)) {
                return value;
            }

            Node* next = hrefTarget(doc, node);

            if (!next || (next->name != AtomLinearGradient && next->name != AtomRadialGradient) || next == node) {
                break;
            }

            node = next;
        }

        return 0;
    }

    static Node* gradientStops(const Document& doc, Node* gradient) {
        Node* node = gradient;

        for (u32 hops = 0; hops < 16; hops++) {
            for (Node* child = node->first; child; child = child->next) {
                if (child->name == AtomStop) {
                    return node;
                }
            }

            Node* next = hrefTarget(doc, node);

            if (!next || (next->name != AtomLinearGradient && next->name != AtomRadialGradient) || next == node) {
                break;
            }

            node = next;
        }

        return nullptr;
    }

    static float gradientLength(Lowering& l, Node* gradient, u32 name, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        u32 value = gradientAttribute(*l.doc, gradient, name);
        Length length;

        if (!value || !parseLength(textOf(*l.doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static float srgbToLinear(float c);
    static float linearToSrgb(float c);

    static DevicePaint* gradientPaint(Lowering& l, Node* gradient, float opacity, Box bbox, const Matrix& ctm, Viewport vp) {
        const Document& doc = *l.doc;
        u32 units = gradientAttribute(doc, gradient, AtomGradientUnits);
        bool boxUnits = !units || textOf(doc, units) != SV("userSpaceOnUse");
        Node* stopsNode = gradientStops(doc, gradient);
        Vector<GradientStop> stops;

        if (stopsNode) {
            float previous = 0.f;

            for (Node* child = stopsNode->first; child; child = child->next) {
                if (child->name != AtomStop) {
                    continue;
                }

                float offset = 0.f;
                u32 value = attribute(child, AtomOffset);

                if (value) {
                    StringView text = trim(textOf(doc, value));
                    const u8* p = text.begin();

                    if (readNumber(p, text.end(), offset)) {
                        if (at(p, text.end(), '%')) {
                            offset /= 100.f;
                            p++;
                        }

                        if (p != text.end()) {
                            offset = 0.f;
                        }
                    } else {
                        offset = 0.f;
                    }
                }

                offset = maxf(clampUnit(offset), previous);
                previous = offset;

                const ComputedStyle& style = documentStyle(l, child, vp);
                Color color = style.stopColor;

                color.a *= style.stopOpacity * opacity;
                stops.pushBack(GradientStop{offset, color});
            }
        }

        if (stops.length() == 0) {
            return nullptr;
        }

        if (stops.length() == 1) {
            Color color = stops[0].color;

            return solidPaint(l, Color{color.r, color.g, color.b, 1.f}, color.a);
        }

        Matrix space = ctm;
        Matrix gradientMatrix = identity();
        u32 transform = gradientAttribute(doc, gradient, AtomGradientTransform);

        if (transform) {
            Matrix m;

            if (parseTransform(textOf(doc, transform), m)) {
                if (u32 origin = attribute(gradient, AtomTransformOrigin)) {
                    Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

                    m = multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
                }

                gradientMatrix = m;
            }
        }

        if (boxUnits) {
            float bw = bbox.x1 - bbox.x0;
            float bh = bbox.y1 - bbox.y0;

            if (!boxValid(bbox) || bw <= 0.f || bh <= 0.f) {
                return nullptr;
            }

            space = multiply(space, Matrix{bw, 0.f, 0.f, bh, bbox.x0, bbox.y0});
        }

        space = multiply(space, gradientMatrix);

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));

        if (!invert(space, paint->inverse)) {
            return nullptr;
        }

        paint->stopCount = (u32)stops.length();
        paint->stops = (GradientStop*)l.pool->allocate(sizeof(GradientStop) * stops.length());
        memcpy(paint->stops, stops.data(), sizeof(GradientStop) * stops.length());
        paint->linear = documentStyle(l, gradient, vp).linearColors;

        if (paint->linear) {
            for (u32 i = 0; i < paint->stopCount; i++) {
                Color& c = paint->stops[i].color;

                c = Color{srgbToLinear(c.r), srgbToLinear(c.g), srgbToLinear(c.b), c.a};
            }
        }

        u32 spread = gradientAttribute(doc, gradient, AtomSpreadMethod);

        paint->spread = SpreadPad;

        if (spread && textOf(doc, spread) == SV("reflect")) {
            paint->spread = SpreadReflect;
        } else if (spread && textOf(doc, spread) == SV("repeat")) {
            paint->spread = SpreadRepeat;
        }

        Color last = stops.back().color;

        if (gradient->name == AtomLinearGradient) {
            paint->type = PaintType::Linear;
            paint->x1 = gradientLength(l, gradient, AtomX1, boxUnits, Axis::X, vp, 0.f);
            paint->y1 = gradientLength(l, gradient, AtomY1, boxUnits, Axis::Y, vp, 0.f);
            paint->x2 = gradientLength(l, gradient, AtomX2, boxUnits, Axis::X, vp, boxUnits ? 1.f : vp.width);
            paint->y2 = gradientLength(l, gradient, AtomY2, boxUnits, Axis::Y, vp, 0.f);

            if (paint->x1 == paint->x2 && paint->y1 == paint->y2) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }
        } else {
            paint->type = PaintType::Radial;
            paint->cx = gradientLength(l, gradient, AtomCx, boxUnits, Axis::X, vp, boxUnits ? 0.5f : vp.width / 2.f);
            paint->cy = gradientLength(l, gradient, AtomCy, boxUnits, Axis::Y, vp, boxUnits ? 0.5f : vp.height / 2.f);
            paint->r = gradientLength(l, gradient, AtomR, boxUnits, Axis::Other, vp, boxUnits ? 0.5f : resolveLength(Length{50.f, Unit::Percent}, Axis::Other, vp, 16.f));
            paint->fx = gradientLength(l, gradient, AtomFx, boxUnits, Axis::X, vp, paint->cx);
            paint->fy = gradientLength(l, gradient, AtomFy, boxUnits, Axis::Y, vp, paint->cy);
            paint->fr = maxf(gradientLength(l, gradient, AtomFr, boxUnits, Axis::Other, vp, 0.f), 0.f);

            if (paint->r <= 1.f / 32768.f) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }

            if (fabsf(paint->fr - paint->r) < 1e-6f * maxf(1.f, paint->r) && paint->fx == paint->cx && paint->fy == paint->cy && paint->spread == SpreadPad) {
                Color first = stops[0].color;
                GradientStop* ring = (GradientStop*)l.pool->allocate(sizeof(GradientStop) * 3);

                ring[0] = GradientStop{0.f, first};
                ring[1] = GradientStop{1.f, first};
                ring[2] = GradientStop{1.f, last};
                paint->stops = ring;
                paint->stopCount = 3;
                paint->fr = 0.f;
            }
        }

        return paint;
    }

    static bool degenerateBox(const Document& doc, Node* gradient, const Box& bbox) {
        u32 units = gradientAttribute(doc, gradient, AtomGradientUnits);
        bool boxUnits = !units || textOf(doc, units) != SV("userSpaceOnUse");

        return boxUnits && (!boxValid(bbox) || bbox.x1 - bbox.x0 <= 0.f || bbox.y1 - bbox.y0 <= 0.f);
    }

    static bool singularTransform(const Document& doc, Node* server) {
        u32 value = server->name == AtomPattern ? patternAttribute(doc, server, AtomPatternTransform, nullptr) : gradientAttribute(doc, server, AtomGradientTransform);
        Matrix m;
        Matrix inverse;

        return value && parseTransform(textOf(doc, value), m) && !invert(m, inverse);
    }

    static u32 meshAttribute(const Document& doc, Node* mesh, u32 name) {
        Node* node = mesh;

        for (u32 hops = 0; hops < 16; hops++) {
            if (u32 value = attribute(node, name); value && !trim(textOf(doc, value)).empty()) {
                return value;
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomMeshGradient || next == node) {
                break;
            }

            node = next;
        }

        return 0;
    }

    static Node* meshRows(const Document& doc, Node* mesh) {
        Node* node = mesh;

        for (u32 hops = 0; hops < 16; hops++) {
            for (Node* child = node->first; child; child = child->next) {
                if (child->name == AtomMeshRow) {
                    return node;
                }
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomMeshGradient || next == node) {
                break;
            }

            node = next;
        }

        return nullptr;
    }

    static bool meshEdge(StringView text, Point start, Point* out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        skipSpaces(p, end);

        if (p >= end) {
            return false;
        }

        u8 command = *p++;
        bool relative = command == 'c' || command == 'l';
        float v[6];
        int count = command == 'c' || command == 'C' ? 6 : (command == 'l' || command == 'L' ? 2 : 0);

        for (int k = 0; k < count; k++) {
            skipSeparator(p, end);

            if (!readNumber(p, end, v[k])) {
                return false;
            }

            if (relative) {
                v[k] += k % 2 ? start.y : start.x;
            }
        }

        out[0] = start;

        if (count == 6) {
            out[1] = Point{v[0], v[1]};
            out[2] = Point{v[2], v[3]};
            out[3] = Point{v[4], v[5]};
        } else if (count == 2) {
            out[3] = Point{v[0], v[1]};
            out[1] = Point{start.x + (out[3].x - start.x) / 3.f, start.y + (out[3].y - start.y) / 3.f};
            out[2] = Point{start.x + (out[3].x - start.x) * 2.f / 3.f, start.y + (out[3].y - start.y) * 2.f / 3.f};
        } else {
            return false;
        }

        return true;
    }

    static Point bezierAt(const Point* c, float t) {
        float s = 1.f - t;
        float a = s * s * s;
        float b = 3.f * s * s * t;
        float d = 3.f * s * t * t;
        float e = t * t * t;

        return Point{a * c[0].x + b * c[1].x + d * c[2].x + e * c[3].x, a * c[0].y + b * c[1].y + d * c[2].y + e * c[3].y};
    }

    struct MeshVertex {
        Point p;
        float c[4];
    };

    static float meshSlope(float before, float value, float after, float left, float right) {
        if (left <= 0.f || right <= 0.f) {
            return 0.f;
        }

        float a = (value - before) / left;
        float b = (after - value) / right;

        if (a * b <= 0.f) {
            return 0.f;
        }

        float magnitude = minf(fabsf((a + b) * 0.5f), 3.f * minf(fabsf(a), fabsf(b)));

        return a < 0.f ? -magnitude : magnitude;
    }

    static void meshSlopes(const Vector<Point>& points, const Vector<Color>& colors, u32 lines, u32 count, u32 stride, u32 step, Vector<Color>& slopes) {
        slopes.zero(colors.length());

        for (u32 line = 0; line < lines; line++) {
            u32 first = line * stride;

            for (u32 i = 1; i + 1 < count; i++) {
                u32 k = first + i * step;
                float left = hypotf(points[k].x - points[k - step].x, points[k].y - points[k - step].y);
                float right = hypotf(points[k + step].x - points[k].x, points[k + step].y - points[k].y);
                const Color& a = colors[k - step];
                const Color& b = colors[k];
                const Color& c = colors[k + step];

                slopes.mutData()[k] = Color{meshSlope(a.r, b.r, c.r, left, right), meshSlope(a.g, b.g, c.g, left, right), meshSlope(a.b, b.b, c.b, left, right), 0.f};
            }

            // Fit a parabola at each boundary using the adjacent interior derivative.
            for (u32 end = 0; end < 2; end++) {
                u32 k = first + (end ? count - 1 : 0) * step;
                u32 neighbor = end ? k - step : k + step;
                float length = hypotf(points[k].x - points[neighbor].x, points[k].y - points[neighbor].y);

                if (length > 0.f) {
                    float scale = (end ? -2.f : 2.f) / length;
                    const Color& a = colors[k];
                    const Color& b = colors[neighbor];
                    const Color& d = slopes[neighbor];

                    slopes.mutData()[k] = Color{scale * (b.r - a.r) - d.r, scale * (b.g - a.g) - d.g, scale * (b.b - a.b) - d.b, 0.f};
                }
            }
        }
    }

    static void meshBasis(float t, float* basis) {
        float square = t * t;
        float cube = square * t;

        basis[0] = 2.f * cube - 3.f * square + 1.f;
        basis[1] = 1.f - basis[0];
        basis[2] = cube - 2.f * square + t;
        basis[3] = cube - square;
    }

    static void meshColors(const Color* colors, const Point* points, const Color* horizontal, const Color* vertical, Color* grid) {
        float widths[2] = {hypotf(points[1].x - points[0].x, points[1].y - points[0].y), hypotf(points[3].x - points[2].x, points[3].y - points[2].y)};
        float heights[2] = {hypotf(points[2].x - points[0].x, points[2].y - points[0].y), hypotf(points[3].x - points[1].x, points[3].y - points[1].y)};

        // Inkscape's bicubic extension samples Hermite colors on an 8 by 8 subdivision.
        for (int y = 0; y <= 8; y++) {
            float v[4];

            meshBasis((float)y / 8.f, v);

            for (int x = 0; x <= 8; x++) {
                float u[4];
                Color color{};

                meshBasis((float)x / 8.f, u);

                for (int k = 0; k < 4; k++) {
                    int column = k % 2;
                    int row = k / 2;
                    float weight = u[column] * v[row];
                    float dx = u[column + 2] * v[row] * widths[row];
                    float dy = u[column] * v[row + 2] * heights[column];

                    color.r += colors[k].r * weight + horizontal[k].r * dx + vertical[k].r * dy;
                    color.g += colors[k].g * weight + horizontal[k].g * dx + vertical[k].g * dy;
                    color.b += colors[k].b * weight + horizontal[k].b * dx + vertical[k].b * dy;
                }

                grid[y * 9 + x] = Color{clampUnit(color.r), clampUnit(color.g), clampUnit(color.b), 0.f};
            }
        }
    }

    static void meshTriangle(float* buffer, int width, int height, const MeshVertex& a, const MeshVertex& b, const MeshVertex& c) {
        float area = (b.p.x - a.p.x) * (c.p.y - a.p.y) - (c.p.x - a.p.x) * (b.p.y - a.p.y);

        if (fabsf(area) < 1e-12f) {
            return;
        }

        int x0 = (int)floorf(minf(a.p.x, minf(b.p.x, c.p.x)));
        int y0 = (int)floorf(minf(a.p.y, minf(b.p.y, c.p.y)));
        int x1 = (int)ceilf(maxf(a.p.x, maxf(b.p.x, c.p.x)));
        int y1 = (int)ceilf(maxf(a.p.y, maxf(b.p.y, c.p.y)));

        x0 = x0 < 0 ? 0 : x0;
        y0 = y0 < 0 ? 0 : y0;
        x1 = x1 > width ? width : x1;
        y1 = y1 > height ? height : y1;

        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                float px = (float)x + 0.5f;
                float py = (float)y + 0.5f;
                float wa = ((b.p.x - px) * (c.p.y - py) - (c.p.x - px) * (b.p.y - py)) / area;
                float wb = ((c.p.x - px) * (a.p.y - py) - (a.p.x - px) * (c.p.y - py)) / area;
                float wc = 1.f - wa - wb;

                if (wa < 0.f || wb < 0.f || wc < 0.f || (wa == 0.f && wb == 0.f)) {
                    continue;
                }

                float* d = buffer + ((size_t)y * width + x) * 4;

                for (int k = 0; k < 4; k++) {
                    d[k] = wa * a.c[k] + wb * b.c[k] + wc * c.c[k];
                }
            }
        }
    }

    static DevicePaint* meshPaint(Lowering& l, Node* mesh, float opacity, Box bbox, const Matrix& ctm, Viewport vp) {
        const Document& doc = *l.doc;
        Node* content = meshRows(doc, mesh);

        if (!content) {
            return nullptr;
        }

        u32 rows = 0;
        u32 columns = 0;

        for (Node* row = content->first; row; row = row->next) {
            if (row->name != AtomMeshRow) {
                continue;
            }

            u32 count = 0;

            for (Node* patch = row->first; patch; patch = patch->next) {
                count += patch->name == AtomMeshPatch;
            }

            if (rows == 0) {
                columns = count;
            } else if (count != columns) {
                return nullptr;
            }

            rows++;
        }

        // meshRows found a row.
        if (!columns || rows * columns > 4096) {
            return nullptr;
        }

        u32 units = meshAttribute(doc, mesh, AtomGradientUnits);
        bool boxUnits = !units || textOf(doc, units) != SV("userSpaceOnUse");
        Matrix space = ctm;

        if (boxUnits) {
            float bw = bbox.x1 - bbox.x0;
            float bh = bbox.y1 - bbox.y0;

            if (!boxValid(bbox) || bw <= 0.f || bh <= 0.f) {
                return nullptr;
            }

            space = multiply(space, Matrix{bw, 0.f, 0.f, bh, bbox.x0, bbox.y0});
        }

        if (u32 transform = meshAttribute(doc, mesh, AtomGradientTransform)) {
            Matrix m;

            if (parseTransform(textOf(doc, transform), m)) {
                space = multiply(space, m);
            }
        }

        u32 stride = columns + 1;
        Vector<Point> corners;
        Vector<Color> colors;
        Vector<Point> horizontal;
        Vector<Point> vertical;

        corners.zero((size_t)(rows + 1) * stride);
        colors.zero((size_t)(rows + 1) * stride);
        horizontal.zero((size_t)(rows + 1) * columns * 4);
        vertical.zero((size_t)rows * stride * 4);

        Length length;
        u32 xValue = meshAttribute(doc, mesh, AtomX);
        u32 yValue = meshAttribute(doc, mesh, AtomY);

        corners.mutData()[0] = Point{xValue && parseLength(textOf(doc, xValue), length) ? resolveLength(length, Axis::X, vp, 16.f) : 0.f, yValue && parseLength(textOf(doc, yValue), length) ? resolveLength(length, Axis::Y, vp, 16.f) : 0.f};

        u32 i = 0;

        for (Node* row = content->first; row; row = row->next) {
            if (row->name != AtomMeshRow) {
                continue;
            }

            u32 j = 0;

            for (Node* patch = row->first; patch; patch = patch->next) {
                if (patch->name != AtomMeshPatch) {
                    continue;
                }

                int edge = i == 0 ? 0 : 1;
                int last = j == 0 ? 3 : 2;

                for (Node* stop = patch->first; stop && edge <= last; stop = stop->next) {
                    if (stop->name != AtomStop) {
                        continue;
                    }

                    u32 starts[4] = {i * stride + j, i * stride + j + 1, (i + 1) * stride + j + 1, (i + 1) * stride + j};
                    u32 start = starts[edge];
                    u32 finish = starts[(edge + 1) % 4];
                    u32 path = attribute(stop, AtomPath);
                    Point curve[4];

                    if (!path || !meshEdge(textOf(doc, path), corners[start], curve)) {
                        return nullptr;
                    }

                    if (!cascaded(doc, stop, AtomStopColor).empty() || !cascaded(doc, stop, AtomStopOpacity).empty()) {
                        const ComputedStyle& stopStyle = documentStyle(l, stop, vp);
                        Color color = stopStyle.stopColor;

                        color.a *= stopStyle.stopOpacity;
                        colors.mutData()[start] = color;
                    }

                    corners.mutData()[finish] = curve[3];

                    Point* target;

                    if (edge == 0) {
                        target = horizontal.mutData() + ((size_t)i * columns + j) * 4;
                    } else if (edge == 1) {
                        target = vertical.mutData() + ((size_t)i * stride + j + 1) * 4;
                    } else if (edge == 2) {
                        target = horizontal.mutData() + ((size_t)(i + 1) * columns + j) * 4;
                    } else {
                        target = vertical.mutData() + ((size_t)i * stride + j) * 4;
                    }

                    if (edge < 2) {
                        for (int k = 0; k < 4; k++) {
                            target[k] = curve[k];
                        }
                    } else {
                        for (int k = 0; k < 4; k++) {
                            target[k] = curve[3 - k];
                        }
                    }

                    edge++;
                }

                if (edge <= last) {
                    return nullptr;
                }

                j++;
            }

            i++;
        }

        u32 type = meshAttribute(doc, mesh, AtomType);
        bool bicubic = type && textOf(doc, type) == SV("bicubic");
        Vector<Color> horizontalSlopes;
        Vector<Color> verticalSlopes;

        if (bicubic) {
            meshSlopes(corners, colors, rows + 1, columns + 1, stride, 1, horizontalSlopes);
            meshSlopes(corners, colors, columns + 1, rows + 1, 1, stride, verticalSlopes);
        }

        Box device = emptyBox();

        for (Point point : horizontal) {
            extend(device, apply(space, point));
        }

        for (Point point : vertical) {
            extend(device, apply(space, point));
        }

        int x0 = (int)floorf(maxf(device.x0, 0.f));
        int y0 = (int)floorf(maxf(device.y0, 0.f));
        int x1 = (int)ceilf(minf(device.x1, (float)l.width));
        int y1 = (int)ceilf(minf(device.y1, (float)l.height));

        if (x1 <= x0 || y1 <= y0) {
            return nullptr;
        }

        int width = x1 - x0;
        int height = y1 - y0;
        float* buffer = (float*)l.pool->allocate(sizeof(float) * (size_t)width * height * 4);
        Matrix local = multiply(translation(-(float)x0, -(float)y0), space);

        memset(buffer, 0, sizeof(float) * (size_t)width * height * 4);

        for (u32 r = 0; r < rows; r++) {
            for (u32 c = 0; c < columns; c++) {
                const Point* top = horizontal.data() + ((size_t)r * columns + c) * 4;
                const Point* bottom = horizontal.data() + ((size_t)(r + 1) * columns + c) * 4;
                const Point* left = vertical.data() + ((size_t)r * stride + c) * 4;
                const Point* right = vertical.data() + ((size_t)r * stride + c + 1) * 4;
                Color corner[4] = {colors[r * stride + c], colors[r * stride + c + 1], colors[(r + 1) * stride + c], colors[(r + 1) * stride + c + 1]};
                Color smooth[81];
                float extent = 0.f;

                if (bicubic) {
                    u32 index[4] = {r * stride + c, r * stride + c + 1, (r + 1) * stride + c, (r + 1) * stride + c + 1};
                    Point points[4];
                    Color du[4];
                    Color dv[4];

                    for (int k = 0; k < 4; k++) {
                        points[k] = corners[index[k]];
                        du[k] = horizontalSlopes[index[k]];
                        dv[k] = verticalSlopes[index[k]];
                    }

                    meshColors(corner, points, du, dv, smooth);
                }

                for (int k = 0; k < 3; k++) {
                    const Point* edges[4] = {top, bottom, left, right};

                    for (const Point* e : edges) {
                        Point a = apply(local, e[k]);
                        Point b = apply(local, e[k + 1]);

                        extent = maxf(extent, fabsf(a.x - b.x) + fabsf(a.y - b.y));
                    }
                }

                int n = (int)ceilf(extent * 3.f / 2.f);

                n = n < 1 ? 1 : (n > 128 ? 128 : n);

                if (bicubic) {
                    n = (n + 7) / 8 * 8;
                }

                Vector<MeshVertex> grid;

                grid.zero((size_t)(n + 1) * (n + 1));

                for (int a = 0; a <= n; a++) {
                    float v = (float)a / (float)n;
                    Point l0 = bezierAt(left, v);
                    Point r0 = bezierAt(right, v);

                    for (int b = 0; b <= n; b++) {
                        float u = (float)b / (float)n;
                        Point t0 = bezierAt(top, u);
                        Point b0 = bezierAt(bottom, u);
                        Point p{
                            (1.f - v) * t0.x + v * b0.x + (1.f - u) * l0.x + u * r0.x - ((1.f - u) * (1.f - v) * top[0].x + u * (1.f - v) * top[3].x + (1.f - u) * v * bottom[0].x + u * v * bottom[3].x),
                            (1.f - v) * t0.y + v * b0.y + (1.f - u) * l0.y + u * r0.y - ((1.f - u) * (1.f - v) * top[0].y + u * (1.f - v) * top[3].y + (1.f - u) * v * bottom[0].y + u * v * bottom[3].y),
                        };
                        MeshVertex& vertex = grid.mutData()[(size_t)a * (n + 1) + b];
                        float w[4] = {(1.f - u) * (1.f - v), u * (1.f - v), (1.f - u) * v, u * v};
                        float color[4] = {0.f, 0.f, 0.f, 0.f};

                        for (int k = 0; k < 4; k++) {
                            color[0] += corner[k].r * w[k];
                            color[1] += corner[k].g * w[k];
                            color[2] += corner[k].b * w[k];
                            color[3] += corner[k].a * w[k];
                        }

                        if (bicubic) {
                            int x = b == n ? 7 : b * 8 / n;
                            int y = a == n ? 7 : a * 8 / n;
                            float fx = u * 8.f - (float)x;
                            float fy = v * 8.f - (float)y;
                            float weights[4] = {(1.f - fx) * (1.f - fy), fx * (1.f - fy), (1.f - fx) * fy, fx * fy};

                            color[0] = 0.f;
                            color[1] = 0.f;
                            color[2] = 0.f;

                            for (int k = 0; k < 4; k++) {
                                const Color& sample = smooth[(y + k / 2) * 9 + x + k % 2];

                                color[0] += sample.r * weights[k];
                                color[1] += sample.g * weights[k];
                                color[2] += sample.b * weights[k];
                            }
                        }

                        vertex.p = apply(local, p);
                        vertex.c[0] = color[0] * color[3] * opacity;
                        vertex.c[1] = color[1] * color[3] * opacity;
                        vertex.c[2] = color[2] * color[3] * opacity;
                        vertex.c[3] = color[3] * opacity;
                    }
                }

                for (int a = 0; a < n; a++) {
                    for (int b = 0; b < n; b++) {
                        const MeshVertex& p00 = grid[(size_t)a * (n + 1) + b];
                        const MeshVertex& p01 = grid[(size_t)a * (n + 1) + b + 1];
                        const MeshVertex& p10 = grid[(size_t)(a + 1) * (n + 1) + b];
                        const MeshVertex& p11 = grid[(size_t)(a + 1) * (n + 1) + b + 1];

                        meshTriangle(buffer, width, height, p00, p01, p11);
                        meshTriangle(buffer, width, height, p00, p11, p10);
                    }
                }
            }
        }

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Mesh;
        paint->tile = buffer;
        paint->imageWidth = (u32)width;
        paint->imageHeight = (u32)height;
        paint->inverse = translation(-(float)x0, -(float)y0);
        paint->opacity = 1.f;

        return paint;
    }

    static DevicePaint* devicePaint(Lowering& l, Node* node, u32 property, const PaintValue& value, float opacity, const ComputedStyle& style, Box bbox, const Matrix& ctm, Viewport vp) {
        switch (value.kind) {
            case PaintKind::None:
                return nullptr;
            case PaintKind::Color:
                return solidPaint(l, value.color, opacity);
            case PaintKind::CurrentColor:
                return solidPaint(l, style.color, opacity);
            default: {
                // PaintKind::Server.
                Node* server = elementById(*l.doc, value.server);

                bool gradient = server && (server->name == AtomLinearGradient || server->name == AtomRadialGradient);
                bool pattern = server && server->name == AtomPattern && !closesCycle(l, node, property, server);

                if ((gradient || pattern) && resourceUsable(l, server, vp) && !singularTransform(*l.doc, server)) {
                    DevicePaint* paint = gradient ? gradientPaint(l, server, opacity, bbox, ctm, vp) : patternPaint(l, server, opacity, bbox, ctm, vp);

                    if (paint || (gradient && !degenerateBox(*l.doc, server, bbox))) {
                        return paint;
                    }
                }

                if (server && server->name == AtomMeshGradient && resourceUsable(l, server, vp)) {
                    if (DevicePaint* paint = meshPaint(l, server, opacity, bbox, ctm, vp)) {
                        return paint;
                    }
                }

                if (value.fallback == PaintKind::Color) {
                    return solidPaint(l, value.fallbackColor, opacity);
                }

                if (value.fallback == PaintKind::CurrentColor) {
                    return solidPaint(l, style.color, opacity);
                }

                return nullptr;
            }
        }
    }

    static bool renderable(u32 name) {
        switch (name) {
            case AtomSvg:
            case AtomG:
            case AtomA:
            case AtomSwitch:
            case AtomUse:
            case AtomPath:
            case AtomRect:
            case AtomCircle:
            case AtomEllipse:
            case AtomLine:
            case AtomPolyline:
            case AtomPolygon:
            case AtomImage:
                return true;
            default:
                return false;
        }
    }

    static bool isShape(u32 name) {
        return name == AtomPath || name == AtomRect || name == AtomCircle || name == AtomEllipse || name == AtomLine || name == AtomPolyline || name == AtomPolygon;
    }

    static bool isAncestor(const Node* ancestor, const Node* node) {
        for (const Node* up = node; up; up = up->parent) {
            if (up == ancestor) {
                return true;
            }
        }

        return false;
    }

    // How an extent counts. Tight, the geometry of children goes through
    // their transforms, as a document's size has it; loose, their boxes do,
    // as browsers have an object bounding box. Markers count only for a
    // document's size.
    struct Extent {
        bool tight;
        bool markers;
    };

    struct ImageSource {
        Document* nested;
        SvgImage* raster;
        float width;
        float height;
        Box viewBox;
        Node* ratio;
    };

    static void elementExtent(Pass& p, Node* node, const ComputedStyle& parent, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box);
    static void markerExtent(Pass& p, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box);
    static bool imageReachable(const Document& doc, const Node* node);
    static bool isAuto(const Document& doc, const Node* node, u32 name);
    static bool imageRect(Pass& p, Node* node, const ComputedStyle& style, Viewport vp, ImageSource& source, float& x, float& y, float& w, float& h);
    static Matrix boxTransform(Pass& p, Node* node, const ComputedStyle& parent, Viewport vp);
    static Box deviceBox(const Matrix& m, const Box& box);
    static bool givenSide(const Document& doc, const Node* node, u32 name);
    static void beginPass(Pass& p, Document& doc, ObjPool& pool, SvgDecode decode, SvgFail fail, void* context);
    static void measureRoot(Pass& p, float& width, float& height);

    // Adds a box, through a matrix, to another.
    static void extendBy(Box& box, const Matrix& m, const Box& inner) {
        Box placed = deviceBox(m, inner);

        extend(box, Point{placed.x0, placed.y0});
        extend(box, Point{placed.x1, placed.y1});
    }

    // Adds to a box what a child draws, placed by a matrix and its own
    // transform and, for a use, its x and y.
    static void childExtent(Pass& p, Node* child, const Matrix& m, const ComputedStyle& inherited, Viewport vp, Extent how, u32 depth, Box& box) {
        Matrix full = multiply(m, boxTransform(p, child, inherited, vp));

        if (child->name == AtomUse) {
            full = multiply(full, translation(lengthOr(*p.doc, child, AtomX, Axis::X, vp, inherited.fontSize, 0.f), lengthOr(*p.doc, child, AtomY, Axis::Y, vp, inherited.fontSize, 0.f)));
        }

        if (how.tight) {
            elementExtent(p, child, inherited, full, vp, how, depth + 1, box);

            return;
        }

        Box own = emptyBox();

        elementExtent(p, child, inherited, identity(), vp, how, depth + 1, own);

        if (boxValid(own)) {
            extendBy(box, full, own);
        }
    }

    // Adds to a box what an svg or symbol element of a size shows: its
    // children under its x, y and viewBox, placed by a matrix. A viewport
    // of no size shows nothing. A side left open, a percentage of an open
    // side while a document is measured, does not clip: the content counts
    // whole where it has geometry of its own, though a viewBox then has
    // nothing to fit to.
    static void viewportExtent(Pass& p, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, float x, float y, float w, float h, bool openWidth, bool openHeight, Extent how, u32 depth, Box& box) {
        const Document& doc = *p.doc;
        Matrix content = multiply(m, translation(x, y));
        Viewport inside{w, h, vp.screenWidth, vp.screenHeight, p.rootFontSize};
        Box viewBox;

        if ((w <= 0.f && !openWidth) || (h <= 0.f && !openHeight)) {
            return;
        }

        if (parseViewBox(doc, node, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0 || w <= 0.f || h <= 0.f) {
                return;
            }

            content = multiply(content, viewBoxTransform(doc, node, viewBox, w, h));
            inside.width = viewBox.x1 - viewBox.x0;
            inside.height = viewBox.y1 - viewBox.y0;
        }

        for (Node* child = node->first; child; child = child->next) {
            childExtent(p, child, content, style, inside, how, depth, box);
        }
    }

    // Adds to a box, in the space a matrix maps to, what an element draws:
    // the exact bounds of its shape, the rectangle of its image, or its
    // children placed in it. It is a box of geometry: hidden content counts,
    // clipped content counts whole, strokes, filters and masks do not.
    static void elementExtent(Pass& p, Node* node, const ComputedStyle& parent, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box) {
        const Document& doc = *p.doc;

        if (depth > 32 || !renderable(node->name) || (node != doc.root && !passesConditions(doc, node))) {
            return;
        }

        ComputedStyle style = computeStyle(p, node, parent, vp);
        float fs = style.fontSize;

        if (style.displayNone) {
            return;
        }

        if (isShape(node->name)) {
            Path path;

            if (!buildShape(p, node, style, vp, path)) {
                return;
            }

            // A built path begins with a move, so its box is never empty.
            Box shape = pathBox(path, m);

            extend(box, Point{shape.x0, shape.y0});
            extend(box, Point{shape.x1, shape.y1});

            if (how.markers) {
                markerExtent(p, node, path, style, m, vp, how, depth, box);
            }

            return;
        }

        if (node->name == AtomImage) {
            float x = lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f);
            float y = lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f);
            float w = lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
            float h = lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);

            // A side left auto is the source's.
            if (isAuto(doc, node, AtomWidth) || isAuto(doc, node, AtomHeight)) {
                ImageSource source;

                if (!imageReachable(doc, node) || !imageRect(p, node, style, vp, source, x, y, w, h)) {
                    return;
                }
            }

            if (w > 0.f && h > 0.f) {
                extendBy(box, m, Box{x, y, x + w, y + h});
            }

            return;
        }

        if (node->name == AtomSvg && node != doc.root) {
            viewportExtent(p, node, style, m, vp, lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f), lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, vp.width), lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, vp.height), vp.width <= 0.f && !givenSide(doc, node, AtomWidth), vp.height <= 0.f && !givenSide(doc, node, AtomHeight), how, depth, box);

            return;
        }

        if (node->name == AtomUse) {
            Node* target = hrefTarget(doc, node);
            bool entered = false;

            for (Node* active : p.active) {
                entered = entered || active == target;
            }

            if (!target || entered || isAncestor(target, node)) {
                return;
            }

            p.active.pushBack(target);

            if (target->name == AtomSymbol || target->name == AtomSvg) {
                // The use sizes the viewport it instances.
                ComputedStyle targetStyle = computeStyle(p, target, style, vp);
                float tfs = targetStyle.fontSize;
                bool svg = target->name == AtomSvg;
                float w = lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, lengthOr(doc, target, AtomWidth, Axis::X, vp, tfs, vp.width));
                float h = lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, lengthOr(doc, target, AtomHeight, Axis::Y, vp, tfs, vp.height));

                if (!targetStyle.displayNone && passesConditions(doc, target)) {
                    bool openWidth = vp.width <= 0.f && !givenSide(doc, node, AtomWidth) && !givenSide(doc, target, AtomWidth);
                    bool openHeight = vp.height <= 0.f && !givenSide(doc, node, AtomHeight) && !givenSide(doc, target, AtomHeight);

                    viewportExtent(p, target, targetStyle, multiply(m, boxTransform(p, target, style, vp)), vp, svg ? lengthOr(doc, target, AtomX, Axis::X, vp, tfs, 0.f) : 0.f, svg ? lengthOr(doc, target, AtomY, Axis::Y, vp, tfs, 0.f) : 0.f, w, h, openWidth, openHeight, how, depth, box);
                }
            } else {
                childExtent(p, target, m, style, vp, how, depth, box);
            }

            p.active.popBack();

            return;
        }

        if (node->name == AtomSwitch) {
            for (Node* child = node->first; child; child = child->next) {
                if (passesConditions(doc, child)) {
                    childExtent(p, child, m, style, vp, how, depth, box);
                    break;
                }
            }

            return;
        }

        for (Node* child = node->first; child; child = child->next) {
            childExtent(p, child, m, style, vp, how, depth, box);
        }
    }

    // The object bounding box of an element in its own user space, before
    // its transform; false for an element of no geometry.
    static bool elementBox(Pass& p, Node* node, const ComputedStyle& parent, Viewport vp, Box& out, u32 depth) {
        Box box = emptyBox();

        elementExtent(p, node, parent, identity(), vp, Extent{false, false}, depth, box);
        out = box;

        return boxValid(box);
    }

    static Op groupOp(float opacity) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Begin;
        op.opacity = opacity;

        return op;
    }

    // The bounds of the ops from one on, which end the group, in device
    // pixels; a nested group counts by its own bounds.
    static Box groupBounds(const Vector<Op>& ops, u32 from) {
        Box box = emptyBox();
        u32 depth = 0;

        // The group's own End is yet to come.
        for (u32 i = from; i < ops.length(); i++) {
            const Op& op = ops[i];

            if (op.kind == OpKind::End) {
                depth--;
                continue;
            }

            if (depth == 0) {
                extend(box, Point{op.bounds.x0, op.bounds.y0});
                extend(box, Point{op.bounds.x1, op.bounds.y1});
            }

            if (op.kind == OpKind::Begin) {
                depth++;
            }
        }

        return box;
    }

    // Ends the group begun at the index: its bounds are its filters' regions,
    // or else the bounds of what it drew.
    static void endGroup(Lowering& l, u32 begin) {
        Box bounds = emptyBox();
        const Filter* filter = (*l.out)[begin].filter;

        if (filter) {
            for (const Filter* f = filter; f; f = f->next) {
                extend(bounds, Point{f->device.x0, f->device.y0});
                extend(bounds, Point{f->device.x1, f->device.y1});
            }
        } else {
            bounds = groupBounds(*l.out, begin + 1);
        }

        l.out->mut(begin).bounds = bounds;

        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::End;
        l.out->pushBack(op);
    }

    static void addClipShape(Lowering& l, const Path& path, const Matrix& m, u8 rule) {
        Flat flat;
        Box bounds = emptyBox();
        ClipShape shape;

        flatten(path, m, tolerance, flat);
        shape.first = (u32)l.edges.length();
        emitEdges(l, flat, bounds);
        shape.count = (u32)l.edges.length() - shape.first;
        shape.rule = rule;
        l.clips.pushBack(shape);
    }

    static void addRectClip(Lowering& l, const Matrix& m, float x, float y, float w, float h) {
        Path path;

        moveTo(path, Point{x, y});
        lineTo(path, Point{x + w, y});
        lineTo(path, Point{x + w, y + h});
        lineTo(path, Point{x, y + h});
        closePath(path);
        addClipShape(l, path, m, WindingNonZero);
    }

    struct ClipChild {
        Node* shape;
        Matrix space;
        ComputedStyle style;
    };

    static bool clipChild(Lowering& l, Node* child, const Matrix& space, Viewport vp, ClipChild& out) {
        const Document& doc = *l.doc;

        out.shape = child;
        out.space = multiply(space, transformOf(doc, child, AtomTransform));

        if (child->name == AtomUse) {
            out.shape = hrefTarget(doc, child);

            if (!out.shape || !isShape(out.shape->name)) {
                return false;
            }

            const ComputedStyle& useStyle = documentStyle(l, child, vp);

            if (useStyle.displayNone) {
                return false;
            }

            out.space = multiply(out.space, translation(lengthOr(doc, child, AtomX, Axis::X, vp, useStyle.fontSize, 0.f), lengthOr(doc, child, AtomY, Axis::Y, vp, useStyle.fontSize, 0.f)));
            out.space = multiply(out.space, transformOf(doc, out.shape, AtomTransform));
            out.style = computeStyle(l, out.shape, useStyle, vp);
        } else if (isShape(child->name)) {
            out.style = documentStyle(l, child, vp);
        } else {
            return false;
        }

        return !out.style.displayNone && out.style.visible;
    }

    static bool clipNeedsProgram(Lowering& l, Node* clip, Viewport vp) {
        if (documentStyle(l, clip, vp).clipPath) {
            return true;
        }

        for (Node* child = clip->first; child; child = child->next) {
            if ((child->name == AtomUse || isShape(child->name)) && documentStyle(l, child, vp).clipPath) {
                return true;
            }
        }

        return false;
    }

    static bool resolveClip(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const Document& doc = *l.doc;
        Node* clip = elementById(doc, style.clipPath);

        if (!clip || clip->name != AtomClipPath || !resourceUsable(l, clip, vp)) {
            return false;
        }

        for (Node* active : l.active) {
            if (active == clip) {
                return false;
            }
        }

        Matrix space = multiply(m, originTransform(doc, clip, AtomTransform, vp));
        u32 units = attribute(clip, AtomClipPathUnits);

        op.clipProgram = false;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 0;

        if (units && textOf(doc, units) == SV("objectBoundingBox")) {
            Box box;

            if (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0) {
                return true;
            }

            space = multiply(space, Matrix{box.x1 - box.x0, 0.f, 0.f, box.y1 - box.y0, box.x0, box.y0});
        }

        if (!clipNeedsProgram(l, clip, vp)) {
            for (Node* child = clip->first; child; child = child->next) {
                ClipChild item;
                Path path;

                if (clipChild(l, child, space, vp, item) && buildShape(l, item.shape, item.style, vp, path)) {
                    addClipShape(l, path, item.space, item.style.clipRule);
                }
            }

            op.clipCount = (u32)l.clips.length() - op.clipFirst;

            return true;
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;
        const ComputedStyle& clipStyle = documentStyle(l, clip, vp);
        Op outer = groupOp(1.f);

        l.active.pushBack(clip);
        l.out = &program;
        outer.clipped = clipStyle.clipPath && resolveClip(l, element, parent, clipStyle, m, vp, outer);

        u32 outerIndex = (u32)program.length();

        if (outer.clipped) {
            program.pushBack(outer);
        }

        for (Node* child = clip->first; child; child = child->next) {
            ClipChild item;
            Path path;

            if (!clipChild(l, child, space, vp, item) || !buildShape(l, item.shape, item.style, vp, path)) {
                continue;
            }

            const ComputedStyle& childStyle = documentStyle(l, child, vp);
            Op inner = groupOp(1.f);

            inner.clipped = childStyle.clipPath && resolveClip(l, child, clipStyle, childStyle, multiply(space, transformOf(doc, child, AtomTransform)), vp, inner);

            u32 innerIndex = (u32)program.length();

            if (inner.clipped) {
                program.pushBack(inner);
            }

            Flat flat;

            flatten(path, item.space, tolerance, flat);
            emitFill(l, flat, item.style.clipRule, solidPaint(l, Color{1.f, 1.f, 1.f, 1.f}, 1.f), false);

            if (inner.clipped) {
                endGroup(l, innerIndex);
            }
        }

        if (outer.clipped) {
            endGroup(l, outerIndex);
        }

        l.out = saved;
        l.active.popBack();
        op.clipProgram = true;
        op.clipFirst = (u32)l.programs.length();
        op.clipCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return true;
    }

    static void shapeClip(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const BasicShape& shape = *style.clipShape;
        Box box{0.f, 0.f, vp.width, vp.height};

        op.clipProgram = false;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 0;

        if (shape.box != GeometryBox::View) {
            if (!elementBox(l, element, parent, vp, box, 0)) {
                return;
            }

            if (shape.box == GeometryBox::Stroke && style.stroke.kind != PaintKind::None && isShape(element->name)) {
                float half = style.strokeWidth / 2.f;

                box = Box{box.x0 - half, box.y0 - half, box.x1 + half, box.y1 + half};
            }
        }

        float w = box.x1 - box.x0;
        float h = box.y1 - box.y0;
        float fs = style.fontSize;
        Path path;

        if (shape.kind == ShapeKind::Circle || shape.kind == ShapeKind::Ellipse) {
            float cx = box.x0 + shapeLength(shape.values[2], w, vp, fs);
            float cy = box.y0 + shapeLength(shape.values[3], h, vp, fs);
            float sides[2][2] = {{cx - box.x0, box.x1 - cx}, {cy - box.y0, box.y1 - cy}};
            float radii[2];

            for (int axis = 0; axis < 2; axis++) {
                const Length& r = shape.values[shape.kind == ShapeKind::Circle ? 0 : axis];
                float reference = shape.kind == ShapeKind::Circle ? sqrtf((w * w + h * h) / 2.f) : (axis ? h : w);

                if (r.unit == Unit::Number && r.value < 0.f) {
                    bool farthest = r.value < -1.5f;

                    if (shape.kind == ShapeKind::Circle) {
                        float a = minf(fabsf(sides[0][0]), fabsf(sides[0][1]));
                        float b = minf(fabsf(sides[1][0]), fabsf(sides[1][1]));
                        float c = maxf(fabsf(sides[0][0]), fabsf(sides[0][1]));
                        float d = maxf(fabsf(sides[1][0]), fabsf(sides[1][1]));

                        radii[axis] = farthest ? maxf(c, d) : minf(a, b);
                    } else {
                        radii[axis] = farthest ? maxf(fabsf(sides[axis][0]), fabsf(sides[axis][1])) : minf(fabsf(sides[axis][0]), fabsf(sides[axis][1]));
                    }
                } else {
                    radii[axis] = shapeLength(r, reference, vp, fs);
                }
            }

            ellipsePath(path, cx, cy, radii[0], radii[1]);
        } else if (shape.kind == ShapeKind::Inset) {
            float top = shapeLength(shape.values[0], h, vp, fs);
            float right = shapeLength(shape.values[1], w, vp, fs);
            float bottom = shapeLength(shape.values[2], h, vp, fs);
            float left = shapeLength(shape.values[3], w, vp, fs);
            float x0 = box.x0 + left;
            float y0 = box.y0 + top;
            float x1 = maxf(x0, box.x1 - right);
            float y1 = maxf(y0, box.y1 - bottom);

            moveTo(path, Point{x0, y0});
            lineTo(path, Point{x1, y0});
            lineTo(path, Point{x1, y1});
            lineTo(path, Point{x0, y1});
            closePath(path);
        } else {
            for (u32 i = 0; i < shape.pointCount; i++) {
                Point point{box.x0 + shapeLength(shape.points[i * 2], w, vp, fs), box.y0 + shapeLength(shape.points[i * 2 + 1], h, vp, fs)};

                if (i == 0) {
                    moveTo(path, point);
                } else {
                    lineTo(path, point);
                }
            }

            closePath(path);
        }

        addClipShape(l, path, m, shape.rule);
        op.clipCount = (u32)l.clips.length() - op.clipFirst;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp);
    static void loadDocument(Document& doc, ObjPool& pool, StringView source, SvgFail fail);
    static void cascade(Document& doc);
    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp);

    static float unitFraction(const Document& doc, const Node* node, u32 name, float fallback) {
        u32 value = attribute(node, name);
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return fallback;
        }

        return length.unit == Unit::Percent ? length.value / 100.f : length.value;
    }

    static bool resolveMask(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const Document& doc = *l.doc;
        Node* mask = elementById(doc, style.mask);

        if (!mask || mask->name != AtomMask || !resourceUsable(l, mask, vp) || closesCycle(l, element, AtomMask, mask)) {
            return false;
        }

        u32 units = attribute(mask, AtomMaskUnits);
        u32 contentUnits = attribute(mask, AtomMaskContentUnits);
        bool boxRegion = !units || textOf(doc, units) != SV("userSpaceOnUse");
        bool boxContent = contentUnits && textOf(doc, contentUnits) == SV("objectBoundingBox");
        const ComputedStyle& maskStyle = documentStyle(l, mask, vp);
        Box box = emptyBox();
        float x;
        float y;
        float w;
        float h;

        op.masked = true;
        op.maskType = (u8)(maskStyle.maskType | (maskStyle.linearColors ? 2 : 0));
        op.maskFirst = (u32)l.programs.length();
        op.maskCount = 0;
        op.maskRegion = (u32)l.clips.length();

        if ((boxRegion || boxContent) && (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0)) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return true;
        }

        if (boxRegion) {
            float bw = box.x1 - box.x0;
            float bh = box.y1 - box.y0;

            x = box.x0 + unitFraction(doc, mask, AtomX, -0.1f) * bw;
            y = box.y0 + unitFraction(doc, mask, AtomY, -0.1f) * bh;
            w = unitFraction(doc, mask, AtomWidth, 1.2f) * bw;
            h = unitFraction(doc, mask, AtomHeight, 1.2f) * bh;
        } else {
            float fs = maskStyle.fontSize;

            x = lengthOr(doc, mask, AtomX, Axis::X, vp, fs, -0.1f * vp.width);
            y = lengthOr(doc, mask, AtomY, Axis::Y, vp, fs, -0.1f * vp.height);
            w = lengthOr(doc, mask, AtomWidth, Axis::X, vp, fs, 1.2f * vp.width);
            h = lengthOr(doc, mask, AtomHeight, Axis::Y, vp, fs, 1.2f * vp.height);
        }

        if (w <= 0.f || h <= 0.f) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return true;
        }

        addRectClip(l, m, x, y, w, h);

        Matrix content = m;

        if (boxContent) {
            content = multiply(content, Matrix{box.x1 - box.x0, 0.f, 0.f, box.y1 - box.y0, box.x0, box.y0});
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;

        l.out = &program;
        l.active.pushBack(mask);
        lowerChildren(l, mask, maskStyle, content, vp);
        l.active.popBack();
        l.out = saved;
        op.maskFirst = (u32)l.programs.length();
        op.maskCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return true;
    }

    static bool filterPrimitive(u32 name) {
        switch (name) {
            case AtomFeBlend:
            case AtomFeColorMatrix:
            case AtomFeComponentTransfer:
            case AtomFeComposite:
            case AtomFeConvolveMatrix:
            case AtomFeDiffuseLighting:
            case AtomFeDisplacementMap:
            case AtomFeDropShadow:
            case AtomFeFlood:
            case AtomFeGaussianBlur:
            case AtomFeImage:
            case AtomFeMerge:
            case AtomFeMorphology:
            case AtomFeOffset:
            case AtomFeSpecularLighting:
            case AtomFeTile:
            case AtomFeTurbulence:
                return true;
            default:
                return false;
        }
    }

    static float filterLength(const Document& doc, u32 value, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static Box deviceBox(const Matrix& m, const Box& box) {
        Box out = emptyBox();
        Point corners[4] = {{box.x0, box.y0}, {box.x1, box.y0}, {box.x1, box.y1}, {box.x0, box.y1}};

        for (Point corner : corners) {
            extend(out, apply(m, corner));
        }

        return out;
    }

    static IntBox pixelBox(const Box& box, const IntBox& limit) {
        IntBox out;
        float x0 = floorf(box.x0 + 1e-4f);
        float y0 = floorf(box.y0 + 1e-4f);
        float x1 = ceilf(box.x1 - 1e-4f);
        float y1 = ceilf(box.y1 - 1e-4f);

        out.x0 = x0 < (float)limit.x0 ? limit.x0 : (x0 > (float)limit.x1 ? limit.x1 : (int)x0);
        out.y0 = y0 < (float)limit.y0 ? limit.y0 : (y0 > (float)limit.y1 ? limit.y1 : (int)y0);
        out.x1 = x1 > (float)limit.x1 ? limit.x1 : (x1 < (float)out.x0 ? out.x0 : (int)x1);
        out.y1 = y1 > (float)limit.y1 ? limit.y1 : (y1 < (float)out.y0 ? out.y0 : (int)y1);

        return out;
    }

    static int filterInput(const Document& doc, const Node* node, u32 name, const Vector<FilterPrimitive>& done, int fallback) {
        u32 value = attribute(node, name);

        if (!value) {
            return fallback;
        }

        StringView text = trim(textOf(doc, value));

        if (text == SV("SourceGraphic")) {
            return InputSourceGraphic;
        }

        if (text == SV("SourceAlpha")) {
            return InputSourceAlpha;
        }

        if (text == SV("BackgroundImage")) {
            return InputBackgroundImage;
        }

        if (text == SV("BackgroundAlpha")) {
            return InputBackgroundAlpha;
        }

        if (text == SV("FillPaint")) {
            return InputFillPaint;
        }

        if (text == SV("StrokePaint")) {
            return InputStrokePaint;
        }

        for (size_t i = done.length(); i > 0; i--) {
            u32 result = attribute(done[i - 1].node, AtomResult);

            if (result && trim(textOf(doc, result)) == text) {
                return (int)(i - 1);
            }
        }

        return fallback;
    }

    static void lowerImageContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, float x, float y, float w, float h);

    static Matrix rootContent(Lowering& l, Node* node);

    static Filter* lowerFilter(Lowering& l, Node* element, const ComputedStyle& parent, const Matrix& m, Viewport vp, u32 reference) {
        const Document& doc = *l.doc;
        Node* filterNode = elementById(doc, reference);

        if (!filterNode || filterNode->name != AtomFilter || !resourceUsable(l, filterNode, vp)) {
            return nullptr;
        }

        for (Node* active : l.active) {
            if (active == filterNode) {
                return nullptr;
            }
        }

        Filter* filter = l.pool->make<Filter>();
        u32 units = attribute(filterNode, AtomFilterUnits);
        u32 primitiveUnits = attribute(filterNode, AtomPrimitiveUnits);
        bool boxRegion = !units || textOf(doc, units) != SV("userSpaceOnUse");
        IntBox canvas{0, 0, (int)l.width, (int)l.height};
        Box bbox;

        filter->doc = l.doc;
        filter->ctm = m;
        filter->boxPrimitives = primitiveUnits && textOf(doc, primitiveUnits) == SV("objectBoundingBox");
        filter->region = IntBox{0, 0, 0, 0};
        filter->count = 0;
        filter->primitives = nullptr;
        filter->next = nullptr;
        filter->device = Box{0.f, 0.f, 0.f, 0.f};

        bool hasBox = elementBox(l, element, parent, vp, bbox, 0) && bbox.x1 > bbox.x0 && bbox.y1 > bbox.y0;

        if (hasBox && element == l.doc->root) {
            // Root filters use the CSS viewport; m supplies the device scale.
            bbox = deviceBox(rootContent(l, element), bbox);
        }

        filter->bbox = hasBox ? bbox : Box{0.f, 0.f, 0.f, 0.f};

        if ((boxRegion || filter->boxPrimitives) && !hasBox) {
            return filter;
        }

        Box region;
        float bw = bbox.x1 - bbox.x0;
        float bh = bbox.y1 - bbox.y0;

        if (boxRegion) {
            region.x0 = bbox.x0 + filterLength(doc, attribute(filterNode, AtomX), true, Axis::X, vp, -0.1f) * bw;
            region.y0 = bbox.y0 + filterLength(doc, attribute(filterNode, AtomY), true, Axis::Y, vp, -0.1f) * bh;
            region.x1 = region.x0 + filterLength(doc, attribute(filterNode, AtomWidth), true, Axis::X, vp, 1.2f) * bw;
            region.y1 = region.y0 + filterLength(doc, attribute(filterNode, AtomHeight), true, Axis::Y, vp, 1.2f) * bh;
        } else {
            region.x0 = filterLength(doc, attribute(filterNode, AtomX), false, Axis::X, vp, -0.1f * vp.width);
            region.y0 = filterLength(doc, attribute(filterNode, AtomY), false, Axis::Y, vp, -0.1f * vp.height);
            region.x1 = region.x0 + filterLength(doc, attribute(filterNode, AtomWidth), false, Axis::X, vp, 1.2f * vp.width);
            region.y1 = region.y0 + filterLength(doc, attribute(filterNode, AtomHeight), false, Axis::Y, vp, 1.2f * vp.height);
        }

        if (region.x1 <= region.x0 || region.y1 <= region.y0) {
            return filter;
        }

        filter->device = deviceBox(m, region);
        filter->region = pixelBox(filter->device, canvas);

        Node* content = filterNode;
        Vector<FilterPrimitive> primitives;
        Vector<Box> userBoxes;

        l.active.pushBack(filterNode);

        for (Node* child = content->first; child; child = child->next) {
            if (!filterPrimitive(child->name)) {
                continue;
            }

            FilterPrimitive primitive;
            const ComputedStyle& childStyle = documentStyle(l, child, vp);
            int previous = primitives.length() ? (int)primitives.length() - 1 : InputSourceGraphic;

            memset(&primitive, 0, sizeof(primitive));
            primitive.node = child;
            primitive.kind = child->name;
            primitive.linear = childStyle.linearFilters;
            primitive.flood = childStyle.floodColor;
            primitive.flood.a *= childStyle.floodOpacity;
            primitive.lighting = childStyle.lightingColor;
            primitive.in1 = filterInput(doc, child, AtomIn, primitives, previous);
            primitive.in2 = filterInput(doc, child, AtomIn2, primitives, previous);

            Vector<int> inputs;

            if (child->name == AtomFeMerge) {
                for (Node* node = child->first; node; node = node->next) {
                    if (node->name == AtomFeMergeNode) {
                        inputs.pushBack(filterInput(doc, node, AtomIn, primitives, previous));
                    }
                }

                primitive.inputCount = (u32)inputs.length();
                primitive.inputs = (int*)l.pool->allocate(sizeof(int) * (inputs.length() + 1));

                if (inputs.length()) {
                    memcpy(primitive.inputs, inputs.data(), sizeof(int) * inputs.length());
                }
            } else if (child->name != AtomFeFlood && child->name != AtomFeImage && child->name != AtomFeTurbulence) {
                inputs.pushBack(primitive.in1);

                if (child->name == AtomFeBlend || child->name == AtomFeComposite || child->name == AtomFeDisplacementMap) {
                    inputs.pushBack(primitive.in2);
                }
            }

            Box user = region;
            bool standard = inputs.length() == 0 || child->name == AtomFeTile;

            for (int input : inputs) {
                if (input < 0) {
                    standard = true;
                }
            }

            if (!standard) {
                user = emptyBox();

                for (int input : inputs) {
                    const Box& b = userBoxes[input];

                    user.x0 = minf(user.x0, b.x0);
                    user.y0 = minf(user.y0, b.y0);
                    user.x1 = maxf(user.x1, b.x1);
                    user.y1 = maxf(user.y1, b.y1);
                }
            }

            u32 xValue = attribute(child, AtomX);
            u32 yValue = attribute(child, AtomY);
            u32 wValue = attribute(child, AtomWidth);
            u32 hValue = attribute(child, AtomHeight);
            bool boxPrimitives = filter->boxPrimitives;

            if (xValue) {
                float x = filterLength(doc, xValue, boxPrimitives, Axis::X, vp, 0.f);
                float width = user.x1 - user.x0;

                user.x0 = boxPrimitives ? bbox.x0 + x * bw : x;
                user.x1 = user.x0 + width;
            }

            if (yValue) {
                float y = filterLength(doc, yValue, boxPrimitives, Axis::Y, vp, 0.f);
                float height = user.y1 - user.y0;

                user.y0 = boxPrimitives ? bbox.y0 + y * bh : y;
                user.y1 = user.y0 + height;
            }

            if (wValue) {
                float width = filterLength(doc, wValue, boxPrimitives, Axis::X, vp, 0.f);

                user.x1 = user.x0 + (boxPrimitives ? width * bw : width);
            }

            if (hValue) {
                float height = filterLength(doc, hValue, boxPrimitives, Axis::Y, vp, 0.f);

                user.y1 = user.y0 + (boxPrimitives ? height * bh : height);
            }

            primitive.user = user;
            primitive.box = user.x1 > user.x0 && user.y1 > user.y0 ? pixelBox(deviceBox(m, user), filter->region) : IntBox{0, 0, 0, 0};

            if (child->name == AtomFeImage) {
                Vector<Op> program;
                Vector<Op>* saved = l.out;

                l.out = &program;
                lowerImageContent(l, child, childStyle, m, vp, user.x0, user.y0, user.x1 - user.x0, user.y1 - user.y0);
                l.out = saved;
                primitive.programFirst = (u32)l.programs.length();
                primitive.programCount = (u32)program.length();
                l.programs.append(program.data(), program.length());
            }

            primitives.pushBack(primitive);
            userBoxes.pushBack(user);
        }

        l.active.popBack();
        filter->count = (u32)primitives.length();
        filter->primitives = (FilterPrimitive*)l.pool->allocate(sizeof(FilterPrimitive) * (primitives.length() + 1));

        if (filter->count) {
            memcpy(filter->primitives, primitives.data(), sizeof(FilterPrimitive) * primitives.length());
        }

        return filter;
    }

    static Filter* functionFilter(Lowering& l, const FilterItem& item, const Matrix& m, bool linear) {
        Filter* filter = l.pool->make<Filter>();
        FilterPrimitive* primitive = (FilterPrimitive*)l.pool->allocate(sizeof(FilterPrimitive));

        memset(primitive, 0, sizeof(FilterPrimitive));
        primitive->kind = item.kind == FilterFunction::Blur ? AtomFeGaussianBlur : (item.kind == FilterFunction::DropShadow ? AtomFeDropShadow : AtomFeColorMatrix);
        primitive->in1 = InputSourceGraphic;
        primitive->in2 = InputSourceGraphic;
        primitive->box = IntBox{0, 0, (int)l.width, (int)l.height};
        primitive->flood = item.color;
        primitive->linear = linear;
        memcpy(primitive->params, item.values, sizeof(item.values));

        filter->doc = l.doc;
        filter->ctm = m;
        filter->bbox = Box{0.f, 0.f, 0.f, 0.f};
        filter->boxPrimitives = false;
        filter->region = primitive->box;
        filter->primitives = primitive;
        filter->count = 1;
        filter->next = nullptr;
        filter->device = Box{0.f, 0.f, (float)l.width, (float)l.height};

        return filter;
    }

    static Filter* lowerFilters(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Filter* first = nullptr;
        Filter* last = nullptr;

        for (u32 i = 0; i < style.filterCount; i++) {
            const FilterItem& item = style.filters[i];
            bool linear = last && last->count && last->primitives[last->count - 1].linear;
            Filter* filter = item.kind == FilterFunction::Reference ? lowerFilter(l, element, parent, m, vp, item.reference) : functionFilter(l, item, m, linear);

            if (!filter) {
                continue;
            }

            bool fill = false;
            bool stroke = false;
            for (u32 k = 0; k < filter->count; k++) {
                const FilterPrimitive& p = filter->primitives[k];
                fill = fill || p.in1 == InputFillPaint || p.in2 == InputFillPaint;
                stroke = stroke || p.in1 == InputStrokePaint || p.in2 == InputStrokePaint;
                for (u32 j = 0; j < p.inputCount; j++) {
                    fill = fill || p.inputs[j] == InputFillPaint;
                    stroke = stroke || p.inputs[j] == InputStrokePaint;
                }
            }
            filter->fillPaint = fill ? devicePaint(l, element, AtomFill, style.fill, style.fillOpacity, style, style.fill.context ? l.contextBox : filter->bbox, style.fill.context ? l.contextMatrix : m, vp) : nullptr;
            filter->strokePaint = stroke ? devicePaint(l, element, AtomStroke, style.stroke, style.strokeOpacity, style, style.stroke.context ? l.contextBox : filter->bbox, style.stroke.context ? l.contextMatrix : m, vp) : nullptr;

            if (last) {
                last->next = filter;
            } else {
                first = filter;
            }

            last = filter;
        }

        return first;
    }

    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp) {
        for (Node* child = node->first; child; child = child->next) {
            lowerElement(l, child, style, ctm, vp);
        }
    }

    static u32** instanceStyles(Pass& l, Node* target) {
        for (size_t i = 0; i < l.instanceTargets.length(); i++) {
            if (l.instanceTargets[i] == target) {
                return l.instanceMaps[i];
            }
        }

        Document& doc = *l.doc;
        u32** map = (u32**)l.pool->allocate(sizeof(u32*) * (doc.nodeCount + 1));

        memset(map, 0, sizeof(u32*) * (doc.nodeCount + 1));

        for (Node* node = target; node; node = nextNode(node, target)) {
            map[node->index] = cascadeNode(doc, node, target);
        }

        l.instanceTargets.pushBack(target);
        l.instanceMaps.pushBack(map);

        return map;
    }

    static PaintValue contextPaint(const PaintValue& paint, const ComputedStyle& style) {
        PaintValue out = paint;

        out.context = false;

        if (out.kind == PaintKind::CurrentColor) {
            out.kind = PaintKind::Color;
            out.color = style.color;
        }

        if (out.fallback == PaintKind::CurrentColor) {
            out.fallback = PaintKind::Color;
            out.fallbackColor = style.color;
        }

        return out;
    }

    struct MarkerVertex {
        Point p;
        Point in;
        Point out;
        bool hasIn;
        bool hasOut;
    };

    static bool nonZero(Point d) {
        return d.x != 0.f || d.y != 0.f;
    }

    static Point direction(Point from, Point to) {
        return Point{to.x - from.x, to.y - from.y};
    }

    static void markerVertices(const Path& path, Vector<MarkerVertex>& out) {
        const Point* pts = path.points.data();
        size_t pi = 0;
        Point current{0.f, 0.f};
        size_t start = 0;
        bool arc = false;

        // A path starts with a move, and a segment never follows an arc's
        // own cubics, which the cubic case joins.
        auto segment = [&](Point startDir, Point endDir, Point end) {
            MarkerVertex& last = out.mutData()[out.length() - 1];

            if (!last.hasOut) {
                last.out = startDir;
                last.hasOut = true;
            }

            out.pushBack(MarkerVertex{end, endDir, Point{0.f, 0.f}, true, false});
            current = end;
        };

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    current = pts[pi++];
                    start = out.length();
                    out.pushBack(MarkerVertex{current, Point{0.f, 0.f}, Point{0.f, 0.f}, false, false});
                    arc = false;
                    break;
                case VerbLine: {
                    Point end = pts[pi++];
                    Point d = direction(current, end);

                    segment(d, d, end);
                    arc = false;
                } break;
                case VerbCubic:
                case VerbArcCubic: {
                    Point c1 = pts[pi];
                    Point c2 = pts[pi + 1];
                    Point end = pts[pi + 2];
                    Point startDir = direction(current, c1);
                    Point endDir = direction(c2, end);

                    pi += 3;

                    if (!nonZero(startDir)) {
                        startDir = nonZero(direction(current, c2)) ? direction(current, c2) : direction(current, end);
                    }

                    if (!nonZero(endDir)) {
                        endDir = nonZero(direction(c1, end)) ? direction(c1, end) : direction(current, end);
                    }

                    if (arc) {
                        MarkerVertex& last = out.mutData()[out.length() - 1];

                        last.p = end;
                        last.in = endDir;
                        current = end;
                    } else {
                        segment(startDir, endDir, end);
                    }

                    arc = verb == VerbArcCubic;
                    break;
                }
                default: {
                    // VerbClose.
                    MarkerVertex first = out[start];
                    Point d = direction(current, first.p);

                    if (!nonZero(d)) {
                        // A zero-length close takes the tangent leaving the subpath start.
                        d = first.out;
                    }

                    segment(d, d, first.p);

                    MarkerVertex& closing = out.mutData()[out.length() - 1];
                    MarkerVertex& opening = out.mutData()[start];

                    closing.out = first.out;
                    closing.hasOut = first.hasOut;
                    opening.in = d;
                    opening.hasIn = true;
                    arc = false;
                    break;
                }
            }
        }
    }

    static float angleOf(Point d) {
        return atan2f(d.y, d.x) * 180.f / pi;
    }

    static float vertexAngle(const MarkerVertex& v) {
        if (v.hasIn && v.hasOut) {
            float a = angleOf(v.in);
            float b = angleOf(v.out);

            if (a - b <= -180.f) {
                a += 360.f;
            } else if (a - b > 180.f) {
                a -= 360.f;
            }

            return (a + b) / 2.f;
        }

        if (v.hasOut) {
            return angleOf(v.out);
        }

        if (v.hasIn) {
            return angleOf(v.in);
        }

        return 0.f;
    }

    static bool readAngle(StringView text, float& out) {
        const u8* p = text.begin();

        if (!readNumber(p, text.end(), out)) {
            return false;
        }

        StringView unit(p, text.end());

        if (unit.empty() || equalsIgnoreCase(unit, SV("deg"))) {
            return true;
        }

        if (equalsIgnoreCase(unit, SV("rad"))) {
            out *= 180.f / pi;
        } else if (equalsIgnoreCase(unit, SV("grad"))) {
            out *= 0.9f;
        } else if (equalsIgnoreCase(unit, SV("turn"))) {
            out *= 360.f;
        } else {
            return false;
        }

        return true;
    }

    // Where a marker of a shape goes: its origin, through the user matrix
    // or, for a non-scaling stroke in marker units, the host's; the transform
    // of its content inside, the viewport there, and its size. False for a
    // marker that draws nothing.
    static bool markerPlacement(Pass& p, Node* marker, const ComputedStyle& shapeStyle, const Matrix& userMatrix, const Matrix& hostMatrix, Viewport vp, const MarkerVertex& userVertex, const MarkerVertex& deviceVertex, bool start, Matrix& origin, Matrix& content, Viewport& inside, float& w, float& h) {
        const Document& doc = *p.doc;
        const ComputedStyle& markerStyle = documentStyle(p, marker, vp);
        float fs = markerStyle.fontSize;

        w = lengthOr(doc, marker, AtomMarkerWidth, Axis::X, vp, fs, 3.f);
        h = lengthOr(doc, marker, AtomMarkerHeight, Axis::Y, vp, fs, 3.f);

        if (w <= 0.f || h <= 0.f) {
            return false;
        }

        float angle = 0.f;
        float autoAngle = vertexAngle(shapeStyle.nonScaling ? deviceVertex : userVertex);
        u32 orient = attribute(marker, AtomOrient);

        if (orient) {
            StringView text = trim(textOf(doc, orient));

            if (text == SV("auto")) {
                angle = autoAngle;
            } else if (text == SV("auto-start-reverse")) {
                angle = start ? autoAngle + 180.f : autoAngle;
            } else if (!readAngle(text, angle)) {
                angle = 0.f;
            }
        }

        u32 units = attribute(marker, AtomMarkerUnits);
        bool userUnits = units && trim(textOf(doc, units)) == SV("userSpaceOnUse");
        float scale = userUnits ? 1.f : shapeStyle.strokeWidth;
        bool device = shapeStyle.nonScaling && !userUnits;
        const MarkerVertex& vertex = device ? deviceVertex : userVertex;
        const Matrix& m = device ? hostMatrix : userMatrix;
        Point at = vertex.p;
        float radians = angle * pi / 180.f;
        Matrix placed = multiply(m, translation(at.x, at.y));

        placed = multiply(placed, Matrix{cosf(radians), sinf(radians), -sinf(radians), cosf(radians), 0.f, 0.f});
        placed = multiply(placed, scaling(scale, scale));

        Box viewBox;

        content = identity();
        inside = Viewport{w, h, vp.screenWidth, vp.screenHeight, p.rootFontSize};

        if (parseViewBox(doc, marker, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return false;
            }

            content = viewBoxTransform(doc, marker, viewBox, w, h);
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, vp.screenWidth, vp.screenHeight, p.rootFontSize};
        }

        Point ref = apply(content, Point{lengthOr(doc, marker, AtomRefX, Axis::X, vp, fs, 0.f), lengthOr(doc, marker, AtomRefY, Axis::Y, vp, fs, 0.f)});

        origin = multiply(placed, translation(-ref.x, -ref.y));

        return true;
    }

    static void lowerMarker(Lowering& l, Node* node, u32 property, Node* marker, const ComputedStyle& shapeStyle, const Matrix& userMatrix, Viewport vp, const MarkerVertex& userVertex, const MarkerVertex& deviceVertex, bool start) {
        Matrix origin;
        Matrix content;
        Viewport inside;
        float w;
        float h;

        if (closesCycle(l, node, property, marker) || !markerPlacement(l, marker, shapeStyle, userMatrix, l.host, vp, userVertex, deviceVertex, start, origin, content, inside, w, h)) {
            return;
        }

        const ComputedStyle& markerStyle = documentStyle(l, marker, vp);
        bool clip = !markerStyle.overflowVisible;
        u32 clipIndex = (u32)l.out->length();

        if (clip) {
            Op op = groupOp(1.f);

            op.clipped = true;
            op.clipFirst = (u32)l.clips.length();
            op.clipCount = 1;
            addRectClip(l, origin, 0.f, 0.f, w, h);
            l.out->pushBack(op);
        }

        bool hadContext = l.hasContext;
        PaintValue outerFill = l.contextFill;
        PaintValue outerStroke = l.contextStroke;

        l.hasContext = true;
        l.contextFill = contextPaint(shapeStyle.fill, shapeStyle);
        l.contextStroke = contextPaint(shapeStyle.stroke, shapeStyle);
        l.active.pushBack(marker);
        lowerChildren(l, marker, markerStyle, multiply(origin, content), inside);
        l.active.popBack();
        l.hasContext = hadContext;
        l.contextFill = outerFill;
        l.contextStroke = outerStroke;

        if (clip) {
            endGroup(l, clipIndex);
        }
    }

    // The markers a shape's style names, those that are markers; false
    // when it names none or is no path, line, polyline or polygon.
    static bool shapeMarkers(const Pass& p, const Node* node, const ComputedStyle& style, Node** markers) {
        bool any = false;

        if (node->name != AtomPath && node->name != AtomLine && node->name != AtomPolyline && node->name != AtomPolygon) {
            return false;
        }

        for (int k = 0; k < 3; k++) {
            markers[k] = style.markers[k] ? elementById(*p.doc, style.markers[k]) : nullptr;

            if (markers[k] && markers[k]->name != AtomMarker) {
                markers[k] = nullptr;
            }

            any = any || markers[k];
        }

        return any;
    }

    static void lowerMarkers(Lowering& l, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Node* markers[3];

        if (!shapeMarkers(l, node, style, markers)) {
            return;
        }

        Vector<MarkerVertex> vertices;
        Vector<MarkerVertex> deviceVertices;
        Matrix hostInverse;

        if (!invert(l.host, hostInverse)) {
            hostInverse = identity();
        }

        markerVertices(path, vertices);
        markerVertices(style.nonScaling ? transformedPath(path, multiply(hostInverse, m)) : path, deviceVertices);

        for (size_t i = 0; i < vertices.length(); i++) {
            int kind = i == 0 ? 0 : (i + 1 == vertices.length() ? 2 : 1);

            if (markers[kind]) {
                lowerMarker(l, node, AtomMarkerStart + kind, markers[kind], style, m, vp, vertices[i], deviceVertices[i], kind == 0);
            }

            if (i == 0 && vertices.length() == 1 && markers[2]) {
                lowerMarker(l, node, AtomMarkerEnd, markers[2], style, m, vp, vertices[i], deviceVertices[i], false);
            }
        }
    }

    // Adds to a box what one marker of a shape draws at a vertex, the shape
    // placed by a matrix on a host that is the identity: the document at its
    // own size. A marker inside itself stops where lowering stops it.
    static void markerContentExtent(Pass& p, Node* marker, const ComputedStyle& shapeStyle, const Matrix& m, Viewport vp, const MarkerVertex& userVertex, const MarkerVertex& deviceVertex, bool start, Extent how, u32 depth, Box& box) {
        Matrix origin;
        Matrix content;
        Viewport inside;
        float w;
        float h;

        for (Node* active : p.active) {
            if (active == marker) {
                return;
            }
        }

        if (!markerPlacement(p, marker, shapeStyle, m, identity(), vp, userVertex, deviceVertex, start, origin, content, inside, w, h)) {
            return;
        }

        const ComputedStyle& markerStyle = documentStyle(p, marker, vp);

        p.active.pushBack(marker);

        for (Node* child = marker->first; child; child = child->next) {
            childExtent(p, child, multiply(origin, content), markerStyle, inside, how, depth, box);
        }

        p.active.popBack();
    }

    static void markerExtent(Pass& p, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box) {
        Node* markers[3];

        if (!shapeMarkers(p, node, style, markers)) {
            return;
        }

        Vector<MarkerVertex> vertices;
        Vector<MarkerVertex> deviceVertices;

        markerVertices(path, vertices);
        markerVertices(style.nonScaling ? transformedPath(path, m) : path, deviceVertices);

        for (size_t i = 0; i < vertices.length(); i++) {
            int kind = i == 0 ? 0 : (i + 1 == vertices.length() ? 2 : 1);

            if (markers[kind]) {
                markerContentExtent(p, markers[kind], style, m, vp, vertices[i], deviceVertices[i], kind == 0, how, depth, box);
            }

            if (i == 0 && vertices.length() == 1 && markers[2]) {
                markerContentExtent(p, markers[2], style, m, vp, vertices[i], deviceVertices[i], false, how, depth, box);
            }
        }
    }

    static float flatLength(const Flat& flat) {
        float total = 0.f;
        u32 begin = 0;

        for (size_t k = 0; k < flat.ends.length(); k++) {
            u32 end = flat.ends[k];

            for (u32 i = begin; i + 1 < end; i++) {
                total += hypotf(flat.points[i + 1].x - flat.points[i].x, flat.points[i + 1].y - flat.points[i].y);
            }

            // flatten closes a subpath of two points or more.
            if (flat.closed[k]) {
                total += hypotf(flat.points[begin].x - flat.points[end - 1].x, flat.points[begin].y - flat.points[end - 1].y);
            }

            begin = end;
        }

        return total;
    }

    static ComputedStyle authorDashes(Lowering& l, const Node* node, const Flat& userFlat, const ComputedStyle& style) {
        ComputedStyle dashed = style;
        u32 value = attribute(node, AtomPathLength);
        float author;

        if (!style.dashCount || !value || !parseNumberText(textOf(*l.doc, value), author) || author < 0.f) {
            return dashed;
        }

        float total = flatLength(userFlat);

        if (total > 0.f && author == 0.f) {
            dashed.dashes = nullptr;
            dashed.dashCount = 0;
            dashed.dashOffset = 0.f;

            return dashed;
        }

        float scale = total > 0.f ? total / author : 0.f;

        dashed.dashes = (float*)l.pool->allocate(sizeof(float) * style.dashCount);

        for (u32 k = 0; k < style.dashCount; k++) {
            dashed.dashes[k] = style.dashes[k] * scale;
        }

        dashed.dashOffset = style.dashOffset * scale;

        return dashed;
    }

    static void lowerShape(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Path path;

        if (!buildShape(l, node, style, vp, path)) {
            return;
        }

        Flat userFlat;

        flatten(path, identity(), 0.01f, userFlat);

        Box box = pathBox(path, identity());

        for (u8 layer : style.paintOrder) {
            if (layer == 0 && style.visible) {
                if (DevicePaint* fill = devicePaint(l, node, AtomFill, style.fill, style.fillOpacity, style, style.fill.context ? l.contextBox : box, style.fill.context ? l.contextMatrix : m, vp)) {
                    Flat flat;

                    flatten(path, m, tolerance, flat);
                    emitFill(l, flat, style.fillRule, fill, style.aliased);
                }
            } else if (layer == 1 && style.visible && style.strokeWidth > 0.f) {
                if (DevicePaint* stroke = devicePaint(l, node, AtomStroke, style.stroke, style.strokeOpacity, style, style.stroke.context ? l.contextBox : box, style.stroke.context ? l.contextMatrix : m, vp)) {
                    ComputedStyle dashed = authorDashes(l, node, userFlat, style);
                    Flat outline;
                    Flat device;

                    Box raster{0.f, 0.f, (float)l.width, (float)l.height};
                    Box cull;

                    if (style.nonScaling) {
                        Matrix hostInverse;
                        Flat hosted;
                        bool culled = invert(l.host, hostInverse);

                        if (!culled) {
                            hostInverse = identity();
                        }

                        cull = deviceBox(hostInverse, raster);
                        strokePath(transformedPath(path, multiply(hostInverse, m)), dashed, 1.f, culled ? &cull : nullptr, hosted);
                        device.ends.xchg(hosted.ends);
                        device.closed.xchg(hosted.closed);

                        for (Point p : hosted.points) {
                            device.points.pushBack(apply(l.host, p));
                        }
                    } else {
                        Matrix inverse;
                        bool culled = invert(m, inverse);

                        cull = culled ? deviceBox(inverse, raster) : raster;
                        strokePath(path, dashed, matrixScale(m), culled ? &cull : nullptr, outline);
                        device.ends.xchg(outline.ends);
                        device.closed.xchg(outline.closed);

                        for (Point p : outline.points) {
                            device.points.pushBack(apply(m, p));
                        }
                    }

                    emitFill(l, device, WindingNonZero, stroke, style.aliased);
                }
            } else if (layer == 2) {
                Matrix outerMatrix = l.contextMatrix;
                Box outerBox = l.contextBox;

                l.contextMatrix = m;
                l.contextBox = box;
                lowerMarkers(l, node, path, style, m, vp);
                l.contextMatrix = outerMatrix;
                l.contextBox = outerBox;
            }
        }
    }

    static void lowerViewport(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, Node* content, float x, float y, float w, float h, bool clip) {
        if (w <= 0.f || h <= 0.f) {
            return;
        }

        Matrix inner = multiply(m, translation(x, y));
        Viewport inside{w, h, vp.screenWidth, vp.screenHeight, l.rootFontSize};
        Box viewBox;

        if (parseViewBox(*l.doc, node, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return;
            }

            inner = multiply(inner, viewBoxTransform(*l.doc, node, viewBox, w, h));
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, vp.screenWidth, vp.screenHeight, l.rootFontSize};
        }

        u32 clipIndex = (u32)l.out->length();

        if (clip) {
            u32 first = (u32)l.clips.length();

            Op op = groupOp(1.f);

            addRectClip(l, m, x, y, w, h);
            op.clipped = true;
            op.clipFirst = first;
            op.clipCount = 1;
            l.out->pushBack(op);
        }

        if (node == l.doc->root) {
            l.screen = inner;
        }

        lowerChildren(l, content, style, inner, inside);

        if (clip) {
            endGroup(l, clipIndex);
        }
    }

    // The size a root gives itself: its width and height, one of them with
    // the viewBox's ratio, or the viewBox's size. A side it does not give,
    // or gives as a percentage, is zero.
    static void givenSize(const Document& doc, const Node* root, float& width, float& height) {
        Viewport none{0.f, 0.f, 0.f, 0.f, 16.f};
        Length w;
        Length h;
        Length size;
        StringView sizeText = cascaded(doc, root, AtomFontSize);
        float fontSize = 16.f;

        if (!sizeText.empty() && parseLength(sizeText, size) && size.value >= 0.f) {
            fontSize = size.unit == Unit::Percent ? 16.f * size.value / 100.f : resolveLength(size, Axis::Other, none, 16.f);
        }

        none.rootFontSize = fontSize;
        u32 widthValue = attribute(root, AtomWidth);
        u32 heightValue = attribute(root, AtomHeight);
        bool hasWidth = widthValue && parseLength(textOf(doc, widthValue), w) && w.unit != Unit::Percent && w.value > 0.f;
        bool hasHeight = heightValue && parseLength(textOf(doc, heightValue), h) && h.unit != Unit::Percent && h.value > 0.f;
        Box viewBox;
        bool hasViewBox = parseViewBox(doc, root, viewBox) && viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0;
        float ratio = hasViewBox ? (viewBox.x1 - viewBox.x0) / (viewBox.y1 - viewBox.y0) : 0.f;

        width = hasWidth ? resolveLength(w, Axis::X, none, fontSize) : 0.f;
        height = hasHeight ? resolveLength(h, Axis::Y, none, fontSize) : 0.f;

        if (hasWidth && !hasHeight && ratio > 0.f) {
            height = width / ratio;
        } else if (hasHeight && !hasWidth && ratio > 0.f) {
            width = height * ratio;
        } else if (!hasWidth && !hasHeight && hasViewBox) {
            width = viewBox.x1 - viewBox.x0;
            height = viewBox.y1 - viewBox.y0;
        }
    }

    static bool explicitSide(const Document& doc, const Node* root, u32 name) {
        u32 value = attribute(root, name);
        Length length;

        return value && parseLength(textOf(doc, value), length) && length.unit != Unit::Percent && length.value > 0.f;
    }

    // Whether an element gives a side as a length of its own, zero too,
    // rather than leaving it to its box: absent or a percentage.
    static bool givenSide(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);
        Length length;

        return value && parseLength(textOf(doc, value), length) && length.unit != Unit::Percent;
    }

    // The transform of the root's content into the box: its viewBox fit to
    // it, or its own size stretched to it.
    static Matrix rootContent(Lowering& l, Node* node) {
        Box viewBox;
        float cw = (float)l.width;
        float ch = (float)l.height;

        if (parseViewBox(*l.doc, node, viewBox)) {
            return viewBoxTransform(*l.doc, node, viewBox, cw, ch);
        }

        return l.docWidth > 0.f && l.docHeight > 0.f ? scaling(cw / l.docWidth, ch / l.docHeight) : identity();
    }

    static void lowerSvg(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        if (node == l.doc->root) {
            float w = (float)l.width;
            float h = (float)l.height;

            l.rootFontSize = style.fontSize;

            if (style.background.a > 0.f) {
                Path canvas;
                Flat flat;

                moveTo(canvas, Point{0.f, 0.f});
                lineTo(canvas, Point{(float)l.width, 0.f});
                lineTo(canvas, Point{(float)l.width, (float)l.height});
                lineTo(canvas, Point{0.f, (float)l.height});
                closePath(canvas);
                flatten(canvas, identity(), tolerance, flat);
                emitFill(l, flat, WindingNonZero, solidPaint(l, style.background, 1.f), false);
            }
            Box viewBox;

            if (!parseViewBox(*l.doc, node, viewBox)) {
                // The document's own size, given or measured, stretched to
                // the box.
                float iw = l.docWidth;
                float ih = l.docHeight;

                if (iw <= 0.f || ih <= 0.f) {
                    return;
                }

                l.screen = multiply(m, scaling(w / iw, h / ih));

                // A root with no side of its own zooms its content.
                if (style.zoom != 1.f && !explicitSide(*l.doc, node, AtomWidth) && !explicitSide(*l.doc, node, AtomHeight)) {
                    lowerViewport(l, node, style, multiply(l.screen, scaling(style.zoom, style.zoom)), vp, node, 0.f, 0.f, iw / style.zoom, ih / style.zoom, false);

                    return;
                }

                lowerChildren(l, node, style, l.screen, Viewport{iw, ih, w, h, l.rootFontSize});

                return;
            }

            lowerViewport(l, node, style, m, vp, node, 0.f, 0.f, w, h, false);

            return;
        }

        float fs = style.fontSize;
        float w = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width);
        float h = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height);

        if (l.sized == node) {
            w = l.sizedWidth >= 0.f ? l.sizedWidth : w;
            h = l.sizedHeight >= 0.f ? l.sizedHeight : h;
            l.sized = nullptr;
        }

        lowerViewport(l, node, style, m, vp, node, lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f), w, h, !style.overflowVisible);
    }

    static void lowerUse(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Node* target = hrefTarget(*l.doc, node);

        if (!target || l.active.length() > 32 || isAncestor(target, node)) {
            return;
        }

        for (size_t k = 1; k < l.hosts.length(); k += 2) {
            for (Node* up = l.hosts[k]; up; up = up->parent) {
                if (up == target) {
                    return;
                }
            }
        }

        for (size_t k = l.active.length(); k > 0; k--) {
            Node* active = l.active[k - 1];

            if (active == target) {
                return;
            }

            // Nothing is lowered while a clip path is.
            if (active->name == AtomMarker || active->name == AtomMask || active->name == AtomPattern || active->name == AtomFilter) {
                break;
            }
        }

        float fs = style.fontSize;
        const Matrix& placed = m;

        bool hadContext = l.hasContext;
        PaintValue outerFill = l.contextFill;
        PaintValue outerStroke = l.contextStroke;

        Matrix outerMatrix = l.contextMatrix;
        Box outerBox = l.contextBox;
        Box targetBox;
        u32** outerInstance = l.instance;

        if (l.doc->structural) {
            l.instance = instanceStyles(l, target);
        }

        l.hasContext = true;
        l.contextFill = contextPaint(style.fill, style);
        l.contextStroke = contextPaint(style.stroke, style);
        l.contextMatrix = m;
        l.contextBox = emptyBox();

        if (elementBox(l, target, style, vp, targetBox, 0)) {
            l.contextBox = deviceBox(originTransform(*l.doc, target, AtomTransform, vp), targetBox);
        }

        l.active.pushBack(target);
        l.hosts.pushBack(target);
        l.hosts.pushBack(node);

        if (target->name == AtomSymbol) {
            l.sized = target;
            l.sizedWidth = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, -1.f);
            l.sizedHeight = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, -1.f);
            lowerElement(l, target, style, placed, vp);
            l.sized = nullptr;
        } else {
            if (target->name == AtomSvg) {
                l.sized = target;
                l.sizedWidth = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, -1.f);
                l.sizedHeight = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, -1.f);
            }

            lowerElement(l, target, style, placed, vp);
            l.sized = nullptr;
        }

        l.hosts.popBack();
        l.hosts.popBack();
        l.active.popBack();
        l.hasContext = hadContext;
        l.contextFill = outerFill;
        l.contextStroke = outerStroke;
        l.contextMatrix = outerMatrix;
        l.contextBox = outerBox;
        l.instance = outerInstance;
    }

    static int base64Value(u8 c) {
        if (c >= 'A' && c <= 'Z') {
            return c - 'A';
        }

        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 26;
        }

        if (c >= '0' && c <= '9') {
            return c - '0' + 52;
        }

        if (c == '+' || c == '-') {
            return 62;
        }

        if (c == '/' || c == '_') {
            return 63;
        }

        return -1;
    }

    static bool decodeDataUri(StringView href, StringView& mime, Buffer& out) {
        StringView text = trim(href);

        if (!startsWithIgnoreCase(text, SV("data:"))) {
            return false;
        }

        const u8* comma = text.begin() + 5;

        while (comma < text.end() && *comma != ',') {
            comma++;
        }

        if (comma >= text.end()) {
            return false;
        }

        StringView header(text.begin() + 5, comma);
        bool base64 = header.length() >= 7 && equalsIgnoreCase(StringView(header.end() - 7, header.end()), SV(";base64"));
        const u8* semicolon = header.begin();

        while (semicolon < header.end() && *semicolon != ';') {
            semicolon++;
        }

        mime = trim(StringView(header.begin(), semicolon));

        const u8* p = comma + 1;

        if (base64) {
            u32 bits = 0;
            int count = 0;

            for (; p < text.end(); p++) {
                if (isSpace(*p) || *p == '=') {
                    continue;
                }

                int value = base64Value(*p);

                if (value < 0) {
                    return false;
                }

                bits = (bits << 6) | (u32)value;
                count += 6;

                if (count >= 8) {
                    count -= 8;

                    u8 byte = (u8)(bits >> count);

                    out.append(&byte, 1);
                }
            }

            return true;
        }

        while (p < text.end()) {
            if (*p == '%' && text.end() - p >= 3 && hexDigit(p[1]) >= 0 && hexDigit(p[2]) >= 0) {
                u8 byte = (u8)(hexDigit(p[1]) * 16 + hexDigit(p[2]));

                out.append(&byte, 1);
                p += 3;
            } else {
                out.append(p, 1);
                p++;
            }
        }

        return true;
    }

    static bool isAuto(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);

        return !value || equalsIgnoreCase(trim(textOf(doc, value)), SV("auto"));
    }

    // Holds a decoded raster image for the pool of the render that uses it.
    struct DecodedImage {
        SvgImage* image;

        explicit DecodedImage(SvgImage* image);
        ~DecodedImage();
    };

    // A rendered image, whose destroy deletes the pool it is made in.
    struct RenderedImage {
        SvgImage image;
        ObjPool* pool;
        const u8* pixels;
        u32 width;
        u32 height;

        RenderedImage(ObjPool* pool, const u8* pixels, u32 width, u32 height);
    };

    static void destroyRendered(SvgImage* image) {
        delete ((RenderedImage*)image)->pool;
    }

    static const void* renderedData(const SvgImage* image) {
        return ((const RenderedImage*)image)->pixels;
    }

    static size_t renderedLength(const SvgImage* image) {
        const RenderedImage* rendered = (const RenderedImage*)image;

        return (size_t)rendered->width * rendered->height * 4;
    }

    static u32 renderedWidth(const SvgImage* image) {
        return ((const RenderedImage*)image)->width;
    }

    static u32 renderedHeight(const SvgImage* image) {
        return ((const RenderedImage*)image)->height;
    }

    const SvgImageVtable renderedVtable = {destroyRendered, renderedData, renderedLength, renderedWidth, renderedHeight};

    static bool loadImage(Pass& l, Node* node, ImageSource& out) {
        const Document& doc = *l.doc;
        u32 href = attribute(node, AtomHref);
        StringView mime;
        Buffer bytes;

        out.nested = nullptr;
        out.raster = nullptr;
        out.ratio = nullptr;

        // Its callers see an href first.

        StringView reference = trim(textOf(doc, href));
        bool remote = startsWithIgnoreCase(reference, SV("http:")) || startsWithIgnoreCase(reference, SV("https:")) || (!reference.empty() && reference[0] == '#');
        StringView fragment;

        if (startsWithIgnoreCase(reference, SV("data:"))) {
            for (const u8* p = reference.end(); p > reference.begin(); p--) {
                if (p[-1] == '#') {
                    fragment = StringView(p, reference.end());
                    reference = StringView(reference.begin(), p - 1);
                    break;
                }
            }
        }

        bool broken = remote || (decodeDataUri(reference, mime, bytes) && bytes.length() == 0);

        if (!remote && !broken && bytes.length() == 0) {
            return false;
        }

        StringView data = keep(*l.pool, bytes.data(), bytes.length());

        if (equalsIgnoreCase(mime, SV("image/svg+xml")) && !broken) {
            if (l.imageDepth >= 16) {
                return false;
            }

            Document* nested = l.pool->make<Document>();

            loadDocument(*nested, *l.pool, data, l.fail);

            if (nested->root && nested->root->name == AtomSvg) {
                cascade(*nested);
                out.nested = nested;

                // The document's size, a side it leaves open zero: the
                // image's box fills it in, or, for an image of no size of
                // its own, a measure of what the document draws.
                givenSize(*nested, nested->root, out.width, out.height);

                if (!parseViewBox(*nested, nested->root, out.viewBox) || out.viewBox.x1 <= out.viewBox.x0 || out.viewBox.y1 <= out.viewBox.y0) {
                    out.viewBox = Box{0.f, 0.f, out.width, out.height};
                }

                Node* view = fragment.empty() ? nullptr : elementById(*nested, lookup(nested->atoms, fragment));
                Box viewBox;

                if (view && view->name == AtomView && parseViewBox(*nested, view, viewBox) && viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0) {
                    out.viewBox = viewBox;
                    out.ratio = view;
                }

                return true;
            }

            broken = true;
        }

        SvgImage* raster = !broken && l.decode ? l.decode(l.context, data.data(), data.length()) : nullptr;

        if (!raster) {
            return false;
        }

        l.pool->make<DecodedImage>(raster);

        u32 width = svg_image_width(raster);
        u32 height = svg_image_height(raster);

        out.raster = raster;
        out.width = (float)width;
        out.height = (float)height;
        out.viewBox = Box{0.f, 0.f, out.width, out.height};

        return true;
    }

    static const u8* halveImage(ObjPool& pool, const u8* pixels, u32& width, u32& height) {
        u32 w = width > 1 ? (width + 1) / 2 : 1;
        u32 h = height > 1 ? (height + 1) / 2 : 1;
        u8* out = (u8*)pool.allocate((size_t)w * h * 4);

        for (u32 y = 0; y < h; y++) {
            u32 y0 = height > 1 ? y * 2 : 0;
            u32 y1 = height > 1 && y * 2 + 1 < height ? y * 2 + 1 : y0;

            for (u32 x = 0; x < w; x++) {
                u32 x0 = width > 1 ? x * 2 : 0;
                u32 x1 = width > 1 && x * 2 + 1 < width ? x * 2 + 1 : x0;
                const u8* taps[4] = {pixels + ((size_t)y0 * width + x0) * 4, pixels + ((size_t)y0 * width + x1) * 4, pixels + ((size_t)y1 * width + x0) * 4, pixels + ((size_t)y1 * width + x1) * 4};
                u32 alpha = 0;
                u32 sum[3] = {0, 0, 0};
                u8* d = out + ((size_t)y * w + x) * 4;

                for (const u8* t : taps) {
                    alpha += t[3];

                    for (int c = 0; c < 3; c++) {
                        sum[c] += (u32)t[c] * t[3];
                    }
                }

                for (int c = 0; c < 3; c++) {
                    d[c] = alpha ? (u8)((sum[c] + alpha / 2) / alpha) : 0;
                }

                d[3] = (u8)((alpha + 2) / 4);
            }
        }

        width = w;
        height = h;

        return out;
    }

    static bool usesBackground(const Document& doc, const Node* node, u32 depth = 0);

    static void drawImage(Lowering& l, Node* node, const ImageSource& source, const Matrix& m, Viewport vp, float x, float y, float w, float h, u8 rendering) {
        const Document& doc = *l.doc;
        u32 ratio = attribute(node, AtomPreserveAspectRatio);
        bool stretch = ratio && trim(textOf(doc, ratio)) == SV("none");
        Box viewBox = source.viewBox;

        // A side a nested document leaves open is the box's, as it is.
        if (viewBox.x1 <= viewBox.x0) {
            viewBox.x1 = viewBox.x0 + w;
        }

        if (viewBox.y1 <= viewBox.y0) {
            viewBox.y1 = viewBox.y0 + h;
        }

        Matrix fit = multiply(translation(x, y), source.nested && !stretch ? viewBoxTransform(*source.nested, source.ratio ? source.ratio : source.nested->root, viewBox, w, h) : viewBoxTransform(doc, node, viewBox, w, h));
        Matrix place = multiply(m, fit);

        if (source.raster) {
            DevicePaint* paint = l.pool->make<DevicePaint>();
            Matrix inverse;

            memset(paint, 0, sizeof(DevicePaint));
            paint->type = PaintType::Image;
            paint->image = (const u8*)svg_image_data(source.raster);
            paint->imageWidth = svg_image_width(source.raster);
            paint->imageHeight = svg_image_height(source.raster);
            paint->smooth = true;

            if (!invert(place, inverse)) {
                return;
            }

            float scale = minf(sqrtf(place.a * place.a + place.b * place.b), sqrtf(place.c * place.c + place.d * place.d));
            int levels = scale < 1.f ? (int)floorf(maxf(-log2f(scale) - 0.5f, 0.f) + 0.5f) : 0;

            paint->smooth = rendering == 0 || (rendering == 2 && scale < 1.f);
            levels = paint->smooth ? levels : 0;

            float fx = 1.f;
            float fy = 1.f;

            for (int k = 0; k < levels && (paint->imageWidth > 1 || paint->imageHeight > 1); k++) {
                fx *= paint->imageWidth > 1 ? 0.5f : 1.f;
                fy *= paint->imageHeight > 1 ? 0.5f : 1.f;
                paint->image = halveImage(*l.pool, paint->image, paint->imageWidth, paint->imageHeight);
            }

            paint->inverse = multiply(scaling(fx, fy), inverse);

            Point a = apply(fit, Point{viewBox.x0, viewBox.y0});
            Point b = apply(fit, Point{viewBox.x1, viewBox.y1});
            float rx0 = maxf(minf(a.x, b.x), x);
            float ry0 = maxf(minf(a.y, b.y), y);
            float rx1 = minf(maxf(a.x, b.x), x + w);
            float ry1 = minf(maxf(a.y, b.y), y + h);
            Path rect;
            Flat flat;

            moveTo(rect, Point{rx0, ry0});
            lineTo(rect, Point{rx1, ry0});
            lineTo(rect, Point{rx1, ry1});
            lineTo(rect, Point{rx0, ry1});
            closePath(rect);
            flatten(rect, m, tolerance, flat);
            emitFill(l, flat, WindingNonZero, paint, false);

            return;
        }

        Op op = groupOp(1.f);

        op.clipped = true;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 1;
        addRectClip(l, m, x, y, w, h);

        Document* outer = l.doc;
        Document* nested = source.nested;
        Vector<ComputedStyle*> outerStyles;

        u32** outerInstance = l.instance;
        Vector<Node*> outerTargets;
        Vector<u32**> outerMaps;

        outerStyles.xchg(l.documentStyles);
        outerTargets.xchg(l.instanceTargets);
        outerMaps.xchg(l.instanceMaps);
        l.instance = nullptr;
        l.documentStyles.zero(nested->nodeCount);
        l.doc = nested;
        l.imageDepth++;

        ComputedStyle rootStyle = computeStyle(l, nested->root, initialStyle(), vp);
        float outerFontSize = l.rootFontSize;
        float outerDocWidth = l.docWidth;
        float outerDocHeight = l.docHeight;

        // The nested root's content, for its filters, stretches a side it
        // gives; a side it leaves open is the box's.
        l.docWidth = explicitSide(*nested, nested->root, AtomWidth) ? source.width : (float)l.width;
        l.docHeight = explicitSide(*nested, nested->root, AtomHeight) ? source.height : (float)l.height;

        l.backgroundInputs = l.backgroundInputs || usesBackground(*nested, nested->root);
        op.resetBackground = true;
        op.newBackground = l.backgroundInputs && rootStyle.newBackground;
        op.backgroundClipped = rootStyle.backgroundClipped;
        op.backgroundBox = rootStyle.backgroundBox;

        if (op.backgroundClipped && !invert(place, op.backgroundMatrix)) {
            op.backgroundBox = Box{0.f, 0.f, 0.f, 0.f};
        }

        u32 imageIndex = (u32)l.out->length();

        l.out->pushBack(op);
        l.rootFontSize = rootStyle.fontSize;
        lowerChildren(l, nested->root, rootStyle, place, Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, source.width > 0.f ? source.width : w, source.height > 0.f ? source.height : h, rootStyle.fontSize});
        l.rootFontSize = outerFontSize;
        l.docWidth = outerDocWidth;
        l.docHeight = outerDocHeight;
        l.imageDepth--;
        l.doc = outer;
        l.documentStyles.xchg(outerStyles);
        l.instanceTargets.xchg(outerTargets);
        l.instanceMaps.xchg(outerMaps);
        l.instance = outerInstance;
        endGroup(l, imageIndex);
    }

    // Where an image goes in its user space: its x, y, width and height, a
    // side left auto from its source's size. A nested document's side left
    // open then measures as what it draws. False for one that draws nothing.
    static bool imageRect(Pass& p, Node* node, const ComputedStyle& style, Viewport vp, ImageSource& source, float& x, float& y, float& w, float& h) {
        const Document& doc = *p.doc;

        if (!loadImage(p, node, source)) {
            return false;
        }

        float fs = style.fontSize;
        bool autoWidth = isAuto(doc, node, AtomWidth);
        bool autoHeight = isAuto(doc, node, AtomHeight);

        if (source.nested && (autoWidth || autoHeight) && (source.width <= 0.f || source.height <= 0.f)) {
            Pass inner;

            beginPass(inner, *source.nested, *p.pool, p.decode, p.fail, p.context);
            inner.imageDepth = p.imageDepth + 1;
            measureRoot(inner, source.width, source.height);

            if (!source.ratio) {
                source.viewBox = Box{0.f, 0.f, source.width, source.height};
            }
        }

        x = lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f);
        y = lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f);
        w = autoWidth ? source.width : lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
        h = autoHeight ? source.height : lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);

        // A source of a size keeps its ratio.
        if (autoWidth && !autoHeight && source.height > 0.f) {
            w = h * source.width / source.height;
        } else if (autoHeight && !autoWidth && source.width > 0.f) {
            h = w * source.height / source.width;
        }

        return w > 0.f && h > 0.f;
    }

    static void lowerImage(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        ImageSource source;
        float x;
        float y;
        float w;
        float h;

        if (!style.visible || !imageRect(l, node, style, vp, source, x, y, w, h)) {
            return;
        }

        drawImage(l, node, source, m, vp, x, y, w, h, style.imageRendering);
    }

    static Matrix boxTransform(Pass& l, Node* node, const ComputedStyle& parent, Viewport vp) {
        const Document& doc = *l.doc;
        u32 kind = attribute(node, AtomTransformBox);
        StringView box = kind ? trim(textOf(doc, kind)) : StringView();
        Matrix m;
        Box bounds;

        if (!(box == SV("fill-box") || box == SV("stroke-box")) || !attribute(node, AtomTransform) || !parseTransform(textOf(doc, attribute(node, AtomTransform)), m)) {
            return originTransform(doc, node, AtomTransform, vp);
        }

        if (!elementBox(l, node, parent, vp, bounds, 0)) {
            return m;
        }

        u32 origin = attribute(node, AtomTransformOrigin);
        Point o = origin ? transformOrigin(textOf(doc, origin), bounds.x1 - bounds.x0, bounds.y1 - bounds.y0) : (node == doc.root ? Point{(bounds.x1 - bounds.x0) / 2.f, (bounds.y1 - bounds.y0) / 2.f} : Point{0.f, 0.f});

        o.x += bounds.x0;
        o.y += bounds.y0;

        return multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
    }

    static bool imageReachable(const Document& doc, const Node* node) {
        u32 href = attribute(node, AtomHref);

        if (!href) {
            return false;
        }

        StringView reference = trim(textOf(doc, href));

        return startsWithIgnoreCase(reference, SV("data:")) || startsWithIgnoreCase(reference, SV("http:")) || startsWithIgnoreCase(reference, SV("https:")) || (!reference.empty() && reference[0] == '#');
    }

    static bool relativeLengths(const Document& doc, const Node* node, u32 depth) {
        static const u32 lengths[] = {AtomX, AtomY, AtomWidth, AtomHeight, AtomCx, AtomCy, AtomR, AtomRx, AtomRy, AtomX1, AtomY1, AtomX2, AtomY2};

        for (u32 name : lengths) {
            u32 value = attribute(node, name);
            Length length;

            if (value && parseLength(textOf(doc, value), length) && length.unit == Unit::Percent) {
                return true;
            }
        }

        if (depth < 16) {
            for (Node* child = node->first; child; child = child->next) {
                if (relativeLengths(doc, child, depth + 1)) {
                    return true;
                }
            }
        }

        return false;
    }

    static void lowerImageContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, float x, float y, float w, float h) {
        const Document& doc = *l.doc;
        u32 href = attribute(node, AtomHref);

        if (!href) {
            return;
        }

        StringView reference = trim(textOf(doc, href));

        if (reference.length() > 1 && reference[0] == '#') {
            Node* target = elementById(doc, fragmentAtom(doc, StringView(reference.begin() + 1, reference.end())));

            if (!target) {
                return;
            }

            for (Node* active : l.active) {
                if (active == target) {
                    return;
                }
            }

            const ComputedStyle& inherited = target->parent ? documentStyle(l, target->parent, vp) : style;

            l.active.pushBack(target);

            // The node is an feImage, in a viewport of some size.
            Matrix place = multiply(m, translation(x, y));

            if (relativeLengths(doc, target, 0)) {
                place = multiply(place, scaling(w / vp.width, h / vp.height));
            }

            lowerElement(l, target, inherited, place, vp);
            l.active.popBack();

            return;
        }

        ImageSource source;

        if (w > 0.f && h > 0.f && loadImage(l, node, source)) {
            drawImage(l, node, source, m, vp, x, y, w, h, style.imageRendering);
        }
    }

    static bool passesConditions(const Document& doc, const Node* node) {
        u32 extensions = attribute(node, AtomRequiredExtensions);

        if (extensions) {
            return false;
        }

        u32 languages = attribute(node, AtomSystemLanguage);

        if (!languages) {
            return true;
        }

        StringView text = textOf(doc, languages);
        const u8* p = text.begin();

        while (p < text.end()) {
            while (p < text.end() && (isSpace(*p) || *p == ',')) {
                p++;
            }

            const u8* start = p;

            while (p < text.end() && *p != ',' && !isSpace(*p)) {
                p++;
            }

            StringView language(start, p);

            if (equalsIgnoreCase(language, SV("en")) || startsWithIgnoreCase(language, SV("en-"))) {
                return true;
            }
        }

        return false;
    }

    static void lowerContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        switch (node->name) {
            case AtomG:
            case AtomA:
                lowerChildren(l, node, style, m, vp);
                break;
            case AtomSvg:
                lowerSvg(l, node, style, m, vp);
                break;
            case AtomUse:
                lowerUse(l, node, style, m, vp);
                break;
            case AtomImage:
                lowerImage(l, node, style, m, vp);
                break;
            case AtomSymbol: {
                float fs = style.fontSize;
                float w = l.sizedWidth >= 0.f ? l.sizedWidth : lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width);
                float h = l.sizedHeight >= 0.f ? l.sizedHeight : lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height);

                l.sized = nullptr;
                lowerViewport(l, node, style, m, vp, node, 0.f, 0.f, w, h, !style.overflowVisible);
                break;
            }
            case AtomSwitch:
                for (Node* child = node->first; child; child = child->next) {
                    if (passesConditions(*l.doc, child)) {
                        lowerElement(l, child, style, m, vp);
                        break;
                    }
                }

                break;
            default:
                lowerShape(l, node, style, m, vp);
                break;
        }
    }

    static float numberOf(const Document& doc, const Node* node, u32 name, float fallback);
    static bool numberPair(const Document& doc, const Node* node, u32 name, float& a, float& b);
    static float userX(const Filter& f, float value);
    static float userY(const Filter& f, float value);
    static int blurReach(float sigma);

    // The farthest, in device pixels, that a pixel of the filters' result
    // depends on their source: the sum of what every primitive reaches, as
    // the primitives themselves measure it. Negative when a primitive has no
    // bound, as feTile has not.
    // Adds how far the primitive reads from a pixel it writes, in device
    // pixels; false for a feTile, which reads its whole input.
    static bool primitiveReach(const Filter& f, const FilterPrimitive& p, float scaleX, float scaleY, float& reach) {
        const Document& doc = *f.doc;
        float dx = 0.f;
        float dy = 0.f;
        float sx = 0.f;
        float sy = 0.f;

        switch (p.kind) {
            case AtomFeOffset:
                dx = userX(f, numberOf(doc, p.node, AtomDx, 0.f));
                dy = userY(f, numberOf(doc, p.node, AtomDy, 0.f));
                break;
            case AtomFeGaussianBlur:
                if (!p.node) {
                    sx = p.params[0];
                    sy = p.params[1];
                } else if (attribute(p.node, AtomStdDeviation) && !numberPair(doc, p.node, AtomStdDeviation, sx, sy)) {
                    sx = 0.f;
                    sy = 0.f;
                }

                break;
            case AtomFeDropShadow:
                dx = p.node ? userX(f, numberOf(doc, p.node, AtomDx, 2.f)) : p.params[0];
                dy = p.node ? userY(f, numberOf(doc, p.node, AtomDy, 2.f)) : p.params[1];
                sx = p.node ? 2.f : p.params[2];
                sy = sx;

                if (p.node && attribute(p.node, AtomStdDeviation) && !numberPair(doc, p.node, AtomStdDeviation, sx, sy)) {
                    sx = 0.f;
                    sy = 0.f;
                }

                break;
            case AtomFeMorphology: {
                float rx = 0.f;
                float ry = 0.f;

                if (!attribute(p.node, AtomRadius) || numberPair(doc, p.node, AtomRadius, rx, ry)) {
                    reach += maxf(userX(f, rx) * scaleX, userY(f, ry) * scaleY) + 1.f;
                }
            } break;
            case AtomFeConvolveMatrix:
                reach += 32.f;
                break;
            case AtomFeDisplacementMap: {
                float scale = numberOf(doc, p.node, AtomScale, 0.f);

                reach += maxf(fabsf(userX(f, scale) * scaleX), fabsf(userY(f, scale) * scaleY)) * 0.5f + 1.f;
            } break;
            case AtomFeDiffuseLighting:
            case AtomFeSpecularLighting:
                reach += 1.f;
                break;
            case AtomFeTile:
                return false;
            default:
                break;
        }

        // A negative deviation blurs nothing along its axis.
        sx = maxf(sx, 0.f);
        sy = maxf(sy, 0.f);

        int blurX = blurReach(userX(f, sx) * scaleX);
        int blurY = blurReach(userY(f, sy) * scaleY);
        float shift = maxf(fabsf(f.ctm.a * dx + f.ctm.c * dy), fabsf(f.ctm.b * dx + f.ctm.d * dy));

        reach += (float)(blurX > blurY ? blurX : blurY);
        reach += shift > 0.f ? shift + 1.f : 0.f;

        return true;
    }

    static float filterReach(const Filter* filter) {
        float reach = 0.f;

        for (const Filter* f = filter; f; f = f->next) {
            float scaleX = sqrtf(f->ctm.a * f->ctm.a + f->ctm.b * f->ctm.b);
            float scaleY = sqrtf(f->ctm.c * f->ctm.c + f->ctm.d * f->ctm.d);

            for (u32 k = 0; k < f->count; k++) {
                if (!primitiveReach(*f, f->primitives[k], scaleX, scaleY, reach)) {
                    return -1.f;
                }
            }
        }

        return reach;
    }

    static bool filterOverflow(const Lowering& l, const Filter* filter, Box& out) {
        out = emptyBox();

        for (const Filter* f = filter; f; f = f->next) {
            out.x0 = minf(out.x0, f->device.x0);
            out.y0 = minf(out.y0, f->device.y0);
            out.x1 = maxf(out.x1, f->device.x1);
            out.y1 = maxf(out.y1, f->device.y1);
        }

        float w = (float)l.width;
        float h = (float)l.height;

        // The union of one filter or more is a box.
        if (out.x0 >= -0.5f && out.y0 >= -0.5f && out.x1 <= w + 0.5f && out.y1 <= h + 0.5f) {
            return false;
        }

        // The canvas depends on the source within the filters' reach of it;
        // the rest of the region is left out of the layer.
        float reach = filterReach(filter);

        if (reach >= 0.f) {
            out.x0 = maxf(out.x0, -reach);
            out.y0 = maxf(out.y0, -reach);
            out.x1 = minf(out.x1, w + reach);
            out.y1 = minf(out.y1, h + reach);
        }

        out.x0 = floorf(maxf(out.x0, -2.f * w));
        out.y0 = floorf(maxf(out.y0, -2.f * h));
        out.x1 = ceilf(minf(out.x1, 3.f * w));
        out.y1 = ceilf(minf(out.y1, 3.f * h));

        return out.x1 > out.x0 && out.y1 > out.y0;
    }

    // Draws the element on a layer of its own: the box fixed, or else the
    // axis-aligned frame of its transform, holding the region of the filters,
    // lowered on the raster to be measured, as far as the raster's reach.
    static bool lowerFilteredLayer(Lowering& l, Node* node, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, const Box* fixed, const Filter* measured) {
        float sx = sqrtf(m.a * m.a + m.b * m.b);
        float sy = sqrtf(m.c * m.c + m.d * m.d);
        Matrix toLayer = identity();
        float x0;
        float y0;
        float lw;
        float lh;

        if (fixed) {
            x0 = fixed->x0;
            y0 = fixed->y0;
            lw = fixed->x1 - fixed->x0;
            lh = fixed->y1 - fixed->y0;
        } else {
            if (sx <= 0.f || sy <= 0.f || !invert(multiply(m, scaling(1.f / sx, 1.f / sy)), toLayer)) {
                return false;
            }

            // Axis permutations can keep device pixel centres exactly on the
            // layer's pixel centres. Retain the element's translation inside
            // the filter instead of resampling its result at a fractional phase.
            if ((fabsf(toLayer.b) < 1e-6f && fabsf(toLayer.c) < 1e-6f) || (fabsf(toLayer.a) < 1e-6f && fabsf(toLayer.d) < 1e-6f)) {
                toLayer.a = roundf(toLayer.a);
                toLayer.b = roundf(toLayer.b);
                toLayer.c = roundf(toLayer.c);
                toLayer.d = roundf(toLayer.d);
                toLayer.e = 0.f;
                toLayer.f = 0.f;
            }

            Box visible = deviceBox(toLayer, Box{0.f, 0.f, (float)l.width, (float)l.height});
            float margin = 0.25f * maxf(visible.x1 - visible.x0, visible.y1 - visible.y0);
            float reach = measured ? filterReach(measured) : -1.f;
            Box wanted = visible;

            if (reach >= 0.f) {
                margin = reach + 1.f;
            }

            wanted.x0 -= margin;
            wanted.y0 -= margin;
            wanted.x1 += margin;
            wanted.y1 += margin;

            if (measured) {
                Box region = emptyBox();

                for (const Filter* f = measured; f; f = f->next) {
                    Box device = deviceBox(toLayer, f->device);

                    region.x0 = minf(region.x0, device.x0);
                    region.y0 = minf(region.y0, device.y0);
                    region.x1 = maxf(region.x1, device.x1);
                    region.y1 = maxf(region.y1, device.y1);
                }

                wanted.x0 = maxf(wanted.x0, region.x0);
                wanted.y0 = maxf(wanted.y0, region.y0);
                wanted.x1 = minf(wanted.x1, region.x1);
                wanted.y1 = minf(wanted.y1, region.y1);
            }

            if (!(wanted.x1 > wanted.x0 && wanted.y1 > wanted.y0)) {
                return false;
            }

            x0 = floorf(wanted.x0);
            y0 = floorf(wanted.y0);
            lw = ceilf(wanted.x1) - x0;
            lh = ceilf(wanted.y1) - y0;
        }

        // The box is of some size, fixed or wanted.
        if (lw * lh > (fixed ? 25.f : 16.f) * (float)l.width * (float)l.height) {
            return false;
        }

        Matrix layer = multiply(translation(-x0, -y0), toLayer);
        Matrix inner = multiply(layer, m);
        u32 width = l.width;
        u32 height = l.height;

        l.width = (u32)lw;
        l.height = (u32)lh;

        Filter* filter = lowerFilters(l, node, parent, style, inner, vp);
        Vector<Op> content;
        Vector<Op>* saved = l.out;

        if (filter) {
            l.out = &content;
            lowerContent(l, node, style, inner, vp);
            l.out = saved;
        }

        l.width = width;
        l.height = height;

        if (!filter) {
            return false;
        }

        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Layer;
        op.first = (u32)l.programs.length();
        op.count = (u32)content.length();
        op.filter = filter;
        op.layerWidth = (u32)lw;
        op.layerHeight = (u32)lh;
        op.layerMatrix = layer;

        Matrix toDevice;

        // The layer's frame inverts, as it came of an inverse.
        invert(layer, toDevice);
        op.bounds = deviceBox(toDevice, Box{-4.f, -4.f, lw + 4.f, lh + 4.f});
        l.programs.append(content.data(), content.length());
        l.out->pushBack(op);

        return true;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp) {
        bool instance = node->name == AtomSymbol && l.sized == node;

        if ((!renderable(node->name) && !instance) || l.depth > 256 || (node != l.doc->root && !passesConditions(*l.doc, node))) {
            return;
        }

        if (node->name == AtomImage && !imageReachable(*l.doc, node)) {
            return;
        }

        ComputedStyle style = computeStyle(l, node, parent, vp);

        if ((style.displayNone && node != l.doc->root) || style.opacity <= 0.f) {
            return;
        }

        Matrix m = multiply(ctm, boxTransform(l, node, parent, vp));
        Op op = groupOp(style.opacity);

        if (node->name == AtomUse) {
            m = multiply(m, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, style.fontSize, 0.f)));
        }

        if (style.clipPath) {
            op.clipped = resolveClip(l, node, parent, style, m, vp, op);
        } else if (style.clipShape) {
            shapeClip(l, node, parent, style, m, vp, op);
            op.clipped = true;
        }

        if (style.mask) {
            resolveMask(l, node, parent, style, m, vp, op);
        }

        bool complex = style.filterCount && node != l.doc->root && (fabsf(m.b) > 1e-6f || fabsf(m.c) > 1e-6f || m.a < 0.f || m.d < 0.f);
        bool offscreen = false;
        Box extent;
        const Filter* measured = nullptr;

        if (style.filterCount) {
            Filter* filter = lowerFilters(l, node, parent, style, m, vp);

            // The filters lowered on the raster size the layer of an element
            // drawn apart, under a transform or with a region past the raster.
            offscreen = filter && !complex && node != l.doc->root && filterOverflow(l, filter, extent);
            measured = complex || offscreen ? filter : nullptr;
            op.filter = measured ? nullptr : filter;
        }

        op.blend = style.blend;

        op.newBackground = l.backgroundInputs && style.newBackground && (node->name == AtomSvg || node->name == AtomG || node->name == AtomA || node->name == AtomSwitch || node->name == AtomSymbol);
        op.backgroundClipped = style.backgroundClipped;
        op.backgroundBox = style.backgroundBox;

        if (op.newBackground && op.backgroundClipped) {
            Matrix content = m;

            if (node == l.doc->root) {
                content = multiply(content, rootContent(l, node));
            } else if (node->name == AtomSvg || node->name == AtomSymbol) {
                float w = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, style.fontSize, vp.width);
                float h = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, style.fontSize, vp.height);
                Box viewBox;

                if (l.sized == node) {
                    w = l.sizedWidth >= 0.f ? l.sizedWidth : w;
                    h = l.sizedHeight >= 0.f ? l.sizedHeight : h;
                }

                content = multiply(content, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, style.fontSize, 0.f)));

                if (parseViewBox(*l.doc, node, viewBox)) {
                    content = multiply(content, viewBoxTransform(*l.doc, node, viewBox, w, h));
                }
            }

            if (!invert(content, op.backgroundMatrix)) {
                op.backgroundBox = Box{0.f, 0.f, 0.f, 0.f};
            }
        }

        bool layer = op.clipped || op.masked || op.filter || style.opacity < 1.f || style.blend || style.isolate;

        op.backgroundOnly = op.newBackground && !layer;
        layer = layer || op.newBackground;

        u32 index = (u32)l.out->length();

        if (layer) {
            l.out->pushBack(op);
        }

        l.depth++;

        if ((offscreen || complex) && !lowerFilteredLayer(l, node, parent, style, m, vp, offscreen ? &extent : nullptr, measured)) {
            Op fallback = groupOp(1.f);
            u32 fallbackIndex = (u32)l.out->length();

            fallback.filter = lowerFilters(l, node, parent, style, m, vp);
            l.out->pushBack(fallback);
            lowerContent(l, node, style, m, vp);
            endGroup(l, fallbackIndex);
        } else if (!offscreen && !complex) {
            lowerContent(l, node, style, m, vp);
        }

        Box outline;

        if (style.outlineWidth > 0.f && style.outlineColor.a > 0.f && elementBox(l, node, parent, vp, outline, 0)) {
            float w = style.outlineWidth;
            Path ring;
            Flat flat;

            moveTo(ring, Point{outline.x0 - w, outline.y0 - w});
            lineTo(ring, Point{outline.x1 + w, outline.y0 - w});
            lineTo(ring, Point{outline.x1 + w, outline.y1 + w});
            lineTo(ring, Point{outline.x0 - w, outline.y1 + w});
            closePath(ring);
            moveTo(ring, Point{outline.x0, outline.y0});
            lineTo(ring, Point{outline.x1, outline.y0});
            lineTo(ring, Point{outline.x1, outline.y1});
            lineTo(ring, Point{outline.x0, outline.y1});
            closePath(ring);
            flatten(ring, m, tolerance, flat);
            emitFill(l, flat, WindingEvenOdd, solidPaint(l, style.outlineColor, 1.f), false);
        }

        l.depth--;

        if (layer) {
            endGroup(l, index);
        }
    }

    const float zeroPixel[4] = {0.f, 0.f, 0.f, 0.f};

    // A box is empty as boxInside leaves it, by its x.
    static Plane makePlane(IntBox box) {
        size_t count = box.x1 > box.x0 ? (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0) : 0;
        Plane plane{count ? new float[count * 4] : nullptr, box};

        if (count) {
            memset(plane.data, 0, sizeof(float) * count * 4);
        }

        return plane;
    }

    static void releasePlane(Plane& plane) {
        delete[] plane.data;
        plane.data = nullptr;
    }

    static int planeWidth(const Plane& plane) {
        return plane.box.x1 - plane.box.x0;
    }

    // The pixel at x, y of a plane whose box holds it.
    static float* planeAt(const Plane& plane, int x, int y) {
        return plane.data + ((size_t)(y - plane.box.y0) * (size_t)planeWidth(plane) + (size_t)(x - plane.box.x0)) * 4;
    }

    // The pixel at x, y, zero outside the plane.
    static const float* planeRead(const Plane& plane, int x, int y) {
        if (x < plane.box.x0 || x >= plane.box.x1 || y < plane.box.y0 || y >= plane.box.y1) {
            return zeroPixel;
        }

        return planeAt(plane, x, y);
    }

    // The pixels from x0 to x1 of row y of a plane, zero outside it.
    static void readSpan(const Plane& plane, int x0, int x1, int y, float* dst) {
        int inside0 = x0 > plane.box.x0 ? x0 : plane.box.x0;
        int inside1 = x1 < plane.box.x1 ? x1 : plane.box.x1;

        if (y < plane.box.y0 || y >= plane.box.y1 || inside1 <= inside0) {
            memset(dst, 0, sizeof(float) * 4 * (size_t)(x1 - x0));

            return;
        }

        memset(dst, 0, sizeof(float) * 4 * (size_t)(inside0 - x0));
        memcpy(dst + (size_t)(inside0 - x0) * 4, planeAt(plane, inside0, y), sizeof(float) * 4 * (size_t)(inside1 - inside0));
        memset(dst + (size_t)(inside1 - x0) * 4, 0, sizeof(float) * 4 * (size_t)(x1 - inside1));
    }

    static IntBox boxInside(IntBox box, const IntBox& bound) {
        box.x0 = box.x0 < bound.x0 ? bound.x0 : box.x0;
        box.y0 = box.y0 < bound.y0 ? bound.y0 : box.y0;
        box.x1 = box.x1 > bound.x1 ? bound.x1 : box.x1;
        box.y1 = box.y1 > bound.y1 ? bound.y1 : box.y1;

        if (box.x1 <= box.x0 || box.y1 <= box.y0) {
            return IntBox{0, 0, 0, 0};
        }

        return box;
    }

    // The bounding box of two pixel boxes, an empty one adding nothing: a
    // box is empty as boxInside leaves it, by its x.
    static IntBox boxJoin(const IntBox& a, const IntBox& b) {
        if (a.x1 <= a.x0) {
            return b;
        }

        if (b.x1 <= b.x0) {
            return a;
        }

        return IntBox{a.x0 < b.x0 ? a.x0 : b.x0, a.y0 < b.y0 ? a.y0 : b.y0, a.x1 > b.x1 ? a.x1 : b.x1, a.y1 > b.y1 ? a.y1 : b.y1};
    }

    // The pixels of the canvas that drawing within the bounds can touch,
    // rounded as rasterize rounds a fill's.
    static IntBox layerBox(const Box& bounds, const IntBox& canvas) {
        if (!boxValid(bounds)) {
            return IntBox{0, 0, 0, 0};
        }

        float x0 = floorf(maxf(bounds.x0, (float)canvas.x0));
        float y0 = floorf(maxf(bounds.y0, (float)canvas.y0));
        float x1 = ceilf(minf(bounds.x1, (float)canvas.x1));
        float y1 = ceilf(minf(bounds.y1, (float)canvas.y1));

        if (!(x1 > x0) || !(y1 > y0)) {
            return IntBox{0, 0, 0, 0};
        }

        return IntBox{(int)x0, (int)y0, (int)x1, (int)y1};
    }

    static void addLine(float* acc, u32 stride, u32 width, float x0, float y0, float x1, float y1) {
        if (y0 == y1) {
            return;
        }

        float dir = 1.f;

        if (y0 > y1) {
            float tx = x0;
            float ty = y0;

            x0 = x1;
            y0 = y1;
            x1 = tx;
            y1 = ty;
            dir = -1.f;
        }

        // A line spans a band of one row of the raster, its cuts.
        float w = (float)width;
        float dxdy = (x1 - x0) / (y1 - y0);
        float x = x0;
        u32 ystart = (u32)y0;
        u32 yend = (u32)ceilf(y1);

        for (u32 y = ystart; y < yend; y++) {
            float* line = acc + (size_t)y * stride;
            float dy = minf((float)(y + 1), y1) - maxf((float)y, y0);
            float xnext = x + dxdy * dy;
            float d = dy * dir;
            float xa = minf(maxf(minf(x, xnext), 0.f), w);
            float xb = minf(maxf(maxf(x, xnext), 0.f), w);
            float x0floor = floorf(xa);
            int x0i = (int)x0floor;
            float x1ceil = ceilf(xb);
            int x1i = (int)x1ceil;

            if (x1i <= x0i + 1) {
                float xmf = 0.5f * (xa + xb) - x0floor;

                line[x0i] += d - d * xmf;
                line[x0i + 1] += d * xmf;
            } else {
                float s = 1.f / (xb - xa);
                float x0f = xa - x0floor;
                float a0 = 0.5f * s * (1.f - x0f) * (1.f - x0f);
                float x1f = xb - x1ceil + 1.f;
                float am = 0.5f * s * x1f * x1f;

                line[x0i] += d * a0;

                if (x1i == x0i + 2) {
                    line[x0i + 1] += d * (1.f - a0 - am);
                } else {
                    float a1 = s * (1.5f - x0f);

                    line[x0i + 1] += d * (a1 - a0);

                    for (int xi = x0i + 2; xi < x1i - 1; xi++) {
                        line[xi] += d * s;
                    }

                    float a2 = a1 + (float)(x1i - x0i - 3) * s;

                    line[x1i - 1] += d * (1.f - a2 - am);
                }

                line[x1i] += d * am;
            }

            x = xnext;
        }
    }

    static void addEdge(float* acc, u32 stride, u32 width, Edge e) {
        float w = (float)width;
        float bounds[2] = {0.f, w};
        float ts[2];
        u32 cuts = 0;

        for (float bound : bounds) {
            if ((e.x0 < bound) != (e.x1 < bound)) {
                float t = (bound - e.x0) / (e.x1 - e.x0);

                if (t > 0.f && t < 1.f) {
                    ts[cuts++] = t;
                }
            }
        }

        if (cuts == 2 && ts[0] > ts[1]) {
            float t = ts[0];

            ts[0] = ts[1];
            ts[1] = t;
        }

        u32 count = 0;
        float px[4];
        float py[4];

        px[count] = e.x0;
        py[count] = e.y0;
        count++;

        for (u32 i = 0; i < cuts; i++) {
            px[count] = e.x0 + (e.x1 - e.x0) * ts[i];
            py[count] = e.y0 + (e.y1 - e.y0) * ts[i];
            count++;
        }

        px[count] = e.x1;
        py[count] = e.y1;
        count++;

        for (u32 i = 0; i + 1 < count; i++) {
            float ax = px[i];
            float bx = px[i + 1];
            float mid = (ax + bx) * 0.5f;

            if (mid >= w) {
                continue;
            }

            if (mid <= 0.f) {
                ax = 0.f;
                bx = 0.f;
            }

            addLine(acc, stride, width, ax, py[i], bx, py[i + 1]);
        }
    }

    static float coverage(float value, u8 rule) {
        float a = fabsf(value);

        if (rule == WindingEvenOdd) {
            a = fmodf(a, 2.f);

            return a > 1.f ? 2.f - a : a;
        }

        return a > 1.f ? 1.f : a;
    }

    static float spreadT(float t, u8 spread) {
        if (spread == SpreadRepeat) {
            return t - floorf(t);
        }

        if (spread == SpreadReflect) {
            float m = fmodf(fabsf(t), 2.f);

            return m > 1.f ? 2.f - m : m;
        }

        return clampUnit(t);
    }

    static Color gradientColor(const DevicePaint& paint, float t) {
        const GradientStop* stops = paint.stops;
        u32 count = paint.stopCount;
        Color c;

        if (t <= stops[0].offset) {
            c = stops[0].color;
        } else if (t >= stops[count - 1].offset) {
            c = stops[count - 1].color;
        } else {
            // t lies past the first offset and before the last, so a stop
            // follows at or past it and the span to it is not empty: the
            // first one, as the offsets never decrease.
            u32 i = 1;
            u32 n = count - 1;

            while (i < n) {
                u32 m = i + (n - i) / 2;

                if (stops[m].offset < t) {
                    i = m + 1;
                } else {
                    n = m;
                }
            }

            const GradientStop& a = stops[i - 1];
            const GradientStop& b = stops[i];
            float f = (t - a.offset) / (b.offset - a.offset);

            c = Color{a.color.r + (b.color.r - a.color.r) * f, a.color.g + (b.color.g - a.color.g) * f, a.color.b + (b.color.b - a.color.b) * f, a.color.a + (b.color.a - a.color.a) * f};
        }

        if (paint.linear) {
            c = Color{linearToSrgb(c.r), linearToSrgb(c.g), linearToSrgb(c.b), c.a};
        }

        return Color{c.r * c.a, c.g * c.a, c.b * c.a, c.a};
    }

    static float mitchell(float x) {
        x = fabsf(x);

        if (x < 1.f) {
            return (7.f * x * x * x - 12.f * x * x + 16.f / 3.f) / 6.f;
        }

        if (x < 2.f) {
            return (-7.f / 3.f * x * x * x + 12.f * x * x - 20.f * x + 32.f / 3.f) / 6.f;
        }

        return 0.f;
    }

    struct ByteUnits {
        float values[256];
    };

    // The quotients byte / 255.f, each computed once.
    constexpr ByteUnits byteUnits = [] {
        ByteUnits units{};

        for (int i = 0; i < 256; i++) {
            units.values[i] = (float)i / 255.f;
        }

        return units;
    }();

    static Color texel(const DevicePaint& paint, int x, int y) {
        x = x < 0 ? 0 : (x >= (int)paint.imageWidth ? (int)paint.imageWidth - 1 : x);
        y = y < 0 ? 0 : (y >= (int)paint.imageHeight ? (int)paint.imageHeight - 1 : y);

        const u8* t = paint.image + ((size_t)y * paint.imageWidth + x) * 4;
        float a = byteUnits.values[t[3]];

        return Color{byteUnits.values[t[0]] * a, byteUnits.values[t[1]] * a, byteUnits.values[t[2]] * a, a};
    }

    static bool paintAt(const DevicePaint& paint, float x, float y, Color& out) {
        if (paint.type == PaintType::Solid) {
            out = paint.color;

            return true;
        }

        Point p = apply(paint.inverse, Point{x, y});

        if (paint.type == PaintType::Mesh) {
            int ix = (int)floorf(p.x);
            int iy = (int)floorf(p.y);

            if (ix < 0 || iy < 0 || ix >= (int)paint.imageWidth || iy >= (int)paint.imageHeight) {
                return false;
            }

            const float* c = paint.tile + ((size_t)iy * paint.imageWidth + ix) * 4;

            out = Color{c[0], c[1], c[2], c[3]};

            return c[3] > 0.f;
        }

        if (paint.type == PaintType::Pattern) {
            int tw = (int)paint.imageWidth;
            int th = (int)paint.imageHeight;
            float fx = p.x - 0.5f;
            float fy = p.y - 0.5f;
            float x0 = floorf(fx);
            float y0 = floorf(fy);
            float tx = fx - x0;
            float ty = fy - y0;
            int ix = (int)fmodf(x0, (float)tw);
            int iy = (int)fmodf(y0, (float)th);

            ix = ix < 0 ? ix + tw : ix;
            iy = iy < 0 ? iy + th : iy;

            int jx = ix + 1 == tw ? 0 : ix + 1;
            int jy = iy + 1 == th ? 0 : iy + 1;
            const float* c00 = paint.tile + ((size_t)iy * tw + ix) * 4;
            const float* c10 = paint.tile + ((size_t)iy * tw + jx) * 4;
            const float* c01 = paint.tile + ((size_t)jy * tw + ix) * 4;
            const float* c11 = paint.tile + ((size_t)jy * tw + jx) * 4;
            float w00 = (1.f - tx) * (1.f - ty) * paint.opacity;
            float w10 = tx * (1.f - ty) * paint.opacity;
            float w01 = (1.f - tx) * ty * paint.opacity;
            float w11 = tx * ty * paint.opacity;

            out = Color{
                c00[0] * w00 + c10[0] * w10 + c01[0] * w01 + c11[0] * w11,
                c00[1] * w00 + c10[1] * w10 + c01[1] * w01 + c11[1] * w11,
                c00[2] * w00 + c10[2] * w10 + c01[2] * w01 + c11[2] * w11,
                c00[3] * w00 + c10[3] * w10 + c01[3] * w01 + c11[3] * w11,
            };

            return out.a > 0.f;
        }

        if (paint.type == PaintType::Image) {
            if (!paint.smooth) {
                out = texel(paint, (int)floorf(p.x), (int)floorf(p.y));

                return true;
            }

            float fx = p.x - 0.5f;
            float fy = p.y - 0.5f;
            float x0 = floorf(fx);
            float y0 = floorf(fy);
            float tx = fx - x0;
            float ty = fy - y0;
            float wx[4] = {mitchell(tx + 1.f), mitchell(tx), mitchell(1.f - tx), mitchell(2.f - tx)};
            float wy[4] = {mitchell(ty + 1.f), mitchell(ty), mitchell(1.f - ty), mitchell(2.f - ty)};
            int xs[4];
            int ys[4];

            for (int i = 0; i < 4; i++) {
                int x = (int)x0 - 1 + i;
                int y = (int)y0 - 1 + i;

                xs[i] = x < 0 ? 0 : (x >= (int)paint.imageWidth ? (int)paint.imageWidth - 1 : x);
                ys[i] = y < 0 ? 0 : (y >= (int)paint.imageHeight ? (int)paint.imageHeight - 1 : y);
            }

            Color sum{0.f, 0.f, 0.f, 0.f};

            for (int j = 0; j < 4; j++) {
                const u8* row = paint.image + (size_t)ys[j] * paint.imageWidth * 4;

                for (int i = 0; i < 4; i++) {
                    const u8* t = row + (size_t)xs[i] * 4;
                    float a = byteUnits.values[t[3]];
                    float w = wx[i] * wy[j];

                    sum.r += byteUnits.values[t[0]] * a * w;
                    sum.g += byteUnits.values[t[1]] * a * w;
                    sum.b += byteUnits.values[t[2]] * a * w;
                    sum.a += a * w;
                }
            }

            float a = clampUnit(sum.a);

            out = Color{maxf(0.f, minf(sum.r, a)), maxf(0.f, minf(sum.g, a)), maxf(0.f, minf(sum.b, a)), a};

            return true;
        }

        if (paint.type == PaintType::Linear) {
            float dx = paint.x2 - paint.x1;
            float dy = paint.y2 - paint.y1;
            float t = ((p.x - paint.x1) * dx + (p.y - paint.y1) * dy) / (dx * dx + dy * dy);

            out = gradientColor(paint, spreadT(t, paint.spread));

            return true;
        }

        float cdx = paint.cx - paint.fx;
        float cdy = paint.cy - paint.fy;
        float dr = paint.r - paint.fr;
        float pdx = p.x - paint.fx;
        float pdy = p.y - paint.fy;
        float a = cdx * cdx + cdy * cdy - dr * dr;
        float b = pdx * cdx + pdy * cdy + paint.fr * dr;
        float c = pdx * pdx + pdy * pdy - paint.fr * paint.fr;
        float t;

        if (fabsf(a) < 1e-9f) {
            if (b == 0.f) {
                return false;
            }

            t = c / (2.f * b);
        } else {
            float disc = b * b - a * c;

            if (disc < 0.f) {
                return false;
            }

            float root = sqrtf(disc);
            float t1 = (b + root) / a;
            float t2 = (b - root) / a;

            t = maxf(t1, t2);

            if (paint.fr + t * dr < 0.f) {
                t = minf(t1, t2);
            }
        }

        if (paint.fr + t * dr < 0.f) {
            return false;
        }

        out = gradientColor(paint, spreadT(t, paint.spread));

        return true;
    }

    static void addAliasedEdge(float* acc, u32 stride, u32 width, u32 height, Edge e) {
        float dir = 1.f;

        if (e.y0 > e.y1) {
            e = Edge{e.x1, e.y1, e.x0, e.y0};
            dir = -1.f;
        }

        float top = floorf(e.y0 + 0.5f);
        float bottom = floorf(e.y1 + 0.5f);
        float dxdy = (e.x1 - e.x0) / (e.y1 - e.y0);
        int y0 = top < 0.f ? 0 : (int)top;
        int y1 = bottom > (float)height ? (int)height : (int)bottom;

        for (int y = y0; y < y1; y++) {
            float x = floorf(e.x0 + ((float)y + 0.5f - e.y0) * dxdy + 0.5f);
            int column = x < 0.f ? 0 : (x > (float)width ? (int)width : (int)x);

            acc[(size_t)y * stride + column] += dir;
        }
    }

    struct BandEdge {
        float x0;
        float y0;
        float x1;
        float y1;
        int dir;
    };

    static float bandX(const BandEdge& e, float y) {
        return e.x0 + (e.x1 - e.x0) * ((y - e.y0) / (e.y1 - e.y0));
    }

    static bool insideWinding(int winding, u8 rule) {
        return rule == WindingEvenOdd ? (winding & 1) != 0 : winding != 0;
    }

    struct BandSample {
        float x0;
        float x1;
        float key;
        int dir;
        u32 order;
        u32 edge;
    };

    static void xchg(BandEdge& l, BandEdge& r) noexcept {
        BandEdge t = l;

        l = r;
        r = t;
    }

    static void xchg(BandSample& l, BandSample& r) noexcept {
        BandSample t = l;

        l = r;
        r = t;
    }

    // The samples of a band: the edges spanning it in the order given, with
    // the x where each enters, leaves and crosses the middle, and the place
    // of each in the active list; the edges not spanning it go to rest.
    static bool bandSamples(const BandEdge* edges, const Vector<u32>& order, const Vector<u32>& position, float a, float b, Vector<BandSample>& samples, Vector<u32>& rest) {
        float mid = (a + b) * 0.5f;
        bool comparable = true;

        samples.clear();
        rest.clear();

        for (u32 index : order) {
            const BandEdge& e = edges[index];

            if (e.y0 <= a && e.y1 >= b) {
                float key = bandX(e, mid);

                comparable = comparable && key == key;
                samples.pushBack(BandSample{bandX(e, a), bandX(e, b), key, e.dir, position[index], index});
            } else {
                rest.pushBack(index);
            }
        }

        return comparable;
    }

    // The samples of a half of a band, in the order of the whole: the cuts
    // leave no edge end inside a band, so the same edges span the half,
    // entering or leaving it where they cross the middle of the whole band.
    static bool halfBandSamples(const BandEdge* edges, const Vector<BandSample>& whole, float a, float b, bool top, Vector<BandSample>& samples) {
        float mid = (a + b) * 0.5f;
        bool comparable = true;

        samples.clear();

        for (const BandSample& s : whole) {
            float key = bandX(edges[s.edge], mid);

            comparable = comparable && key == key;
            samples.pushBack(BandSample{top ? s.x0 : s.key, top ? s.key : s.x1, key, s.dir, s.order, s.edge});
        }

        return comparable;
    }

    static void unionBand(float* acc, u32 stride, u32 w, u32 h, const BandEdge* edges, const Vector<u32>& active, const Vector<u32>& position, Vector<BandSample>* samples, float a, float b, u8 rule, int depth, bool comparable) {
        Vector<BandSample>& band = samples[depth];

        if (band.empty()) {
            return;
        }

        // Samples sort by their keys, equal keys by the order of their edges
        // in the active list. A whole band comes in the order of the band
        // before and a half band in the order of the whole, nearly sorted, so
        // the insertion sort is about linear; a band that moves them far is
        // sorted outright. A key that is not a number stops the insertion
        // sort where it stands, so that sort runs over the order of the
        // active list, which the samples take first.
        BandSample* sorted = band.mutData();
        size_t count = band.length();
        size_t moves = 0;

        if (!comparable) {
            quickSort(sorted, sorted + count, [](const BandSample& p, const BandSample& q) {
                return p.order < q.order;
            });
        }

        for (size_t i = 1; i < count; i++) {
            BandSample sample = sorted[i];
            size_t k = i;

            while (k > 0 && (sorted[k - 1].key > sample.key || (comparable && sorted[k - 1].key == sample.key && sorted[k - 1].order > sample.order))) {
                sorted[k] = sorted[k - 1];
                k--;
            }

            sorted[k] = sample;
            moves += i - k;

            if (comparable && moves > 8 * count + 64) {
                quickSort(sorted, sorted + count, [](const BandSample& p, const BandSample& q) {
                    return p.key < q.key || (p.key == q.key && p.order < q.order);
                });

                break;
            }
        }

        if (depth < 6) {
            for (size_t k = 0; k + 1 < band.length(); k++) {
                const BandSample& p = band[k];
                const BandSample& q = band[k + 1];

                // Edges crossing within a sixty-fourth of a pixel are left as they are.
                if (p.x0 > q.x0 + 1.f / 64.f || p.x1 > q.x1 + 1.f / 64.f) {
                    float mid = (a + b) * 0.5f;
                    Vector<BandSample>& half = samples[depth + 1];
                    // A middle that rounds onto an end of the band is spanned by more edges.
                    bool split = a < mid && mid < b;
                    Vector<u32> spare;
                    bool comparableHalf = split ? halfBandSamples(edges, band, a, mid, true, half) : bandSamples(edges, active, position, a, mid, half, spare);

                    unionBand(acc, stride, w, h, edges, active, position, samples, a, mid, rule, depth + 1, comparableHalf);

                    comparableHalf = split ? halfBandSamples(edges, band, mid, b, false, half) : bandSamples(edges, active, position, mid, b, half, spare);

                    unionBand(acc, stride, w, h, edges, active, position, samples, mid, b, rule, depth + 1, comparableHalf);

                    return;
                }
            }
        }

        int winding = 0;

        for (const BandSample& sample : band) {
            bool before = insideWinding(winding, rule);

            winding += sample.dir;

            bool after = insideWinding(winding, rule);

            if (before != after) {
                addEdge(acc, stride, w, after ? Edge{sample.x0, a, sample.x1, b} : Edge{sample.x1, b, sample.x0, a});
            }
        }
    }

    struct Crossing {
        float x;
        int dir;
        u32 edge;
    };

    // Counts the crossings into place by direction, then by each byte of
    // the bits of x, low to high: x clamped to the raster is not negative,
    // so its bits order as it does, and the spans clamp it anyway.
    static void countCrossings(Vector<Crossing>& crossings, float width, Vector<Crossing>& scratch) {
        size_t count = crossings.length();
        Crossing* data = crossings.mutData();

        scratch.zero(count);

        Crossing* other = scratch.mutData();
        size_t counts[256];

        for (size_t i = 0; i < count; i++) {
            data[i].x = data[i].x < 0.f ? 0.f : (data[i].x > width ? width : data[i].x);
        }

        for (int pass = -1; pass < 4; pass++) {
            auto key = [&](const Crossing& c) {
                if (pass < 0) {
                    return (size_t)(c.dir > 0);
                }

                u32 bits;

                memcpy(&bits, &c.x, sizeof(bits));

                return (size_t)((bits >> (8 * pass)) & 0xff);
            };

            memset(counts, 0, sizeof(counts));

            for (size_t i = 0; i < count; i++) {
                counts[key(data[i])]++;
            }

            size_t start = 0;

            for (size_t k = 0; k < 256; k++) {
                size_t n = counts[k];

                counts[k] = start;
                start += n;
            }

            for (size_t i = 0; i < count; i++) {
                other[counts[key(data[i])]++] = data[i];
            }

            Crossing* t = data;

            data = other;
            other = t;
        }

        // Five passes leave the sorted crossings in the scratch.
        crossings.xchg(scratch);
    }

    // Sorts the crossings by x, equal ones by direction. Those of a scanline
    // come in the order of the one before, nearly sorted, so the insertion
    // sort is about linear; a scanline that moves them far counts them into
    // place instead.
    static void sortCrossings(Vector<Crossing>& crossings, float width, Vector<Crossing>& scratch) {
        Crossing* data = crossings.mutData();
        size_t count = crossings.length();
        size_t moves = 0;

        for (size_t i = 1; i < count; i++) {
            Crossing c = data[i];
            size_t k = i;

            while (k > 0 && (data[k - 1].x > c.x || (data[k - 1].x == c.x && data[k - 1].dir > c.dir))) {
                data[k] = data[k - 1];
                k--;
            }

            data[k] = c;
            moves += i - k;

            if (moves > 2 * count + 64) {
                countCrossings(crossings, width, scratch);

                return;
            }
        }
    }

    // The crossings of the edges at y, in the order the edges come, sorted;
    // the edges that do not span y go to rest.
    static void scanCrossings(const BandEdge* edges, const Vector<u32>& order, float y, float width, Vector<Crossing>& crossings, Vector<Crossing>& scratch, Vector<u32>& rest) {
        crossings.clear();
        rest.clear();

        for (u32 index : order) {
            const BandEdge& e = edges[index];

            if (e.y0 <= y && e.y1 > y) {
                crossings.pushBack(Crossing{bandX(e, y), e.dir, index});
            } else {
                rest.pushBack(index);
            }
        }

        sortCrossings(crossings, width, scratch);
    }

    static void addSpan(float* acc, u32 w, float x0, float x1, float weight) {
        float limit = (float)w;

        x0 = x0 < 0.f ? 0.f : (x0 > limit ? limit : x0);
        x1 = x1 < 0.f ? 0.f : (x1 > limit ? limit : x1);

        if (!(x1 > x0)) {
            return;
        }

        float c0 = floorf(x0);
        float c1 = floorf(x1);
        u32 i0 = (u32)c0;
        u32 i1 = (u32)c1;

        acc[i0] += (1.f - (x0 - c0)) * weight;
        acc[i0 + 1] += (x0 - c0) * weight;
        acc[i1] -= (1.f - (x1 - c1)) * weight;
        acc[i1 + 1] -= (x1 - c1) * weight;
    }

    static bool edgeBefore(const BandEdge& p, const BandEdge& q) {
        if (p.y0 != q.y0) {
            return p.y0 < q.y0;
        }

        if (p.x0 != q.x0) {
            return p.x0 < q.x0;
        }

        if (p.y1 != q.y1) {
            return p.y1 < q.y1;
        }

        return p.x1 < q.x1 || (p.x1 == q.x1 && p.dir < q.dir);
    }

    // The band edge of an edge of the lowering, its ends in order of y and
    // its direction kept, in the raster's coordinates.
    static BandEdge bandEdge(Edge e, float ox, float oy) {
        if (e.y0 < e.y1) {
            return BandEdge{e.x0 - ox, e.y0 - oy, e.x1 - ox, e.y1 - oy, 1};
        }

        return BandEdge{e.x1 - ox, e.y1 - oy, e.x0 - ox, e.y0 - oy, -1};
    }

    static void rasterizeUnion(float* acc, u32 stride, u32 w, u32 h, const Lowering& l, u32 first, u32 count, float ox, float oy, u8 rule) {
        Vector<BandEdge> edges;
        // The band edges are built in place, counted into the rows they start
        // in; an edge level in y is no band edge. A few are then sorted whole,
        // many are left in their order within a row, which only breaks ties
        // between samples of one key. The rows above and below the raster are
        // a bucket each.
        auto bucket = [&](float y0) {
            return y0 < 0.f ? 0u : (y0 >= (float)h ? h + 1 : (u32)y0 + 1);
        };
        Vector<u32> starts;

        starts.zero((size_t)h + 3);

        for (u32 i = first; i < first + count; i++) {
            Edge e = l.edges[i];

            if (e.y0 < e.y1 || e.y1 < e.y0) {
                starts.mut(bucket(minf(e.y0, e.y1) - oy) + 1)++;
            }
        }

        for (size_t k = 1; k < starts.length(); k++) {
            starts.mut(k) += starts[k - 1];
        }

        if (starts.back() == 0) {
            return;
        }

        edges.zero(starts.back());

        for (u32 i = first; i < first + count; i++) {
            Edge e = l.edges[i];

            if (e.y0 < e.y1 || e.y1 < e.y0) {
                edges.mut(starts.mut(bucket(minf(e.y0, e.y1) - oy))++) = bandEdge(e, ox, oy);
            }
        }

        if (edges.length() < 1024) {
            quickSort(edges.mutBegin(), edges.mutEnd(), edgeBefore);
        }

        Vector<u32> active;
        // The active edges again, in the x order of the last band or
        // scanline, and the place of each in the active list.
        Vector<u32> order;
        Vector<u32> position;
        Vector<u32> rest;
        Vector<BandSample> samples[8];
        Vector<float> cuts;
        Vector<Crossing> crossings;
        Vector<Crossing> scratch;
        size_t next = 0;

        position.zero(edges.length());

        for (u32 row = 0; row < h; row++) {
            float top = (float)row;
            float bottom = (float)(row + 1);
            size_t kept = 0;

            for (size_t k = 0; k < active.length(); k++) {
                if (edges[active[k]].y1 > top) {
                    active.mutData()[kept++] = active[k];
                }
            }

            while (active.length() > kept) {
                active.popBack();
            }

            kept = 0;

            for (size_t k = 0; k < order.length(); k++) {
                if (edges[order[k]].y1 > top) {
                    order.mutData()[kept++] = order[k];
                }
            }

            while (order.length() > kept) {
                order.popBack();
            }

            while (next < edges.length() && edges[next].y0 < bottom) {
                if (edges[next].y1 > top) {
                    active.pushBack((u32)next);
                    order.pushBack((u32)next);
                }

                next++;
            }

            if (active.empty()) {
                continue;
            }

            // Resolve winding before integrating coverage: disjoint contours
            // with opposite directions can share a pixel and cancel signed area.
            // More active edges than half the limit take the scanlines below
            // whatever the cuts: a row has two at least.
            if (active.length() <= 2048) {
                cuts.clear();
                cuts.pushBack(top);
                cuts.pushBack(bottom);

                // An active edge starts above the bottom and ends below the top.
                for (u32 index : active) {
                    const BandEdge& e = edges[index];

                    if (e.y0 > top) {
                        cuts.pushBack(e.y0);
                    }

                    if (e.y1 < bottom) {
                        cuts.pushBack(e.y1);
                    }
                }

                quickSort(cuts.mutBegin(), cuts.mutEnd(), [](float p, float q) {
                    return p < q;
                });

                size_t cutCount = 0;

                for (float cut : cuts) {
                    if (cutCount == 0 || cut > cuts[cutCount - 1]) {
                        cuts.mutData()[cutCount++] = cut;
                    }
                }

                // Repeated boundaries of aligned dashes do not add integration bands.
                if (cutCount * active.length() <= 4096) {
                    for (size_t k = 0; k < active.length(); k++) {
                        position.mut(active[k]) = (u32)k;
                    }

                    for (size_t k = 0; k + 1 < cutCount; k++) {
                        bool comparable = bandSamples(edges.data(), order, position, cuts[k], cuts[k + 1], samples[0], rest);

                        unionBand(acc, stride, w, h, edges.data(), active, position, samples, cuts[k], cuts[k + 1], rule, 0, comparable);

                        // The samples in x order, then the rest: the band
                        // below comes nearly sorted.
                        order.clear();

                        for (const BandSample& s : samples[0]) {
                            order.pushBack(s.edge);
                        }

                        order.append(rest.data(), rest.length());
                    }

                    continue;
                }
            }

            float* line = acc + (size_t)row * stride;
            // A row of many edges takes fewer scanlines, 4 at least: their
            // crossings are capped near 16384 a row.
            int subs = 16;

            while (subs > 4 && active.length() * (size_t)subs > 16384) {
                subs /= 2;
            }

            for (int sub = 0; sub < subs; sub++) {
                int winding = 0;
                float start = 0.f;
                float y = top + (sub + 0.5f) / (float)subs;

                scanCrossings(edges.data(), order, y, (float)w, crossings, scratch, rest);

                for (const Crossing& c : crossings) {
                    bool before = insideWinding(winding, rule);

                    winding += c.dir;

                    bool after = insideWinding(winding, rule);

                    if (!before) {
                        start = c.x;
                    } else if (!after) {
                        addSpan(line, w, start, c.x, 1.f / (float)subs);
                    }
                }

                // The crossings in x order, then the rest: the scanline
                // below comes nearly sorted.
                order.clear();

                for (const Crossing& c : crossings) {
                    order.pushBack(c.edge);
                }

                order.append(rest.data(), rest.length());
            }
        }
    }

    static bool rasterize(Raster& r, const Lowering& l, u32 first, u32 count, Box bounds, bool aliased, u32& ix0, u32& iy0, u32& w, u32& h, u8 rule) {
        float bx0 = floorf(maxf(bounds.x0, 0.f));
        float by0 = floorf(maxf(bounds.y0, 0.f));
        float bx1 = ceilf(minf(bounds.x1, (float)r.width));
        float by1 = ceilf(minf(bounds.y1, (float)r.height));

        if (!(bx1 > bx0) || !(by1 > by0)) {
            return false;
        }

        ix0 = (u32)bx0;
        iy0 = (u32)by0;
        w = (u32)bx1 - ix0;
        h = (u32)by1 - iy0;

        u32 stride = w + 2;

        r.accumulation.zero((size_t)stride * h);

        float* acc = r.accumulation.mutData();

        if (!aliased) {
            rasterizeUnion(acc, stride, w, h, l, first, count, (float)ix0, (float)iy0, rule);

            return true;
        }

        for (u32 i = first; i < first + count; i++) {
            Edge e = l.edges[i];

            addAliasedEdge(acc, stride, w, h, Edge{e.x0 - ix0, e.y0 - iy0, e.x1 - ix0, e.y1 - iy0});
        }

        return true;
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base);

    // A pattern paint fills one op, so its tile is drawn once.
    static void prepareTile(const Lowering& l, DevicePaint& paint) {
        size_t size = (size_t)paint.imageWidth * paint.imageHeight;
        Raster tile;

        paint.tile = (float*)l.pool->allocate(sizeof(float) * size * 4);
        memset(paint.tile, 0, sizeof(float) * size * 4);
        tile.width = paint.imageWidth;
        tile.height = paint.imageHeight;
        runProgram(tile, l, l.programs.data() + paint.programFirst, paint.programCount, Plane{paint.tile, IntBox{0, 0, (int)paint.imageWidth, (int)paint.imageHeight}});
    }

    static void clearSpan(const Plane& plane, int y, int x0, int x1) {
        if (x1 > x0) {
            memset(planeAt(plane, x0, y), 0, sizeof(float) * 4 * (size_t)(x1 - x0));
        }
    }

    // Zeroes what the box adds to the valid part of the plane; what lies
    // outside the plane is left out.
    // An empty box is all zero, as boxInside makes it.
    static void ensure(const Plane& plane, IntBox& valid, IntBox add) {
        add = boxInside(add, plane.box);

        if (add.x0 >= add.x1) {
            return;
        }

        if (valid.x0 >= valid.x1) {
            for (int y = add.y0; y < add.y1; y++) {
                clearSpan(plane, y, add.x0, add.x1);
            }

            valid = add;

            return;
        }

        IntBox grown{add.x0 < valid.x0 ? add.x0 : valid.x0, add.y0 < valid.y0 ? add.y0 : valid.y0, add.x1 > valid.x1 ? add.x1 : valid.x1, add.y1 > valid.y1 ? add.y1 : valid.y1};

        for (int y = grown.y0; y < grown.y1; y++) {
            if (y < valid.y0 || y >= valid.y1) {
                clearSpan(plane, y, grown.x0, grown.x1);
            } else {
                clearSpan(plane, y, grown.x0, valid.x0);
                clearSpan(plane, y, valid.x1, grown.x1);
            }
        }

        valid = grown;
    }

    static size_t firstTarget(const Raster& r) {
        size_t first = r.layers.length() - 1;

        // Only the base of a program has no Begin.
        while (first > r.backgroundBase && r.begins[first]->backgroundOnly) {
            first--;
        }

        return first;
    }

    static void fillOp(Raster& r, const Lowering& l, const Op& op) {
        if (op.paint->type == PaintType::Pattern) {
            prepareTile(l, *op.paint);
        }

        u32 ix0;
        u32 iy0;
        u32 w;
        u32 h;

        if (!rasterize(r, l, op.first, op.count, op.bounds, op.aliased, ix0, iy0, w, h, op.rule)) {
            return;
        }

        const float* acc = r.accumulation.data();
        u32 stride = w + 2;
        const DevicePaint& paint = *op.paint;

        size_t first = firstTarget(r);
        IntBox fill{(int)ix0, (int)iy0, (int)(ix0 + w), (int)(iy0 + h)};

        // The layer of a filtered group holds its filters' region, which the
        // fill may pass; the background-only layers above it hold their groups'
        // bounds, the fill's among them.
        const IntBox& box = r.layers[first].box;
        bool inside = fill.x0 >= box.x0 && fill.y0 >= box.y0 && fill.x1 <= box.x1 && fill.y1 <= box.y1;

        for (size_t level = first; level < r.layers.length(); level++) {
            ensure(r.layers[level], r.dirty.mut(level), fill);
        }

        for (u32 y = 0; y < h; y++) {
            float sum = 0.f;
            const float* line = acc + (size_t)y * stride;

            for (u32 x = 0; x < w; x++) {
                sum += line[x];

                float cov = coverage(sum, op.rule);

                if (cov <= 0.f) {
                    continue;
                }

                Color src;

                if (paint.type == PaintType::Solid) {
                    src = paint.color;
                } else if (!paintAt(paint, (float)(ix0 + x) + 0.5f, (float)(iy0 + y) + 0.5f, src)) {
                    continue;
                }

                float a = src.a * cov;
                float keep = 1.f - a;

                for (size_t level = first; level < r.layers.length(); level++) {
                    const Plane& plane = r.layers[level];
                    int dx = (int)(ix0 + x);
                    int dy = (int)(iy0 + y);

                    if (!inside && (dx < plane.box.x0 || dx >= plane.box.x1 || dy < plane.box.y0 || dy >= plane.box.y1)) {
                        continue;
                    }

                    float* px = planeAt(plane, dx, dy);

                    px[0] = src.r * cov + px[0] * keep;
                    px[1] = src.g * cov + px[1] * keep;
                    px[2] = src.b * cov + px[2] * keep;
                    px[3] = a + px[3] * keep;
                }
            }
        }
    }

    // Adds the coverage of the shape to a map of one float a pixel over the box.
    static void accumulateShape(Raster& r, const Lowering& l, const ClipShape& shape, float* target, const IntBox& box) {
        Box bounds = emptyBox();

        for (u32 i = shape.first; i < shape.first + shape.count; i++) {
            extend(bounds, Point{l.edges[i].x0, l.edges[i].y0});
            extend(bounds, Point{l.edges[i].x1, l.edges[i].y1});
        }

        u32 ix0;
        u32 iy0;
        u32 w;
        u32 h;

        if (!boxValid(bounds) || !rasterize(r, l, shape.first, shape.count, bounds, false, ix0, iy0, w, h, shape.rule)) {
            return;
        }

        const float* acc = r.accumulation.data();
        u32 stride = w + 2;
        int width = box.x1 - box.x0;

        for (u32 y = 0; y < h; y++) {
            float sum = 0.f;
            const float* line = acc + (size_t)y * stride;
            int dy = (int)(iy0 + y);

            for (u32 x = 0; x < w; x++) {
                sum += line[x];

                int dx = (int)(ix0 + x);

                if (dx < box.x0 || dx >= box.x1 || dy < box.y0 || dy >= box.y1) {
                    continue;
                }

                float& value = target[(size_t)(dy - box.y0) * width + (size_t)(dx - box.x0)];

                value = maxf(value, coverage(sum, shape.rule));
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base);

    // The clip of a group, one float a pixel over the box of its layer.
    static float* clipBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool, const IntBox& box) {
        size_t size = (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0);
        float* mask = (float*)pool.allocate(sizeof(float) * size);

        memset(mask, 0, sizeof(float) * size);

        if (op.clipProgram) {
            Plane content{(float*)pool.allocate(sizeof(float) * size * 4), box};

            memset(content.data, 0, sizeof(float) * size * 4);
            runProgram(r, l, l.programs.data() + op.clipFirst, op.clipCount, content);

            for (size_t i = 0; i < size; i++) {
                mask[i] = content.data[i * 4 + 3];
            }

            return mask;
        }

        for (u32 s = op.clipFirst; s < op.clipFirst + op.clipCount; s++) {
            accumulateShape(r, l, l.clips[s], mask, box);
        }

        return mask;
    }

    static float* maskBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool, const IntBox& box) {
        size_t size = (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0);
        Plane content{(float*)pool.allocate(sizeof(float) * size * 4), box};
        float* mask = (float*)pool.allocate(sizeof(float) * size);

        memset(content.data, 0, sizeof(float) * size * 4);
        memset(mask, 0, sizeof(float) * size);
        runProgram(r, l, l.programs.data() + op.maskFirst, op.maskCount, content);
        accumulateShape(r, l, l.clips[op.maskRegion], mask, box);

        for (size_t i = 0; i < size; i++) {
            const float* c = content.data + i * 4;
            float value = (op.maskType & 1) ? c[3] : 0.2126f * c[0] + 0.7152f * c[1] + 0.0722f * c[2];

            if ((op.maskType & 3) == 2 && c[3] > 0.f) {
                value = c[3] * (0.2126f * srgbToLinear(clampUnit(c[0] / c[3])) + 0.7152f * srgbToLinear(clampUnit(c[1] / c[3])) + 0.0722f * srgbToLinear(clampUnit(c[2] / c[3])));
            }

            mask[i] *= value;
        }

        return mask;
    }

    struct FilterResult {
        Plane plane;
        bool linear;
    };

    struct FilterRun {
        Raster* raster;
        const Lowering* lowering;
        const Filter* filter;
        ObjPool* pool;
        IntBox canvas;
        Plane source;
        Plane sourceLinear;
        Plane sourceAlpha;
        Plane background;
        Plane backgroundLinear;
        Plane backgroundAlpha;
        bool resetBackground;
        Plane paint[2][2];
        Plane empty;
        Vector<FilterResult> results;
        Vector<Plane> linearCache;
        Vector<Plane> srgbCache;
        // The readers each result and input still has among the primitives
        // the final one reaches.
        Vector<u32> readers;
        u32 sourceReaders;
        u32 sourceAlphaReaders;
        u32 backgroundReaders;
        u32 backgroundAlphaReaders;
        float scaleX;
        float scaleY;
    };

    static float srgbToLinear(float c) {
        return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f);
    }

    static float linearToSrgb(float c) {
        return c <= 0.0031308f ? c * 12.92f : 1.055f * powf(c, 1.f / 2.4f) - 0.055f;
    }

    static Color toSpace(Color c, bool linear) {
        if (!linear) {
            return c;
        }

        return Color{srgbToLinear(c.r), srgbToLinear(c.g), srgbToLinear(c.b), c.a};
    }

    // Converts a premultiplied pixel between sRGB and linear light; the
    // destination may be the pixel itself.
    // The value each channel converted last and what it came to: a flat
    // area converts its colour once. A value is the same by its bits, so a
    // zero of either sign converts on its own.
    struct Conversion {
        float in[3] = {-1.f, -1.f, -1.f};
        float out[3] = {};
    };

    static bool sameBits(float a, float b) {
        u32 x;
        u32 y;

        memcpy(&x, &a, sizeof(x));
        memcpy(&y, &b, sizeof(y));

        return x == y;
    }

    static void convertPixel(const float* s, float* d, bool toLinear, Conversion& last) {
        float a = s[3];

        if (a <= 0.f) {
            d[0] = 0.f;
            d[1] = 0.f;
            d[2] = 0.f;
            d[3] = 0.f;

            return;
        }

        for (int c = 0; c < 3; c++) {
            float v = clampUnit(s[c] / a);

            if (!sameBits(v, last.in[c])) {
                last.in[c] = v;
                last.out[c] = toLinear ? srgbToLinear(v) : linearToSrgb(v);
            }

            d[c] = last.out[c] * a;
        }

        d[3] = a;
    }

    static void convertInPlace(const Plane& plane, bool toLinear) {
        Conversion last;

        for (int y = plane.box.y0; y < plane.box.y1; y++) {
            for (int x = plane.box.x0; x < plane.box.x1; x++) {
                convertPixel(planeAt(plane, x, y), planeAt(plane, x, y), toLinear, last);
            }
        }
    }

    static Plane convertBuffer(const Plane& src, bool toLinear) {
        Plane out = makePlane(src.box);
        Conversion last;

        for (int y = src.box.y0; y < src.box.y1; y++) {
            for (int x = src.box.x0; x < src.box.x1; x++) {
                convertPixel(planeAt(src, x, y), planeAt(out, x, y), toLinear, last);
            }
        }

        return out;
    }

    // The background under the box: the layers of the current scope over the
    // backdrop. A plane without data when there is none.
    static Plane backgroundBuffer(Raster& r, const IntBox& box) {
        size_t first = r.backgroundBase;
        const Op* scope = nullptr;

        // The layers past the base have their Begins.
        for (size_t i = r.begins.length(); i > first + 1; i--) {
            const Op* op = r.begins[i - 1];

            if (op->resetBackground && !op->newBackground) {
                return Plane{nullptr, box};
            }

            if (op->newBackground) {
                first = i - 1;
                scope = op;
                break;
            }
        }

        if (!scope && !r.backdrop.data) {
            return Plane{nullptr, box};
        }

        Plane out = makePlane(box);

        if (!scope) {
            for (int y = box.y0; y < box.y1; y++) {
                for (int x = box.x0; x < box.x1; x++) {
                    memcpy(planeAt(out, x, y), planeRead(r.backdrop, x, y), sizeof(float) * 4);
                }
            }
        }

        // Ancestor opacity, masks and filters apply when that ancestor is
        // composited, not while its children form the current background.
        for (size_t level = first; level < r.layers.length(); level++) {
            IntBox drawn = boxInside(r.dirty[level], box);
            const Plane& layer = r.layers[level];

            for (int y = drawn.y0; y < drawn.y1; y++) {
                for (int x = drawn.x0; x < drawn.x1; x++) {
                    const float* s = planeAt(layer, x, y);
                    float* d = planeAt(out, x, y);
                    float keep = 1.f - s[3];

                    for (int c = 0; c < 4; c++) {
                        d[c] = s[c] + d[c] * keep;
                    }
                }
            }
        }

        if (scope && scope->backgroundClipped) {
            const Box& clip = scope->backgroundBox;

            for (int y = box.y0; y < box.y1; y++) {
                for (int x = box.x0; x < box.x1; x++) {
                    Point point = apply(scope->backgroundMatrix, Point{(float)x + .5f, (float)y + .5f});

                    if (point.x < clip.x0 || point.x >= clip.x1 || point.y < clip.y0 || point.y >= clip.y1) {
                        memset(planeAt(out, x, y), 0, sizeof(float) * 4);
                    }
                }
            }
        }

        return out;
    }

    static bool readsResult(const FilterPrimitive& p, int index);

    // The index of the last primitive reading the input, or -1.
    static bool readsInSrgb(const Filter& filter, int index) {
        for (u32 k = 0; k < filter.count; k++) {
            const FilterPrimitive& p = filter.primitives[k];

            if (!p.linear && readsResult(p, index)) {
                return true;
            }
        }

        return false;
    }

    // Whether the plane the primitive being computed reads as the input has
    // no other reader left, so that it may be written over.
    static bool consumesInput(const FilterRun& run, int index, const Plane& in) {
        if (!in.data) {
            return false;
        }

        // The source stays while its alpha is still to be taken from it.
        if (index == InputSourceGraphic) {
            return run.sourceReaders == 1 && (in.data != run.source.data || run.sourceAlphaReaders == 0 || run.sourceAlpha.data);
        }

        if (index == InputSourceAlpha) {
            return run.sourceAlphaReaders == 1;
        }

        // The background stays while its alpha is still to be read.
        if (index == InputBackgroundImage) {
            return run.backgroundReaders == 1 && run.backgroundAlphaReaders == 0;
        }

        if (index == InputBackgroundAlpha) {
            return run.backgroundAlphaReaders == 1;
        }

        return index >= 0 && run.readers[index] == 1;
    }

    static Plane filterInputBuffer(FilterRun& run, int index, bool linear) {
        if (index == InputFillPaint || index == InputStrokePaint) {
            int kind = index == InputStrokePaint ? 1 : 0;
            Plane& buffer = run.paint[kind][0];

            // A paint has no extent: its plane is the raster.
            if (!buffer.data) {
                buffer = makePlane(run.canvas);
                DevicePaint* paint = kind ? run.filter->strokePaint : run.filter->fillPaint;

                if (paint) {
                    if (paint->type == PaintType::Pattern) {
                        prepareTile(*run.lowering, *paint);
                    }

                    for (int y = run.canvas.y0; y < run.canvas.y1; y++) {
                        for (int x = run.canvas.x0; x < run.canvas.x1; x++) {
                            Color color;

                            if (paintAt(*paint, (float)x + .5f, (float)y + .5f, color)) {
                                float* d = planeAt(buffer, x, y);

                                d[0] = color.r;
                                d[1] = color.g;
                                d[2] = color.b;
                                d[3] = color.a;
                            }
                        }
                    }
                }
            }

            if (linear && !run.paint[kind][1].data) {
                run.paint[kind][1] = convertBuffer(buffer, true);
            }

            return run.paint[kind][linear ? 1 : 0];
        }

        if (index == InputBackgroundImage || index == InputBackgroundAlpha) {
            if (!run.background.data) {
                const IntBox& box = run.filter->region;
                Plane background = run.resetBackground ? Plane{nullptr, box} : backgroundBuffer(*run.raster, box);

                run.background = background.data ? background : makePlane(box);

                // A background read in linear light alone is converted in place.
                if (!readsInSrgb(*run.filter, InputBackgroundImage)) {
                    convertInPlace(run.background, true);
                    run.backgroundLinear = run.background;
                }
            }

            if (index == InputBackgroundAlpha) {
                if (!run.backgroundAlpha.data) {
                    const IntBox& box = run.background.box;

                    run.backgroundAlpha = makePlane(box);

                    for (int y = box.y0; y < box.y1; y++) {
                        for (int x = box.x0; x < box.x1; x++) {
                            planeAt(run.backgroundAlpha, x, y)[3] = planeAt(run.background, x, y)[3];
                        }
                    }
                }

                return run.backgroundAlpha;
            }

            if (linear && !run.backgroundLinear.data) {
                run.backgroundLinear = convertBuffer(run.background, true);
            }

            return linear ? run.backgroundLinear : run.background;
        }

        if (index == InputSourceAlpha) {
            if (!run.sourceAlpha.data) {
                const IntBox& box = run.filter->region;

                run.sourceAlpha = makePlane(box);

                for (int y = box.y0; y < box.y1; y++) {
                    for (int x = box.x0; x < box.x1; x++) {
                        planeAt(run.sourceAlpha, x, y)[3] = planeAt(run.source, x, y)[3];
                    }
                }
            }

            return run.sourceAlpha;
        }

        if (index == InputSourceGraphic) {
            if (!linear) {
                return run.source;
            }

            if (!run.sourceLinear.data) {
                run.sourceLinear = convertBuffer(run.source, true);
            }

            return run.sourceLinear;
        }

        const FilterResult& result = run.results[index];

        if (result.linear == linear) {
            return result.plane;
        }

        Vector<Plane>& cache = linear ? run.linearCache : run.srgbCache;

        if (!cache[index].data) {
            cache.mut(index) = convertBuffer(result.plane, linear);
        }

        return cache[index];
    }

    static IntBox inputBox(const FilterRun& run, int index) {
        if (index >= 0) {
            return run.filter->primitives[index].box;
        }

        return run.filter->region;
    }

    static float numberOf(const Document& doc, const Node* node, u32 name, float fallback) {
        float value;
        u32 atom = attribute(node, name);

        return atom && parseNumberText(textOf(doc, atom), value) ? value : fallback;
    }

    static int integerOf(const Document& doc, const Node* node, u32 name, int fallback) {
        u32 atom = attribute(node, name);
        StringView text = atom ? trim(textOf(doc, atom)) : StringView();
        const u8* p = text.begin();
        const u8* end = text.end();
        bool negative = at(p, end, '-');

        if (p < end && (*p == '+' || *p == '-')) {
            p++;
        }

        if (p == end) {
            return fallback;
        }

        u64 value = 0;
        u64 limit = negative ? 2147483648ull : 2147483647ull;

        for (; p < end; p++) {
            if (*p < '0' || *p > '9' || value > (limit - (*p - '0')) / 10) {
                return fallback;
            }

            value = value * 10 + (*p - '0');
        }

        return (int)(negative ? -(i64)value : (i64)value);
    }

    static bool numberList(const Document& doc, const Node* node, u32 name, Vector<float>& out) {
        u32 atom = attribute(node, name);

        if (!atom) {
            return false;
        }

        StringView text = textOf(doc, atom);
        const u8* p = text.begin();

        for (;;) {
            float value;

            skipSpaces(p, text.end());

            if (p >= text.end()) {
                return true;
            }

            if (!readNumber(p, text.end(), value)) {
                out.clear();

                return false;
            }

            out.pushBack(value);
            skipSeparator(p, text.end());
        }
    }

    static bool numberPair(const Document& doc, const Node* node, u32 name, float& a, float& b) {
        Vector<float> values;

        if (!numberList(doc, node, name, values) || values.length() == 0 || values.length() > 2) {
            return false;
        }

        a = values[0];
        b = values.length() == 2 ? values[1] : values[0];

        return true;
    }

    static float userX(const Filter& f, float value) {
        return f.boxPrimitives ? value * (f.bbox.x1 - f.bbox.x0) : value;
    }

    static float userY(const Filter& f, float value) {
        return f.boxPrimitives ? value * (f.bbox.y1 - f.bbox.y0) : value;
    }

    static void filterFlood(const FilterPrimitive& p, const Plane& out) {
        Color c = toSpace(p.flood, p.linear);
        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                float* d = planeAt(out, x, y);

                d[0] = c.r * c.a;
                d[1] = c.g * c.a;
                d[2] = c.b * c.a;
                d[3] = c.a;
            }
        }
    }

    // Copies the box, which the output plane holds, from one plane to another.
    static void copyBox(const Plane& in, const Plane& out, const IntBox& b) {
        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                memcpy(planeAt(out, x, y), planeRead(in, x, y), sizeof(float) * 4);
            }
        }
    }

    // A shift by a fraction of a pixel: the whole part and the bilinear
    // weights of the four pixels around.
    struct Shift {
        int ox;
        int oy;
        float weights[4];
    };

    static Shift shiftBy(float dx, float dy) {
        float floorX = floorf(dx);
        float floorY = floorf(dy);
        float fx = dx - floorX;
        float fy = dy - floorY;

        return Shift{(int)floorX, (int)floorY, {(1.f - fx) * (1.f - fy), fx * (1.f - fy), (1.f - fx) * fy, fx * fy}};
    }

    static void shiftPixel(const Plane& in, int x, int y, const Shift& shift, float* d) {
        float sum[4] = {0.f, 0.f, 0.f, 0.f};

        for (int k = 0; k < 4; k++) {
            if (shift.weights[k] == 0.f) {
                continue;
            }

            const float* src = planeRead(in, x - shift.ox - (k & 1), y - shift.oy - (k >> 1));

            for (int c = 0; c < 4; c++) {
                sum[c] += src[c] * shift.weights[k];
            }
        }

        d[0] = sum[0];
        d[1] = sum[1];
        d[2] = sum[2];
        d[3] = sum[3];
    }

    static void shiftBuffer(const Plane& in, const Plane& out, const IntBox& b, float dx, float dy) {
        Shift shift = shiftBy(dx, dy);

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                shiftPixel(in, x, y, shift, planeAt(out, x, y));
            }
        }
    }

    static void filterOffset(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        float dx = userX(f, numberOf(doc, p.node, AtomDx, 0.f));
        float dy = userY(f, numberOf(doc, p.node, AtomDy, 0.f));
        Plane in = filterInputBuffer(run, p.in1, p.linear);

        shiftBuffer(in, out, p.box, f.ctm.a * dx + f.ctm.c * dy, f.ctm.b * dx + f.ctm.d * dy);
    }

    static void boxBlurLine(const float* src, float* dst, int count, int stride, int left, int right) {
        int window = left + right + 1;
        float sum[4] = {0.f, 0.f, 0.f, 0.f};
        float scale = 1.f / (float)window;

        // The padding of a line passes the window, so i stays below count.
        for (int i = -left; i <= right; i++) {
            if (i >= 0) {
                for (int c = 0; c < 4; c++) {
                    sum[c] += src[(size_t)i * stride + c];
                }
            }
        }

        for (int i = 0; i < count; i++) {
            for (int c = 0; c < 4; c++) {
                dst[(size_t)i * stride + c] = sum[c] * scale;
            }

            int out = i - left;
            int in = i + right + 1;

            // out stays below count, and in above zero.
            if (out >= 0) {
                for (int c = 0; c < 4; c++) {
                    sum[c] -= src[(size_t)out * stride + c];
                }
            }

            if (in < count) {
                for (int c = 0; c < 4; c++) {
                    sum[c] += src[(size_t)in * stride + c];
                }
            }
        }
    }

    static void gaussLine(const float* src, float* dst, int count, int stride, const float* kernel, int radius) {
        for (int i = 0; i < count; i++) {
            float sum[4] = {0.f, 0.f, 0.f, 0.f};
            // The taps within the line.
            int k0 = i < radius ? -i : -radius;
            int k1 = i + radius < count ? radius : count - 1 - i;

            for (int k = k0; k <= k1; k++) {
                const float* tap = src + (size_t)(i + k) * stride;
                float w = kernel[k + radius];

                for (int c = 0; c < 4; c++) {
                    sum[c] += tap[c] * w;
                }
            }

            for (int c = 0; c < 4; c++) {
                dst[(size_t)i * stride + c] = sum[c];
            }
        }
    }

    static int blurReach(float sigma) {
        if (!(sigma > 0.f)) {
            return 0;
        }

        if (sigma < 2.f) {
            return (int)ceilf(sigma * 3.f);
        }

        int d = (int)minf(floorf(sigma * 3.f * sqrtf(2.f * pi) / 4.f + 0.5f), (float)(1 << 26));

        return 3 * (d / 2 + 1);
    }

    static void blurLine(const float* src, float* dst, Vector<float>& first, Vector<float>& second, int count, int stride, float sigma) {
        if (!(sigma > 0.f)) {
            for (int i = 0; i < count; i++) {
                for (int c = 0; c < 4; c++) {
                    dst[(size_t)i * stride + c] = src[(size_t)i * stride + c];
                }
            }

            return;
        }

        if (sigma < 2.f) {
            // A sigma below 2 reaches 6 pixels at most.
            int radius = (int)ceilf(sigma * 3.f);
            float kernel[64];
            float total = 0.f;

            for (int k = -radius; k <= radius; k++) {
                kernel[k + radius] = expf(-(float)(k * k) / (2.f * sigma * sigma));
                total += kernel[k + radius];
            }

            for (int k = 0; k <= 2 * radius; k++) {
                kernel[k] /= total;
            }

            gaussLine(src, dst, count, stride, kernel, radius);

            return;
        }

        double window = floorf(sigma * 3.f * sqrtf(2.f * pi) / 4.f + 0.5f);

        if (window >= 2.0 * count) {
            // The central part of three convolved boxes is a quadratic. Integrate
            // it from the input moments instead of allocating padding proportional to sigma.
            double moments[3][4] = {};
            bool even = fmod(window, 2.0) == 0.0;
            double center = (3.0 * window * window + (even ? 2.0 * window : 1.0)) / 4.0;
            double divisor = window * window * (even ? window + 1.0 : window);

            for (int i = 0; i < count; i++) {
                for (int c = 0; c < 4; c++) {
                    double value = src[(size_t)i * stride + c];

                    moments[0][c] += value;
                    moments[1][c] += i * value;
                    moments[2][c] += (double)i * i * value;
                }
            }

            for (int i = 0; i < count; i++) {
                for (int c = 0; c < 4; c++) {
                    double value = ((center - (double)i * i) * moments[0][c] + 2.0 * i * moments[1][c] - moments[2][c]) / divisor;

                    dst[(size_t)i * stride + c] = value > 0.0 ? (float)value : 0.f;
                }
            }

            return;
        }

        int d = (int)window;
        int pad = 3 * (d / 2 + 1);
        int total = count + 2 * pad;

        if (first.length() < (size_t)total * 4) {
            first.zero((size_t)total * 4);
            second.zero((size_t)total * 4);
        }

        memset(first.mutData(), 0, sizeof(float) * 4 * (size_t)pad);
        memset(first.mutData() + (size_t)(pad + count) * 4, 0, sizeof(float) * 4 * (size_t)pad);

        for (int i = 0; i < count; i++) {
            for (int c = 0; c < 4; c++) {
                first.mutData()[(size_t)(i + pad) * 4 + c] = src[(size_t)i * stride + c];
            }
        }

        if (d % 2 == 1) {
            int r = (d - 1) / 2;

            boxBlurLine(first.data(), second.mutData(), total, 4, r, r);
            boxBlurLine(second.data(), first.mutData(), total, 4, r, r);
            boxBlurLine(first.data(), second.mutData(), total, 4, r, r);
        } else {
            boxBlurLine(first.data(), second.mutData(), total, 4, d / 2, d / 2 - 1);
            boxBlurLine(second.data(), first.mutData(), total, 4, d / 2 - 1, d / 2);
            boxBlurLine(first.data(), second.mutData(), total, 4, d / 2, d / 2);
        }

        for (int i = 0; i < count; i++) {
            for (int c = 0; c < 4; c++) {
                dst[(size_t)i * stride + c] = second.data()[(size_t)(i + pad) * 4 + c];
            }
        }
    }

    static void blurBuffer(FilterRun& run, const Plane& in, const Plane& out, const IntBox& box, float sigmaX, float sigmaY, bool consume) {
        int height = (int)run.raster->height;
        int width = (int)run.raster->width;
        int reachX = blurReach(sigmaX);
        int reachY = blurReach(sigmaY);
        int x0 = box.x0 - reachX > 0 ? box.x0 - reachX : 0;
        int x1 = box.x1 + reachX < width ? box.x1 + reachX : width;
        int y0 = box.y0 - reachY > 0 ? box.y0 - reachY : 0;
        int y1 = box.y1 + reachY < height ? box.y1 + reachY : height;
        Vector<float> row;
        Vector<float> line;
        Vector<float> lineOut;
        Vector<float> first;
        Vector<float> second;

        // An empty box is empty by its x, as boxInside leaves it.
        if (box.x1 <= box.x0) {
            return;
        }

        // The horizontal pass goes over the input itself when the input is
        // dead after the blur: a row is read whole before it is written.
        bool inPlace = consume && x0 >= in.box.x0 && y0 >= in.box.y0 && x1 <= in.box.x1 && y1 <= in.box.y1;
        Plane horizontal = inPlace ? in : makePlane(IntBox{x0, y0, x1, y1});

        row.zero((size_t)(x1 - x0) * 4);
        line.zero((size_t)(y1 - y0) * 4);
        lineOut.zero((size_t)(y1 - y0) * 4);

        for (int y = y0; y < y1; y++) {
            readSpan(in, x0, x1, y, row.mutData());
            blurLine(row.data(), planeAt(horizontal, x0, y), first, second, x1 - x0, 4, sigmaX);
        }

        for (int x = box.x0; x < box.x1; x++) {
            for (int y = y0; y < y1; y++) {
                memcpy(line.mutData() + (size_t)(y - y0) * 4, planeAt(horizontal, x, y), sizeof(float) * 4);
            }

            blurLine(line.data(), lineOut.mutData(), first, second, y1 - y0, 4, sigmaY);

            for (int y = box.y0; y < box.y1; y++) {
                memcpy(planeAt(out, x, y), lineOut.data() + (size_t)(y - y0) * 4, sizeof(float) * 4);
            }
        }

        if (!inPlace) {
            releasePlane(horizontal);
        }
    }

    static void filterBlur(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        float sx = 0.f;
        float sy = 0.f;
        Plane in = filterInputBuffer(run, p.in1, p.linear);

        bool valid = true;

        if (!p.node) {
            sx = p.params[0];
            sy = p.params[1];
        } else if (attribute(p.node, AtomStdDeviation)) {
            valid = numberPair(*f.doc, p.node, AtomStdDeviation, sx, sy);
        }

        if (!valid) {
            copyBox(in, out, p.box);

            return;
        }

        // A negative deviation is zero: no blur along that axis.
        sx = userX(f, maxf(sx, 0.f)) * run.scaleX;
        sy = userY(f, maxf(sy, 0.f)) * run.scaleY;

        if (sx <= 0.f && sy <= 0.f) {
            copyBox(in, out, p.box);

            return;
        }

        if (p.in1 == InputFillPaint || p.in1 == InputStrokePaint) {
            const DevicePaint* paint = p.in1 == InputFillPaint ? f.fillPaint : f.strokePaint;
            // These inputs have infinite extent; blurring a constant paint
            // cannot introduce transparent edges at the filter region.
            if (!paint || paint->type == PaintType::Solid) {
                copyBox(in, out, p.box);
                return;
            }
        }

        blurBuffer(run, in, out, p.box, sx, sy, consumesInput(run, p.in1, in));
    }

    static void filterDropShadow(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        float sx = 2.f;
        float sy = 2.f;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        float dx = p.node ? userX(f, numberOf(*f.doc, p.node, AtomDx, 2.f)) : p.params[0];
        float dy = p.node ? userY(f, numberOf(*f.doc, p.node, AtomDy, 2.f)) : p.params[1];

        if (!p.node) {
            sx = p.params[2];
            sy = p.params[2];
        } else if (attribute(p.node, AtomStdDeviation) && !numberPair(*f.doc, p.node, AtomStdDeviation, sx, sy)) {
            return;
        }

        if (sx < 0.f || sy < 0.f) {
            sx = 0.f;
            sy = 0.f;
        }

        Color c = toSpace(p.flood, p.linear);
        IntBox all = run.filter->region;
        Plane shadow = makePlane(all);

        if (userX(f, sx) * run.scaleX > 0.f || userY(f, sy) * run.scaleY > 0.f) {
            blurBuffer(run, in, shadow, all, userX(f, sx) * run.scaleX, userY(f, sy) * run.scaleY, false);
        } else {
            copyBox(in, shadow, all);
        }

        Shift shift = shiftBy(f.ctm.a * dx + f.ctm.c * dy, f.ctm.b * dx + f.ctm.d * dy);
        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                float moved[4];

                shiftPixel(shadow, x, y, shift, moved);

                float a = moved[3] * c.a;
                const float* s = planeRead(in, x, y);
                float* o = planeAt(out, x, y);
                float keep = 1.f - s[3];

                o[0] = s[0] + c.r * a * keep;
                o[1] = s[1] + c.g * a * keep;
                o[2] = s[2] + c.b * a * keep;
                o[3] = s[3] + a * keep;
            }
        }

        releasePlane(shadow);
    }

    static void unpremultiply(const float* s, float* c) {
        float a = s[3];

        if (a < 0.5f / 255.f) {
            c[0] = 0.f;
            c[1] = 0.f;
            c[2] = 0.f;
            c[3] = 0.f;

            return;
        }

        c[0] = clampUnit(s[0] / a);
        c[1] = clampUnit(s[1] / a);
        c[2] = clampUnit(s[2] / a);
        c[3] = a;
    }

    static void storePremultiplied(float* d, const float* c) {
        float a = clampUnit(c[3]);

        d[0] = clampUnit(c[0]) * a;
        d[1] = clampUnit(c[1]) * a;
        d[2] = clampUnit(c[2]) * a;
        d[3] = a;
    }

    static void filterColorMatrix(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        float m[20] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};
        u32 type = p.node ? attribute(p.node, AtomType) : 0;
        StringView kind = type ? trim(textOf(doc, type)) : SV("matrix");
        Vector<float> values;
        bool hasValues = p.node && numberList(doc, p.node, AtomValues, values);

        if (kind != SV("saturate") && kind != SV("hueRotate") && kind != SV("luminanceToAlpha")) {
            kind = SV("matrix");
        }

        if (!p.node) {
            memcpy(m, p.params, sizeof(m));
        } else if (kind == SV("saturate")) {
            float s = hasValues && values.length() == 1 ? values[0] : 1.f;
            float t[20] = {0.213f + 0.787f * s, 0.715f - 0.715f * s, 0.072f - 0.072f * s, 0, 0, 0.213f - 0.213f * s, 0.715f + 0.285f * s, 0.072f - 0.072f * s, 0, 0, 0.213f - 0.213f * s, 0.715f - 0.715f * s, 0.072f + 0.928f * s, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(m));
        } else if (kind == SV("hueRotate")) {
            float angle = (hasValues && values.length() == 1 ? values[0] : 0.f) * pi / 180.f;
            float cs = cosf(angle);
            float sn = sinf(angle);
            float t[20] = {
                0.213f + cs * 0.787f - sn * 0.213f,
                0.715f - cs * 0.715f - sn * 0.715f,
                0.072f - cs * 0.072f + sn * 0.928f,
                0,
                0,
                0.213f - cs * 0.213f + sn * 0.143f,
                0.715f + cs * 0.285f + sn * 0.140f,
                0.072f - cs * 0.072f - sn * 0.283f,
                0,
                0,
                0.213f - cs * 0.213f - sn * 0.787f,
                0.715f - cs * 0.715f + sn * 0.715f,
                0.072f + cs * 0.928f + sn * 0.072f,
                0,
                0,
                0,
                0,
                0,
                1,
                0,
            };

            memcpy(m, t, sizeof(m));
        } else if (kind == SV("luminanceToAlpha")) {
            float t[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2125f, 0.7154f, 0.0721f, 0, 0};

            memcpy(m, t, sizeof(m));
        } else if (hasValues && values.length() == 20) {
            for (int i = 0; i < 20; i++) {
                m[i] = values[i];
            }
        }

        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                float c[4];
                float r[4];

                unpremultiply(planeRead(in, x, y), c);

                for (int k = 0; k < 4; k++) {
                    r[k] = m[k * 5] * c[0] + m[k * 5 + 1] * c[1] + m[k * 5 + 2] * c[2] + m[k * 5 + 3] * c[3] + m[k * 5 + 4];
                }

                storePremultiplied(planeAt(out, x, y), r);
            }
        }
    }

    struct TransferFunction {
        int type;
        Vector<float> table;
        float slope;
        float intercept;
        float amplitude;
        float exponent;
        float offset;
    };

    static float transfer(const TransferFunction& f, float c) {
        size_t n = f.table.length();

        switch (f.type) {
            case 1:
                if (n == 0) {
                    return c;
                }

                if (n == 1) {
                    return f.table[0];
                }

                {
                    size_t k = c >= 1.f ? n - 2 : (size_t)(c * (float)(n - 1));
                    float v0 = f.table[k];
                    float v1 = f.table[k + 1];

                    return v0 + (c * (float)(n - 1) - (float)k) * (v1 - v0);
                }
            case 2:
                if (n == 0) {
                    return c;
                }

                {
                    size_t k = c >= 1.f ? n - 1 : (size_t)(c * (float)n);

                    return f.table[k];
                }
            case 3:
                return f.slope * c + f.intercept;
            case 4:
                return f.amplitude * powf(c, f.exponent) + f.offset;
            default:
                return c;
        }
    }

    static void filterComponentTransfer(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        TransferFunction functions[4];
        u32 names[4] = {AtomFeFuncR, AtomFeFuncG, AtomFeFuncB, AtomFeFuncA};

        for (int k = 0; k < 4; k++) {
            functions[k].type = 0;
        }

        for (Node* child = p.node->first; child; child = child->next) {
            for (int k = 0; k < 4; k++) {
                if (child->name != names[k]) {
                    continue;
                }

                TransferFunction& f = functions[k];
                u32 type = attribute(child, AtomType);
                StringView kind = type ? trim(textOf(doc, type)) : StringView();

                f.table.clear();
                f.type = kind == SV("table") ? 1 : kind == SV("discrete") ? 2 : kind == SV("linear") ? 3 : kind == SV("gamma") ? 4 : 0;
                numberList(doc, child, AtomTableValues, f.table);
                f.slope = numberOf(doc, child, AtomSlope, 1.f);
                f.intercept = numberOf(doc, child, AtomIntercept, 0.f);
                f.amplitude = numberOf(doc, child, AtomAmplitude, 1.f);
                f.exponent = numberOf(doc, child, AtomExponent, 1.f);
                f.offset = numberOf(doc, child, AtomOffset, 0.f);
            }
        }

        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                float c[4];

                unpremultiply(planeRead(in, x, y), c);

                for (int k = 0; k < 4; k++) {
                    c[k] = transfer(functions[k], c[k]);
                }

                storePremultiplied(planeAt(out, x, y), c);
            }
        }
    }

    static bool arithmeticOf(const Document& doc, const FilterPrimitive& p) {
        // A primitive read by another is an element.
        u32 op = attribute(p.node, AtomOperator);

        return p.kind == AtomFeComposite && op && trim(textOf(doc, op)) == SV("arithmetic");
    }

    static bool keepsInvalidColor(const Filter& f, const FilterPrimitive& p) {
        int index = (int)(&p - f.primitives);
        bool read = false;

        for (u32 j = (u32)index + 1; j < f.count; j++) {
            const FilterPrimitive& q = f.primitives[j];
            bool second = q.kind == AtomFeComposite || q.kind == AtomFeBlend || q.kind == AtomFeDisplacementMap;
            bool reads = q.in1 == index || (second && q.in2 == index);

            for (u32 k = 0; k < q.inputCount; k++) {
                reads = reads || q.inputs[k] == index;
            }

            if (!reads) {
                continue;
            }

            if (!arithmeticOf(*f.doc, q) || q.linear != p.linear) {
                return false;
            }

            read = true;
        }

        return read;
    }

    static void filterComposite(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane a = filterInputBuffer(run, p.in1, p.linear);
        Plane b = filterInputBuffer(run, p.in2, p.linear);
        u32 op = attribute(p.node, AtomOperator);
        StringView kind = op ? trim(textOf(doc, op)) : SV("over");
        float k1 = numberOf(doc, p.node, AtomK1, 0.f);
        float k2 = numberOf(doc, p.node, AtomK2, 0.f);
        float k3 = numberOf(doc, p.node, AtomK3, 0.f);
        float k4 = numberOf(doc, p.node, AtomK4, 0.f);
        const IntBox& box = p.box;
        bool arithmetic = kind == SV("arithmetic");
        float ceiling = arithmetic && keepsInvalidColor(*run.filter, p) ? 1.f : 0.f;
        int mode = kind == SV("in") ? 1 : (kind == SV("out") ? 2 : (kind == SV("atop") ? 3 : (kind == SV("xor") ? 4 : (kind == SV("lighter") ? 5 : 0))));

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                const float* s = planeRead(a, x, y);
                const float* d = planeRead(b, x, y);
                float* o = planeAt(out, x, y);

                if (arithmetic) {
                    float alpha = clampUnit(k1 * s[3] * d[3] + k2 * s[3] + k3 * d[3] + k4);

                    for (int c = 0; c < 3; c++) {
                        o[c] = minf(clampUnit(k1 * s[c] * d[c] + k2 * s[c] + k3 * d[c] + k4), maxf(alpha, ceiling));
                    }

                    o[3] = alpha;
                    continue;
                }

                float fa = 1.f;
                float fb = 1.f - s[3];

                if (mode == 1) {
                    fa = d[3];
                    fb = 0.f;
                } else if (mode == 2) {
                    fa = 1.f - d[3];
                    fb = 0.f;
                } else if (mode == 3) {
                    fa = d[3];
                    fb = 1.f - s[3];
                } else if (mode == 4) {
                    fa = 1.f - d[3];
                    fb = 1.f - s[3];
                } else if (mode == 5) {
                    fa = 1.f;
                    fb = 1.f;
                }

                for (int c = 0; c < 4; c++) {
                    o[c] = minf(s[c] * fa + d[c] * fb, 1.f);
                }
            }
        }
    }

    static float blendChannel(int mode, float cb, float cs) {
        switch (mode) {
            case 1:
                return cb * cs;
            case 2:
                return cb + cs - cb * cs;
            case 3:
                return cb <= 0.5f ? 2.f * cb * cs : 1.f - 2.f * (1.f - cb) * (1.f - cs);
            case 4:
                return minf(cb, cs);
            case 5:
                return maxf(cb, cs);
            case 6:
                return cb <= 0.f ? 0.f : (cs >= 1.f ? 1.f : minf(1.f, cb / (1.f - cs)));
            case 7:
                return cb >= 1.f ? 1.f : (cs <= 0.f ? 0.f : 1.f - minf(1.f, (1.f - cb) / cs));
            case 8:
                return cs <= 0.5f ? 2.f * cb * cs : 1.f - 2.f * (1.f - cb) * (1.f - cs);
            case 9: {
                if (cs <= 0.5f) {
                    return cb - (1.f - 2.f * cs) * cb * (1.f - cb);
                }

                float dcb = cb <= 0.25f ? ((16.f * cb - 12.f) * cb + 4.f) * cb : sqrtf(cb);

                return cb + (2.f * cs - 1.f) * (dcb - cb);
            }
            case 10:
                return fabsf(cb - cs);
            case 11:
                return cb + cs - 2.f * cb * cs;
            default:
                return cs;
        }
    }

    static float luminosity(const float* c) {
        return 0.3f * c[0] + 0.59f * c[1] + 0.11f * c[2];
    }

    static void clipColor(float* c) {
        float l = luminosity(c);
        float n = minf(c[0], minf(c[1], c[2]));
        float x = maxf(c[0], maxf(c[1], c[2]));

        // Rounding a shifted gray can put its luminosity just outside [0, 1].
        // At either endpoint the clipped color is constant; the formulas below
        // would otherwise divide by zero when all three channels are equal.
        if (l <= 0.f || l >= 1.f) {
            c[0] = c[1] = c[2] = clampUnit(l);

            return;
        }

        for (int k = 0; k < 3; k++) {
            if (n < 0.f) {
                c[k] = l + (c[k] - l) * l / (l - n);
            }

            if (x > 1.f) {
                c[k] = l + (c[k] - l) * (1.f - l) / (x - l);
            }
        }
    }

    static void setLuminosity(float* c, float l) {
        float d = l - luminosity(c);

        c[0] += d;
        c[1] += d;
        c[2] += d;
        clipColor(c);
    }

    static float saturation(const float* c) {
        return maxf(c[0], maxf(c[1], c[2])) - minf(c[0], minf(c[1], c[2]));
    }

    static void setSaturation(float* c, float s) {
        int imax = 0;
        int imin = 0;

        for (int k = 1; k < 3; k++) {
            if (c[k] > c[imax]) {
                imax = k;
            }

            if (c[k] < c[imin]) {
                imin = k;
            }
        }

        if (imax == imin) {
            c[0] = 0.f;
            c[1] = 0.f;
            c[2] = 0.f;

            return;
        }

        int imid = 3 - imax - imin;
        float range = c[imax] - c[imin];

        c[imid] = (c[imid] - c[imin]) * s / range;
        c[imax] = s;
        c[imin] = 0.f;
    }

    static int blendMode(StringView text) {
        static const char* const modes[] = {"normal", "multiply", "screen", "overlay", "darken", "lighten", "color-dodge", "color-burn", "hard-light", "soft-light", "difference", "exclusion", "hue", "saturation", "color", "luminosity"};

        for (int k = 0; k < 16; k++) {
            if (text == StringView(modes[k])) {
                return k;
            }
        }

        return 0;
    }

    static void blendPixel(int mode, const float* src, const float* dst, float* out) {
        float cs[4];
        float cb[4];
        float mixed[3];

        unpremultiply(src, cs);
        unpremultiply(dst, cb);

        float as = src[3];
        float ab = dst[3];

        if (mode < 12) {
            for (int k = 0; k < 3; k++) {
                mixed[k] = blendChannel(mode, cb[k], cs[k]);
            }
        } else {
            float c[3] = {cs[0], cs[1], cs[2]};
            float b[3] = {cb[0], cb[1], cb[2]};

            if (mode == 12) {
                float t[3] = {c[0], c[1], c[2]};

                setSaturation(t, saturation(b));
                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else if (mode == 13) {
                float t[3] = {b[0], b[1], b[2]};

                setSaturation(t, saturation(c));
                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else if (mode == 14) {
                float t[3] = {c[0], c[1], c[2]};

                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else {
                float t[3] = {b[0], b[1], b[2]};

                setLuminosity(t, luminosity(c));
                memcpy(mixed, t, sizeof(mixed));
            }
        }

        for (int k = 0; k < 3; k++) {
            out[k] = src[k] * (1.f - ab) + dst[k] * (1.f - as) + as * ab * clampUnit(mixed[k]);
        }

        out[3] = as + ab - as * ab;
    }

    static void filterBlend(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane a = filterInputBuffer(run, p.in1, p.linear);
        Plane b = filterInputBuffer(run, p.in2, p.linear);
        u32 modeValue = attribute(p.node, AtomMode);
        int mode = modeValue ? blendMode(trim(textOf(doc, modeValue))) : 0;
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                blendPixel(mode, planeRead(a, x, y), planeRead(b, x, y), planeAt(out, x, y));
            }
        }
    }

    static void filterMerge(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const IntBox& box = p.box;

        for (u32 k = 0; k < p.inputCount; k++) {
            Plane in = filterInputBuffer(run, p.inputs[k], p.linear);

            for (int y = box.y0; y < box.y1; y++) {
                for (int x = box.x0; x < box.x1; x++) {
                    const float* s = planeRead(in, x, y);
                    float* d = planeAt(out, x, y);
                    float keep = 1.f - s[3];

                    d[0] = s[0] + d[0] * keep;
                    d[1] = s[1] + d[1] * keep;
                    d[2] = s[2] + d[2] * keep;
                    d[3] = s[3] + d[3] * keep;
                }
            }
        }
    }

    // The running extreme of windows of w values over the line padded by
    // r zeros on each side, forward in g and backward in h, so that the
    // extreme of the window at x is the extreme of h[x] and g[x + 2r].
    template <typename Extreme>
    static void slideExtremeBy(const float* src, size_t stride, int n, int r, float* dst, size_t dstStride, float* pg, float* ph, Extreme extreme) {
        int m = n + 2 * r;
        int w = 2 * r + 1;
        int run = 0;

        for (int i = 0; i < m; i++) {
            int k = i - r;
            const float* v = k >= 0 && k < n ? src + (size_t)k * stride : zeroPixel;
            float* out = pg + (size_t)i * 4;

            if (run == 0) {
                memcpy(out, v, sizeof(float) * 4);
            } else {
                for (int c = 0; c < 4; c++) {
                    out[c] = extreme(out[c - 4], v[c]);
                }
            }

            run = run + 1 == w ? 0 : run + 1;
        }

        // Backward, a window starts at the last value and wherever i + 1 is
        // a multiple of w; phase counts i + 1 down modulo w.
        int phase = m % w;

        for (int i = m - 1; i >= 0; i--) {
            int k = i - r;
            const float* v = k >= 0 && k < n ? src + (size_t)k * stride : zeroPixel;
            float* out = ph + (size_t)i * 4;

            if (i == m - 1 || phase == 0) {
                memcpy(out, v, sizeof(float) * 4);
            } else {
                for (int c = 0; c < 4; c++) {
                    out[c] = extreme(out[c + 4], v[c]);
                }
            }

            phase = phase == 0 ? w - 1 : phase - 1;
        }

        for (int x = 0; x < n; x++) {
            float* d = dst + (size_t)x * dstStride;

            for (int c = 0; c < 4; c++) {
                d[c] = extreme(ph[(size_t)x * 4 + c], pg[(size_t)(x + 2 * r) * 4 + c]);
            }
        }
    }

    static void slideExtreme(const float* src, size_t stride, int n, int r, bool dilate, float* dst, size_t dstStride, Vector<float>& g, Vector<float>& h) {
        int m = n + 2 * r;

        g.zero((size_t)m * 4);
        h.zero((size_t)m * 4);

        if (dilate) {
            slideExtremeBy(src, stride, n, r, dst, dstStride, g.mutData(), h.mutData(), maxf);
        } else {
            slideExtremeBy(src, stride, n, r, dst, dstStride, g.mutData(), h.mutData(), minf);
        }
    }

    static void filterMorphology(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        u32 op = attribute(p.node, AtomOperator);
        bool dilate = op && trim(textOf(*f.doc, op)) == SV("dilate");
        float rx = 0.f;
        float ry = 0.f;

        bool valid = !attribute(p.node, AtomRadius) || numberPair(*f.doc, p.node, AtomRadius, rx, ry);

        if (!valid || rx < 0.f || ry < 0.f || (rx == 0.f && ry == 0.f)) {
            copyBox(in, out, p.box);

            return;
        }

        int height = (int)run.raster->height;
        int width = (int)run.raster->width;
        float ux = userX(f, rx) * run.scaleX + 0.5f;
        float uy = userY(f, ry) * run.scaleY + 0.5f;
        int ix = (int)floorf(minf(ux, (float)width));
        int iy = (int)floorf(minf(uy, (float)height));
        const IntBox& box = p.box;
        Vector<float> row;
        Vector<float> line;
        Vector<float> column;
        Vector<float> g;
        Vector<float> h;

        // An empty box is empty by its x, as boxInside leaves it.
        if (box.x1 <= box.x0) {
            return;
        }

        int x0 = box.x0 - ix > 0 ? box.x0 - ix : 0;
        int x1 = box.x1 + ix < width ? box.x1 + ix : width;
        int y0 = box.y0 - iy > 0 ? box.y0 - iy : 0;
        int y1 = box.y1 + iy < height ? box.y1 + iy : height;
        Plane pass = makePlane(IntBox{box.x0, y0, box.x1, y1});

        row.zero((size_t)(x1 - x0) * 4);
        line.zero((size_t)(x1 - x0) * 4);
        column.zero((size_t)(y1 - y0) * 4);

        for (int y = y0; y < y1; y++) {
            for (int x = x0; x < x1; x++) {
                memcpy(row.mutData() + (size_t)(x - x0) * 4, planeRead(in, x, y), sizeof(float) * 4);
            }

            slideExtreme(row.data(), 4, x1 - x0, ix, dilate, line.mutData(), 4, g, h);
            memcpy(planeAt(pass, box.x0, y), line.data() + (size_t)(box.x0 - x0) * 4, sizeof(float) * 4 * (box.x1 - box.x0));
        }

        for (int x = box.x0; x < box.x1; x++) {
            slideExtreme(planeAt(pass, x, y0), (size_t)planeWidth(pass) * 4, y1 - y0, iy, dilate, column.mutData(), 4, g, h);

            for (int y = box.y0; y < box.y1; y++) {
                memcpy(planeAt(out, x, y), column.data() + (size_t)(y - y0) * 4, sizeof(float) * 4);
            }
        }

        releasePlane(pass);
    }

    static void filterTile(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        IntBox tile = inputBox(run, p.in1);

        if (p.in1 >= 0) {
            Box exact = deviceBox(run.filter->ctm, run.filter->primitives[p.in1].user);
            IntBox inner{(int)ceilf(exact.x0 - 1e-3f), (int)ceilf(exact.y0 - 1e-3f), (int)floorf(exact.x1 + 1e-3f), (int)floorf(exact.y1 + 1e-3f)};

            if (inner.x0 >= tile.x0 && inner.y0 >= tile.y0 && inner.x1 <= tile.x1 && inner.y1 <= tile.y1 && inner.x1 > inner.x0 && inner.y1 > inner.y0) {
                tile = inner;
            }
        }

        int tw = tile.x1 - tile.x0;
        int th = tile.y1 - tile.y0;
        const IntBox& box = p.box;

        // An empty tile is empty by its width, as boxInside leaves a box.
        if (tw <= 0) {
            return;
        }

        for (int y = box.y0; y < box.y1; y++) {
            int sy = tile.y0 + (((y - tile.y0) % th) + th) % th;

            for (int x = box.x0; x < box.x1; x++) {
                int sx = tile.x0 + (((x - tile.x0) % tw) + tw) % tw;

                memcpy(planeAt(out, x, y), planeRead(in, sx, sy), sizeof(float) * 4);
            }
        }
    }

    static void filterConvolve(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        float ox = 3.f;
        float oy = 3.f;

        if (attribute(p.node, AtomOrder) && !numberPair(doc, p.node, AtomOrder, ox, oy)) {
            return;
        }

        int orderX = (int)ox;
        int orderY = (int)oy;

        if ((float)orderX != ox || (float)orderY != oy || orderX < 1 || orderY < 1 || orderX > 32 || orderY > 32) {
            return;
        }

        Vector<float> kernel;

        if (!numberList(doc, p.node, AtomKernelMatrix, kernel) || kernel.length() != (size_t)(orderX * orderY)) {
            return;
        }

        float sum = 0.f;

        for (float k : kernel) {
            sum += k;
        }

        float divisor = numberOf(doc, p.node, AtomDivisor, sum == 0.f ? 1.f : sum);

        if (divisor == 0.f) {
            divisor = sum == 0.f ? 1.f : sum;
        }

        float bias = numberOf(doc, p.node, AtomBias, 0.f);
        int targetX = (int)numberOf(doc, p.node, AtomTargetX, (float)(orderX / 2));
        int targetY = (int)numberOf(doc, p.node, AtomTargetY, (float)(orderY / 2));

        if (targetX < 0 || targetX >= orderX || targetY < 0 || targetY >= orderY) {
            return;
        }

        u32 edgeValue = attribute(p.node, AtomEdgeMode);
        StringView edge = edgeValue ? trim(textOf(doc, edgeValue)) : SV("duplicate");
        bool edgeNone = edge == SV("none");
        bool edgeWrap = edge == SV("wrap");
        u32 preserveValue = attribute(p.node, AtomPreserveAlpha);
        bool preserve = preserveValue && trim(textOf(doc, preserveValue)) == SV("true");
        IntBox src = inputBox(run, p.in1);
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float acc[4] = {0.f, 0.f, 0.f, 0.f};

                for (int j = 0; j < orderY; j++) {
                    for (int i = 0; i < orderX; i++) {
                        int sx = x - targetX + i;
                        int sy = y - targetY + j;
                        bool inside = sx >= src.x0 && sx < src.x1 && sy >= src.y0 && sy < src.y1;

                        if (!inside) {
                            if (edgeNone) {
                                continue;
                            }

                            if (edgeWrap) {
                                int w = src.x1 - src.x0;
                                int h = src.y1 - src.y0;

                                // An empty input is empty by its width, as boxInside leaves a box.
                                if (w <= 0) {
                                    continue;
                                }

                                sx = src.x0 + (((sx - src.x0) % w) + w) % w;
                                sy = src.y0 + (((sy - src.y0) % h) + h) % h;
                            } else {
                                sx = sx < src.x0 ? src.x0 : (sx >= src.x1 ? src.x1 - 1 : sx);
                                sy = sy < src.y0 ? src.y0 : (sy >= src.y1 ? src.y1 - 1 : sy);
                            }
                        }

                        float k = kernel[(size_t)(orderY - 1 - j) * orderX + (orderX - 1 - i)];
                        const float* s = planeRead(in, sx, sy);

                        if (preserve) {
                            float c[4];

                            unpremultiply(s, c);

                            for (int ch = 0; ch < 3; ch++) {
                                acc[ch] += c[ch] * k;
                            }
                        } else {
                            for (int ch = 0; ch < 4; ch++) {
                                acc[ch] += rintf(s[ch] * 255.f) / 255.f * k;
                            }
                        }
                    }
                }

                float* o = planeAt(out, x, y);

                if (preserve) {
                    float a = planeRead(in, x, y)[3];
                    float c[4] = {acc[0] / divisor + bias, acc[1] / divisor + bias, acc[2] / divisor + bias, a};

                    storePremultiplied(o, c);
                } else {
                    float a = clampUnit(acc[3] / divisor + bias);

                    for (int ch = 0; ch < 3; ch++) {
                        o[ch] = minf(clampUnit(acc[ch] / divisor + bias), a);
                    }

                    o[3] = a;
                }
            }
        }
    }

    // The noise is computed in single precision, as Chromium's Skia does.
    struct Turbulence {
        int lattice[0x100 + 0x100 + 2];
        float gradient[0x100 + 0x100 + 2][2][4];
    };

    struct Stitch {
        i64 width;
        i64 height;
        i64 wrapX;
        i64 wrapY;
    };

    // Stitching lengths and offsets, in lattice cells, saturate far beyond
    // any cell a sample reaches, so that every tile and octave converts.
    static i64 latticeCells(double cells) {
        constexpr double limit = 0x1p40;

        return cells != cells ? 0 : (i64)(cells < -limit ? -limit : (cells > limit ? limit : cells));
    }

    static long turbulenceRandom(long seed) {
        long result = 16807 * (seed % 127773) - 2836 * (seed / 127773);

        return result <= 0 ? result + 2147483647 : result;
    }

    static void turbulenceInit(Turbulence& t, long seed) {
        if (seed <= 0) {
            seed = -(seed % (2147483647 - 1)) + 1;
        }

        if (seed > 2147483647 - 1) {
            seed = 2147483647 - 1;
        }

        int i;
        int j;
        int k;

        for (k = 0; k < 4; k++) {
            for (i = 0; i < 0x100; i++) {
                t.lattice[i] = i;

                for (j = 0; j < 2; j++) {
                    seed = turbulenceRandom(seed);
                    t.gradient[i][j][k] = (float)((seed % (0x100 + 0x100)) - 0x100) / 0x100;
                }

                float s = sqrtf(t.gradient[i][0][k] * t.gradient[i][0][k] + t.gradient[i][1][k] * t.gradient[i][1][k]);

                t.gradient[i][0][k] /= s;
                t.gradient[i][1][k] /= s;
            }
        }

        while (--i) {
            k = t.lattice[i];
            seed = turbulenceRandom(seed);
            j = (int)(seed % 0x100);
            t.lattice[i] = t.lattice[j];
            t.lattice[j] = k;
        }

        for (i = 0; i < 0x100 + 2; i++) {
            t.lattice[0x100 + i] = t.lattice[i];

            for (k = 0; k < 4; k++) {
                for (j = 0; j < 2; j++) {
                    t.gradient[0x100 + i][j][k] = t.gradient[i][j][k];
                }
            }
        }
    }

    static float sCurve(float t) {
        return t * t * (3.f - 2.f * t);
    }

    static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    static void noise2(const Turbulence& tb, float vx, float vy, const Stitch* stitch, float* out) {
        float t = vx + 0x1000;

        // The lattice repeats every 256 cells, so a far coordinate keeps its
        // cell modulo 256; one that is not finite has no cell and takes 0.
        if (!(t >= -2147483647.f && t <= 2147483520.f)) {
            t = isfinite(t) ? fmodf(t, 256.f) : 0.f;
        }

        int bx0 = (int)t;
        float rx0 = t - bx0;

        if (rx0 < 0.f) {
            // Choose the cell below the point, rather than extrapolating from
            // the next cell when the coordinate is below -4096.
            bx0--;
            rx0 += 1.f;
        }

        int bx1 = bx0 + 1;
        float rx1 = rx0 - 1.f;

        t = vy + 0x1000;

        if (!(t >= -2147483647.f && t <= 2147483520.f)) {
            t = isfinite(t) ? fmodf(t, 256.f) : 0.f;
        }

        int by0 = (int)t;
        float ry0 = t - by0;

        if (ry0 < 0.f) {
            by0--;
            ry0 += 1.f;
        }

        int by1 = by0 + 1;
        float ry1 = ry0 - 1.f;

        // Narrowing a wrapped cell keeps its low byte, all the lattice reads.
        if (stitch) {
            if (bx0 >= stitch->wrapX) {
                bx0 = (int)(bx0 - stitch->width);
            }

            if (bx1 >= stitch->wrapX) {
                bx1 = (int)(bx1 - stitch->width);
            }

            if (by0 >= stitch->wrapY) {
                by0 = (int)(by0 - stitch->height);
            }

            if (by1 >= stitch->wrapY) {
                by1 = (int)(by1 - stitch->height);
            }
        }

        bx0 &= 0xff;
        bx1 &= 0xff;
        by0 &= 0xff;
        by1 &= 0xff;

        int i = tb.lattice[bx0];
        int j = tb.lattice[bx1];
        int b00 = tb.lattice[i + by0];
        int b10 = tb.lattice[j + by0];
        int b01 = tb.lattice[i + by1];
        int b11 = tb.lattice[j + by1];
        float sx = sCurve(rx0);
        float sy = sCurve(ry0);
        for (int channel = 0; channel < 4; channel++) {
            float u = rx0 * tb.gradient[b00][0][channel] + ry0 * tb.gradient[b00][1][channel];
            float v = rx1 * tb.gradient[b10][0][channel] + ry0 * tb.gradient[b10][1][channel];
            float a = lerp(sx, u, v);

            u = rx0 * tb.gradient[b01][0][channel] + ry1 * tb.gradient[b01][1][channel];
            v = rx1 * tb.gradient[b11][0][channel] + ry1 * tb.gradient[b11][1][channel];

            float b = lerp(sx, u, v);

            out[channel] = lerp(sy, a, b);
        }
    }

    // Snaps the frequencies to the tile and sets up the wrapping of the first
    // octave, as feTurbulence stitches its tiles.
    static void stitchTiles(double& fx, double& fy, double tileX, double tileY, double tileW, double tileH, Stitch& stitch) {
        if (fx != 0.0) {
            double low = floor(tileW * fx) / tileW;
            double high = ceil(tileW * fx) / tileW;

            fx = fx / low < high / fx ? low : high;
        }

        if (fy != 0.0) {
            double low = floor(tileH * fy) / tileH;
            double high = ceil(tileH * fy) / tileH;

            fy = fy / low < high / fy ? low : high;
        }

        stitch.width = latticeCells(tileW * fx + 0.5);
        stitch.wrapX = latticeCells(tileX * fx + 0x1000 + stitch.width);
        stitch.height = latticeCells(tileH * fy + 0.5);
        stitch.wrapY = latticeCells(tileY * fy + 0x1000 + stitch.height);
    }

    static void turbulenceAt(const Turbulence& tb, double px, double py, double fx, double fy, int octaves, bool fractal, const Stitch* stitching, float* out) {
        Stitch stitch;
        Stitch* active = nullptr;

        if (stitching) {
            stitch = *stitching;
            active = &stitch;
        }

        float sum[4] = {};
        float vx = (float)(px * fx);
        float vy = (float)(py * fy);
        float weight = 1.f;

        for (int octave = 0; octave < octaves; octave++) {
            float noise[4];

            noise2(tb, vx, vy, active, noise);

            // The weight is a power of two, so this scaling is as exact as a division.
            for (int channel = 0; channel < 4; channel++) {
                sum[channel] += fractal ? noise[channel] * weight : fabsf(noise[channel]) * weight;
            }

            vx *= 2;
            vy *= 2;
            weight *= 0.5f;

            if (active) {
                stitch.width = latticeCells(2.0 * stitch.width);
                stitch.wrapX = latticeCells(2.0 * stitch.wrapX - 0x1000);
                stitch.height = latticeCells(2.0 * stitch.height);
                stitch.wrapY = latticeCells(2.0 * stitch.wrapY - 0x1000);
            }
        }

        for (int channel = 0; channel < 4; channel++) {
            out[channel] = fractal ? (sum[channel] + 1.f) * 0.5f : sum[channel];
        }
    }

    static void filterTurbulence(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        float fx = 0.f;
        float fy = 0.f;

        if (attribute(p.node, AtomBaseFrequency) && !numberPair(doc, p.node, AtomBaseFrequency, fx, fy)) {
            return;
        }

        if (fx < 0.f || fy < 0.f) {
            return;
        }

        int octaves = integerOf(doc, p.node, AtomNumOctaves, 1);

        if (octaves < 0) {
            return;
        }

        // Octave k weighs 2^-k, which is zero in single precision from
        // octave 150 on, so the octaves past it add nothing and the count
        // stops there.
        octaves = octaves < 150 ? octaves : 150;

        // The seed saturates to 32 bits before it is truncated; turbulenceInit
        // clamps the large ones further.
        float seedValue = numberOf(doc, p.node, AtomSeed, 0.f);
        long seed = seedValue >= 2147483648.f ? 2147483647 : (seedValue < -2147483648.f ? -2147483647 - 1 : (long)seedValue);
        u32 type = attribute(p.node, AtomType);
        bool fractal = type && trim(textOf(doc, type)) == SV("fractalNoise");
        u32 stitchValue = attribute(p.node, AtomStitchTiles);
        bool stitching = stitchValue && trim(textOf(doc, stitchValue)) == SV("stitch");
        Matrix inverse;

        if (!invert(f.ctm, inverse)) {
            return;
        }

        Turbulence* tb = (Turbulence*)run.pool->allocate(sizeof(Turbulence));

        turbulenceInit(*tb, seed);

        const IntBox& box = p.box;
        float tileW = p.user.x1 - p.user.x0;
        float tileH = p.user.y1 - p.user.y0;
        double frequencyX = fx;
        double frequencyY = fy;
        Stitch stitch;

        if (stitching) {
            stitchTiles(frequencyX, frequencyY, p.user.x0, p.user.y0, tileW, tileH, stitch);
        }

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                Point point = apply(inverse, Point{(float)x + 0.5f, (float)y + 0.5f});
                float c[4];

                turbulenceAt(*tb, point.x, point.y, frequencyX, frequencyY, octaves, fractal, stitching ? &stitch : nullptr, c);
                storePremultiplied(planeAt(out, x, y), c);
            }
        }
    }

    static void filterDisplacement(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        Plane map = filterInputBuffer(run, p.in2, p.linear);
        float scale = numberOf(doc, p.node, AtomScale, 0.f);
        float sx = userX(f, scale) * run.scaleX;
        float sy = userY(f, scale) * run.scaleY;
        auto channel = [&](u32 name) {
            u32 value = attribute(p.node, name);
            StringView text = value ? trim(textOf(doc, value)) : SV("A");

            return text == SV("R") ? 0 : (text == SV("G") ? 1 : (text == SV("B") ? 2 : 3));
        };
        int cx = channel(AtomXChannelSelector);
        int cy = channel(AtomYChannelSelector);
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float m[4];

                unpremultiply(planeRead(map, x, y), m);

                int tx = (int)floorf((float)x + sx * (m[cx] - 0.5f) + 0.5f);
                int ty = (int)floorf((float)y + sy * (m[cy] - 0.5f) + 0.5f);

                if (tx < 0 || ty < 0 || tx >= (int)run.raster->width || ty >= (int)run.raster->height) {
                    continue;
                }

                memcpy(planeAt(out, x, y), planeRead(in, tx, ty), sizeof(float) * 4);
            }
        }
    }

    struct Vector3 {
        float x;
        float y;
        float z;
    };

    static Vector3 normalize3(Vector3 v) {
        float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

        return length > 0.f ? Vector3{v.x / length, v.y / length, v.z / length} : Vector3{0.f, 0.f, 0.f};
    }

    static float dot3(Vector3 a, Vector3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static void filterLighting(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        bool specular = p.kind == AtomFeSpecularLighting;
        float surfaceScale = numberOf(doc, p.node, AtomSurfaceScale, 1.f);
        Matrix inverse;

        if (!invert(f.ctm, inverse)) {
            return;
        }

        float constant = numberOf(doc, p.node, specular ? AtomSpecularConstant : AtomDiffuseConstant, 1.f);
        float shininess = numberOf(doc, p.node, AtomSpecularExponent, 1.f);
        Node* light = nullptr;

        for (Node* child = p.node->first; child; child = child->next) {
            if (child->name == AtomFeDistantLight || child->name == AtomFePointLight || child->name == AtomFeSpotLight) {
                light = child;
                break;
            }
        }

        if (!light) {
            return;
        }

        constant = constant < 0.f ? 0.f : constant;
        shininess = shininess < 1.f ? 1.f : (shininess > 128.f ? 128.f : shininess);

        // The specular power of a cosine, tabulated over 0 to 1.
        constexpr int powerEntries = 2048;
        float powers[powerEntries + 2];

        for (int i = 0; i <= powerEntries + 1; i++) {
            powers[i] = powf(minf((float)i / (float)powerEntries, 1.f), shininess);
        }

        Color lightColor = toSpace(p.lighting, p.linear);
        float bw = f.bbox.x1 - f.bbox.x0;
        float bh = f.bbox.y1 - f.bbox.y0;
        auto position = [&](u32 nx, u32 ny, u32 nz) {
            float x = numberOf(doc, light, nx, 0.f);
            float y = numberOf(doc, light, ny, 0.f);
            float z = numberOf(doc, light, nz, 0.f);

            if (f.boxPrimitives) {
                x = f.bbox.x0 + x * bw;
                y = f.bbox.y0 + y * bh;
                z *= sqrtf((bw * bw + bh * bh) / 2.f);
            }

            return Vector3{x, y, z};
        };
        Vector3 lightPosition{0.f, 0.f, 0.f};
        Vector3 direction{0.f, 0.f, 0.f};
        Vector3 spotDirection{0.f, 0.f, 0.f};
        float spotExponent = 1.f;
        float cosOuter = -1.f;
        float cosInner = -1.f;
        bool limited = false;

        if (light->name == AtomFeDistantLight) {
            float azimuth = numberOf(doc, light, AtomAzimuth, 0.f) * pi / 180.f;
            float elevation = numberOf(doc, light, AtomElevation, 0.f) * pi / 180.f;

            direction = Vector3{cosf(azimuth) * cosf(elevation), sinf(azimuth) * cosf(elevation), sinf(elevation)};
        } else {
            lightPosition = position(AtomX, AtomY, AtomZ);

            if (light->name == AtomFeSpotLight) {
                Vector3 target = position(AtomPointsAtX, AtomPointsAtY, AtomPointsAtZ);

                spotDirection = normalize3(Vector3{target.x - lightPosition.x, target.y - lightPosition.y, target.z - lightPosition.z});
                spotExponent = numberOf(doc, light, AtomSpecularExponent, 1.f);
                spotExponent = spotExponent < 1.f ? 1.f : (spotExponent > 128.f ? 128.f : spotExponent);

                if (attribute(light, AtomLimitingConeAngle) && numberOf(doc, light, AtomLimitingConeAngle, 0.f) != 0.f) {
                    float angle = fabsf(numberOf(doc, light, AtomLimitingConeAngle, 90.f));

                    limited = true;
                    cosOuter = cosf(angle * pi / 180.f);
                    cosInner = cosOuter + 0.016f;
                }
            }
        }

        // The box lies on the raster.
        const IntBox& box = p.box;
        auto alphaAt = [&](int x, int y) {
            x = x < box.x0 ? box.x0 : (x >= box.x1 ? box.x1 - 1 : x);
            y = y < box.y0 ? box.y0 : (y >= box.y1 ? box.y1 - 1 : y);

            return planeRead(in, x, y)[3];
        };

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                bool left = x == box.x0;
                bool right = x == box.x1 - 1;
                bool top = y == box.y0;
                bool bottom = y == box.y1 - 1;
                float nx;
                float ny;
                float factorX;
                float factorY;
                float a00 = alphaAt(x - 1, y - 1);
                float a01 = alphaAt(x, y - 1);
                float a02 = alphaAt(x + 1, y - 1);
                float a10 = alphaAt(x - 1, y);
                float a11 = alphaAt(x, y);
                float a12 = alphaAt(x + 1, y);
                float a20 = alphaAt(x - 1, y + 1);
                float a21 = alphaAt(x, y + 1);
                float a22 = alphaAt(x + 1, y + 1);

                if (box.x1 - box.x0 == 1 || box.y1 - box.y0 == 1) {
                    nx = 0.f;
                    ny = 0.f;
                    factorX = 0.f;
                    factorY = 0.f;
                } else if (top && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2.f * a11 + 2.f * a12 - a21 + a22;
                    ny = -2.f * a11 - a12 + 2.f * a21 + a22;
                } else if (top && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2.f * a10 + 2.f * a11 - a20 + a21;
                    ny = -a10 - 2.f * a11 + a20 + 2.f * a21;
                } else if (bottom && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a01 + a02 - 2.f * a11 + 2.f * a12;
                    ny = -2.f * a01 - a02 + 2.f * a11 + a12;
                } else if (bottom && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a00 + a01 - 2.f * a10 + 2.f * a11;
                    ny = -a00 - 2.f * a01 + a10 + 2.f * a11;
                } else if (top) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -2.f * a10 + 2.f * a12 - a20 + a22;
                    ny = -a10 - 2.f * a11 - a12 + a20 + 2.f * a21 + a22;
                } else if (bottom) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -a00 + a02 - 2.f * a10 + 2.f * a12;
                    ny = -a00 - 2.f * a01 - a02 + a10 + 2.f * a11 + a12;
                } else if (left) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a01 + a02 - 2.f * a11 + 2.f * a12 - a21 + a22;
                    ny = -2.f * a01 - a02 + 2.f * a21 + a22;
                } else if (right) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a00 + a01 - 2.f * a10 + 2.f * a11 - a20 + a21;
                    ny = -a00 - 2.f * a01 + a20 + 2.f * a21;
                } else {
                    factorX = 1.f / 4.f;
                    factorY = 1.f / 4.f;
                    nx = -a00 + a02 - 2.f * a10 + 2.f * a12 - a20 + a22;
                    ny = -a00 - 2.f * a01 - a02 + a20 + 2.f * a21 + a22;
                }

                float dx = factorX * nx;
                float dy = factorY * ny;
                // The height samples are in device space; the light is in user space.
                Vector3 normal = normalize3(Vector3{-surfaceScale * (f.ctm.a * dx + f.ctm.b * dy), -surfaceScale * (f.ctm.c * dx + f.ctm.d * dy), 1.f});
                Vector3 toLight = direction;
                Color color = lightColor;

                if (light->name != AtomFeDistantLight) {
                    Point surface = apply(inverse, Point{(float)x + 0.5f, (float)y + 0.5f});

                    toLight = normalize3(Vector3{lightPosition.x - surface.x, lightPosition.y - surface.y, lightPosition.z - surfaceScale * a11});
                }

                if (light->name == AtomFeSpotLight) {
                    float cosAngle = -dot3(toLight, spotDirection);
                    float s = 0.f;

                    if (!limited || cosAngle >= cosOuter) {
                        s = cosAngle > 0.f ? powf(cosAngle, spotExponent) : 0.f;

                        if (limited && cosAngle < cosInner) {
                            s *= (cosAngle - cosOuter) / 0.016f;
                        }
                    }

                    color = Color{color.r * s, color.g * s, color.b * s, 1.f};
                }

                float factor;

                if (specular) {
                    Vector3 half = normalize3(Vector3{toLight.x, toLight.y, toLight.z + 1.f});
                    // The power of the cosine comes from its table, between
                    // the two entries around it.
                    float at = clampUnit(dot3(normal, half)) * (float)powerEntries;
                    float whole = floorf(at);
                    int index = (int)whole;

                    factor = constant * (powers[index] + (powers[index + 1] - powers[index]) * (at - whole));
                } else {
                    factor = constant * dot3(normal, toLight);
                }

                float r = clampUnit(color.r * factor);
                float g = clampUnit(color.g * factor);
                float b = clampUnit(color.b * factor);
                float* o = planeAt(out, x, y);

                if (specular) {
                    float a = maxf(r, maxf(g, b));

                    o[0] = r;
                    o[1] = g;
                    o[2] = b;
                    o[3] = a;
                } else {
                    o[0] = r;
                    o[1] = g;
                    o[2] = b;
                    o[3] = 1.f;
                }
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base);

    static void filterImage(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        Plane content = makePlane(p.box);

        runProgram(*run.raster, *run.lowering, run.lowering->programs.data() + p.programFirst, p.programCount, content);

        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float* o = planeAt(out, x, y);
                const float* s = planeAt(content, x, y);

                if (p.linear) {
                    float c[4];

                    unpremultiply(s, c);

                    for (int k = 0; k < 3; k++) {
                        c[k] = srgbToLinear(c[k]);
                    }

                    storePremultiplied(o, c);
                } else {
                    memcpy(o, s, sizeof(float) * 4);
                }
            }
        }

        releasePlane(content);
    }

    // Whether the primitive reads the result of the primitive at index: the
    // inputs lowerFilter gave it.
    static bool readsResult(const FilterPrimitive& p, int index) {
        if (p.kind == AtomFeMerge) {
            for (u32 k = 0; k < p.inputCount; k++) {
                if (p.inputs[k] == index) {
                    return true;
                }
            }

            return false;
        }

        if (p.kind == AtomFeFlood || p.kind == AtomFeImage || p.kind == AtomFeTurbulence) {
            return false;
        }

        bool second = p.kind == AtomFeBlend || p.kind == AtomFeComposite || p.kind == AtomFeDisplacementMap;

        return p.in1 == index || (second && p.in2 == index);
    }

    // Runs the filter over the layer and writes its result back over it: the
    // final primitive's result inside its box, zero elsewhere.
    static void applyFilter(Raster& r, const Lowering& l, const Filter& lowered, const Plane& layer, ObjPool& pool, const IntBox& visible, bool resetBackground = false) {
        // The region and the primitive boxes were clipped to the canvas when
        // the filter was lowered, the raster of the pattern tile or layer it
        // is drawn on. The final result is read within the visible box alone.
        Filter filter = lowered;
        size_t size = (size_t)planeWidth(layer) * (size_t)(layer.box.y1 - layer.box.y0);

        filter.region = boxInside(lowered.region, layer.box);

        if (lowered.count) {
            FilterPrimitive* primitives = (FilterPrimitive*)pool.allocate(sizeof(FilterPrimitive) * lowered.count);

            for (u32 k = 0; k < lowered.count; k++) {
                primitives[k] = lowered.primitives[k];
            }

            primitives[lowered.count - 1].box = boxInside(primitives[lowered.count - 1].box, visible);
            filter.primitives = primitives;

            // A primitive is read within the boxes of its readers, each
            // widened by its own reach, and is computed there alone: the rest
            // of its box cannot reach the visible result. A feTile reads its
            // whole input, which keeps its box.
            float scaleX = sqrtf(filter.ctm.a * filter.ctm.a + filter.ctm.b * filter.ctm.b);
            float scaleY = sqrtf(filter.ctm.c * filter.ctm.c + filter.ctm.d * filter.ctm.d);
            Vector<IntBox> need;

            need.zero(lowered.count);
            need.mut(lowered.count - 1) = primitives[lowered.count - 1].box;

            for (u32 j = lowered.count; j-- > 0;) {
                FilterPrimitive& p = primitives[j];
                float reach = 0.f;

                p.box = boxInside(p.box, need[j]);

                bool bounded = primitiveReach(filter, p, scaleX, scaleY, reach) && reach < 16777216.f;
                IntBox wide = p.box;

                if (bounded && wide.x1 > wide.x0) {
                    int r = (int)ceilf(reach);

                    wide = IntBox{wide.x0 - r, wide.y0 - r, wide.x1 + r, wide.y1 + r};
                }

                for (u32 i = 0; i < j; i++) {
                    if (readsResult(p, (int)i)) {
                        need.mut(i) = boxJoin(need[i], bounded ? wide : primitives[i].box);
                    }
                }
            }
        }

        const IntBox& region = filter.region;

        // An empty region is all zero, as boxInside makes it.
        if (filter.count == 0 || region.x1 <= region.x0) {
            // The callers filter only layers that have pixels.
            memset(layer.data, 0, sizeof(float) * size * 4);

            return;
        }

        FilterRun run;

        run.raster = &r;
        run.lowering = &l;
        run.filter = &filter;
        run.pool = &pool;
        run.canvas = IntBox{0, 0, (int)r.width, (int)r.height};
        run.scaleX = sqrtf(filter.ctm.a * filter.ctm.a + filter.ctm.b * filter.ctm.b);
        run.scaleY = sqrtf(filter.ctm.c * filter.ctm.c + filter.ctm.d * filter.ctm.d);
        run.sourceLinear = Plane{nullptr, IntBox{0, 0, 0, 0}};
        run.background = run.sourceLinear;
        run.backgroundLinear = run.sourceLinear;
        run.backgroundAlpha = run.sourceLinear;
        run.empty = run.sourceLinear;
        run.resetBackground = resetBackground;

        for (int kind = 0; kind < 2; kind++) {
            for (int space = 0; space < 2; space++) {
                run.paint[kind][space] = run.sourceLinear;
            }
        }

        // SourceGraphic is the layer, transparent outside the filter region;
        // the result overwrites the layer anyway.
        run.source = layer;

        for (int y = layer.box.y0; y < layer.box.y1; y++) {
            if (y < region.y0 || y >= region.y1) {
                clearSpan(layer, y, layer.box.x0, layer.box.x1);
            } else {
                clearSpan(layer, y, layer.box.x0, region.x0);
                clearSpan(layer, y, region.x1, layer.box.x1);
            }
        }

        // A source read in linear light alone is converted in place: the
        // result overwrites the layer anyway.
        if (!readsInSrgb(filter, InputSourceGraphic)) {
            convertInPlace(run.source, true);
            run.sourceLinear = run.source;
        }

        run.sourceAlpha = run.empty;
        run.linearCache.zero(filter.count);
        run.srgbCache.zero(filter.count);
        run.results.zero(filter.count);

        // A primitive is computed when a reader needs it, the final one first
        // of all, so that a result lives from just before its first reader
        // to just after its last; a primitive the final one does not reach
        // is not computed, and the readers are counted among those reached.
        Vector<u8> reached;
        Vector<u8> done;
        Vector<u32> stack;

        reached.zero(filter.count);
        done.zero(filter.count);
        stack.pushBack(filter.count - 1);

        while (!stack.empty()) {
            u32 k = stack.popBack();

            if (reached[k]) {
                continue;
            }

            reached.mut(k) = 1;

            for (u32 i = 0; i < k; i++) {
                if (readsResult(filter.primitives[k], (int)i)) {
                    stack.pushBack(i);
                }
            }
        }

        run.readers.zero(filter.count);
        run.sourceReaders = 0;
        run.sourceAlphaReaders = 0;
        run.backgroundReaders = 0;
        run.backgroundAlphaReaders = 0;

        for (u32 k = 0; k < filter.count; k++) {
            const FilterPrimitive& p = filter.primitives[k];

            if (!reached[k]) {
                continue;
            }

            for (u32 i = 0; i < k; i++) {
                run.readers.mut(i) += readsResult(p, (int)i);
            }

            run.sourceReaders += readsResult(p, InputSourceGraphic);
            run.sourceAlphaReaders += readsResult(p, InputSourceAlpha);
            run.backgroundReaders += readsResult(p, InputBackgroundImage);
            run.backgroundAlphaReaders += readsResult(p, InputBackgroundAlpha);
        }

        stack.pushBack(filter.count - 1);

        while (!stack.empty()) {
            u32 k = stack.back();

            if (done[k]) {
                stack.popBack();
                continue;
            }

            const FilterPrimitive& p = filter.primitives[k];
            u32 pending = k;

            for (u32 i = 0; i < k && pending == k; i++) {
                if (!done[i] && readsResult(p, (int)i)) {
                    pending = i;
                }
            }

            if (pending != k) {
                stack.pushBack(pending);
                continue;
            }

            Plane out = makePlane(p.box);

            switch (p.kind) {
                case AtomFeFlood:
                    filterFlood(p, out);
                    break;
                case AtomFeOffset:
                    filterOffset(run, p, out);
                    break;
                case AtomFeGaussianBlur:
                    filterBlur(run, p, out);
                    break;
                case AtomFeDropShadow:
                    filterDropShadow(run, p, out);
                    break;
                case AtomFeColorMatrix:
                    filterColorMatrix(run, p, out);
                    break;
                case AtomFeComponentTransfer:
                    filterComponentTransfer(run, p, out);
                    break;
                case AtomFeComposite:
                    filterComposite(run, p, out);
                    break;
                case AtomFeBlend:
                    filterBlend(run, p, out);
                    break;
                case AtomFeMerge:
                    filterMerge(run, p, out);
                    break;
                case AtomFeMorphology:
                    filterMorphology(run, p, out);
                    break;
                case AtomFeTile:
                    filterTile(run, p, out);
                    break;
                case AtomFeConvolveMatrix:
                    filterConvolve(run, p, out);
                    break;
                case AtomFeTurbulence:
                    filterTurbulence(run, p, out);
                    break;
                case AtomFeDisplacementMap:
                    filterDisplacement(run, p, out);
                    break;
                case AtomFeDiffuseLighting:
                case AtomFeSpecularLighting:
                    filterLighting(run, p, out);
                    break;
                default:
                    // AtomFeImage, the last primitive.
                    filterImage(run, p, out);
                    break;
            }

            run.results.mut(k) = FilterResult{out, p.linear};
            done.mut(k) = 1;
            stack.popBack();

            // The results and inputs this primitive was the last to read go.
            for (u32 i = 0; i < k; i++) {
                if (readsResult(p, (int)i) && --run.readers.mut(i) == 0) {
                    releasePlane(run.results.mut(i).plane);
                    releasePlane(run.linearCache.mut(i));
                    releasePlane(run.srgbCache.mut(i));
                }
            }

            if (readsResult(p, InputSourceAlpha) && --run.sourceAlphaReaders == 0) {
                releasePlane(run.sourceAlpha);
            }

            if (readsResult(p, InputSourceGraphic) && --run.sourceReaders == 0 && run.sourceLinear.data != run.source.data) {
                releasePlane(run.sourceLinear);
            }

            if (readsResult(p, InputBackgroundAlpha) && --run.backgroundAlphaReaders == 0) {
                releasePlane(run.backgroundAlpha);
            }

            if (readsResult(p, InputBackgroundImage)) {
                run.backgroundReaders--;
            }

            if ((readsResult(p, InputBackgroundImage) || readsResult(p, InputBackgroundAlpha)) && run.backgroundReaders == 0 && run.backgroundAlphaReaders == 0) {
                if (run.backgroundLinear.data != run.background.data) {
                    releasePlane(run.backgroundLinear);
                }

                releasePlane(run.background);
                run.backgroundLinear = run.empty;
            }
        }

        // The final result, back in sRGB, replaces the layer.
        const FilterResult& last = run.results.back();
        IntBox kept = boxInside(last.plane.box, layer.box);

        Conversion converted;

        memset(layer.data, 0, sizeof(float) * size * 4);

        for (int y = kept.y0; last.plane.data && y < kept.y1; y++) {
            if (!last.linear) {
                memcpy(planeAt(layer, kept.x0, y), planeAt(last.plane, kept.x0, y), sizeof(float) * 4 * (size_t)(kept.x1 - kept.x0));
                continue;
            }

            for (int x = kept.x0; x < kept.x1; x++) {
                convertPixel(planeAt(last.plane, x, y), planeAt(layer, x, y), false, converted);
            }
        }

        for (u32 k = 0; k < filter.count; k++) {
            releasePlane(run.results.mut(k).plane);
            releasePlane(run.linearCache.mut(k));
            releasePlane(run.srgbCache.mut(k));
        }

        if (run.sourceLinear.data != run.source.data) {
            releasePlane(run.sourceLinear);
        }

        releasePlane(run.sourceAlpha);

        // The last reader of the background released it.
        releasePlane(run.background);
        releasePlane(run.backgroundAlpha);

        for (int kind = 0; kind < 2; kind++) {
            for (int space = 0; space < 2; space++) {
                releasePlane(run.paint[kind][space]);
            }
        }
    }

    static void compositeLayer(Raster& r, const Lowering& l, const Op& op) {
        Raster sub;
        ObjPool pool;
        int lw = (int)op.layerWidth;
        int lh = (int)op.layerHeight;
        size_t size = (size_t)op.layerWidth * op.layerHeight;
        Plane content{(float*)pool.allocate(sizeof(float) * size * 4), IntBox{0, 0, lw, lh}};
        Matrix toDevice;

        // A layer's frame inverts.
        invert(op.layerMatrix, toDevice);
        sub.width = op.layerWidth;
        sub.height = op.layerHeight;

        if (l.backgroundInputs) {
            // The layer inherits the background under its footprint, resampled
            // from the pixels around it.
            IntBox canvas{0, 0, (int)r.width, (int)r.height};
            Box footprint = deviceBox(toDevice, Box{0.f, 0.f, (float)lw, (float)lh});
            Plane background = backgroundBuffer(r, pixelBox(Box{footprint.x0 - 2.f, footprint.y0 - 2.f, footprint.x1 + 2.f, footprint.y1 + 2.f}, canvas));

            if (background.data) {
                float* inherited = (float*)pool.allocate(sizeof(float) * size * 4);

                memset(inherited, 0, sizeof(float) * size * 4);

                for (u32 y = 0; y < sub.height; y++) {
                    for (u32 x = 0; x < sub.width; x++) {
                        Point point = apply(toDevice, Point{(float)x + .5f, (float)y + .5f});
                        float fx = point.x - .5f;
                        float fy = point.y - .5f;
                        int ix = (int)floorf(fx);
                        int iy = (int)floorf(fy);
                        float tx = fx - ix;
                        float ty = fy - iy;

                        for (int dy = 0; dy < 2; dy++) {
                            for (int dx = 0; dx < 2; dx++) {
                                const float* s = planeRead(background, ix + dx, iy + dy);
                                float weight = (dx ? tx : 1.f - tx) * (dy ? ty : 1.f - ty);

                                for (int c = 0; c < 4; c++) {
                                    inherited[((size_t)y * sub.width + x) * 4 + c] += s[c] * weight;
                                }
                            }
                        }
                    }
                }

                sub.backdrop = Plane{inherited, content.box};
            }

            releasePlane(background);
        }

        memset(content.data, 0, sizeof(float) * size * 4);
        runProgram(sub, l, l.programs.data() + op.first, op.count, content);

        // The device canvas and the taps around it, in layer pixels: the
        // part of the last result that is drawn.
        Box footprint = deviceBox(op.layerMatrix, Box{0.f, 0.f, (float)r.width, (float)r.height});
        IntBox visible = pixelBox(Box{footprint.x0 - 2.f, footprint.y0 - 2.f, footprint.x1 + 2.f, footprint.y1 + 2.f}, content.box);
        IntBox region{0, 0, lw, lh};

        for (const Filter* filter = op.filter; filter; filter = filter->next) {
            applyFilter(sub, l, *filter, content, pool, filter->next ? content.box : visible);
            region = filter->region;
        }

        // A filter's result is zero outside its region, so only the device
        // pixels whose bilinear taps reach the region of the last one can draw.
        Box reach = deviceBox(toDevice, Box{(float)region.x0 - 1.f, (float)region.y0 - 1.f, (float)region.x1 + 1.f, (float)region.y1 + 1.f});
        IntBox drawn{reach.x0 - 2.f > 0.f ? (int)(reach.x0 - 2.f) : 0, reach.y0 - 2.f > 0.f ? (int)(reach.y0 - 2.f) : 0, reach.x1 + 3.f < (float)r.width ? (int)(reach.x1 + 3.f) : (int)r.width, reach.y1 + 3.f < (float)r.height ? (int)(reach.y1 + 3.f) : (int)r.height};

        auto texel = [&](int x, int y, int c) {
            return x < 0 || y < 0 || x >= lw || y >= lh ? 0.f : content.data[((size_t)y * lw + x) * 4 + c];
        };
        // A layer shifted by whole pixels samples each pixel as it is: its
        // matrix passes all eight tests of a whole shift.
        const Matrix& lm = op.layerMatrix;
        int tests = (lm.a == 1.f) + (lm.b == 0.f) + (lm.c == 0.f) + (lm.d == 1.f) + (lm.e == floorf(lm.e)) + (lm.f == floorf(lm.f)) + (fabsf(lm.e) < 16777216.f) + (fabsf(lm.f) < 16777216.f);
        bool shifted = tests == 8;
        int shiftX = shifted ? (int)lm.e : 0;
        int shiftY = shifted ? (int)lm.f : 0;

        for (int y = drawn.y0; y < drawn.y1; y++) {
            for (int x = drawn.x0; x < drawn.x1; x++) {
                float sample[4];

                if (shifted) {
                    int ix = x + shiftX;
                    int iy = y + shiftY;

                    if (ix < 0 || iy < 0 || ix >= lw || iy >= lh) {
                        continue;
                    }

                    memcpy(sample, content.data + ((size_t)iy * lw + ix) * 4, sizeof(sample));
                } else {
                    Point p = apply(op.layerMatrix, Point{(float)x + 0.5f, (float)y + 0.5f});
                    float fx = p.x - 0.5f;
                    float fy = p.y - 0.5f;
                    float bx = floorf(fx);
                    float by = floorf(fy);

                    if (bx < -1.f || by < -1.f || bx >= (float)lw || by >= (float)lh) {
                        continue;
                    }

                    int ix = (int)bx;
                    int iy = (int)by;
                    float tx = fx - bx;
                    float ty = fy - by;

                    if (ix >= 0 && iy >= 0 && ix + 1 < lw && iy + 1 < lh) {
                        // The four taps are inside the layer.
                        const float* above = content.data + ((size_t)iy * lw + ix) * 4;
                        const float* below = above + (size_t)lw * 4;

                        for (int c = 0; c < 4; c++) {
                            float top = above[c] * (1.f - tx) + above[4 + c] * tx;
                            float bottom = below[c] * (1.f - tx) + below[4 + c] * tx;

                            sample[c] = top * (1.f - ty) + bottom * ty;
                        }
                    } else {
                        for (int c = 0; c < 4; c++) {
                            float top = texel(ix, iy, c) * (1.f - tx) + texel(ix + 1, iy, c) * tx;
                            float bottom = texel(ix, iy + 1, c) * (1.f - tx) + texel(ix + 1, iy + 1, c) * tx;

                            sample[c] = top * (1.f - ty) + bottom * ty;
                        }
                    }
                }

                if (sample[3] <= 0.f) {
                    continue;
                }

                float keep = 1.f - sample[3];

                for (size_t level = firstTarget(r); level < r.layers.length(); level++) {
                    const Plane& plane = r.layers[level];

                    if (x < plane.box.x0 || x >= plane.box.x1 || y < plane.box.y0 || y >= plane.box.y1) {
                        continue;
                    }

                    float* d = planeAt(plane, x, y);

                    for (int c = 0; c < 4; c++) {
                        d[c] = sample[c] + d[c] * keep;
                    }
                }
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base) {
        size_t depth = r.layers.length();
        IntBox canvas{0, 0, (int)r.width, (int)r.height};
        size_t backgroundBase = r.backgroundBase;
        Plane backdrop = r.backdrop;

        r.backgroundBase = depth;

        if (depth) {
            r.backdrop = Plane{nullptr, IntBox{0, 0, 0, 0}};
        }

        r.layers.pushBack(base);
        r.dirty.pushBack(base.box);
        r.begins.pushBack(nullptr);

        for (u32 k = 0; k < count; k++) {
            const Op& op = ops[k];

            switch (op.kind) {
                case OpKind::Fill:
                    fillOp(r, l, op);
                    break;
                case OpKind::Layer:
                    for (size_t level = firstTarget(r); level < r.layers.length(); level++) {
                        ensure(r.layers[level], r.dirty.mut(level), layerBox(op.bounds, canvas));
                    }

                    compositeLayer(r, l, op);
                    break;
                case OpKind::Begin: {
                    // The layer holds the pixels its group was lowered to draw.
                    ObjPool* pool = new ObjPool();
                    IntBox box = layerBox(op.bounds, canvas);
                    size_t area = (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0);
                    Plane layer{area ? (float*)pool->allocate(sizeof(float) * area * 4) : nullptr, box};

                    r.layers.pushBack(layer);
                    r.dirty.pushBack(IntBox{0, 0, 0, 0});
                    r.pools.pushBack(pool);
                    r.begins.pushBack(&op);
                } break;
                default: {
                    // OpKind::End.
                    Plane layer = r.layers.popBack();
                    ObjPool* pool = r.pools.popBack();
                    const Op* begin = r.begins.popBack();
                    IntBox box = r.dirty.popBack();

                    // A background is read in place; a group of no pixels
                    // draws nothing.
                    if (begin->backgroundOnly || !layer.data) {
                        delete pool;
                        break;
                    }

                    if (begin->filter) {
                        ensure(layer, box, layer.box);
                    }

                    for (const Filter* filter = begin->filter; filter; filter = filter->next) {
                        applyFilter(r, l, *filter, layer, *pool, canvas, begin->newBackground);
                    }

                    const float* clip = begin->clipped ? clipBuffer(r, l, *begin, *pool, layer.box) : nullptr;
                    const float* mask = begin->masked ? maskBuffer(r, l, *begin, *pool, layer.box) : nullptr;
                    int width = planeWidth(layer);

                    for (size_t level = firstTarget(r); level < r.layers.length(); level++) {
                        const Plane& parent = r.layers[level];
                        IntBox target = boxInside(box, parent.box);

                        ensure(parent, r.dirty.mut(level), target);

                        for (int y = target.y0; y < target.y1; y++) {
                            for (int x = target.x0; x < target.x1; x++) {
                                size_t i = (size_t)(y - layer.box.y0) * width + (size_t)(x - layer.box.x0);
                                float f = begin->opacity * (clip ? clip[i] : 1.f) * (mask ? mask[i] : 1.f);
                                const float* s = planeAt(layer, x, y);

                                if (f <= 0.f || s[3] <= 0.f) {
                                    continue;
                                }

                                float* d = planeAt(parent, x, y);

                                if (begin->blend && d[3] > 0.f) {
                                    float source[4] = {s[0] * f, s[1] * f, s[2] * f, s[3] * f};
                                    float blended[4];

                                    blendPixel(begin->blend, source, d, blended);
                                    memcpy(d, blended, sizeof(blended));
                                    continue;
                                }

                                float keep = 1.f - s[3] * f;

                                d[0] = s[0] * f + d[0] * keep;
                                d[1] = s[1] * f + d[1] * keep;
                                d[2] = s[2] * f + d[2] * keep;
                                d[3] = s[3] * f + d[3] * keep;
                            }
                        }
                    }

                    delete pool;
                    break;
                }
            }
        }

        // A program ends every group it begins.
        r.layers.popBack();
        r.dirty.popBack();
        r.begins.popBack();
        r.backgroundBase = backgroundBase;
        r.backdrop = backdrop;
    }

    static u8 clampByte(float v) {
        v = v > 0.f ? v : 0.f;
        v = v > 255.f ? 255.f : v;

        return (u8)(int)v;
    }

    static bool usesBackground(const Document& doc, const Node* node, u32 depth) {
        if (depth > 256) {
            return false;
        }

        const u32 names[] = {AtomIn, AtomIn2};

        for (u32 name : names) {
            u32 value = attribute(node, name);
            StringView text = value ? trim(textOf(doc, value)) : StringView();

            if (text == SV("BackgroundImage") || text == SV("BackgroundAlpha")) {
                return true;
            }
        }

        for (const Node* child = node->first; child; child = child->next) {
            if (usesBackground(doc, child, depth + 1)) {
                return true;
            }
        }

        return false;
    }

    static void loadDocument(Document& doc, ObjPool& pool, StringView source, SvgFail fail) {
        doc.pool = &pool;
        doc.root = nullptr;
        doc.nodeCount = 0;
        initAtoms(doc.atoms);
        parseXml(doc, source, fail);
    }

    // A pass over a cascaded document, out of a pool of its own.
    static void beginPass(Pass& p, Document& doc, ObjPool& pool, SvgDecode decode, SvgFail fail, void* context) {
        p.doc = &doc;
        p.pool = &pool;
        p.decode = decode;
        p.fail = fail;
        p.context = context;
        p.imageDepth = 0;
        p.rootFontSize = 16.f;
        p.hasContext = false;
        p.instance = nullptr;
        p.documentStyles.zero(doc.nodeCount);
    }

    // The size of a root without a viewBox, for the sides it leaves open:
    // the right and bottom edges of what it draws, with its markers, in its
    // user units, under its transform and, for a root with no side of its
    // own, its zoom. An open side is zero while it is measured, so a
    // percentage along it is zero; a root that draws nothing past the origin
    // stays zero.
    static void measureRoot(Pass& p, float& width, float& height) {
        Node* root = p.doc->root;
        Viewport vp{width, height, width, height, 16.f};
        ComputedStyle style = computeStyle(p, root, initialStyle(), vp);
        Box box = emptyBox();

        p.rootFontSize = style.fontSize;
        vp.rootFontSize = style.fontSize;

        float zoom = width <= 0.f && height <= 0.f ? style.zoom : 1.f;
        Matrix m = multiply(boxTransform(p, root, initialStyle(), vp), scaling(zoom, zoom));

        for (Node* child = root->first; child; child = child->next) {
            childExtent(p, child, m, style, vp, Extent{true, true}, 0, box);
        }

        if (!boxValid(box)) {
            return;
        }

        if (width <= 0.f) {
            width = maxf(box.x1, 0.f);
        }

        if (height <= 0.f) {
            height = maxf(box.y1, 0.f);
        }
    }

    // Holds the pool of an object not made yet: a fail that throws on the
    // way to making it deletes the pool.
    struct PoolGuard {
        ObjPool* pool;

        ~PoolGuard();
    };

    PoolGuard::~PoolGuard() {
        delete pool;
    }
}

DecodedImage::DecodedImage(SvgImage* image_)
    : image(image_)
{
}

DecodedImage::~DecodedImage() {
    svg_image_destroy(image);
}

RenderedImage::RenderedImage(ObjPool* pool_, const u8* pixels_, u32 width_, u32 height_)
    : image{&renderedVtable}
    , pool(pool_)
    , pixels(pixels_)
    , width(width_)
    , height(height_)
{
}

void svg_image_destroy(SvgImage* image) {
    image->vtable->destroy(image);
}

const void* svg_image_data(const SvgImage* image) {
    return image->vtable->data(image);
}

size_t svg_image_length(const SvgImage* image) {
    return image->vtable->length(image);
}

uint32_t svg_image_width(const SvgImage* image) {
    return image->vtable->width(image);
}

uint32_t svg_image_height(const SvgImage* image) {
    return image->vtable->height(image);
}

SvgLibrary* svg_library_create(SvgDecode decode, SvgFail fail, void* context) {
    ObjPool* pool = new ObjPool();

    return pool->make<SvgLibrary>(pool, decode, fail, context);
}

void svg_library_destroy(SvgLibrary* library) {
    delete library->pool;
}

void svg_document_destroy(SvgDocument* document) {
    delete document->pool;
}

float svg_document_width(const SvgDocument* document) {
    return document->width;
}

float svg_document_height(const SvgDocument* document) {
    return document->height;
}

SvgImage* svg_document_render(SvgDocument* document, uint32_t width, uint32_t height) {
    if (width == 0 || height == 0 || (u64)width * height > (1u << 26)) {
        fail(document->fail, SVG_RENDER_SIZE);
    }

    ObjPool scratch;
    Document& doc = *document->doc;
    size_t size = (size_t)width * height;
    float* base = (float*)scratch.allocate(sizeof(float) * size * 4);

    memset(base, 0, sizeof(float) * size * 4);

    {
        Lowering l;

        beginPass(l, doc, scratch, document->decode, document->fail, document->context);
        l.width = width;
        l.height = height;
        l.docWidth = document->width;
        l.docHeight = document->height;
        l.depth = 0;
        l.out = &l.ops;
        l.backgroundInputs = usesBackground(doc, doc.root);
        l.sized = nullptr;
        l.contextMatrix = identity();
        l.contextBox = emptyBox();
        l.screen = identity();
        l.host = identity();
        lowerElement(l, doc.root, initialStyle(), identity(), Viewport{(float)width, (float)height, (float)width, (float)height, 16.f});

        Raster raster;

        raster.width = width;
        raster.height = height;
        runProgram(raster, l, l.ops.data(), (u32)l.ops.length(), Plane{base, IntBox{0, 0, (int)width, (int)height}});
    }

    ObjPool* pool = new ObjPool();
    u8* pixels = (u8*)pool->allocate(size * 4);

    for (size_t i = 0; i < size; i++) {
        const float* s = base + i * 4;
        u8* d = pixels + i * 4;
        float a = s[3];
        float divisor = a > 0.f ? a : 1.f;
        float keep = a > 0.f ? 1.f : 0.f;

        for (int c = 0; c < 3; c++) {
            d[c] = clampByte((s[c] / divisor * keep) * 255.f + 0.5f);
        }

        d[3] = clampByte(a * keep * 255.f + 0.5f);
    }

    return &pool->make<RenderedImage>(pool, pixels, width, height)->image;
}

SvgDocument* svg_library_parse(SvgLibrary* library, const void* file, size_t length) {
    PoolGuard guard{new ObjPool()};
    ObjPool& pool = *guard.pool;
    Document* doc = pool.make<Document>();

    // The file, kept with the document that refers into it.
    loadDocument(*doc, pool, keep(pool, file, length), library->fail);

    if (!doc->root || doc->root->name != AtomSvg) {
        fail(library->fail, SVG_NOT_SVG);
    }

    cascade(*doc);

    float width;
    float height;
    Box viewBox;

    givenSize(*doc, doc->root, width, height);

    // A root without a viewBox that gives no width or height is as large
    // as what it draws.
    if ((width <= 0.f || height <= 0.f) && !parseViewBox(*doc, doc->root, viewBox)) {
        ObjPool scratch;
        Pass pass;

        beginPass(pass, *doc, scratch, library->decode, library->fail, library->context);
        measureRoot(pass, width, height);
    }

    SvgDocument* document = pool.make<SvgDocument>(&pool, library->decode, library->fail, library->context, doc, width, height);

    guard.pool = nullptr;

    return document;
}
