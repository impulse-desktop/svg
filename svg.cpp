#include "svg.h"

#include <math.h>
#include <string.h>

// The pixel kernels take 16-bit lanes eight at a time and multiply 32-bit
// ones, which x86-64 does from SSE4.1 on; it asks for x86-64-v2, which every
// Intel processor since 2008 and AMD one since 2011 has.
#if defined(__x86_64__) && !defined(__SSE4_2__)
    #error "svg.cpp needs SSE4.2 on x86-64: build it with -march=x86-64-v2 or later"
#endif

namespace {
    using u8 = unsigned char;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using i32 = int32_t;
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
    ATOM(Text, "text")                                             \
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
    ATOM(PathLengthProperty, "path-length")                        \
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
    ATOM(KernelUnitLength, "kernelUnitLength")                     \
    ATOM(NoComposite, "no-composite")                              \
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
    ATOM(Mask, "mask")                                             \
    ATOM(MaskBorder, "mask-border")                                \
    ATOM(Media, "media")                                           \
    ATOM(Title, "title")                                           \
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
    ATOM(Clip, "clip")                                             \
    ATOM(FontSize, "font-size")                                    \
    ATOM(MaskType, "mask-type")                                    \
    ATOM(MaskImage, "mask-image")                                  \
    ATOM(MaskMode, "mask-mode")                                    \
    ATOM(MaskRepeat, "mask-repeat")                                \
    ATOM(MaskPosition, "mask-position")                            \
    ATOM(MaskClip, "mask-clip")                                    \
    ATOM(MaskOrigin, "mask-origin")                                \
    ATOM(MaskSize, "mask-size")                                    \
    ATOM(MaskComposite, "mask-composite")                          \
    ATOM(MaskBorderSource, "mask-border-source")                   \
    ATOM(MaskBorderMode, "mask-border-mode")                       \
    ATOM(MaskBorderSlice, "mask-border-slice")                     \
    ATOM(MaskBorderWidth, "mask-border-width")                     \
    ATOM(MaskBorderOutset, "mask-border-outset")                   \
    ATOM(MaskBorderRepeat, "mask-border-repeat")                   \
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

    struct DPoint {
        double x;
        double y;
    };

    struct Matrix {
        float a;
        float b;
        float c;
        float d;
        float e;
        float f;
    };

    // A 3D transform seen on the plane z = 0 in perspective: a point goes to
    // (m[0] x + m[1] y + m[2], m[3] x + m[4] y + m[5]) over the weight
    // m[6] x + m[7] y + m[8], and of a weight of no sign it is behind the
    // eye. on when the transform is such, past an affine one.
    struct Projective {
        float m[9];
        bool on;
        // Made by a 3D transform function, affine or not.
        bool solid;
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

    // A geometry property's value a node's declarations give it, the
    // CSS-wide keywords resolved, none for the initial one.
    struct GeometryValue {
        u32 node;
        u32 property;
        u32 value;
    };

    struct Document {
        ObjPool* pool;
        Interner atoms;
        Node* root;
        u32 nodeCount;
        Vector<Node*> byId;
        Rule* rules;
        u32 ruleCount;
        bool structural;
        // The geometry values of declarations, by node in document order.
        Vector<GeometryValue> geometry;
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

    // What a stroke does at a point between two segments of its polyline:
    // at a corner it takes the join; at a cusp, where a curve turns back
    // with no speed, nothing, its normal one line either way; at a smooth
    // point, inside a curve, it sweeps the normal round.
    enum PointKind : u8 {
        PointCorner,
        PointCusp,
        PointSmooth,
    };

    // A cubic of a dashed stroke, its controls.
    struct FlatCurve {
        Point p[4];
    };

    struct Flat {
        Vector<Point> points;
        Vector<u32> ends;
        Vector<u8> closed;
        // For a stroke, per point: its kind, and the direction of the
        // segment that comes to it, measured on the curve rather than from
        // the rounded points, none where it has no length.
        Vector<u8> kinds;
        Vector<Point> dirs;
        // For a dashed stroke, per point: the cubic it lies on, an index into
        // curves or none, and its parameter there, so that dashes are
        // measured and cut on the cubic, not on its chords.
        bool measured = false;
        Vector<u32> curveOf;
        Vector<double> paramOf;
        Vector<FlatCurve> curves;
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
        JoinMiterClip,
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
        Rect,
        Xywh,
        Polygon,
        Path,
        // shape(): its commands.
        Commands,
    };

    enum class GeometryBox : u8 {
        Fill,
        Stroke,
        // Midway from the fill box to the stroke box, as half-border-box
        // is from the padding box to the border box they stand for.
        HalfBorder,
        View,
    };

    // A basic shape: an inset, rect() and xywh() keep their four values in
    // the order they are written, rect() its auto sides as bits, and the
    // radii of their corners, horizontal from the top left round, then
    // vertical; path() its data; shape() its start and commands.
    struct ShapePoint;
    struct ShapeCommand;

    struct BasicShape {
        ShapeKind kind;
        GeometryBox box;
        u8 rule;
        u8 autos;
        Length values[4];
        Length radii[8];
        Length* points;
        u32 pointCount;
        StringView data;
        ShapeCommand* commands;
        u32 commandCount;
    };

    // A point of shape(): offsets over the reference box, from its top
    // left, the command's start or its end.
    enum class ShapeAnchor : u8 {
        Box,
        Start,
        End,
    };

    struct ShapePoint {
        Length x;
        Length y;
        ShapeAnchor anchor;
    };

    // A command of shape(), as the path data command it stands for: M, L,
    // H, V, C (one control a quadratic, two a cubic), S (none a smooth
    // quadratic, one a smooth cubic), A or Z; the first's point is the
    // start. hline and vline keep their one offset in x and y.
    struct ShapeCommand {
        u8 kind;
        u8 controls;
        u8 radii;
        bool sweep;
        bool large;
        float angle;
        ShapePoint end;
        ShapePoint control[2];
        Length radius[2];
    };

    // The longhands of mask in the order of their atoms.
    enum MaskPart : u32 {
        MaskPartImage,
        MaskPartMode,
        MaskPartRepeat,
        MaskPartPosition,
        MaskPartClip,
        MaskPartOrigin,
        MaskPartSize,
        MaskPartComposite,
        MaskParts,
    };

    enum MaskBorderPart : u32 {
        MaskBorderPartSource,
        MaskBorderPartMode,
        MaskBorderPartSlice,
        MaskBorderPartWidth,
        MaskBorderPartOutset,
        MaskBorderPartRepeat,
        MaskBorderParts,
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
        // The clip property's rect(), as the four sides of a rect().
        BasicShape* clipRect;
        // The values of mask-image and the other longhands of mask, in
        // their order, none for the initial one; of mask-border's the same.
        u32 maskParts[MaskParts];
        u32 maskBorder[MaskBorderParts];
        u32 markers[3];
        bool nonScaling;
        // vector-effect's constrained transformations, as VectorNonScaling
        // and the bits after it.
        u8 vectorEffects;
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
        // visibility: force-hidden here or on an ancestor, which hides
        // whatever the descendants' own visibility.
        bool forceHidden;
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
        // A conic gradient: its centre, the turn it starts at in x1's
        // degrees, the turns its stops span from fr to r.
        Conic,
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
        // The colour stored, not premultiplied.
        u16 stored[4];
    };

    struct IntBox {
        int x0;
        int y0;
        int x1;
        int y1;
    };

    struct CachedImage;

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
        u16* tile;
        float opacity;
        bool linear;
        // A solid colour stored premultiplied; the opacity of a pattern stored.
        u16 color16[4];
        u16 opacity16;
        // The colours of a gradient at evenly spaced t, stored premultiplied,
        // and the steps between entries that hold a stop.
        u16* table;
        u8* kinks;
        u32 tableSize;
        // Of a radial gradient whose focal point lies on its end circle, the
        // colour past the focal point, stored: its last stop's padded, else
        // the average of its colours (pservers.md, RadialGradientNotes).
        bool beyond;
        u16 beyond16[4];
        // The device bounds of the fill a pattern paints and whether a filter
        // samples it everywhere; the tile pixels its tile is drawn over,
        // unwrapped.
        Box sampled;
        bool sampledAll;
        IntBox window;
        // The image a raster paint samples and the halvings it takes along
        // each side, its pixels read when a fill first draws it; the pixels
        // it shrank them to.
        CachedImage* source;
        u32 kx;
        u32 ky;
        u8* shrunk;
        // Whether a pattern or raster paint is filled outside patterns'
        // content, so drawn once: its tile or pixels go after the fill.
        bool once;
        // A raster of a CSS image samples the four texels about a point, as
        // browsers draw one, round the sides it repeats along: bit 1
        // across, 2 down.
        bool tent;
        u8 wraps;
        // The texels a raster paint reads, shrunk: the image's, or a part's
        // of a border image.
        IntBox texels;
    };

    // The paints whose tiles or shrunk pixels a render holds, let go with it.
    struct HeldPaints {
        Vector<DevicePaint*> paints;

        ~HeldPaints();
    };

    struct ClipShape {
        u32 first;
        u32 count;
        u8 rule;
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
        // The subregion in pixels, within the filter region, and the pixels
        // it touches: the two cut in fractions first, then rounded.
        Box clip;
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

    // A mask layer as drawn: the program of its image, the clip of its
    // painting area, its type, bit 0 alpha and bit 1 linear colours, and
    // its compositing over the layers below, as MaskLayer has it.
    struct MaskLayerOp {
        u32 first;
        u32 count;
        u32 region;
        u8 type;
        u8 composite;
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
        // Of a fill, laid over in linear light, as its shape's own
        // color-interpolation has it (painting.md, ColorInterpolation); of a
        // group, its layer so composited, a shape's or an image's.
        bool linear;
        const MaskLayerOp* maskLayers;
        u32 maskLayerCount;
        Filter* filter;
        u32 layerWidth;
        u32 layerHeight;
        Matrix layerMatrix;
        // Of a layer seen in perspective, the maps from the device to the
        // layer and back, which layerMatrix stands for otherwise.
        const Projective* perspective;
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
    struct DecodedImage;
    struct CachedImage;

    struct ImageSource {
        Document* nested;
        SvgImage* raster;
        // What destroys the raster with the pool, unless it is released
        // first, and the pass's entry of the raster.
        DecodedImage* decoded;
        CachedImage* cached;
        float width;
        float height;
        Box viewBox;
        Node* ratio;
        // The transform an svgView() fragment adds after the viewBox.
        Matrix transform;
    };

    // A raster a pass decoded for an element or a mask image's url, so
    // that the element drawn again, as a pattern's content is for each fill
    // it paints, decodes once: the source and its size, and the paints that
    // sample it yet to be let go, after the last of which the raster goes. A
    // paint in patterns' content is drawn again for each piece of a tile and
    // never lets go, so its raster stays; any other is drawn once.
    struct CachedImage {
        const void* key;
        ImageSource source;
        u32 width;
        u32 height;
        u32 paintsLeft;
    };

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
        Vector<CachedImage*> images;
        // Whether the pass lowers to draw, so that its paints sample the images it decodes.
        bool draws;
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
        // An element drawn on its layer of a transform in perspective: its
        // own transform is the layer's.
        Node* flattened;
        Matrix contextMatrix;
        Box contextBox;
        // How deep in patterns' content the lowering is: that content is
        // drawn again for each piece of a tile.
        u32 patternDepth;
        mutable HeldPaints held;
    };

    // A buffer of float RGBA pixels over a box of the raster, zero at first:
    // a pixel outside the box reads as zero and is never written.
    // Premultiplied RGBA over a box, 16 bits a channel, 0 to 65535 for 0 to
    // 1: a pixel is read into floats and stored back rounded.
    struct Plane {
        u16* data;
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
        // The coverage deltas of a fill's rows, 65536 for a whole pixel,
        // summed modulo 2^32.
        Vector<u32> accumulation;
        Plane backdrop{nullptr, IntBox{0, 0, 0, 0}};
        size_t backgroundBase = 0;
        // The sRGB curve each way over stored values, to sRGB and to linear
        // light, and which values of it a render has met.
        Vector<u16> curves[2];
        Vector<u8> curvesKnown[2];
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

    // A stored channel is 16 bits, 0 to 65535 for 0 to 1. The helpers go
    // over spans of pixels, four lanes a pixel, so that clang vectorizes
    // them eight lanes a step.
    const u16 zeroStored[4] = {0, 0, 0, 0};

    // x / 65535 rounded to nearest, for x up to 65535 * 65535.
    static u32 div65535(u32 x) {
        x += 32768;

        return (x + (x >> 16)) >> 16;
    }

    // A sum of two channels saturated at 65535; it passes only for a colour
    // over its alpha, which an arithmetic composite may keep.
    static u16 saturate(u32 v) {
        return (u16)(v | (0u - (v >> 16)));
    }

    static u32 clampStored(i64 v) {
        return v < 0 ? 0u : (v > 65535 ? 65535u : (u32)v);
    }

    // Pixels read into floats: 65535 is 1 exactly.
    static void loadSpan(const u16* s, float* d, size_t count) {
        constexpr float unit = 1.f / 65535.f;

        for (size_t i = 0; i < count * 4; i++) {
            d[i] = (float)s[i] * unit;
        }
    }

    // Pixels stored rounded, each channel clamped to 0 to 1, a value that is
    // not a number as 0.
    static void storeSpan(u16* d, const float* s, size_t count) {
        for (size_t i = 0; i < count * 4; i++) {
            d[i] = (u16)(int)(minf(maxf(s[i], 0.f), 1.f) * 65535.f + 0.5f);
        }
    }

    static void loadPixel(const u16* p, float* c) {
        loadSpan(p, c, 1);
    }

    // A unit value stored: clamped to 0 to 1, a value that is not a number as 0.
    static u32 toUnit16(float v) {
        return (u32)(minf(maxf(v, 0.f), 1.f) * 65535.f + 0.5f);
    }

    // A colour stored, each channel as a unit value.
    // A stored pixel not premultiplied: each colour channel, clamped to the
    // alpha, over it through one reciprocal. Any alpha but none keeps its
    // colour.
    static void unpremultiplyStored(const u16* s, u32* c) {
        u32 a = s[3];
        float inverse = a ? 65535.f / (float)a : 0.f;

        for (int k = 0; k < 3; k++) {
            u32 v = s[k] < a ? s[k] : a;

            c[k] = (u32)((float)v * inverse + 0.5f);
        }

        c[3] = a;
    }

    static void storeColor(u16* out, Color c) {
        float f[4] = {c.r, c.g, c.b, c.a};

        storeSpan(out, f, 1);
    }

    // The blend of two stored channels at f, 65535 being all of b.
    static u16 mix16(u32 a, u32 b, u32 f) {
        return (u16)div65535(a * (65535u - f) + b * f);
    }

    static void storePixel(u16* p, const float* c) {
        storeSpan(p, c, 1);
    }

    // The span helpers write a buffer none of their inputs overlaps.

    // The source pixels over the destination: each lane of the destination
    // is kept by the complement of its pixel's alpha. Two pixels, eight
    // lanes, a step, each pixel's alpha spread over its four lanes.
    static void overSpan(u16* __restrict d, const u16* __restrict s, size_t count) {
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            const u16* sp = s + p * 4;
            u16* dp = d + p * 4;

            for (int i = 0; i < 8; i++) {
                dp[i] = saturate(sp[i] + div65535((u32)dp[i] * (65535u - sp[(i & 4) | 3])));
            }
        }

        for (; p < count; p++) {
            const u16* sp = s + p * 4;
            u16* dp = d + p * 4;

            for (int i = 0; i < 4; i++) {
                dp[i] = saturate(sp[i] + div65535((u32)dp[i] * (65535u - sp[3])));
            }
        }
    }

    // A colour scaled by the factor of each pixel, 65535 for 1.
    static void solidSpan(u16* __restrict d, const u16* __restrict color, const u16* __restrict factor, size_t count) {
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            for (int i = 0; i < 8; i++) {
                d[p * 4 + i] = (u16)div65535((u32)color[i & 3] * factor[p + (i >> 2)]);
            }
        }

        for (; p < count; p++) {
            for (int i = 0; i < 4; i++) {
                d[p * 4 + i] = (u16)div65535((u32)color[i] * factor[p]);
            }
        }
    }

    // The pixels scaled each by its factor, 65535 for 1.
    static void scaleSpan(u16* __restrict d, const u16* __restrict s, const u16* __restrict factor, size_t count) {
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            for (int i = 0; i < 8; i++) {
                d[p * 4 + i] = (u16)div65535((u32)s[p * 4 + i] * factor[p + (i >> 2)]);
            }
        }

        for (; p < count; p++) {
            for (int i = 0; i < 4; i++) {
                d[p * 4 + i] = (u16)div65535((u32)s[p * 4 + i] * factor[p]);
            }
        }
    }

    // Blends of two pixels at a factor each, 65535 for all of the second.
    static void mixSpan(u16* __restrict d, const u16* __restrict a, const u16* __restrict b, const u16* __restrict factor, size_t count) {
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            for (int i = 0; i < 8; i++) {
                u32 f = factor[p + (i >> 2)];

                d[p * 4 + i] = (u16)div65535((u32)a[p * 4 + i] * (65535u - f) + (u32)b[p * 4 + i] * f);
            }
        }

        for (; p < count; p++) {
            for (int i = 0; i < 4; i++) {
                u32 f = factor[p];

                d[p * 4 + i] = (u16)div65535((u32)a[p * 4 + i] * (65535u - f) + (u32)b[p * 4 + i] * f);
            }
        }
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

    // SVG 2 makes a CTM in doubles and keeps it in singles.
    static Matrix multiply(const Matrix& m, const Matrix& n) {
        return Matrix{
            (float)((double)m.a * n.a + (double)m.c * n.b),
            (float)((double)m.b * n.a + (double)m.d * n.b),
            (float)((double)m.a * n.c + (double)m.c * n.d),
            (float)((double)m.b * n.c + (double)m.d * n.d),
            (float)((double)m.a * n.e + (double)m.c * n.f + m.e),
            (float)((double)m.b * n.e + (double)m.d * n.f + m.f),
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

    static Projective projectiveOf(const Matrix& m) {
        return Projective{{m.a, m.c, m.e, m.b, m.d, m.f, 0.f, 0.f, 1.f}, true, false};
    }

    static Projective compose(const Projective& p, const Projective& q) {
        Projective out{{}, true, false};

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                out.m[r * 3 + c] = (float)((double)p.m[r * 3] * q.m[c] + (double)p.m[r * 3 + 1] * q.m[3 + c] + (double)p.m[r * 3 + 2] * q.m[6 + c]);
            }
        }

        return out;
    }

    // The inverse, by the adjugate over the determinant, which keeps the
    // weight's sign: a point in front of the eye maps back to one.
    static bool invertProjective(const Projective& p, Projective& out) {
        const float* m = p.m;
        float adjugate[9] = {m[4] * m[8] - m[5] * m[7], m[2] * m[7] - m[1] * m[8], m[1] * m[5] - m[2] * m[4], m[5] * m[6] - m[3] * m[8], m[0] * m[8] - m[2] * m[6], m[2] * m[3] - m[0] * m[5], m[3] * m[7] - m[4] * m[6], m[1] * m[6] - m[0] * m[7], m[0] * m[4] - m[1] * m[3]};
        float determinant = m[0] * adjugate[0] + m[1] * adjugate[3] + m[2] * adjugate[6];

        if (!(fabsf(determinant) > 0.f) || !isfinite(1.f / determinant)) {
            return false;
        }

        out.on = true;

        for (int k = 0; k < 9; k++) {
            out.m[k] = adjugate[k] / determinant;
        }

        return true;
    }

    // Where a point goes, false behind the eye.
    static bool projectPoint(const Projective& p, Point in, Point& out) {
        const float* m = p.m;
        float w = m[6] * in.x + m[7] * in.y + m[8];

        if (!(w > 0.f)) {
            return false;
        }

        out = Point{(m[0] * in.x + m[1] * in.y + m[2]) / w, (m[3] * in.x + m[4] * in.y + m[5]) / w};

        return true;
    }

    // The box a box goes to, all of its corners in front of the eye; else
    // the fallback.
    static Box projectBox(const Projective& p, const Box& box, const Box& fallback) {
        Point corners[4] = {{box.x0, box.y0}, {box.x1, box.y0}, {box.x0, box.y1}, {box.x1, box.y1}};
        Box out{3e38f, 3e38f, -3e38f, -3e38f};

        for (Point corner : corners) {
            Point q;

            if (!projectPoint(p, corner, q)) {
                return fallback;
            }

            out = Box{minf(out.x0, q.x), minf(out.y0, q.y), maxf(out.x1, q.x), maxf(out.y1, q.y)};
        }

        return out;
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

    static Box meetBoxes(const Box& a, const Box& b) {
        return Box{maxf(a.x0, b.x0), maxf(a.y0, b.y0), minf(a.x1, b.x1), minf(a.y1, b.y1)};
    }

    static Box joinBoxes(const Box& a, const Box& b) {
        return Box{minf(a.x0, b.x0), minf(a.y0, b.y0), maxf(a.x1, b.x1), maxf(a.y1, b.y1)};
    }

    static Box allBox() {
        return Box{-INFINITY, -INFINITY, INFINITY, INFINITY};
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

    static bool startsWith(StringView text, StringView prefix) {
        return text.length() >= prefix.length() && StringView(text.begin(), text.begin() + prefix.length()) == prefix;
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
    // The grammar a transform list is read in: an attribute's, SVG's; a
    // declaration's, CSS's; or either, for a value one of them took.
    enum class TransformGrammar : u8 {
        Attribute,
        Declaration,
        Either,
    };

    static bool parseTransform(StringView text, Matrix& out, TransformGrammar grammar, Projective* perspective = nullptr);
    static bool parseNumberText(StringView text, float& out);

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

                // A transform attribute takes SVG's grammar: CSS's functions
                // and units only a declaration takes. One past it is as if
                // it were not there, as CSS Transforms 1 defines it. So is
                // a pathLength but a number of px.
                Matrix parsed;
                float number;

                if ((atom == AtomTransform || atom == AtomGradientTransform || atom == AtomPatternTransform) && !parseTransform(textOf(*xml.doc, value), parsed, TransformGrammar::Attribute)) {
                    continue;
                }

                if (atom == AtomPathLength && !parseNumberText(textOf(*xml.doc, value), number)) {
                    continue;
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
            case AtomPathLengthProperty:
                return true;
            default:
                return false;
        }
    }

    static bool isShape(u32 name);

    static bool geometryApplies(u32 element, u32 property) {
        switch (property) {
            case AtomX:
            case AtomY:
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
            case AtomPathLengthProperty:
                return isShape(element);
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

            // A function left open closes at the end, as CSS reads it.
            if (!comma || depth > 32) {
                return false;
            }

            if (!substituteFallbacks(trim(StringView(comma + 1, q)), out, depth + 1)) {
                return false;
            }

            p = q < text.end() ? q + 1 : q;
        }

        return true;
    }

    static bool validGeometry(u32 property, StringView value);

    static bool parseLength(StringView text, Length& out);
    static bool parsePosition(StringView text, Length& x, Length& y);

    // Where a CSS mask layer takes its image from.
    enum class MaskSource : u8 {
        None,
        Element,
        Image,
        Gradient,
    };

    // A CSS mask layer: its image, mask-image's url target or gradient, and
    // the other longhands' values for it: mode 0 match-source, 1 alpha, 2
    // luminance; repeat by axis 0 repeat, 1 space, 2 round, 3 no-repeat;
    // the position by axis, from the far side where told; size 0 as given,
    // 1 cover, 2 contain, an extent of auto where told; the boxes, clip 255
    // for no-clip; composite 0 add, 1 subtract, 2 intersect, 3 exclude.
    struct MaskLayer {
        MaskSource source;
        StringView url;
        StringView image;
        u8 mode;
        u8 repeat[2];
        Length position[2];
        bool far[2];
        u8 size;
        Length extent[2];
        bool fit[2];
        GeometryBox origin;
        u8 clip;
        u8 composite;
    };

    // A word of a mask value: a function whole with its arguments, a slash
    // alone, or a run past spaces.
    static StringView maskWord(const u8*& p, const u8* end) {
        skipSpaces(p, end);

        const u8* start = p;
        int depth = 0;

        if (at(p, end, '/')) {
            p++;

            return StringView(start, p);
        }

        while (p < end && (depth > 0 || (!isSpace(*p) && *p != '/'))) {
            if (*p == '"' || *p == '\'') {
                p = skipCssString(p, end);
                continue;
            }

            depth += *p == '(';
            depth -= *p == ')' && depth > 0;
            p++;
        }

        return StringView(start, p);
    }

    // The index of a word among keywords, -1 for none.
    static int maskKeyword(StringView word, const char* const* words, int count) {
        for (int k = 0; k < count; k++) {
            if (equalsIgnoreCase(word, StringView(words[k]))) {
                return k;
            }
        }

        return -1;
    }

    const char* const maskModes[] = {"match-source", "alpha", "luminance"};
    const char* const maskPlaces[] = {"left", "right", "top", "bottom", "center"};
    const char* const maskComposites[] = {"add", "subtract", "intersect", "exclude"};
    const char* const maskRepeats[] = {"repeat", "space", "round", "no-repeat"};
    // The boxes of mask-clip and mask-origin, the CSS ones as an SVG
    // element has them, for the clip a border box the stroke box, for the
    // origin the fill box, as CSS Masking maps them; no-clip.
    const char* const maskBoxes[] = {"fill-box", "stroke-box", "view-box", "content-box", "padding-box", "border-box", "no-clip"};
    const u8 maskClipBoxes[] = {(u8)GeometryBox::Fill, (u8)GeometryBox::Stroke, (u8)GeometryBox::View, (u8)GeometryBox::Fill, (u8)GeometryBox::Fill, (u8)GeometryBox::Stroke, 255};
    const u8 maskOriginBoxes[] = {(u8)GeometryBox::Fill, (u8)GeometryBox::Stroke, (u8)GeometryBox::View, (u8)GeometryBox::Fill, (u8)GeometryBox::Fill, (u8)GeometryBox::Fill};

    static bool parseColor(StringView source, Color& out);
    static bool readAngle(StringView text, float& out, bool& unit);

    // A stop of a CSS gradient: its colour and its place along the line as
    // written, if any; a hint has its place alone.
    struct CssStop {
        Color color;
        Length at;
        bool placed;
        bool hint;
    };

    // A CSS gradient: linear or radial, perhaps repeating; the linear's
    // angle, or the side or corner it goes to as bits, 1 left, 2 right, 4
    // top, 8 bottom; the radial's shape, its extent 0 closest-side, 1
    // farthest-side, 2 closest-corner, 3 farthest-corner or 4 as given, and
    // its centre; the stops.
    struct CssGradient {
        bool radial;
        bool conic;
        bool repeating;
        float angle;
        u8 corner;
        bool circle;
        u8 extent;
        Length radius[2];
        Length center[2];
        Vector<CssStop> stops;
    };

    // An angle of CSS: its unit, but zero; in degrees.
    static bool cssAngle(StringView word, float& out) {
        bool unit;

        return readAngle(word, out, unit) && (unit || out == 0.f);
    }

    // The line of a linear gradient: an angle with its unit, zero without,
    // or to a side or a corner.
    static bool gradientLine(StringView text, CssGradient& out) {
        const u8* p = text.begin();
        StringView word = maskWord(p, text.end());

        if (!equalsIgnoreCase(word, SV("to"))) {
            out.corner = 0;

            return maskWord(p, text.end()).empty() && cssAngle(word, out.angle);
        }

        out.corner = 0;

        for (u32 count = 0; count < 2; count++) {
            int side = maskKeyword(maskWord(p, text.end()), maskPlaces, 4);
            u8 bit = (u8)(side >= 0 ? 1 << side : 0);

            // One of each axis, the first given.
            if ((side < 0 && count == 0) || (side >= 0 && (out.corner & (side < 2 ? 3 : 12)))) {
                return false;
            }

            out.corner |= bit;
        }

        return maskWord(p, text.end()).empty();
    }

    // The cone of a conic gradient: from an angle, at a position, either
    // or both, in that order.
    static bool gradientCone(StringView text, CssGradient& out) {
        const u8* p = text.begin();
        StringView word = maskWord(p, text.end());

        if (equalsIgnoreCase(word, SV("from"))) {
            if (!cssAngle(maskWord(p, text.end()), out.angle)) {
                return false;
            }

            word = maskWord(p, text.end());

            if (word.empty()) {
                return true;
            }
        }

        return equalsIgnoreCase(word, SV("at")) && !trim(StringView(p, text.end())).empty() && parsePosition(StringView(p, text.end()), out.center[0], out.center[1]);
    }

    // The shape of a radial gradient: circle or ellipse, an extent keyword
    // or one or two lengths, in any order, then at a position.
    static bool gradientShape(StringView text, CssGradient& out) {
        static const char* const extents[] = {"closest-side", "farthest-side", "closest-corner", "farthest-corner"};
        const u8* p = text.begin();
        u32 lengths = 0;
        bool shaped = false;
        bool sized = false;

        out.circle = false;
        out.extent = 3;
        out.center[0] = Length{50.f, Unit::Percent};
        out.center[1] = Length{50.f, Unit::Percent};

        for (StringView word = maskWord(p, text.end()); !word.empty(); word = maskWord(p, text.end())) {
            int extent = maskKeyword(word, extents, 4);

            if (equalsIgnoreCase(word, SV("at"))) {
                return !trim(StringView(p, text.end())).empty() && parsePosition(StringView(p, text.end()), out.center[0], out.center[1]);
            }

            if (!shaped && (equalsIgnoreCase(word, SV("circle")) || equalsIgnoreCase(word, SV("ellipse")))) {
                out.circle = equalsIgnoreCase(word, SV("circle"));
                shaped = true;
            } else if (!sized && extent >= 0) {
                out.extent = (u8)extent;
                sized = true;
            } else if ((!sized || lengths == 1) && parseLength(word, out.radius[lengths]) && out.radius[lengths].value >= 0.f) {
                lengths++;
                out.extent = 4;
                sized = true;
            } else {
                return false;
            }
        }

        // One length is a circle's, two an ellipse's; a circle's percentage
        // is not.
        if (lengths == 1) {
            if (shaped && !out.circle) {
                return false;
            }

            out.circle = true;
        }

        return !(lengths == 2 && out.circle) && !(lengths == 1 && out.radius[0].unit == Unit::Percent) && (shaped || sized);
    }

    // A CSS gradient's function, one of the six by its name: its line,
    // shape or cone first, perhaps, then the stops and hints apart by
    // commas.
    static bool parseCssGradient(StringView text, Color current, CssGradient& out) {
        const u8* open = findOutside(text.begin(), text.end(), '(');
        StringView name(text.begin(), open);
        const u8* p = open + 1;
        const u8* end = text.end() - 1;
        u32 colors = 0;
        bool first = true;

        out.repeating = startsWithIgnoreCase(name, SV("repeating-"));
        out.radial = equalsIgnoreCase(name, out.repeating ? SV("repeating-radial-gradient") : SV("radial-gradient"));
        out.conic = equalsIgnoreCase(name, out.repeating ? SV("repeating-conic-gradient") : SV("conic-gradient"));
        out.angle = out.conic ? 0.f : 180.f;
        out.corner = 0;
        out.circle = false;
        out.extent = 3;
        out.center[0] = Length{50.f, Unit::Percent};
        out.center[1] = Length{50.f, Unit::Percent};

        // A conic's places are angles, kept as the turn's percentages.
        auto place = [&](StringView word, Length& at) {
            float angle;

            if (out.conic && cssAngle(word, angle)) {
                at = Length{angle / 3.6f, Unit::Percent};

                return true;
            }

            return parseLength(word, at) && (!out.conic || at.unit == Unit::Percent);
        };

        for (;;) {
            const u8* comma = findOutside(p, end, ',');
            StringView item = trim(StringView(p, comma ? comma : end));
            const u8* q = item.begin();
            StringView word = maskWord(q, item.end());
            CssStop stop{};

            // The first item may be the line, the shape or the cone.
            if (first && (out.conic ? gradientCone(item, out) : (out.radial ? gradientShape(item, out) : gradientLine(item, out)))) {
                first = false;
            } else if (place(item, stop.at)) {
                stop.hint = true;
                stop.placed = true;
                out.stops.pushBack(stop);
            } else {
                if (equalsIgnoreCase(word, SV("currentcolor"))) {
                    stop.color = current;
                } else if (!parseColor(word, stop.color)) {
                    return false;
                }

                colors++;
                first = false;

                for (u32 k = 0; k < 2; k++) {
                    StringView spot = maskWord(q, item.end());

                    if (spot.empty()) {
                        break;
                    }

                    if (!place(spot, stop.at)) {
                        return false;
                    }

                    stop.placed = true;
                    out.stops.pushBack(stop);
                }

                if (!stop.placed) {
                    out.stops.pushBack(stop);
                }

                if (!maskWord(q, item.end()).empty()) {
                    return false;
                }
            }

            first = false;

            if (!comma) {
                break;
            }

            p = comma + 1;
        }

        // A hint stands between two colours.
        for (size_t i = 0; i < out.stops.length(); i++) {
            if (out.stops[i].hint && (i == 0 || i + 1 == out.stops.length() || out.stops[i + 1].hint)) {
                return false;
            }
        }

        return colors >= 2;
    }

    // mask-image: none, a url, to a mask element by its fragment or an
    // image, or a gradient.
    static bool maskImageOf(StringView text, MaskLayer& out) {
        static const char* const gradients[] = {"linear-gradient(", "radial-gradient(", "conic-gradient(", "repeating-linear-gradient(", "repeating-radial-gradient(", "repeating-conic-gradient("};

        out.source = MaskSource::None;

        if (equalsIgnoreCase(text, SV("none"))) {
            return true;
        }

        if (text.length() < 5 || text.back() != ')') {
            return false;
        }

        if (equalsIgnoreCase(StringView(text.begin(), text.begin() + 4), SV("url("))) {
            StringView url = trim(StringView(text.begin() + 4, text.end() - 1));

            // A quoted one is the string alone.
            if (at(url.begin(), url.end(), '"') || at(url.begin(), url.end(), '\'')) {
                if (url.length() < 2 || url.back() != url[0]) {
                    return false;
                }

                url = StringView(url.begin() + 1, url.end() - 1);
            }

            out.source = at(url.begin(), url.end(), '#') ? MaskSource::Element : MaskSource::Image;
            out.url = out.source == MaskSource::Element ? StringView(url.begin() + 1, url.end()) : url;

            return true;
        }

        for (const char* gradient : gradients) {
            StringView name(gradient);

            CssGradient parsed;

            if (text.length() > name.length() && equalsIgnoreCase(StringView(text.begin(), text.begin() + name.length()), name)) {
                out.source = MaskSource::Gradient;
                out.image = text;

                return parseCssGradient(text, Color{0.f, 0.f, 0.f, 1.f}, parsed);
            }
        }

        return false;
    }

    // mask-repeat: repeat-x or repeat-y, repeat-inline or repeat-block,
    // the same in horizontal writing, or one or two of the four.
    static bool maskRepeatOf(StringView text, MaskLayer& out) {
        static const char* const axes[] = {"repeat-x", "repeat-inline", "repeat-y", "repeat-block"};
        const u8* p = text.begin();
        StringView first = maskWord(p, text.end());
        StringView second = maskWord(p, text.end());
        int x = maskKeyword(first, maskRepeats, 4);
        int y = second.empty() ? x : maskKeyword(second, maskRepeats, 4);
        int axis = maskKeyword(first, axes, 4);

        if (second.empty() && axis >= 0) {
            x = axis < 2 ? 0 : 3;
            y = 3 - x;
        }

        out.repeat[0] = (u8)x;
        out.repeat[1] = (u8)y;

        return x >= 0 && y >= 0 && maskWord(p, text.end()).empty();
    }

    // mask-position: background-position's one to four words.
    static bool maskPositionOf(StringView text, MaskLayer& out) {
        const u8* p = text.begin();
        StringView words[5];
        u32 count = 0;

        out.far[0] = false;
        out.far[1] = false;

        while (count < 5 && !(words[count] = maskWord(p, text.end())).empty()) {
            count++;
        }

        if (count == 0 || count == 5) {
            return false;
        }

        if (count <= 2) {
            return parsePosition(text, out.position[0], out.position[1]);
        }

        // Four: two sides, one of each axis, each with its offset; CSS
        // Values' position has no three.
        bool set[2] = {false, false};

        for (u32 i = 0; i < count; i += 2) {
            int keyword = maskKeyword(words[i], maskPlaces, 4);
            int axis = keyword / 2;

            if (count != 4 || keyword < 0 || set[axis] || !parseLength(words[i + 1], out.position[axis])) {
                return false;
            }

            set[axis] = true;
            out.far[axis] = keyword % 2 == 1;
        }

        return true;
    }

    // mask-size: cover, contain, or one or two extents of no sign or auto.
    static bool maskSizeOf(StringView text, MaskLayer& out) {
        const u8* p = text.begin();
        StringView first = maskWord(p, text.end());
        StringView second = maskWord(p, text.end());

        out.size = equalsIgnoreCase(first, SV("cover")) ? 1 : (equalsIgnoreCase(first, SV("contain")) ? 2 : 0);

        if (out.size) {
            return second.empty();
        }

        StringView words[2] = {first, second.empty() ? SV("auto") : second};

        for (int axis = 0; axis < 2; axis++) {
            out.fit[axis] = equalsIgnoreCase(words[axis], SV("auto"));

            if (!out.fit[axis] && (!parseLength(words[axis], out.extent[axis]) || out.extent[axis].value < 0.f)) {
                return false;
            }
        }

        return maskWord(p, text.end()).empty();
    }

    // One value of a mask longhand, for the layer.
    static bool maskPartOf(u32 part, StringView text, MaskLayer& out) {
        int k;

        switch (part) {
            case MaskPartImage:
                return maskImageOf(text, out);
            case MaskPartMode:
                k = maskKeyword(text, maskModes, 3);
                out.mode = (u8)k;

                return k >= 0;
            case MaskPartRepeat:
                return maskRepeatOf(text, out);
            case MaskPartPosition:
                return maskPositionOf(text, out);
            case MaskPartClip:
            case MaskPartOrigin:
                k = maskKeyword(text, maskBoxes, 7);

                if (k < 0 || (part == MaskPartOrigin && k == 6)) {
                    return false;
                }

                if (part == MaskPartClip) {
                    out.clip = maskClipBoxes[k];
                } else {
                    out.origin = (GeometryBox)maskOriginBoxes[k];
                }

                return true;
            case MaskPartSize:
                return maskSizeOf(text, out);
            default:
                // MaskPartComposite.
                k = maskKeyword(text, maskComposites, 4);
                out.composite = (u8)k;

                return k >= 0;
        }
    }

    // A mask longhand's values apart by commas, each valid.
    static bool maskListValid(u32 part, StringView text) {
        const u8* p = text.begin();
        MaskLayer layer;

        for (;;) {
            const u8* comma = findOutside(p, text.end(), ',');

            if (!maskPartOf(part, trim(StringView(p, comma ? comma : text.end())), layer)) {
                return false;
            }

            if (!comma) {
                return true;
            }

            p = comma + 1;
        }
    }

    // The mask shorthand into its longhands: each layer's parts in any
    // order, a position perhaps with a size after a slash, one box both the
    // origin and the clip, two the one then the other; a part left out
    // takes its initial value.
    static bool expandMask(Document& doc, StringView value, u32 (&out)[MaskParts]) {
        static const char* const initial[MaskParts] = {"none", "match-source", "repeat", "0% 0%", "border-box", "border-box", "auto", "add"};
        Buffer lists[MaskParts];
        const u8* p = value.begin();

        if (equalsIgnoreCase(value, SV("inherit")) || equalsIgnoreCase(value, SV("initial")) || equalsIgnoreCase(value, SV("unset"))) {
            for (u32 k = 0; k < MaskParts; k++) {
                out[k] = intern(doc.atoms, value);
            }

            return true;
        }

        for (u32 layer = 0;; layer++) {
            const u8* comma = findOutside(p, value.end(), ',');
            const u8* end = comma ? comma : value.end();
            StringView parts[MaskParts];
            StringView boxes[2];
            u32 boxCount = 0;
            MaskLayer probe;

            memset(&probe, 0, sizeof(probe));

            for (const u8* q = p;;) {
                const u8* start = q;
                StringView word = maskWord(q, end);
                Length length;

                if (word.empty()) {
                    break;
                }

                bool positional = maskKeyword(word, maskPlaces, 5) >= 0 || parseLength(word, length);

                if (parts[MaskPartImage].empty() && maskImageOf(word, probe)) {
                    parts[MaskPartImage] = word;
                } else if (parts[MaskPartMode].empty() && maskKeyword(word, maskModes, 3) >= 0) {
                    parts[MaskPartMode] = word;
                } else if (parts[MaskPartComposite].empty() && maskKeyword(word, maskComposites, 4) >= 0) {
                    parts[MaskPartComposite] = word;
                } else if (parts[MaskPartRepeat].empty() && (maskKeyword(word, maskRepeats, 4) >= 0 || maskRepeatOf(word, probe))) {
                    const u8* r = q;

                    // A second of the four belongs to the first.
                    if (maskKeyword(word, maskRepeats, 4) >= 0 && maskKeyword(maskWord(r, end), maskRepeats, 4) >= 0) {
                        q = r;
                    }

                    parts[MaskPartRepeat] = trim(StringView(start, q));
                } else if (boxCount < 2 && maskKeyword(word, maskBoxes, 7) >= 0) {
                    boxes[boxCount++] = word;
                } else if (parts[MaskPartPosition].empty() && positional) {
                    const u8* last = q;
                    const u8* r = q;

                    for (u32 count = 1; count < 4; count++) {
                        StringView next = maskWord(r, end);

                        if (!(maskKeyword(next, maskPlaces, 5) >= 0 || parseLength(next, length))) {
                            break;
                        }

                        last = r;
                    }

                    q = last;
                    parts[MaskPartPosition] = trim(StringView(start, q));
                    r = q;

                    if (equalsIgnoreCase(maskWord(r, end), SV("/"))) {
                        const u8* sizeStart = r;
                        const u8* sizeEnd = r;
                        StringView first = maskWord(r, end);

                        sizeEnd = r;

                        if (!(equalsIgnoreCase(first, SV("cover")) || equalsIgnoreCase(first, SV("contain")))) {
                            const u8* s = r;
                            StringView second = maskWord(s, end);

                            if (equalsIgnoreCase(second, SV("auto")) || parseLength(second, length)) {
                                sizeEnd = s;
                            }
                        }

                        parts[MaskPartSize] = trim(StringView(sizeStart, sizeEnd));
                        q = sizeEnd;
                    }
                } else {
                    return false;
                }
            }

            // One box is both, two the origin then the clip.
            if (boxCount) {
                parts[MaskPartOrigin] = boxCount == 2 || !equalsIgnoreCase(boxes[0], SV("no-clip")) ? boxes[0] : StringView();
                parts[MaskPartClip] = boxes[boxCount - 1];
            }

            for (u32 k = 0; k < MaskParts; k++) {
                StringView part = parts[k].empty() ? StringView(initial[k]) : parts[k];

                if (!maskPartOf(k, part, probe)) {
                    return false;
                }

                if (layer) {
                    lists[k].append(", ", 2);
                }

                lists[k].append(part.begin(), part.length());
            }

            if (!comma) {
                break;
            }

            p = comma + 1;
        }

        for (u32 k = 0; k < MaskParts; k++) {
            out[k] = intern(doc.atoms, keep(*doc.pool, (const u8*)lists[k].data(), lists[k].length()));
        }

        return true;
    }

    // A mask border: its image as a mask layer has one; the mode, 1 alpha
    // or 0 luminance; by side, top, right, bottom and left, the slices,
    // numbers or percentages, the widths, lengths, percentages, numbers or
    // auto where its bit is set, and the outsets, lengths or numbers; the
    // middle filled; the repeats across and down, 0 stretch, 1 repeat, 2
    // round, 3 space.
    struct MaskBorder {
        MaskLayer image;
        u8 mode;
        Length slice[4];
        bool fill;
        Length width[4];
        u8 autos;
        Length outset[4];
        u8 repeat[2];
    };

    const char* const maskBorderRepeats[] = {"stretch", "repeat", "round", "space"};
    const char* const maskBorderInitials[MaskBorderParts] = {"none", "alpha", "0", "auto", "0", "stretch"};

    // One to four values for the sides top, right, bottom and left, as CSS
    // gives a side left out the one across from it: a slice's numbers and
    // percentages, kind 0; a width's lengths, percentages, numbers and
    // auto, 1; an outset's lengths and numbers, 2; none negative.
    static bool borderSides(StringView text, u32 kind, Length (&out)[4], u8& autos) {
        const u8* p = text.begin();
        u32 count = 0;

        autos = 0;

        for (StringView word = maskWord(p, text.end()); !word.empty(); word = maskWord(p, text.end())) {
            Length& side = out[count & 3];

            if (count == 4) {
                return false;
            }

            if (kind == 1 && equalsIgnoreCase(word, SV("auto"))) {
                autos |= (u8)(1 << count);
                side = Length{0.f, Unit::Number};
            } else if (!parseLength(word, side) || side.value < 0.f || (kind == 0 && side.unit != Unit::Number && side.unit != Unit::Percent) || (kind == 2 && side.unit == Unit::Percent)) {
                return false;
            }

            count++;
        }

        // Right is top's, bottom top's, left right's; a value is never
        // empty.
        for (u32 k = count; k < 4; k++) {
            out[k] = out[k == 3 ? 1 : 0];
            autos |= (u8)(((autos >> (k == 3 ? 1 : 0)) & 1) << k);
        }

        return true;
    }

    // One value of a mask-border longhand, into the border.
    static bool maskBorderPartOf(u32 part, StringView text, MaskBorder& out) {
        const u8* p = text.begin();
        StringView first;
        StringView second;
        StringView numbers = text;
        u8 autos;
        int k;

        switch (part) {
            case MaskBorderPartSource:
                return maskImageOf(text, out.image);
            case MaskBorderPartMode:
                k = equalsIgnoreCase(text, SV("alpha")) ? 1 : (equalsIgnoreCase(text, SV("luminance")) ? 0 : -1);
                out.mode = (u8)k;

                return k >= 0;
            case MaskBorderPartSlice:
                // fill after the numbers.
                out.fill = text.length() > 4 && equalsIgnoreCase(StringView(text.end() - 4, text.end()), SV("fill")) && isSpace(text.end()[-5]);

                if (out.fill) {
                    numbers = trim(StringView(text.begin(), text.end() - 4));
                }

                return borderSides(numbers, 0, out.slice, autos);
            case MaskBorderPartWidth:
                return borderSides(text, 1, out.width, out.autos);
            case MaskBorderPartOutset:
                return borderSides(text, 2, out.outset, autos);
            default:
                // MaskBorderPartRepeat.
                first = maskWord(p, text.end());
                second = maskWord(p, text.end());
                out.repeat[0] = (u8)maskKeyword(first, maskBorderRepeats, 4);
                out.repeat[1] = second.empty() ? out.repeat[0] : (u8)maskKeyword(second, maskBorderRepeats, 4);

                return out.repeat[0] < 4 && out.repeat[1] < 4 && maskWord(p, text.end()).empty();
        }
    }

    // The mask-border shorthand into its longhands: the source, the slices
    // perhaps with the widths after a slash and the outsets after another,
    // the repeats and the mode, in any order; a part left out takes its
    // initial value.
    static bool expandMaskBorder(Document& doc, StringView value, u32 (&out)[MaskBorderParts]) {
        StringView parts[MaskBorderParts];
        const u8* p = value.begin();
        const u8* end = value.end();
        MaskBorder probe;

        if (equalsIgnoreCase(value, SV("inherit")) || equalsIgnoreCase(value, SV("initial")) || equalsIgnoreCase(value, SV("unset"))) {
            for (u32 k = 0; k < MaskBorderParts; k++) {
                out[k] = intern(doc.atoms, value);
            }

            return true;
        }

        for (;;) {
            const u8* start = p;
            StringView word = maskWord(p, end);
            Length length;

            if (word.empty()) {
                break;
            }

            if (parts[MaskBorderPartSource].empty() && maskImageOf(word, probe.image)) {
                parts[MaskBorderPartSource] = word;
            } else if (parts[MaskBorderPartMode].empty() && maskBorderPartOf(MaskBorderPartMode, word, probe)) {
                parts[MaskBorderPartMode] = word;
            } else if (parts[MaskBorderPartRepeat].empty() && maskKeyword(word, maskBorderRepeats, 4) >= 0) {
                const u8* q = p;

                // A second of the four belongs to the first.
                if (maskKeyword(maskWord(q, end), maskBorderRepeats, 4) >= 0) {
                    p = q;
                }

                parts[MaskBorderPartRepeat] = trim(StringView(start, p));
            } else if (parts[MaskBorderPartSlice].empty() && parseLength(word, length)) {
                // The slices run to the first word of another part, with
                // fill; a slash takes the widths, another the outsets.
                u32 part = MaskBorderPartSlice;
                const u8* from = start;

                for (const u8* q = p;; q = p) {
                    StringView next = maskWord(q, end);

                    if (equalsIgnoreCase(next, SV("/")) && part < MaskBorderPartOutset) {
                        parts[part] = trim(StringView(from, p));

                        // A slash follows a part, not none.
                        if (parts[part].empty()) {
                            return false;
                        }

                        part++;
                        from = q;
                    } else if (!(parseLength(next, length) || equalsIgnoreCase(next, SV("auto")) || (part == MaskBorderPartSlice && equalsIgnoreCase(next, SV("fill"))))) {
                        break;
                    }

                    p = q;
                }

                parts[part] = trim(StringView(from, p));

                if (parts[part].empty()) {
                    return false;
                }
            } else {
                return false;
            }
        }

        for (u32 k = 0; k < MaskBorderParts; k++) {
            StringView part = parts[k].empty() ? StringView(maskBorderInitials[k]) : parts[k];

            if (!maskBorderPartOf(k, part, probe)) {
                return false;
            }

            out[k] = intern(doc.atoms, part);
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

            if (!(property >= firstProperty && property < firstProperty + propertyCount) && !geometryProperty(property) && property != AtomMarker && property != AtomMask && property != AtomMaskBorder) {
                continue;
            }

            if (value.empty()) {
                continue;
            }

            Buffer substituted;

            // A var() with no fallback, as the renderer knows no custom
            // property, or an env() of none, leaves the declaration invalid
            // at computed-value time: it is unset.
            if (findFunction(value, SV("var(")) || findFunction(value, SV("env("))) {
                value = substituteFallbacks(value, substituted, 0) ? trim(keep(*doc.pool, substituted.data(), substituted.length())) : SV("unset");
            }

            u32 parts[MaskParts];
            u32 borders[MaskBorderParts];

            // mask sets its longhands, and mask-border's to their initial
            // values, as CSS Masking has it; mask-border its own. An
            // invalid one is dropped whole.
            if (property == AtomMask) {
                if (expandMask(doc, value, parts)) {
                    for (u32 k = 0; k < MaskParts; k++) {
                        out.pushBack(Declaration{AtomMaskImage + k, parts[k], important});
                    }

                    for (u32 k = 0; k < MaskBorderParts; k++) {
                        out.pushBack(Declaration{AtomMaskBorderSource + k, intern(doc.atoms, SV("initial")), important});
                    }
                }

                continue;
            }

            if (property == AtomMaskBorder) {
                if (expandMaskBorder(doc, value, borders)) {
                    for (u32 k = 0; k < MaskBorderParts; k++) {
                        out.pushBack(Declaration{AtomMaskBorderSource + k, borders[k], important});
                    }
                }

                continue;
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

            // An invalid declaration of geometry is dropped, as CSS drops
            // it: the attribute stays.
            if (geometryProperty(property) && !validGeometry(property, value)) {
                continue;
            }

            // A valid path() holds one quoted string: its data.
            if (property == AtomD && startsWithIgnoreCase(value, SV("path("))) {
                StringView inner = trim(StringView(value.begin() + 5, value.end() - 1));

                value = StringView(inner.begin() + 1, inner.end() - 1);
            }

            out.pushBack(Declaration{property, intern(doc.atoms, value), important});
        }
    }

    static bool readNumber(const u8*& p, const u8* end, float& out);
    static bool parseLength(StringView text, Length& out);
    static float resolveLength(Length length, Axis axis, Viewport vp, float fontSize);
    static bool parseViewBox(const Document& doc, const Node* node, Box& out);

    // Media queries as a still picture on a screen answers them: the types
    // all and screen; the viewport the root's own size; a colour display of
    // 8 bits a channel at a dot a px; a light scheme, nothing to point or
    // hover with, no script, motion or update. An unknown query is unknown,
    // which no query list takes.
    enum class Tristate : u8 {
        False,
        True,
        Unknown,
    };

    static Tristate tristate(bool value) {
        return value ? Tristate::True : Tristate::False;
    }

    static Tristate tristateNot(Tristate t) {
        return t == Tristate::Unknown ? t : tristate(t == Tristate::False);
    }

    // A word of a media query, lowered: letters, digits and hyphens.
    static StringView mediaWord(const u8*& p, const u8* end, u8* buffer) {
        size_t n = 0;

        skipSpaces(p, end);

        while (p < end && n < 64 && isIdentChar(*p)) {
            buffer[n++] = lowerAscii(*p++);
        }

        return StringView(buffer, n);
    }

    // A keyword feature's value; empty for a name that is not one.
    static StringView mediaKeyword(Viewport view, StringView name) {
        static const char* const values[][2] = {
            {"prefers-color-scheme", "light"},
            {"hover", "none"},
            {"any-hover", "none"},
            {"pointer", "none"},
            {"any-pointer", "none"},
            {"scan", "progressive"},
            {"update", "none"},
            {"overflow-block", "none"},
            {"overflow-inline", "none"},
            {"prefers-reduced-motion", "no-preference"},
            {"prefers-reduced-transparency", "no-preference"},
            {"prefers-reduced-data", "no-preference"},
            {"prefers-contrast", "no-preference"},
            {"forced-colors", "none"},
            {"inverted-colors", "none"},
            {"color-gamut", "srgb"},
            {"dynamic-range", "standard"},
            {"video-dynamic-range", "standard"},
            {"display-mode", "browser"},
            {"scripting", "none"},
        };

        if (name == SV("orientation")) {
            return view.width > view.height ? SV("landscape") : SV("portrait");
        }

        for (const auto& value : values) {
            if (name == StringView(value[0])) {
                return StringView(value[1]);
            }
        }

        return StringView();
    }

    // A numeric feature's value: a length in px, a ratio, dots a px or
    // bits; false for a name that is not one.
    static bool mediaNumber(Viewport view, StringView name, float& out) {
        if (name == SV("width") || name == SV("device-width")) {
            out = view.width;
        } else if (name == SV("height") || name == SV("device-height")) {
            out = view.height;
        } else if (name == SV("aspect-ratio") || name == SV("device-aspect-ratio")) {
            out = view.height > 0.f ? view.width / view.height : 0.f;
        } else if (name == SV("resolution")) {
            out = 1.f;
        } else if (name == SV("color")) {
            out = 8.f;
        } else if (name == SV("color-index") || name == SV("monochrome") || name == SV("grid")) {
            out = 0.f;
        } else {
            return false;
        }

        return true;
    }

    // A value a numeric feature compares with, as the name takes it.
    static bool mediaValue(Viewport view, StringView name, StringView text, float& out) {
        const u8* p = text.begin();
        const u8* end = text.end();
        u8 buffer[64];
        Length length;
        float below = 1.f;

        if (name == SV("aspect-ratio") || name == SV("device-aspect-ratio")) {
            if (!readNumber(p, end, out)) {
                return false;
            }

            skipSpaces(p, end);

            if (at(p, end, '/')) {
                p++;
                skipSpaces(p, end);

                if (!readNumber(p, end, below) || below <= 0.f) {
                    return false;
                }
            }

            out /= below;

            return p == end;
        }

        if (name == SV("resolution")) {
            if (!readNumber(p, end, out)) {
                return false;
            }

            StringView unit = mediaWord(p, end, buffer);

            if (unit == SV("dpi")) {
                out /= 96.f;
            } else if (unit == SV("dpcm")) {
                out *= 2.54f / 96.f;
            } else if (unit != SV("dppx") && unit != SV("x")) {
                return false;
            }

            return p == end;
        }

        if (name == SV("width") || name == SV("height") || name == SV("device-width") || name == SV("device-height")) {
            // A length takes a unit, but zero.
            if (!parseLength(text, length) || length.unit == Unit::Percent || (length.unit == Unit::Number && length.value != 0.f)) {
                return false;
            }

            out = resolveLength(length, Axis::X, view, 16.f);

            return true;
        }

        return parseNumberText(text, out);
    }

    // Whether have compares with want as the operator says: < <= > >= =.
    static bool mediaCompare(float have, StringView op, float want) {
        if (op == SV("<")) {
            return have < want;
        }

        if (op == SV("<=")) {
            return have <= want;
        }

        if (op == SV(">")) {
            return have > want;
        }

        return op == SV(">=") ? have >= want : have == want;
    }

    // One media feature, the text in its parentheses: a name alone, a name
    // and a value, or a range of one or two comparisons.
    static Tristate mediaFeature(Viewport view, StringView text) {
        u8 buffer[64];
        const u8* colon = findOutside(text.begin(), text.end(), ':');
        float have;
        float want;

        if (colon) {
            const u8* p = text.begin();
            StringView name = mediaWord(p, colon, buffer);
            StringView value = trim(StringView(colon + 1, text.end()));
            bool min = startsWithIgnoreCase(name, SV("min-"));
            bool max = startsWithIgnoreCase(name, SV("max-"));
            StringView base = min || max ? StringView(name.begin() + 4, name.end()) : name;
            StringView keyword = mediaKeyword(view, base);

            skipSpaces(p, colon);

            if (p != colon) {
                return Tristate::Unknown;
            }

            if (!keyword.empty()) {
                return min || max ? Tristate::Unknown : tristate(equalsIgnoreCase(value, keyword));
            }

            if (!mediaNumber(view, base, have) || !mediaValue(view, base, value, want)) {
                return Tristate::Unknown;
            }

            return tristate(min ? have >= want : max ? have <= want : have == want);
        }

        // A range: operands apart by the operators.
        StringView parts[3];
        StringView ops[2];
        u32 count = 0;
        const u8* start = text.begin();

        for (const u8* p = text.begin(); p < text.end() && count < 3;) {
            if (*p == '<' || *p == '>' || *p == '=') {
                const u8* op = p;

                p += 1 + (*p != '=' && p + 1 < text.end() && p[1] == '=');
                parts[count] = trim(StringView(start, op));
                ops[count < 2 ? count : 1] = StringView(op, p);
                count++;
                start = p;
            } else {
                p++;
            }
        }

        if (count == 0) {
            const u8* p = text.begin();
            StringView name = mediaWord(p, text.end(), buffer);
            StringView keyword = mediaKeyword(view, name);

            skipSpaces(p, text.end());

            if (p != text.end()) {
                return Tristate::Unknown;
            }

            if (!keyword.empty()) {
                return tristate(keyword != SV("none") && keyword != SV("no-preference"));
            }

            return mediaNumber(view, name, have) ? tristate(have != 0.f) : Tristate::Unknown;
        }

        if (count > 2) {
            return Tristate::Unknown;
        }

        parts[count] = trim(StringView(start, text.end()));

        // The name is the operand that reads as one: first or last of one
        // comparison, the middle of two whose operators point one way.
        for (u32 at = 0; at <= count; at++) {
            const u8* p = parts[at].begin();
            StringView name = mediaWord(p, parts[at].end(), buffer);

            if (p != parts[at].end() || (count == 2 && at != 1) || !mediaNumber(view, name, have)) {
                continue;
            }

            Tristate result = Tristate::True;

            for (u32 k = 0; k < count; k++) {
                u32 other = k < at ? k : k + 1;
                bool before = other < at;

                if (count == 2 && ops[0][0] != ops[1][0]) {
                    return Tristate::Unknown;
                }

                if (!mediaValue(view, name, parts[other], want)) {
                    return Tristate::Unknown;
                }

                // A value before the name compares the other way round.
                result = mediaCompare(before ? want : have, ops[k], before ? have : want) ? result : Tristate::False;
            }

            return result;
        }

        return Tristate::Unknown;
    }

    static Tristate mediaCondition(Viewport view, const u8*& p, const u8* end, u32 depth);

    // A condition in parentheses: a nested condition or a feature; unknown
    // past them, or without them.
    static Tristate mediaInParens(Viewport view, const u8*& p, const u8* end, u32 depth) {
        skipSpaces(p, end);

        if (!at(p, end, '(') || depth > 32) {
            p = end;

            return Tristate::Unknown;
        }

        const u8* close = p + 1;
        u32 level = 1;

        while (close < end && level) {
            level += *close == '(';
            level -= *close == ')';
            close += level != 0;
        }

        StringView inner = trim(StringView(p + 1, close));
        const u8* q = inner.begin();
        u8 buffer[64];
        StringView first = mediaWord(q, inner.end(), buffer);
        Tristate result;

        p = close < end ? close + 1 : end;
        q = inner.begin();

        if (at(q, inner.end(), '(') || first == SV("not")) {
            result = mediaCondition(view, q, inner.end(), depth + 1);
            skipSpaces(q, inner.end());

            return q == inner.end() ? result : Tristate::Unknown;
        }

        return mediaFeature(view, inner);
    }

    // not, or conditions in parentheses joined by and or by or.
    static Tristate mediaCondition(Viewport view, const u8*& p, const u8* end, u32 depth) {
        u8 buffer[64];
        const u8* q = p;
        StringView word = mediaWord(q, end, buffer);

        if (word == SV("not")) {
            p = q;

            return tristateNot(mediaInParens(view, p, end, depth));
        }

        Tristate result = mediaInParens(view, p, end, depth);

        for (;;) {
            q = p;
            word = mediaWord(q, end, buffer);

            if (word != SV("and") && word != SV("or")) {
                return result;
            }

            bool both = word == SV("and");

            p = q;

            Tristate next = mediaInParens(view, p, end, depth);

            if (both) {
                result = result == Tristate::False || next == Tristate::False ? Tristate::False : (result == Tristate::True && next == Tristate::True ? Tristate::True : Tristate::Unknown);
            } else {
                result = result == Tristate::True || next == Tristate::True ? Tristate::True : (result == Tristate::False && next == Tristate::False ? Tristate::False : Tristate::Unknown);
            }
        }
    }

    // One media query: a condition, or a type, perhaps after not or only,
    // and conditions after and.
    static bool mediaQuery(Viewport view, StringView text) {
        const u8* p = text.begin();
        const u8* end = text.end();
        u8 buffer[64];
        u8 typeBuffer[64];
        const u8* q = p;
        StringView word = mediaWord(q, end, buffer);
        bool negate = false;
        Tristate result;

        skipSpaces(p, end);

        if (at(p, end, '(') || (word == SV("not") && (skipSpaces(q, end), at(q, end, '(')))) {
            result = mediaCondition(view, p, end, 0);
        } else {
            if (word == SV("not") || word == SV("only")) {
                negate = word == SV("not");
                p = q;
            }

            StringView type = mediaWord(p, end, typeBuffer);

            if (type.empty() || type == SV("and") || type == SV("or") || type == SV("not") || type == SV("only")) {
                return false;
            }

            result = tristate(type == SV("all") || type == SV("screen"));

            for (;;) {
                q = p;
                word = mediaWord(q, end, buffer);

                if (word != SV("and")) {
                    break;
                }

                p = q;

                Tristate next = mediaInParens(view, p, end, 0);

                result = result == Tristate::False || next == Tristate::False ? Tristate::False : (result == Tristate::True && next == Tristate::True ? Tristate::True : Tristate::Unknown);
            }

            result = negate ? tristateNot(result) : result;
        }

        skipSpaces(p, end);

        return p == end && result == Tristate::True;
    }

    // A media query list: any of its queries; an empty list is all.
    static bool mediaMatches(Viewport view, StringView text) {
        const u8* p = text.begin();
        const u8* end = text.end();

        if (trim(text).empty()) {
            return true;
        }

        for (;;) {
            const u8* comma = findOutside(p, end, ',');
            const u8* stop = comma ? comma : end;

            if (mediaQuery(view, trim(StringView(p, stop)))) {
                return true;
            }

            if (!comma) {
                return false;
            }

            p = comma + 1;
        }
    }

    // The viewport media queries see: the root's own width and height, or
    // its viewBox's size.
    static Viewport mediaViewport(const Document& doc) {
        Viewport view{0.f, 0.f, 0.f, 0.f, 16.f};
        Box viewBox;
        bool boxed = parseViewBox(doc, doc.root, viewBox);
        float sides[2] = {boxed ? viewBox.x1 - viewBox.x0 : 0.f, boxed ? viewBox.y1 - viewBox.y0 : 0.f};
        u32 names[2] = {AtomWidth, AtomHeight};

        for (int k = 0; k < 2; k++) {
            u32 value = attribute(doc.root, names[k]);
            Length length;

            if (value && parseLength(textOf(doc, value), length) && length.unit != Unit::Percent && length.value > 0.f) {
                sides[k] = resolveLength(length, Axis::X, view, 16.f);
            }
        }

        return Viewport{sides[0], sides[1], sides[0], sides[1], 16.f};
    }

    // The rules of a sheet's text, the rules of @media blocks the query of
    // which matches among them; other at-rules are left out.
    static void parseRules(Document& doc, Vector<Rule>& rules, StringView text, Viewport view, u32 depth) {
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
                const u8* q = p + 1;
                u8 buffer[64];
                StringView name = mediaWord(q, end, buffer);
                const u8* prelude = q;

                while (q < end && *q != ';' && *q != '{') {
                    q = *q == '"' || *q == '\'' ? skipCssString(q, end) : q + 1;
                }

                if (at(q, end, '{')) {
                    const u8* close = matchingBrace(q + 1, end);

                    if (name == SV("media") && depth < 32 && mediaMatches(view, StringView(prelude, q))) {
                        parseRules(doc, rules, StringView(q + 1, close), view, depth + 1);
                    }

                    q = close;
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

    static void parseSheet(Document& doc, Vector<Rule>& rules, StringView source, Viewport view) {
        Buffer cleaned;

        stripComments(source, cleaned);
        parseRules(doc, rules, keep(*doc.pool, cleaned.data(), cleaned.length()), view, 0);
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
        MaskBorder border;

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
            case AtomMaskImage:
            case AtomMaskMode:
            case AtomMaskRepeat:
            case AtomMaskPosition:
            case AtomMaskClip:
            case AtomMaskOrigin:
            case AtomMaskSize:
            case AtomMaskComposite:
                return maskListValid(property - AtomMaskImage, text);
            case AtomMaskBorderSource:
            case AtomMaskBorderMode:
            case AtomMaskBorderSlice:
            case AtomMaskBorderWidth:
            case AtomMaskBorderOutset:
            case AtomMaskBorderRepeat:
                return maskBorderPartOf(property - AtomMaskBorderSource, text, border);
            default:
                return true;
        }
    }

    static bool isKeyword(StringView text, StringView keyword);

    // The attribute a geometry property sets on an element.
    static u32 geometryAttribute(u32 element, u32 property) {
        if (property == AtomTransform && (element == AtomLinearGradient || element == AtomRadialGradient)) {
            return AtomGradientTransform;
        }

        if (property == AtomTransform && element == AtomPattern) {
            return AtomPatternTransform;
        }

        return property == AtomPathLengthProperty ? AtomPathLength : property;
    }

    // A geometry property's initial value.
    static StringView geometryInitial(u32 property) {
        static const struct {
            u32 property;
            const char* value;
        } initials[] = {{AtomX, "0"}, {AtomY, "0"}, {AtomCx, "0"}, {AtomCy, "0"}, {AtomR, "0"}, {AtomTransform, "none"}, {AtomD, "none"}, {AtomPathLengthProperty, "none"}, {AtomTransformOrigin, "50% 50%"}, {AtomTransformBox, "view-box"}};

        for (const auto& initial : initials) {
            if (initial.property == property) {
                return StringView(initial.value);
            }
        }

        // width, height, rx and ry.
        return SV("auto");
    }

    // The value of a geometry property a child inherits: what the parent's
    // declarations gave it, else its presentation attribute, else none for
    // the initial one. A use's instance inherits its target's parent's.
    static u32 inheritedGeometry(const Document& doc, const Node* parent, u32 property) {
        if (!parent) {
            return 0;
        }

        for (size_t i = doc.geometry.length(); i-- > 0 && doc.geometry[i].node >= parent->index;) {
            if (doc.geometry[i].node == parent->index && doc.geometry[i].property == property) {
                return doc.geometry[i].value;
            }
        }

        return geometryApplies(parent->name, property) ? attribute(parent, geometryAttribute(parent->name, property)) : 0;
    }

    static u32* cascadeNode(Document& doc, Node* node, const Node* boundary) {
        Candidate best[propertyCount];
        Vector<Declaration> geometry;
        Vector<u64> geometryPriorities;

        memset(best, 0, sizeof(best));

        for (u32 i = 0; i < node->count; i++) {
            u32 name = node->attributes[i].name;

            u32 parts[MaskParts];

            // mask is the presentation attribute of the shorthand, not of
            // its longhands.
            if (name == AtomMask && expandMask(doc, textOf(doc, node->attributes[i].value), parts)) {
                for (u32 k = 0; k < MaskParts; k++) {
                    consider(best, AtomMaskImage + k, parts[k], 1);
                }
            }

            if (name >= firstProperty && name < firstProperty + propertyCount && name != AtomMixBlendMode && name != AtomIsolation && name != AtomOutline && name != AtomZoom && !(name >= AtomMaskImage && name <= AtomMaskBorderRepeat)) {
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

            // inherit takes the parent's value, but d's, which a path
            // follows up as it draws, through a use to its instance;
            // initial and unset, as no geometry property inherits, the
            // initial one.
            u32 value = declaration.value;
            StringView text = textOf(doc, value);

            if (isKeyword(text, SV("inherit")) && declaration.property != AtomD) {
                value = inheritedGeometry(doc, node->parent, declaration.property);
            } else if (isKeyword(text, SV("initial")) || isKeyword(text, SV("unset"))) {
                value = 0;
            }

            doc.geometry.pushBack(GeometryValue{node->index, declaration.property, value});

            bool outer = node == doc.root && declaration.property != AtomTransform && declaration.property != AtomTransformOrigin;

            if ((geometryApplies(node->name, declaration.property) || declaration.property == AtomD) && !outer) {
                setAttribute(doc, node, geometryAttribute(node->name, declaration.property), value ? value : intern(doc.atoms, geometryInitial(declaration.property)));
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
        Viewport view = mediaViewport(doc);
        // The first title names the preferred set of sheets: a sheet of
        // another title is an alternate one, which is off.
        StringView preferred;

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            if (node->name == AtomStyle && !node->text.empty()) {
                u32 type = attribute(node, AtomType);
                u32 media = attribute(node, AtomMedia);
                u32 titleValue = attribute(node, AtomTitle);
                StringView title = titleValue ? textOf(doc, titleValue) : StringView();

                preferred = preferred.empty() ? title : preferred;

                if ((!type || textOf(doc, type).empty() || equalsIgnoreCase(textOf(doc, type), SV("text/css"))) && (!media || mediaMatches(view, textOf(doc, media))) && (title.empty() || title == preferred)) {
                    parseSheet(doc, rules, node->text, view);
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

    // A number in doubles, within the floats' range.
    static bool readDouble(const u8*& p, const u8* end, double& out) {
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

        out = result;
        p = s;

        return true;
    }

    static bool readNumber(const u8*& p, const u8* end, float& out) {
        double value;

        if (!readDouble(p, end, value)) {
            return false;
        }

        out = (float)value;

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

    // A refX or refY: a length, or a keyword for 0%, 50% or 100% of the
    // size the content draws in, as SVG 2 maps them. False when the
    // attribute gives neither.
    static bool referenceOf(const Document& doc, const Node* node, u32 name, Axis axis, Viewport vp, float fontSize, float size, float& out) {
        u32 value = attribute(node, name);
        StringView text = value ? trim(textOf(doc, value)) : StringView();
        StringView low = axis == Axis::X ? SV("left") : SV("top");
        StringView high = axis == Axis::X ? SV("right") : SV("bottom");

        if (text == low || text == SV("center") || text == high) {
            out = text == low ? 0.f : text == high ? size : size / 2.f;
            return true;
        }

        return lengthAttribute(doc, node, name, axis, vp, fontSize, out);
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

    // A transform list: in an attribute SVG's, numbers alone and its six
    // functions; in a declaration CSS's too, with angle and length units and
    // the functions of one axis.
    // The kinds of a CSS transform function's arguments, a length L, an
    // angle A or a number N, of which it may leave the last ones out: a
    // length takes its unit, an angle too, but zero.
    static bool cssTransformArguments(StringView name, const u8* units, const float* args, u32 count) {
        static const struct {
            const char* name;
            const char* kinds;
            u32 optional;
        } functions[] = {
            {"matrix", "NNNNNN", 0},
            {"translate", "LL", 1},
            {"translateX", "L", 0},
            {"translateY", "L", 0},
            {"translateZ", "L", 0},
            {"translate3d", "LLL", 0},
            {"scale", "NN", 1},
            {"scaleX", "N", 0},
            {"scaleY", "N", 0},
            {"scaleZ", "N", 0},
            {"scale3d", "NNN", 0},
            {"rotate", "A", 0},
            {"rotateX", "A", 0},
            {"rotateY", "A", 0},
            {"rotateZ", "A", 0},
            {"rotate3d", "NNNA", 0},
            {"skew", "AA", 1},
            {"skewX", "A", 0},
            {"skewY", "A", 0},
            {"perspective", "L", 0},
            {"matrix3d", "NNNNNNNNNNNNNNNN", 0},
        };

        for (const auto& function : functions) {
            u32 n = (u32)strlen(function.kinds);

            if (name != StringView(function.name)) {
                continue;
            }

            if (count > n || count + function.optional < n) {
                return false;
            }

            for (u32 i = 0; i < count; i++) {
                u8 kind = function.kinds[i] == 'L' ? 1 : (function.kinds[i] == 'A' ? 2 : 0);

                // A zero needs no unit.
                if (units[i] != kind && !(units[i] == 0 && args[i] == 0.f)) {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    static bool parseTransform(StringView text, Matrix& out, TransformGrammar grammar, Projective* perspective) {
        bool css = grammar != TransformGrammar::Attribute;
        const u8* p = text.begin();
        const u8* end = text.end();
        Matrix result = identity();
        // Past the first 3D function the list goes on in 4 by 4, rows by
        // columns, a point a column.
        bool deep = false;
        float m4[4][4];
        auto compose = [&](const float (&n)[4][4]) {
            float t[4][4];

            if (!deep) {
                float flat[4][4] = {{result.a, result.c, 0.f, result.e}, {result.b, result.d, 0.f, result.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f}};

                memcpy(m4, flat, sizeof(flat));
                deep = true;
            }

            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    t[r][c] = (float)((double)m4[r][0] * n[0][c] + (double)m4[r][1] * n[1][c] + (double)m4[r][2] * n[2][c] + (double)m4[r][3] * n[3][c]);
                }
            }

            memcpy(m4, t, sizeof(t));
        };

        if (perspective) {
            perspective->on = false;
            perspective->solid = false;
        }

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

            // A name of letters, and digits after them: translate3d.
            while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (p > start && (u8)(*p - '0') < 10))) {
                p++;
            }

            StringView name(start, p);

            skipSpaces(p, end);

            if (p >= end || *p != '(') {
                return false;
            }

            p++;

            float args[16];
            // Each argument's unit: none, a length, an angle.
            u8 units[16];
            u32 count = 0;

            for (;;) {
                skipSpaces(p, end);

                if (at(p, end, ')')) {
                    p++;
                    break;
                }

                if (count == 16 || !readNumber(p, end, args[count])) {
                    return false;
                }

                bool comma = false;

                const u8* unitStart = p;

                while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
                    p++;
                }

                StringView unit(unitStart, p);

                if (!css && !unit.empty()) {
                    return false;
                }

                units[count] = unit.empty() ? 0 : (equalsIgnoreCase(unit, SV("px")) ? 1 : 2);

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

            if (!css && !(name == SV("matrix") || name == SV("translate") || name == SV("scale") || name == SV("rotate") || name == SV("skewX") || name == SV("skewY"))) {
                return false;
            }

            if (grammar == TransformGrammar::Declaration && !cssTransformArguments(name, units, args, count)) {
                return false;
            }

            // A function of CSS alone comes here with its count: a
            // declaration checked it above, a value either grammar takes
            // was one.
            if (name == SV("translateX")) {
                name = SV("translate");
            } else if (name == SV("translateY")) {
                name = SV("translate");
                args[1] = args[0];
                args[0] = 0.f;
                count = 2;
            } else if (name == SV("scaleX")) {
                name = SV("scale");
                args[1] = 1.f;
                count = 2;
            } else if (name == SV("scaleY")) {
                name = SV("scale");
                args[1] = args[0];
                args[0] = 1.f;
                count = 2;
            } else if (name == SV("skew")) {
                name = SV("matrix");
                args[1] = count == 2 ? tanf(args[1] * pi / 180.f) : 0.f;
                args[2] = tanf(args[0] * pi / 180.f);
                args[0] = 1.f;
                args[3] = 1.f;
                args[4] = 0.f;
                args[5] = 0.f;
                count = 6;
            }

            // The 3D functions of CSS Transforms 2.
            float n[4][4] = {{1.f, 0.f, 0.f, 0.f}, {0.f, 1.f, 0.f, 0.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f}};
            bool rotation = name == SV("rotateX") || name == SV("rotateY") || name == SV("rotateZ");
            bool three = true;

            if (name == SV("translate3d")) {
                n[0][3] = args[0];
                n[1][3] = args[1];
                n[2][3] = args[2];
            } else if (name == SV("translateZ")) {
                n[2][3] = args[0];
            } else if (name == SV("scale3d")) {
                n[0][0] = args[0];
                n[1][1] = args[1];
                n[2][2] = args[2];
            } else if (name == SV("scaleZ")) {
                n[2][2] = args[0];
            } else if (name == SV("rotate3d") || rotation) {
                float axis[3] = {name == SV("rotateX") ? 1.f : 0.f, name == SV("rotateY") ? 1.f : 0.f, name == SV("rotateZ") ? 1.f : 0.f};
                float angle = (rotation ? args[0] : args[3]) * pi / 180.f;
                float length = rotation ? 1.f : sqrtf(args[0] * args[0] + args[1] * args[1] + args[2] * args[2]);

                // An axis of no length turns nothing.
                for (int k = 0; k < 3 && !rotation; k++) {
                    axis[k] = length > 0.f ? args[k] / length : 0.f;
                }

                float x = axis[0];
                float y = axis[1];
                float z = axis[2];
                float sc = sinf(angle / 2.f) * cosf(angle / 2.f);
                float sq = sinf(angle / 2.f) * sinf(angle / 2.f);
                float turn[4][4] = {{1.f - 2.f * (y * y + z * z) * sq, 2.f * (x * y * sq - z * sc), 2.f * (x * z * sq + y * sc), 0.f}, {2.f * (x * y * sq + z * sc), 1.f - 2.f * (x * x + z * z) * sq, 2.f * (y * z * sq - x * sc), 0.f}, {2.f * (x * z * sq - y * sc), 2.f * (y * z * sq + x * sc), 1.f - 2.f * (x * x + y * y) * sq, 0.f}, {0.f, 0.f, 0.f, 1.f}};

                memcpy(n, turn, sizeof(n));
            } else if (name == SV("perspective") && args[0] >= 0.f) {
                // A depth below a px is a px.
                n[3][2] = -1.f / maxf(args[0], 1.f);
            } else if (name == SV("matrix3d")) {
                for (int k = 0; k < 16; k++) {
                    n[k % 4][k / 4] = args[k];
                }
            } else {
                three = false;
            }

            if (three) {
                compose(n);
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

            if (deep) {
                float flat[4][4] = {{m.a, m.c, 0.f, m.e}, {m.b, m.d, 0.f, m.f}, {0.f, 0.f, 1.f, 0.f}, {0.f, 0.f, 0.f, 1.f}};

                compose(flat);
            } else {
                result = multiply(result, m);
            }
        }

        // On z = 0 a point goes by the first, second and last columns: in
        // perspective where the weight varies or has no sign.
        if (deep) {
            float w = m4[3][3] != 0.f ? m4[3][3] : 1.f;

            result = Matrix{m4[0][0] / w, m4[1][0] / w, m4[0][1] / w, m4[1][1] / w, m4[0][3] / w, m4[1][3] / w};

            if (perspective && (m4[3][0] != 0.f || m4[3][1] != 0.f || !(m4[3][3] > 0.f))) {
                *perspective = Projective{{m4[0][0], m4[0][1], m4[0][3], m4[1][0], m4[1][1], m4[1][3], m4[3][0], m4[3][1], m4[3][3]}, true, true};
            }

            if (perspective) {
                perspective->solid = true;
            }
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

    // Whether a declaration of a geometry property is one CSS keeps: a
    // length, of no sign for a size, auto for a size of a rect or its
    // corners, path() of a string or none, a transform list, an origin of
    // one to three values, or a box.
    static bool validGeometry(u32 property, StringView value) {
        Length length;
        Matrix m;

        if (equalsIgnoreCase(value, SV("inherit")) || equalsIgnoreCase(value, SV("initial")) || equalsIgnoreCase(value, SV("unset"))) {
            return true;
        }

        switch (property) {
            case AtomX:
            case AtomY:
            case AtomCx:
            case AtomCy:
                return parseLength(value, length);
            case AtomR:
                return parseLength(value, length) && length.value >= 0.f;
            case AtomWidth:
            case AtomHeight:
            case AtomRx:
            case AtomRy:
                return equalsIgnoreCase(value, SV("auto")) || (parseLength(value, length) && length.value >= 0.f);
            case AtomD: {
                StringView inner = value.length() > 6 ? trim(StringView(value.begin() + 5, value.end() - 1)) : StringView();

                return equalsIgnoreCase(value, SV("none")) || (startsWithIgnoreCase(value, SV("path(")) && value.back() == ')' && inner.length() >= 2 && (inner[0] == '"' || inner[0] == '\'') && inner.back() == inner[0]);
            }
            case AtomTransform:
                return parseTransform(value, m, TransformGrammar::Declaration);
            case AtomPathLengthProperty:
                return equalsIgnoreCase(value, SV("none")) || (parseLength(value, length) && length.unit != Unit::Percent && length.value >= 0.f);
            case AtomTransformOrigin: {
                const u8* p = value.begin();
                float x;
                float y;
                int axis;
                int count = 0;

                while (count < 3 && readOrigin(p, value.end(), 0.f, 0.f, x, y, axis)) {
                    count++;
                }

                skipSpaces(p, value.end());

                return count > 0 && p == value.end();
            }
            default:
                // AtomTransformBox.
                return equalsIgnoreCase(value, SV("content-box")) || equalsIgnoreCase(value, SV("border-box")) || equalsIgnoreCase(value, SV("fill-box")) || equalsIgnoreCase(value, SV("stroke-box")) || equalsIgnoreCase(value, SV("view-box"));
        }
    }

    static Matrix originTransform(const Document& doc, const Node* node, u32 name, Viewport vp) {
        u32 value = attribute(node, name);
        Matrix m;

        if (!value) {
            return identity();
        }

        // The parse took the attribute whole.
        parseTransform(textOf(doc, value), m, TransformGrammar::Either);

        u32 origin = attribute(node, AtomTransformOrigin);

        if (!origin) {
            return m;
        }

        Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

        return multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
    }

    static Matrix transformOf(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);
        Matrix m = identity();

        // The parse and the cascade took a transform whole.
        if (value) {
            parseTransform(textOf(doc, value), m, TransformGrammar::Either);
        }

        return m;
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
        double sx = (double)width / vw;
        double sy = (double)height / vh;
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
            return multiply(scaling((float)sx, (float)sy), translation(-viewBox.x0, -viewBox.y0));
        }

        // The side that sets the scale is filled exactly; only the other is
        // aligned, so a scale rounded to a float shifts nothing.
        double s = slice ? (sx > sy ? sx : sy) : (sx < sy ? sx : sy);
        double tx = -(double)viewBox.x0 * s + (s == sx ? 0.0 : (width - vw * s) * alignX * 0.5);
        double ty = -(double)viewBox.y0 * s + (s == sy ? 0.0 : (height - vh * s) * alignY * 0.5);

        return Matrix{(float)s, 0.f, 0.f, (float)s, (float)tx, (float)ty};
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

    // The numbers and the current point go in doubles, each point stored
    // rounded once to floats: relative steps that come back to a point come
    // back to it, as written, without the floats' error adding up along the
    // path.
    static void parsePathData(StringView text, Path& path) {
        const u8* p = text.begin();
        const u8* end = text.end();
        DPoint current{0.0, 0.0};
        DPoint start{0.0, 0.0};
        DPoint control{0.0, 0.0};
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
            DPoint base = relative ? current : DPoint{0.0, 0.0};
            auto stored = [](DPoint q) {
                return Point{(float)q.x, (float)q.y};
            };

            if (upper == 'Z') {
                closePath(path);
                current = start;
                needMove = true;
                previous = upper;
                skipSeparator(p, end);
                continue;
            }

            if (needMove && upper != 'M') {
                moveTo(path, stored(current));
            }

            needMove = false;

            double v[7];
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
                    ok = readDouble(p, end, v[i]);

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
                    current = DPoint{base.x + v[0], base.y + v[1]};
                    start = current;
                    moveTo(path, stored(current));
                    break;
                case 'L':
                    current = DPoint{base.x + v[0], base.y + v[1]};
                    lineTo(path, stored(current));
                    break;
                case 'H':
                    current = DPoint{base.x + v[0], current.y};
                    lineTo(path, stored(current));
                    break;
                case 'V':
                    current = DPoint{current.x, base.y + v[0]};
                    lineTo(path, stored(current));
                    break;
                case 'C': {
                    DPoint c1{base.x + v[0], base.y + v[1]};
                    DPoint c2{base.x + v[2], base.y + v[3]};
                    DPoint to{base.x + v[4], base.y + v[5]};

                    cubicTo(path, stored(c1), stored(c2), stored(to));
                    control = c2;
                    current = to;
                } break;
                case 'S': {
                    DPoint c1 = previous == 'C' || previous == 'S' ? DPoint{2.0 * current.x - control.x, 2.0 * current.y - control.y} : current;
                    DPoint c2{base.x + v[0], base.y + v[1]};
                    DPoint to{base.x + v[2], base.y + v[3]};

                    cubicTo(path, stored(c1), stored(c2), stored(to));
                    control = c2;
                    current = to;
                } break;
                case 'Q':
                case 'T': {
                    DPoint q = upper == 'Q' ? DPoint{base.x + v[0], base.y + v[1]} : (previous == 'Q' || previous == 'T' ? DPoint{2.0 * current.x - control.x, 2.0 * current.y - control.y} : current);
                    DPoint to = upper == 'Q' ? DPoint{base.x + v[2], base.y + v[3]} : DPoint{base.x + v[0], base.y + v[1]};
                    DPoint c1{current.x + 2.0 / 3.0 * (q.x - current.x), current.y + 2.0 / 3.0 * (q.y - current.y)};
                    DPoint c2{to.x + 2.0 / 3.0 * (q.x - to.x), to.y + 2.0 / 3.0 * (q.y - to.y)};

                    cubicTo(path, stored(c1), stored(c2), stored(to));
                    control = q;
                    current = to;
                    break;
                }
                default: {
                    // 'A', the last command of the count switch above.
                    DPoint to{base.x + v[5], base.y + v[6]};

                    arcTo(path, stored(current), (float)v[0], (float)v[1], (float)v[2], largeArc, sweepArc, stored(to));
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

    // The points of a cubic after its start, to the tolerance.
    static void flattenCubic(Point p0, Point p1, Point p2, Point p3, float tol, Vector<Point>& points) {
        float ddx = maxf(fabsf(p0.x - 2.f * p1.x + p2.x), fabsf(p1.x - 2.f * p2.x + p3.x));
        float ddy = maxf(fabsf(p0.y - 2.f * p1.y + p2.y), fabsf(p1.y - 2.f * p2.y + p3.y));
        float dd = sqrtf(ddx * ddx + ddy * ddy);
        float steps = ceilf(sqrtf(0.75f * dd / tol));
        int n = steps >= 1.f ? (steps < 512.f ? (int)steps : 512) : 1;

        for (int i = 1; i < n; i++) {
            points.pushBack(cubicAt(p0, p1, p2, p3, (float)i / n));
        }

        points.pushBack(p3);
    }

    // A cubic as powers of t from its start, in doubles, which keep its
    // shape where the floats of points along it cannot: from p0 it is at
    // 3 a t + 3 b t^2 + c t^3, and its velocity over 3 is a + 2 b t + c t^2.
    struct Powers {
        DPoint a;
        DPoint b;
        DPoint c;
    };

    static Powers powers(Point p0, Point p1, Point p2, Point p3) {
        return Powers{DPoint{(double)p1.x - p0.x, (double)p1.y - p0.y}, DPoint{(double)p2.x - 2.0 * p1.x + p0.x, (double)p2.y - 2.0 * p1.y + p0.y}, DPoint{(double)p3.x - 3.0 * p2.x + 3.0 * p1.x - p0.x, (double)p3.y - 3.0 * p2.y + 3.0 * p1.y - p0.y}};
    }

    static DPoint powerAt(const Powers& q, double t) {
        return DPoint{((q.c.x * t + 3.0 * q.b.x) * t + 3.0 * q.a.x) * t, ((q.c.y * t + 3.0 * q.b.y) * t + 3.0 * q.a.y) * t};
    }

    static DPoint powerVelocity(const Powers& q, double t) {
        return DPoint{q.a.x + (2.0 * q.b.x + q.c.x * t) * t, q.a.y + (2.0 * q.b.y + q.c.y * t) * t};
    }

    // The chord of a cubic from t to t + h, as powers of h, exact however
    // short.
    static DPoint powerChord(const Powers& q, double t, double h) {
        DPoint v = powerVelocity(q, t);
        double wx = q.b.x + q.c.x * t;
        double wy = q.b.y + q.c.y * t;

        return DPoint{((q.c.x * h + 3.0 * wx) * h + 3.0 * v.x) * h, ((q.c.y * h + 3.0 * wy) * h + 3.0 * v.y) * h};
    }

    static Point unitOf(DPoint d) {
        double length = sqrt(d.x * d.x + d.y * d.y);

        return Point{(float)(d.x / length), (float)(d.y / length)};
    }

    static double deviation(DPoint a, DPoint b) {
        return fabs(atan2(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y));
    }

    // The way a cubic leaves t on toward side, 1 or -1: along its velocity,
    // or where it rests, at a cusp or at an end on its control, along its
    // acceleration, which it leaves by either way.
    static DPoint leaving(const Powers& q, double t, double side, bool cusp) {
        DPoint v = powerVelocity(q, t);

        if (cusp || (v.x == 0.0 && v.y == 0.0)) {
            return DPoint{q.b.x + q.c.x * t, q.b.y + q.c.y * t};
        }

        return DPoint{v.x * side, v.y * side};
    }

    // How many times the step from t toward side halves until the chord
    // runs the way the cubic leaves t, to the turn, however short, as a cap
    // and the joins after it go by its direction; and until its end lies as
    // near the tangent there as the even steps of the tolerance keep it,
    // 4 tol, which a cubic too long for its steps does not.
    static int refinements(const Powers& q, double t, double step, double side, bool cusp, float turn, float tol) {
        DPoint way = leaving(q, t, side, cusp);
        double length = sqrt(way.x * way.x + way.y * way.y);
        int k = 0;

        while (k < 40) {
            DPoint chord = powerChord(q, t, side * step);

            if (!(deviation(way, chord) > turn || fabs(way.x * chord.y - way.y * chord.x) > 4.0 * tol * length)) {
                break;
            }

            step *= 0.5;
            k++;
        }

        return k;
    }

    // The parameters of a cubic strictly between t0 and t1, halving the
    // span where its direction bends past bend: a stroke's join between two
    // points then lies within the tolerance of the sweep of its normals.
    static void bendParameters(const Powers& q, double t0, double t1, double bend, int depth, Vector<double>& out) {
        // A bend past the numbers is no bend to halve.
        if (depth == 0 || !(deviation(powerVelocity(q, t0), powerVelocity(q, t1)) > bend)) {
            return;
        }

        double t = (t0 + t1) / 2.0;

        bendParameters(q, t0, t, bend, depth - 1, out);
        out.pushBack(t);
        bendParameters(q, t, t1, bend, depth - 1, out);
    }

    // The roots of c + 2 b t + a t^2 where its sign changes, in order, the
    // rest of the two left at -1.
    static void signChanges(double a, double b, double c, double* roots) {
        double d = b * b - a * c;

        if (a == 0.0) {
            roots[0] = b != 0.0 ? -c / (2.0 * b) : -1.0;
        } else if (d > 0.0) {
            double q = -(b + copysign(sqrt(d), b));
            double r0 = q / a;
            double r1 = c / q;

            roots[0] = fmin(r0, r1);
            roots[1] = fmax(r0, r1);
        }
    }

    // The parameters inside a cubic where its tangent turns back, in order,
    // and their kind. Its turning goes as cross(v, v') = cross(a, b)
    // + cross(a, c) t + cross(b, c) t^2: where that changes sign the cubic
    // inflects, and where it touches zero its velocity is none, a cusp. A
    // cubic along a line turns back where its speed along it changes sign,
    // each a cusp.
    static int turnPoints(const Powers& q, double* out, u8& kind, bool& line) {
        double qa = q.b.x * q.c.y - q.b.y * q.c.x;
        double qb = q.a.x * q.c.y - q.a.y * q.c.x;
        double qc = q.a.x * q.b.y - q.a.y * q.b.x;
        double roots[2] = {-1.0, -1.0};

        line = qa == 0.0 && qb == 0.0 && qc == 0.0;
        kind = PointCusp;

        if (line) {
            // Along a, or b where the first control is at the start: a cubic
            // with both there is straight and never comes here.
            bool rest = q.a.x == 0.0 && q.a.y == 0.0;
            DPoint u = rest ? q.b : q.a;

            signChanges(q.c.x * u.x + q.c.y * u.y, q.b.x * u.x + q.b.y * u.y, q.a.x * u.x + q.a.y * u.y, roots);
        } else if (qa != 0.0 && qb * qb == 4.0 * qa * qc) {
            roots[0] = -qb / (2.0 * qa);
        } else {
            signChanges(qa, qb / 2.0, qc, roots);
            kind = PointSmooth;
        }

        int count = 0;

        for (double t : roots) {
            if (t > 0.0 && t < 1.0) {
                out[count++] = t;
            }
        }

        return count;
    }

    // Adds a stroke's point of a cubic at t, smooth, with the direction of
    // the chord to it from where its last point was, at from; none where
    // the two are one parameter, as the steps from a turn point the doubles
    // cannot tell from an end.
    static void addCurvePoint(Flat& out, Point p0, Point p3, const Powers& q, double from, double t) {
        DPoint at = powerAt(q, t);
        DPoint d = powerChord(q, from, t - from);
        double length = sqrt(d.x * d.x + d.y * d.y);

        out.points.pushBack(t == 1.0 ? p3 : Point{(float)(p0.x + at.x), (float)(p0.y + at.y)});
        out.kinds.pushBack(PointSmooth);
        out.dirs.pushBack(length > 0.0 ? Point{(float)(d.x / length), (float)(d.y / length)} : Point{0.f, 0.f});

        if (out.measured) {
            out.curveOf.pushBack((u32)out.curves.length() - 1);
            out.paramOf.pushBack(t);
        }
    }

    // A stroke's piece of a cubic from ta to tb, its points after ta: the
    // even steps of the tolerance, each end refined until its chord runs as
    // the cubic leaves it, and the points between where it bends sharply.
    // rate is the steps of the whole cubic. Where an end is the cubic's own
    // or a dash's cut, its direction is the tangent there.
    static void strokePiece(Flat& out, Point p0, Point p3, const Powers& q, double ta, double tb, bool cuspA, bool cuspB, double rate, float turn, float tol, bool tangentA, bool tangentB) {
        double span = tb - ta;
        double steps = ceil(rate * span);
        // At least a point between the ends, each then refining a chord of
        // its own.
        int n = steps >= 2.0 ? (steps < 512.0 ? (int)steps : 512) : 2;
        double step = span / n;
        int head = refinements(q, ta, step, 1.0, cuspA, turn, tol);
        int tail = refinements(q, tb, step, -1.0, cuspB, turn, tol);
        Vector<double> ts;

        ts.pushBack(ta);

        for (int k = head; k >= 1; k--) {
            ts.pushBack(ta + ldexp(step, -k));
        }

        for (int i = 1; i < n; i++) {
            ts.pushBack(ta + span * i / n);
        }

        for (int k = 1; k <= tail; k++) {
            ts.pushBack(tb - ldexp(step, -k));
        }

        ts.pushBack(tb);

        // The joins between the points keep within the tolerance of the
        // turn the offset allows, sqrt(8 turn).
        Vector<double> all;

        for (size_t i = 1; i < ts.length(); i++) {
            bendParameters(q, ts[i - 1], ts[i], sqrt(8.0 * turn), 24, all);
            all.pushBack(ts[i]);
        }

        double from = ta;
        size_t first = out.dirs.length();

        for (double t : all) {
            addCurvePoint(out, p0, p3, q, from, t);
            from = t;
        }

        // The cubic's own ends are corners, whose joins and caps go by its
        // tangents there, not by the chords near them.
        if (tangentA) {
            out.dirs.mut(first) = unitOf(leaving(q, ta, 1.0, false));
        }

        if (tangentB) {
            DPoint back = leaving(q, tb, -1.0, false);

            out.dirs.mutBack() = unitOf(DPoint{-back.x, -back.y});
        }
    }

    // The steps a cubic takes at the tolerance, from its second
    // differences.
    static double curveRate(const Point* p, float tol) {
        double ddx = fmax(fabs((double)p[0].x - 2.0 * p[1].x + p[2].x), fabs((double)p[1].x - 2.0 * p[2].x + p[3].x));
        double ddy = fmax(fabs((double)p[0].y - 2.0 * p[1].y + p[2].y), fabs((double)p[1].y - 2.0 * p[2].y + p[3].y));

        return sqrt(0.75 * sqrt(ddx * ddx + ddy * ddy) / tol);
    }

    // A stroke's cubic from ta to tb: its points after ta, their kinds and
    // the directions of the chords to them, the last a corner. It goes in
    // pieces between the points where its tangent turns back: the normals
    // there bound its sweep as the ends' do, and each piece refines its
    // chords at its ends, the range's own running along the tangents
    // there. A cubic along a line is straight between its cusps.
    static void strokeRange(Flat& out, const Point* p, double ta, double tb, float tol, float turn) {
        Powers q = powers(p[0], p[1], p[2], p[3]);
        double at[2];
        u8 kind;
        bool line;
        int count = turnPoints(q, at, kind, line);
        double rate = curveRate(p, tol);
        double cuts[4] = {ta, 0.0, 0.0, 0.0};
        int n = 1;

        for (int k = 0; k < count; k++) {
            if (at[k] > ta && at[k] < tb) {
                cuts[n++] = at[k];
            }
        }

        cuts[n++] = tb;

        for (int k = 0; k + 1 < n; k++) {
            bool first = k == 0;
            bool last = k + 2 == n;

            if (line) {
                addCurvePoint(out, p[0], p[3], q, cuts[k], cuts[k + 1]);
            } else {
                strokePiece(out, p[0], p[3], q, cuts[k], cuts[k + 1], !first && kind == PointCusp, !last && kind == PointCusp, rate, turn, tol, first, last);
            }

            out.kinds.mutBack() = last ? PointCorner : kind;
        }
    }

    static void strokeCurve(Flat& out, Point p0, Point p1, Point p2, Point p3, float tol, float turn) {
        Point controls[4] = {p0, p1, p2, p3};

        if (out.measured) {
            out.curves.pushBack(FlatCurve{{p0, p1, p2, p3}});
        }

        strokeRange(out, controls, 0.0, 1.0, tol, turn);
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

        // The point a verb adds for a stroke, unless a curve gave its own: a
        // corner, come to along the chord from the point before.
        auto mark = [&]() {
            if (turn > 0.f && out.kinds.length() < out.points.length()) {
                size_t k = out.points.length() - 1;
                Point before = k ? out.points[k - 1] : out.points[k];
                double dx = (double)out.points[k].x - before.x;
                double dy = (double)out.points[k].y - before.y;
                double length = sqrt(dx * dx + dy * dy);

                out.kinds.pushBack(PointCorner);
                out.dirs.pushBack(length > 0.0 ? Point{(float)(dx / length), (float)(dy / length)} : Point{0.f, 0.f});
            }

            while (out.measured && out.curveOf.length() < out.points.length()) {
                out.curveOf.pushBack(~0u);
                out.paramOf.pushBack(0.0);
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

                    if (turn > 0.f) {
                        strokeCurve(out, p0, p1, p2, p3, tol, turn);
                    } else {
                        flattenCubic(p0, p1, p2, p3, tol, out.points);
                    }

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

            mark();
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

    // Adds a dot of the stroke, a fan or a square, which turn the way the
    // outlines do.
    static void addPolygon(Flat& out, const Point* points, size_t count) {
        out.points.append(points, count);
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

    // The inner points of an arc about c from the angle from through sweep.
    static void addArcPoints(Vector<Point>& out, Point c, float radius, float from, float sweep, float tol) {
        int n = arcSegments(sweep, radius, tol);

        for (int i = 1; i < n; i++) {
            float a = from + sweep * i / n;

            out.pushBack(Point{c.x + radius * cosf(a), c.y + radius * sinf(a)});
        }
    }

    // The angle through which the outer normal n0 of a turn from d0 to d1
    // sweeps round to n1, the way the turn goes, and from where.
    static float normalSweep(Point n0, Point n1, Point d0, Point d1, float& from) {
        from = atan2f(n0.y, n0.x);

        float sweep = atan2f(n1.y, n1.x) - from;

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

        return sweep;
    }

    // The points of the join of a turn at p from d0 to d1 on its outer side,
    // between the ends of the two offset edges there: an arc, a miter's tip
    // or, for a bevel, none. A miter-clip past the limit is cut across its
    // bisector at the limit's half width times the limit from p, into the
    // bevel too where the limit is under 1.
    static void addJoin(Vector<Point>& out, const StrokeParams& s, Point p, Point d0, Point d1) {
        float cross = d0.x * d1.y - d0.y * d1.x;
        float dot = d0.x * d1.x + d0.y * d1.y;
        float side = cross > 0.f ? -1.f : 1.f;
        Point n0{-d0.y * side, d0.x * side};
        Point n1{-d1.y * side, d1.x * side};

        if (s.join == JoinRound) {
            float from;
            float sweep = normalSweep(n0, n1, d0, d1, from);

            addArcPoints(out, p, s.half, from, sweep, s.tol);

            return;
        }

        if (s.join == JoinMiter || s.join == JoinMiterClip) {
            float cosHalf = sqrtf(maxf(0.f, (1.f + dot) / 2.f));
            Point m{n0.x + n1.x, n0.y + n1.y};
            float length = sqrtf(m.x * m.x + m.y * m.y);

            // The normals do not cancel when the turn leaves a half angle.
            if (cosHalf > 1e-6f && 1.f / cosHalf <= s.miterLimit) {
                float reach = s.half / cosHalf;

                out.pushBack(Point{p.x + m.x / length * reach, p.y + m.y / length * reach});
            } else if (s.join == JoinMiterClip) {
                // A turn back has its miter run on along d0 for ever.
                Point u = cosHalf > 1e-6f ? Point{m.x / length, m.y / length} : d0;
                float limit = s.half * maxf(s.miterLimit, 0.f);
                float near = s.half * cosHalf;
                Point a{p.x + n0.x * s.half, p.y + n0.y * s.half};
                Point b{p.x + n1.x * s.half, p.y + n1.y * s.half};

                if (limit >= near) {
                    // Along each offset edge on from its end to the tip.
                    Point e0 = cosHalf > 1e-6f ? Point{d0.x, d0.y} : u;
                    Point e1 = cosHalf > 1e-6f ? Point{-d1.x, -d1.y} : u;
                    float along0 = (limit - near) / maxf(e0.x * u.x + e0.y * u.y, 1e-6f);
                    float along1 = (limit - near) / maxf(e1.x * u.x + e1.y * u.y, 1e-6f);

                    out.pushBack(Point{a.x + e0.x * along0, a.y + e0.y * along0});
                    out.pushBack(Point{b.x + e1.x * along1, b.y + e1.y * along1});
                } else {
                    // Back along each end towards p.
                    float f = 1.f - limit / near;

                    out.pushBack(Point{a.x + (p.x - a.x) * f, a.y + (p.y - a.y) * f});
                    out.pushBack(Point{b.x + (p.x - b.x) * f, b.y + (p.y - b.y) * f});
                }
            }
        }
    }

    // The join of a turn at p from d0 to d1 as a shape of its own: p, the
    // ends of the two offset edges on the outer side and the join between,
    // turning the way the outlines do.
    static void addJoinShape(Flat& out, const StrokeParams& s, Point p, Point d0, Point d1) {
        float side = d0.x * d1.y - d0.y * d1.x > 0.f ? -1.f : 1.f;
        Vector<Point> shape;

        shape.pushBack(p);
        shape.pushBack(Point{p.x - d0.y * s.half * side, p.y + d0.x * s.half * side});
        addJoin(shape, s, p, d0, d1);
        shape.pushBack(Point{p.x - d1.y * s.half * side, p.y + d1.x * s.half * side});

        // Where the turn goes the other way, so does the shape.
        if (side > 0.f) {
            for (size_t k = 0; k < shape.length() / 2; k++) {
                Point t = shape[k];

                shape.mut(k) = shape[shape.length() - 1 - k];
                shape.mut(shape.length() - 1 - k) = t;
            }
        }

        addPolygon(out, shape.data(), shape.length());
    }

    // The sector the inner end of a stroke's normal sweeps at a smooth point
    // p, as the outer end does from from through sweep, turned the way the
    // outlines do; within the tolerance of its arc, a triangle.
    static void addSector(Flat& out, const StrokeParams& s, Point p, float from, float sweep, bool arc) {
        float start = sweep > 0.f ? from + pi : from + pi + sweep;

        if (arc) {
            addFan(out, p, s.half, start, fabsf(sweep), s.tol);
        } else {
            Point sector[3] = {p, Point{p.x + s.half * cosf(start), p.y + s.half * sinf(start)}, Point{p.x + s.half * cosf(start + fabsf(sweep)), p.y + s.half * sinf(start + fabsf(sweep))}};

            addPolygon(out, sector, 3);
        }
    }

    // The points of the cap at the end p of a stroke going d, from its left
    // offset round to its right one.
    static void addCapPoints(Vector<Point>& out, const StrokeParams& s, Point p, Point d) {
        Point n{-d.y, d.x};

        if (s.cap == CapSquare) {
            out.pushBack(Point{p.x + n.x * s.half + d.x * s.half, p.y + n.y * s.half + d.y * s.half});
            out.pushBack(Point{p.x - n.x * s.half + d.x * s.half, p.y - n.y * s.half + d.y * s.half});
        } else if (s.cap == CapRound) {
            addArcPoints(out, p, s.half, atan2f(n.y, n.x), -pi, s.tol);
        }
    }

    static void addOutline(Flat& out, const Vector<Point>& points) {
        out.points.append(points.data(), points.length());
        out.ends.pushBack((u32)out.points.length());
        out.closed.pushBack(1);
    }

    static bool nonZero(Point d) {
        return d.x != 0.f || d.y != 0.f;
    }

    static Point direction(Point from, Point to) {
        return Point{to.x - from.x, to.y - from.y};
    }

    // The outline of a stroke of a polyline: its left side along the points,
    // the cap at the end, its right side back and the cap at the start; a
    // closed one has each side a loop. At a turn the outer side takes the
    // join between its two offset edges; the inner side takes the point
    // where its two offset edges cross, when that lies on both, else it runs
    // in to the turn's point and out again. The outline's winding is then
    // the count of the segments' quads, joins and caps over a point less
    // the inner corners where two quads overlap, at least one inside their
    // union and none outside: the nonzero rule fills exactly the union.
    // A cusp takes no join. At a smooth point, inside a curve, the stroke
    // sweeps its normal round the turn instead: its outer end arcs whatever
    // the join, and its inner end sweeps a sector of the same angle, a
    // polygon of its own where the inner offset edges do not cross.
    static void strokePolyline(Flat& out, const StrokeParams& s, const Point* points, const u8* kinds, const Point* into, size_t count, bool closed) {
        Vector<Point> pts;
        Vector<u8> kind;
        Vector<Point> came;

        // Each point after the first comes along the direction of its
        // segment, which may be shorter than the floats can tell; one come
        // along none, a line's after no length, goes, the corner before it
        // standing for it.
        for (size_t i = 0; i < count; i++) {
            if (pts.length() == 0 || into[i].x != 0.f || into[i].y != 0.f) {
                pts.pushBack(points[i]);
                kind.pushBack(kinds[i]);
                came.pushBack(into[i]);
            }
        }

        // A closed polyline comes back to its start along its last segment,
        // or where its last point is at the start, along that point's own.
        bool returned = closed && pts.length() > 1 && pts.back().x == pts[0].x && pts.back().y == pts[0].y;
        Point closing{0.f, 0.f};

        if (returned) {
            pts.popBack();
            kind.popBack();
            closing = came.popBack();
        }

        // A run of two or more points at one place: its caps make a dot, a
        // square one along the direction its first point is given, else
        // along x.
        if (pts.length() == 1) {
            if (s.cap == CapRound) {
                addFan(out, pts[0], s.half, 0.f, 2.f * pi, s.tol);
            } else if (s.cap == CapSquare) {
                Point p = pts[0];
                Point d = came[0].x != 0.f || came[0].y != 0.f ? came[0] : Point{1.f, 0.f};
                Point a{(d.x - d.y) * s.half, (d.y + d.x) * s.half};
                Point b{(d.x + d.y) * s.half, (d.y - d.x) * s.half};
                Point quad[4] = {Point{p.x - a.x, p.y - a.y}, Point{p.x + b.x, p.y + b.y}, Point{p.x + a.x, p.y + a.y}, Point{p.x - b.x, p.y - b.y}};

                addPolygon(out, quad, 4);
            }

            return;
        }

        size_t n = pts.length();
        size_t segments = closed ? n : n - 1;
        Vector<Point> dirs;
        Vector<float> lengths;

        for (size_t i = 0; i < segments; i++) {
            Point a = pts[i];
            Point b = pts[(i + 1) % n];
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float length = (float)sqrt((double)dx * dx + (double)dy * dy);

            dirs.pushBack(i + 1 < n ? came[i + 1] : (returned ? closing : Point{dx / length, dy / length}));
            lengths.pushBack(length);
        }

        // Each side of the stroke, the left one along the points and the
        // right one along them too, then reversed.
        Vector<Point> sides[2];

        auto offset = [&](Point p, Point d, float side) {
            return Point{p.x - d.y * s.half * side, p.y + d.x * s.half * side};
        };

        // How far a turn cuts its inner offset edges back to where they
        // cross, or -1 where its inner side is to run through the point.
        auto cutOf = [&](size_t k, size_t in, size_t next) {
            Point d0 = dirs[in];
            Point d1 = dirs[next];
            float cross = d0.x * d1.y - d0.y * d1.x;
            float dot = d0.x * d1.x + d0.y * d1.y;
            // The inner offset edges cross where each is cut back by half
            // tan(turn / 2); past either one's length the side runs through
            // the point instead. Where they cross, the two quads hold the
            // sector a smooth point sweeps.
            float back = 1.f + dot > 1e-6f ? s.half * fabsf(cross) / (1.f + dot) : lengths[in] + 1.f;

            // Inside a curve tighter than the half width, h times the turn
            // past a segment's length, the crossing lies beyond the centre of
            // the curve and the inner side would turn inside out: it runs
            // through the point, and the sector covers what it leaves.
            bool inverted = kind[k] == PointSmooth && s.half * fabsf(atan2f(cross, dot)) > fminf(lengths[in], lengths[next]);

            return !inverted && back <= lengths[in] && back <= lengths[next] ? back : -1.f;
        };

        Vector<float> cuts;
        Vector<u8> inners;

        for (size_t k = 0; k < n; k++) {
            Point d0 = dirs[k == 0 ? segments - 1 : k - 1];
            Point d1 = dirs[k == segments ? k - 1 : k];
            bool turns = closed || (k > 0 && k + 1 < n);

            cuts.pushBack(turns ? cutOf(k, k == 0 ? segments - 1 : k - 1, k) : -1.f);
            inners.pushBack(d0.x * d1.y - d0.y * d1.x > 0.f ? 0 : 1);
        }

        auto turn = [&](size_t k, size_t in, size_t next) {
            Point p = pts[k];
            Point d0 = dirs[in];
            Point d1 = dirs[next];
            float cross = d0.x * d1.y - d0.y * d1.x;
            float dot = d0.x * d1.x + d0.y * d1.y;
            // The right side is outer where the turn is to the left.
            int outer = cross > 0.f ? 1 : 0;
            int inner = 1 - outer;

            for (int i = 0; i < 2; i++) {
                sides[i].pushBack(offset(p, d0, i ? -1.f : 1.f));
            }

            if (fabsf(cross) < 1e-6f && dot > 0.f) {
                for (int i = 0; i < 2; i++) {
                    sides[i].pushBack(offset(p, d1, i ? -1.f : 1.f));
                }

                return;
            }

            float side = outer ? -1.f : 1.f;
            float from = 0.f;
            float sweep = 0.f;
            bool arc = false;

            if (kind[k] == PointSmooth) {
                sweep = normalSweep(Point{-d0.y * side, d0.x * side}, Point{-d1.y * side, d1.x * side}, d0, d1, from);

                // The chord between the offset edges' ends does where it
                // keeps to the tolerance of the arc, as the bends of a
                // curve's points mostly keep it.
                arc = s.half * (1.f - cosf(sweep / 2.f)) > s.tol;

                if (arc) {
                    addArcPoints(sides[outer], p, s.half, from, sweep, s.tol);
                }
            } else if (kind[k] == PointCorner) {
                addJoin(sides[outer], s, p, d0, d1);
            }

            sides[outer].pushBack(offset(p, d1, side));

            // A segment cut back at both its ends on one side keeps some
            // length between the cuts, else that side turns inside out
            // between them and its loop takes from the stroke.
            float back = cuts[k];
            size_t after = next + 1 == n ? 0 : next + 1;

            if (inners[in] == inner && cuts[in] >= 0.f && back + cuts[in] > lengths[in]) {
                back = -1.f;
            }

            if (inners[after] == inner && cuts[after] >= 0.f && back + cuts[after] > lengths[next]) {
                back = -1.f;
            }

            if (back >= 0.f) {
                Point q = sides[inner].back();

                sides[inner].mut(sides[inner].length() - 1) = Point{q.x - d0.x * back, q.y - d0.y * back};

                return;
            }

            sides[inner].pushBack(p);
            sides[inner].pushBack(offset(p, d1, -side));

            if (kind[k] == PointSmooth) {
                addSector(out, s, p, from, sweep, arc);
            }
        };

        // Turns at the inner points, and for a closed polyline at its first.
        for (size_t k = closed ? 0 : 1; k < (closed ? n : n - 1); k++) {
            turn(k, k == 0 ? segments - 1 : k - 1, k);
        }

        Vector<Point> outline;

        if (closed) {
            for (int i = 0; i < 2; i++) {
                outline.clear();

                for (size_t k = 0; k < sides[i].length(); k++) {
                    outline.pushBack(sides[i][i ? k : sides[i].length() - 1 - k]);
                }

                addOutline(out, outline);
            }

            return;
        }

        // An open one: the left side, the cap at the end, the right side
        // back and the cap at the start, the whole reversed.
        Point d0 = dirs[0];
        Point d1 = dirs[segments - 1];

        outline.pushBack(offset(pts[0], d0, 1.f));
        outline.append(sides[0].data(), sides[0].length());
        outline.pushBack(offset(pts[n - 1], d1, 1.f));
        addCapPoints(outline, s, pts[n - 1], d1);
        outline.pushBack(offset(pts[n - 1], d1, -1.f));

        for (size_t k = sides[1].length(); k > 0; k--) {
            outline.pushBack(sides[1][k - 1]);
        }

        outline.pushBack(offset(pts[0], d0, -1.f));
        addCapPoints(outline, s, pts[0], Point{-d0.x, -d0.y});

        for (size_t k = 0; k < outline.length() / 2; k++) {
            Point t = outline[k];

            outline.mut(k) = outline[outline.length() - 1 - k];
            outline.mut(outline.length() - 1 - k) = t;
        }

        addOutline(out, outline);
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

    // The length of a cubic from t0 to t1 by Gauss and Legendre's eight
    // points: a stroke's chord spans a short piece of it, nearly straight.
    static double arcLength(const Powers& q, double t0, double t1) {
        static const double nodes[4] = {0.1834346424956498, 0.5255324099163290, 0.7966664774136267, 0.9602898564975363};
        static const double weights[4] = {0.3626837833783620, 0.3137066458778873, 0.2223810344533745, 0.1012285362903763};
        double half = (t1 - t0) / 2.0;
        double sum = 0.0;

        for (int k = 0; k < 8; k++) {
            DPoint v = powerVelocity(q, (t0 + t1) / 2.0 + (k % 2 ? -half : half) * nodes[k / 2]);

            sum += weights[k / 2] * sqrt(v.x * v.x + v.y * v.y);
        }

        return 3.0 * half * sum;
    }

    // The parameter past t0, before t1, where a cubic has run length of
    // the whole between them: from its share of the parameters, Newton's
    // steps along the speed, kept between the two.
    static double lengthParameter(const Powers& q, double t0, double t1, double length, double whole) {
        double t = t0 + (t1 - t0) * (length / whole);

        for (int k = 0; k < 4; k++) {
            DPoint v = powerVelocity(q, t);

            t = fmin(fmax(t - (arcLength(q, t0, t) - length) / (3.0 * sqrt(v.x * v.x + v.y * v.y)), t0), t1);
        }

        return t;
    }

    static void dashPolyline(Flat& out, const StrokeParams& s, const Point* points, const u8* kinds, const Point* into, const u32* curveOf, const double* paramOf, const FlatCurve* curves, float turn, size_t count, bool closed, const float* dashes, u32 dashCount, float offset, const Box* cull) {
        Vector<Point> loop;
        Vector<u8> kind;
        Vector<Point> came;

        loop.append(points, count);
        kind.append(kinds, count);
        came.append(into, count);

        // A closed one comes back along the chord to its start, none where
        // it ends there.
        if (closed) {
            double dx = (double)points[0].x - points[count - 1].x;
            double dy = (double)points[0].y - points[count - 1].y;
            double length = sqrt(dx * dx + dy * dy);

            loop.pushBack(points[0]);
            kind.pushBack(PointCorner);
            came.pushBack(length > 0.0 ? Point{(float)(dx / length), (float)(dy / length)} : Point{0.f, 0.f});
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
            strokePolyline(out, s, points, kinds, into, count, closed);

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

            // The first entry the offset reaches the end of, a dash of no
            // length where it ends there (painting.md, TermDashPositions).
            while (phase > dashes[index]) {
                phase -= dashes[index];
                index = (index + 1) % dashCount;
            }

            float remaining = dashes[index] - phase;
            bool on = index % 2 == 0;
            // A dash's points, their kinds and the directions they come
            // along; its ends are cuts.
            Vector<Point> piece;
            Vector<u8> flags;
            Vector<Point> ways;

            if (on) {
                piece.pushBack(loop[0]);
                flags.pushBack(PointCorner);
                ways.pushBack(came[0]);
            }

            // The direction the polyline comes to a point along, past
            // segments of no length.
            Point incoming{0.f, 0.f};
            // A dash along a cubic takes its points from the cubic between
            // where it came on it, from, and where it leaves, flattened as a
            // stroke's cubic is, not from the chords it was measured on: a
            // cut close past one of their points would leave there a turn
            // too sharp for the step after it. run is that cubic, if any.
            u32 run = ~0u;
            double from = 0.0;
            auto leave = [&](u32 cubic, double ta, double tb, u8 last) {
                Flat span;

                strokeRange(span, curves[cubic].p, ta, tb, s.tol, turn);
                piece.append(span.points.data(), span.points.length());
                flags.append(span.kinds.data(), span.kinds.length());
                ways.append(span.dirs.data(), span.dirs.length());
                flags.mutBack() = last;
                run = ~0u;
            };

            for (size_t i = 0; i + 1 < loop.length(); i++) {
                Point a = loop[i];
                Point b = loop[i + 1];
                float dx = b.x - a.x;
                float dy = b.y - a.y;
                float length = (float)sqrt((double)dx * dx + (double)dy * dy);
                float travelled = 0.f;
                // A chord of a cubic is measured along the cubic, from t0,
                // and a dash cut on it there.
                u32 c = i + 1 < count ? curveOf[i + 1] : ~0u;
                const FlatCurve* curve = c != ~0u ? &curves[c] : nullptr;
                Powers q{};
                double t0 = 0.0;

                if (curve) {
                    q = powers(curve->p[0], curve->p[1], curve->p[2], curve->p[3]);
                    t0 = curveOf[i] == c ? paramOf[i] : 0.0;
                    length = (float)arcLength(q, t0, paramOf[i + 1]);
                }

                if (came[i].x != 0.f || came[i].y != 0.f) {
                    incoming = came[i];
                }

                while (length - travelled > remaining) {
                    travelled += remaining;

                    double at = 0.0;
                    Point cut{a.x + dx * travelled / length, a.y + dy * travelled / length};

                    if (curve) {
                        at = lengthParameter(q, t0, paramOf[i + 1], travelled, length);

                        DPoint along = powerAt(q, at);

                        cut = Point{(float)(curve->p[0].x + along.x), (float)(curve->p[0].y + along.y)};
                    }

                    if (on) {
                        // A dash that ends at the start of a segment ends
                        // along the one before, its cap that way, and takes
                        // the join there as the segment it ends in is the
                        // later one: a segment holds its start, not its end.
                        bool corner = travelled == 0.f;

                        if (curve && (run == c || !corner)) {
                            leave(c, run == c ? from : t0, at, PointCorner);
                        } else if (!corner) {
                            piece.pushBack(cut);
                            flags.pushBack(PointCorner);
                            ways.pushBack(came[i + 1]);
                        }

                        if (!strokeOutside(s, piece.data(), piece.length(), cull)) {
                            if (pass) {
                                strokePolyline(out, s, piece.data(), flags.data(), ways.data(), piece.length(), false);

                                // The direction come along is never none
                                // past a cut: a subpath with a cut has
                                // length, and its first point the direction
                                // of its first segment that has.
                                if (corner && piece.length() > 1 && kind[i] == PointCorner) {
                                    addJoinShape(out, s, a, incoming, came[i + 1]);
                                }
                            } else {
                                visible += 1.0;
                            }
                        }

                        piece.clear();
                        flags.clear();
                        ways.clear();
                    } else {
                        piece.pushBack(cut);
                        flags.pushBack(PointCorner);
                        ways.pushBack(came[i + 1]);
                        run = curve ? c : ~0u;
                        from = at;
                    }

                    on = !on;
                    index = (index + 1) % dashCount;
                    remaining = dashes[index];
                }

                remaining -= length - travelled;

                if (on && curve) {
                    from = run == c ? from : t0;
                    run = c;

                    if (paramOf[i + 1] == 1.0) {
                        leave(c, from, 1.0, kind[i + 1]);
                    }
                } else if (on) {
                    piece.pushBack(b);
                    flags.pushBack(kind[i + 1]);
                    ways.pushBack(came[i + 1]);
                }
            }

            // A dash on at the end holds its start and the end; a closed
            // polyline's ends at its start, where its first segment is the
            // later one, and takes the join there.
            if (on && !strokeOutside(s, piece.data(), piece.length(), cull)) {
                if (pass) {
                    Point last = came[loop.length() - 1];

                    strokePolyline(out, s, piece.data(), flags.data(), ways.data(), piece.length(), false);

                    if (closed) {
                        addJoinShape(out, s, loop[0], last.x != 0.f || last.y != 0.f ? last : incoming, came[1]);
                    }
                } else {
                    visible += 1.0;
                }
            }

            if (pass == 0 && visible * (4.0 + 2.0 * capPoints) > maxOutline) {
                strokePolyline(out, s, points, kinds, into, count, closed);

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
        float turn = maxf(s.tol / s.half, 1.f / 1024.f);

        flat.measured = style.dashCount > 0;
        flatten(path, identity(), s.tol, turn, flat);

        // A path past the finite numbers has no stroke, as browsers draw it:
        // the sum of a point's coordinates less itself is then no zero.
        for (const Point& p : flat.points) {
            float sum = p.x + p.y;

            if (!(sum - sum == 0.f)) {
                return;
            }
        }

        // The first point of each subpath is given the direction a dot
        // there goes: the subpath's first, else, as it has no length, the
        // path's last before it, a closing segment counting, or its first
        // after it (paths.md, TermPathSegmentStartDirection).
        Point lastEnd{0.f, 0.f};
        Point firstAny{0.f, 0.f};
        u32 begin = 0;

        for (size_t i = 0; i < flat.ends.length(); i++) {
            u32 end = flat.ends[i];
            Point first{0.f, 0.f};
            Point last{0.f, 0.f};
            Point back = direction(flat.points[end - 1], flat.points[begin]);
            double length = sqrt((double)back.x * back.x + (double)back.y * back.y);

            for (u32 k = begin + 1; k < end; k++) {
                if (nonZero(flat.dirs[k])) {
                    first = nonZero(first) ? first : flat.dirs[k];
                    last = flat.dirs[k];
                }
            }

            if (flat.closed[i] && length > 0.0) {
                last = Point{(float)(back.x / length), (float)(back.y / length)};
            }

            flat.dirs.mut(begin) = nonZero(first) ? first : lastEnd;
            firstAny = nonZero(firstAny) ? firstAny : first;
            lastEnd = nonZero(last) ? last : lastEnd;
            begin = end;
        }

        // Only subpaths with no length before them are left without.
        begin = 0;

        for (u32 end : flat.ends) {
            flat.dirs.mut(begin) = nonZero(flat.dirs[begin]) ? flat.dirs[begin] : firstAny;
            begin = end;
        }

        begin = 0;

        for (size_t i = 0; i < flat.ends.length(); i++) {
            u32 end = flat.ends[i];
            const Point* points = flat.points.data() + begin;
            const u8* kinds = flat.kinds.data() + begin;
            const Point* into = flat.dirs.data() + begin;
            size_t count = end - begin;

            if (count >= 2) {
                if (style.dashCount) {
                    dashPolyline(out, s, points, kinds, into, flat.curveOf.data() + begin, flat.paramOf.data() + begin, flat.curves.data(), turn, count, flat.closed[i], style.dashes, style.dashCount, style.dashOffset, cull);
                } else {
                    strokePolyline(out, s, points, kinds, into, count, flat.closed[i]);
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
        bool boxed = false;

        out.kind = ShapeKind::None;
        out.box = GeometryBox::Stroke;
        out.rule = WindingNonZero;
        out.autos = 0;
        out.points = nullptr;
        out.pointCount = 0;
        out.data = StringView();
        out.commands = nullptr;
        out.commandCount = 0;

        for (Length& radius : out.radii) {
            radius = Length{0.f, Unit::Number};
        }

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

                // The end of the value closes a function left open, as CSS
                // closes every block at its end.
                function = name;

                StringView args = trim(StringView(open, p));

                p += p < end;

                if (!parseShapeFunction(pool, name, args, out)) {
                    return false;
                }

                continue;
            }

            boxed = true;

            if (equalsIgnoreCase(name, SV("fill-box")) || equalsIgnoreCase(name, SV("content-box")) || equalsIgnoreCase(name, SV("padding-box"))) {
                out.box = GeometryBox::Fill;
            } else if (equalsIgnoreCase(name, SV("stroke-box")) || equalsIgnoreCase(name, SV("border-box")) || equalsIgnoreCase(name, SV("margin-box"))) {
                out.box = GeometryBox::Stroke;
            } else if (equalsIgnoreCase(name, SV("half-border-box"))) {
                out.box = GeometryBox::HalfBorder;
            } else if (equalsIgnoreCase(name, SV("view-box"))) {
                out.box = GeometryBox::View;
            } else {
                return false;
            }
        }

        // A box alone clips to that box.
        if (out.kind == ShapeKind::None && boxed) {
            out.kind = ShapeKind::Inset;

            for (Length& value : out.values) {
                value = Length{0.f, Unit::Number};
            }
        }

        return out.kind != ShapeKind::None;
    }

    // The radii after round: one to four horizontal ones, then after a
    // slash as many vertical ones, each set spread over the corners as
    // border-radius spreads it; the vertical ones are the horizontal ones
    // when left out.
    static bool parseRadii(const u8*& p, const u8* end, BasicShape& out) {
        Length sets[2][4];
        u32 counts[2] = {0, 0};
        u32 set = 0;

        for (;;) {
            StringView word = shapeWord(p, end);

            if (word.empty()) {
                break;
            }

            if (word == SV("/") && set == 0 && counts[0]) {
                set = 1;
                continue;
            }

            if (counts[set] == 4 || !parseLength(word, sets[set][counts[set]]) || sets[set][counts[set]].value < 0.f) {
                return false;
            }

            counts[set]++;
        }

        if (counts[0] == 0 || (set == 1 && counts[1] == 0)) {
            return false;
        }

        if (set == 0) {
            memcpy(sets[1], sets[0], sizeof(sets[0]));
            counts[1] = counts[0];
        }

        for (u32 k = 0; k < 2; k++) {
            Length* r = sets[k];
            u32 n = counts[k];
            Length corners[4] = {r[0], n > 1 ? r[1] : r[0], n > 2 ? r[2] : r[0], n > 3 ? r[3] : (n > 1 ? r[1] : r[0])};

            memcpy(out.radii + k * 4, corners, sizeof(corners));
        }

        return true;
    }

    // Four values, each a length or for rect() auto, then the radii.
    static bool parseSides(const u8*& p, const u8* end, BasicShape& out, bool autos) {
        for (u32 count = 0; count < 4; count++) {
            StringView word = shapeWord(p, end);

            if (autos && equalsIgnoreCase(word, SV("auto"))) {
                out.autos |= (u8)(1u << count);
                out.values[count] = Length{0.f, Unit::Number};
            } else if (!parseLength(word, out.values[count])) {
                return false;
            }
        }

        StringView word = shapeWord(p, end);

        return word.empty() || (equalsIgnoreCase(word, SV("round")) && parseRadii(p, end, out));
    }

    // The clip property's rect(): offsets from the top left of the box to
    // its top, right, bottom and left sides, each a length or auto, apart
    // by commas or by spaces. Unclosed, it closes at the end, as CSS reads
    // it.
    static bool parseClipRect(StringView text, BasicShape& out) {
        text = trim(text);

        if (text.length() < 5 || !equalsIgnoreCase(StringView(text.begin(), text.begin() + 5), SV("rect("))) {
            return false;
        }

        const u8* p = text.begin() + 5;
        const u8* end = text.end()[-1] == ')' ? text.end() - 1 : text.end();

        out.autos = 0;

        for (u32 side = 0; side < 4; side++) {
            StringView word = shapeWord(p, end);

            out.values[side] = Length{0.f, Unit::Number};

            if (equalsIgnoreCase(word, SV("auto"))) {
                out.autos |= (u8)(1u << side);
            } else if (!parseLength(word, out.values[side]) || out.values[side].unit == Unit::Percent) {
                return false;
            }
        }

        return shapeWord(p, end).empty();
    }

    static bool readAngle(StringView text, float& out, bool& unit);

    // A word of a shape() command: a run past spaces, or a slash alone.
    static StringView commandWord(const u8*& p, const u8* end) {
        skipSpaces(p, end);

        const u8* start = p;

        if (at(p, end, '/')) {
            p++;

            return StringView(start, p);
        }

        while (p < end && !isSpace(*p) && *p != '/') {
            p++;
        }

        return StringView(start, p);
    }

    // A word that ends a position in a command.
    static bool commandKeyword(StringView word) {
        static const char* const words[] = {"with", "of", "cw", "ccw", "large", "small", "rotate", "from", "/"};

        for (const char* keyword : words) {
            if (equalsIgnoreCase(word, StringView(keyword))) {
                return true;
            }
        }

        return word.empty();
    }

    // Two lengths, x then y.
    static bool commandPair(const u8*& p, const u8* end, ShapePoint& out) {
        return parseLength(commandWord(p, end), out.x) && parseLength(commandWord(p, end), out.y);
    }

    // A position's words, up to the keyword after them.
    static bool commandPosition(const u8*& p, const u8* end, ShapePoint& out) {
        const u8* first = p;
        const u8* last = p;

        for (;;) {
            const u8* q = p;

            if (commandKeyword(commandWord(q, end))) {
                break;
            }

            p = q;
            last = p;
        }

        out.anchor = ShapeAnchor::Box;

        return last != first && parsePosition(StringView(first, last), out.x, out.y);
    }

    // to a position, or by a pair from the start.
    static bool commandEnd(const u8*& p, const u8* end, ShapePoint& out, bool& to) {
        StringView word = commandWord(p, end);

        to = equalsIgnoreCase(word, SV("to"));
        out.anchor = ShapeAnchor::Start;

        if (to) {
            return commandPosition(p, end, out);
        }

        return equalsIgnoreCase(word, SV("by")) && commandPair(p, end, out);
    }

    // A control point: after to a position, or a pair from start, end or
    // the box's origin; after by a pair, from the start but as told.
    static bool commandControl(const u8*& p, const u8* end, ShapePoint& out, bool to) {
        const u8* start = p;

        if (to && commandPosition(p, end, out)) {
            const u8* q = p;

            if (!equalsIgnoreCase(commandWord(q, end), SV("from"))) {
                return true;
            }

            p = start;
        }

        if (!commandPair(p, end, out)) {
            return false;
        }

        const u8* q = p;

        out.anchor = ShapeAnchor::Start;

        if (!equalsIgnoreCase(commandWord(q, end), SV("from"))) {
            return true;
        }

        StringView anchor = commandWord(q, end);

        p = q;
        out.anchor = equalsIgnoreCase(anchor, SV("end")) ? ShapeAnchor::End : (equalsIgnoreCase(anchor, SV("origin")) ? ShapeAnchor::Box : ShapeAnchor::Start);

        return equalsIgnoreCase(anchor, SV("start")) || equalsIgnoreCase(anchor, SV("end")) || equalsIgnoreCase(anchor, SV("origin"));
    }

    // One command of shape(): all of its words, or false.
    static bool parseShapeCommand(StringView text, ShapeCommand& out) {
        const u8* p = text.begin();
        const u8* end = text.end();
        StringView name = commandWord(p, end);
        bool to = false;

        memset(&out, 0, sizeof(out));

        if (equalsIgnoreCase(name, SV("close"))) {
            out.kind = 'Z';
        } else if (equalsIgnoreCase(name, SV("move")) || equalsIgnoreCase(name, SV("line"))) {
            out.kind = equalsIgnoreCase(name, SV("move")) ? 'M' : 'L';

            if (!commandEnd(p, end, out.end, to)) {
                return false;
            }
        } else if (equalsIgnoreCase(name, SV("hline")) || equalsIgnoreCase(name, SV("vline"))) {
            bool across = equalsIgnoreCase(name, SV("hline"));
            StringView how = commandWord(p, end);
            StringView word = commandWord(p, end);
            Length& value = across ? out.end.x : out.end.y;
            float at = -1.f;

            out.kind = across ? 'H' : 'V';
            to = equalsIgnoreCase(how, SV("to"));
            out.end.anchor = to ? ShapeAnchor::Box : ShapeAnchor::Start;

            // The keywords of the axis, to the box's edges or middle.
            if (to) {
                StringView low[2] = {across ? SV("left") : SV("top"), across ? SV("x-start") : SV("y-start")};
                StringView high[2] = {across ? SV("right") : SV("bottom"), across ? SV("x-end") : SV("y-end")};

                at = equalsIgnoreCase(word, low[0]) || equalsIgnoreCase(word, low[1]) ? 0.f : at;
                at = equalsIgnoreCase(word, high[0]) || equalsIgnoreCase(word, high[1]) ? 100.f : at;
                at = equalsIgnoreCase(word, SV("center")) ? 50.f : at;
            }

            if (at >= 0.f) {
                value = Length{at, Unit::Percent};
            } else if (!(to || equalsIgnoreCase(how, SV("by"))) || !parseLength(word, value)) {
                return false;
            }
        } else if (equalsIgnoreCase(name, SV("curve")) || equalsIgnoreCase(name, SV("smooth"))) {
            bool curve = equalsIgnoreCase(name, SV("curve"));

            out.kind = curve ? 'C' : 'S';

            if (!commandEnd(p, end, out.end, to)) {
                return false;
            }

            const u8* q = p;

            if (equalsIgnoreCase(commandWord(q, end), SV("with"))) {
                p = q;

                if (!commandControl(p, end, out.control[0], to)) {
                    return false;
                }

                out.controls = 1;
                q = p;

                if (curve && equalsIgnoreCase(commandWord(q, end), SV("/"))) {
                    p = q;

                    if (!commandControl(p, end, out.control[1], to)) {
                        return false;
                    }

                    out.controls = 2;
                }
            } else if (curve) {
                return false;
            }
        } else if (equalsIgnoreCase(name, SV("arc"))) {
            u32 seen = 0;

            out.kind = 'A';

            if (!commandEnd(p, end, out.end, to)) {
                return false;
            }

            // of the radii, a sweep, a size and rotate an angle, in any
            // order, each once; of is not left out.
            for (StringView word = commandWord(p, end); !word.empty(); word = commandWord(p, end)) {
                u32 part = equalsIgnoreCase(word, SV("of")) ? 1 : (equalsIgnoreCase(word, SV("cw")) || equalsIgnoreCase(word, SV("ccw")) ? 2 : (equalsIgnoreCase(word, SV("large")) || equalsIgnoreCase(word, SV("small")) ? 4 : (equalsIgnoreCase(word, SV("rotate")) ? 8 : 0)));

                if (!part || (seen & part)) {
                    return false;
                }

                seen |= part;
                out.sweep = out.sweep || equalsIgnoreCase(word, SV("cw"));
                out.large = out.large || equalsIgnoreCase(word, SV("large"));

                if (part == 1) {
                    const u8* q = p;

                    if (!parseLength(commandWord(p, end), out.radius[0])) {
                        return false;
                    }

                    out.radii = 1;
                    q = p;

                    if (parseLength(commandWord(q, end), out.radius[1])) {
                        p = q;
                        out.radii = 2;
                    }
                } else if (part == 8) {
                    bool unit;

                    // An angle takes a unit, zero too, as browsers read it.
                    if (!readAngle(commandWord(p, end), out.angle, unit) || !unit) {
                        return false;
                    }
                }
            }

            if (!(seen & 1)) {
                return false;
            }
        } else {
            return false;
        }

        skipSpaces(p, end);

        return p == end;
    }

    // shape([<fill-rule>] from <position>, <command>#): a move to the
    // start, then the commands apart by commas.
    static bool parseShapeCommands(ObjPool& pool, StringView args, BasicShape& out) {
        const u8* p = args.begin();
        const u8* end = args.end();
        const u8* q = p;
        StringView word = commandWord(q, end);
        Vector<ShapeCommand> commands;
        ShapeCommand command;

        out.kind = ShapeKind::Commands;

        if (equalsIgnoreCase(word, SV("evenodd")) || equalsIgnoreCase(word, SV("nonzero"))) {
            out.rule = equalsIgnoreCase(word, SV("evenodd")) ? WindingEvenOdd : WindingNonZero;
            p = q;
        }

        const u8* comma = findOutside(p, end, ',');

        memset(&command, 0, sizeof(command));
        command.kind = 'M';

        if (!comma || !equalsIgnoreCase(commandWord(p, comma), SV("from")) || !commandPosition(p, comma, command.end)) {
            return false;
        }

        skipSpaces(p, comma);

        if (p != comma) {
            return false;
        }

        commands.pushBack(command);

        while (comma) {
            p = comma + 1;
            comma = findOutside(p, end, ',');

            if (!parseShapeCommand(StringView(p, comma ? comma : end), command)) {
                return false;
            }

            commands.pushBack(command);
        }

        out.commands = (ShapeCommand*)pool.allocate(sizeof(ShapeCommand) * commands.length());
        out.commandCount = (u32)commands.length();
        memcpy(out.commands, commands.data(), sizeof(ShapeCommand) * commands.length());

        return true;
    }

    static bool parseShapeFunction(ObjPool& pool, StringView name, StringView args, BasicShape& out) {
        const u8* end = args.end();

        if (equalsIgnoreCase(name, SV("shape"))) {
            return parseShapeCommands(pool, args, out);
        }

        if (equalsIgnoreCase(name, SV("rect")) || equalsIgnoreCase(name, SV("xywh"))) {
            const u8* p = args.begin();
            bool rect = equalsIgnoreCase(name, SV("rect"));

            out.kind = rect ? ShapeKind::Rect : ShapeKind::Xywh;

            return parseSides(p, end, out, rect) && (rect || (out.values[2].value >= 0.f && out.values[3].value >= 0.f));
        }

        // path([<fill-rule>,] <string>): the data in the reference box.
        if (equalsIgnoreCase(name, SV("path"))) {
            const u8* p = args.begin();
            const u8* comma = p;

            out.kind = ShapeKind::Path;

            while (comma < end && *comma != ',' && *comma != '"' && *comma != '\'') {
                comma++;
            }

            if (comma < end && *comma == ',') {
                StringView rule = trim(StringView(p, comma));

                if (!equalsIgnoreCase(rule, SV("evenodd")) && !equalsIgnoreCase(rule, SV("nonzero"))) {
                    return false;
                }

                out.rule = equalsIgnoreCase(rule, SV("evenodd")) ? WindingEvenOdd : WindingNonZero;
                p = comma + 1;
            }

            StringView quoted = trim(StringView(p, end));

            if (quoted.length() < 2 || (quoted[0] != '"' && quoted[0] != '\'') || quoted[quoted.length() - 1] != quoted[0]) {
                return false;
            }

            out.data = StringView(quoted.begin() + 1, quoted.end() - 1);

            return true;
        }

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

                if (word.empty()) {
                    break;
                }

                if (equalsIgnoreCase(word, SV("round"))) {
                    if (!parseRadii(p, end, out)) {
                        return false;
                    }

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
        style.clipRect = nullptr;
        style.nonScaling = false;
        style.vectorEffects = 0;
        style.blend = 0;
        style.isolate = false;
        style.newBackground = false;
        style.backgroundClipped = false;
        style.outlineWidth = 0.f;
        style.zoom = 1.f;
        memset(style.maskParts, 0, sizeof(style.maskParts));
        memset(style.maskBorder, 0, sizeof(style.maskBorder));
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
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomClip && !(property >= AtomMaskType && property <= AtomMaskBorderRepeat) && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation && property != AtomEnableBackground && property != AtomOutline && property != AtomZoom;

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
            u32 atom = l.instance && l.instance[node->index] ? l.instance[node->index][property - firstProperty] : node->style[property - firstProperty];
            StringView value = atom ? textOf(doc, atom) : StringView();

            if (value.empty() || property == AtomFontSize) {
                continue;
            }

            bool inherit = isKeyword(value, SV("inherit"));
            bool reset = isKeyword(value, SV("initial"));

            if (isKeyword(value, SV("unset"))) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomClip && !(property >= AtomMaskType && property <= AtomMaskBorderRepeat) && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation && property != AtomEnableBackground && property != AtomOutline && property != AtomZoom;

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
                    } else if (isKeyword(value, SV("miter"))) {
                        style.join = JoinMiter;
                    } else if (isKeyword(value, SV("miter-clip")) || isKeyword(value, SV("arcs"))) {
                        // arcs as miter-clip, which it is on straight
                        // segments (tst/spec/decisions.md, D4).
                        style.join = JoinMiterClip;
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

                        // A limit under 1 is valid in SVG 2, every miter
                        // past it (painting.md, StrokeMiterlimitProperty).
                        if (parseNumberText(value, limit) && limit >= 0.f) {
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
                    } else if (isKeyword(value, SV("force-hidden"))) {
                        style.forceHidden = true;
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
                case AtomClip: {
                    BasicShape rect;

                    if (keyword) {
                        style.clipRect = source.clipRect;
                    } else if (isKeyword(value, SV("auto"))) {
                        style.clipRect = nullptr;
                    } else if (parseClipRect(value, rect)) {
                        style.clipRect = l.pool->make<BasicShape>(rect);
                    }

                    break;
                }
                case AtomClipPathProperty: {
                    if (keyword) {
                        style.clipPath = source.clipPath;
                        style.clipShape = source.clipShape;

                        break;
                    }

                    PaintValue reference;
                    BasicShape shape;

                    if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        style.clipPath = reference.server;
                    } else if (isKeyword(value, SV("none"))) {
                        style.clipPath = 0;
                    } else if (parseBasicShape(*l.pool, value, shape)) {
                        style.clipShape = l.pool->make<BasicShape>(shape);
                    }

                    break;
                }
                case AtomMaskImage:
                case AtomMaskMode:
                case AtomMaskRepeat:
                case AtomMaskPosition:
                case AtomMaskClip:
                case AtomMaskOrigin:
                case AtomMaskSize:
                case AtomMaskComposite:
                    style.maskParts[property - AtomMaskImage] = keyword ? source.maskParts[property - AtomMaskImage] : atom;
                    break;
                case AtomMaskBorderSource:
                case AtomMaskBorderMode:
                case AtomMaskBorderSlice:
                case AtomMaskBorderWidth:
                case AtomMaskBorderOutset:
                case AtomMaskBorderRepeat:
                    style.maskBorder[property - AtomMaskBorderSource] = keyword ? source.maskBorder[property - AtomMaskBorderSource] : atom;
                    break;
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
                    // The blend modes, then plus-lighter, which adds.
                    static const char* const modes[] = {"normal", "multiply", "screen", "overlay", "darken", "lighten", "color-dodge", "color-burn", "hard-light", "soft-light", "difference", "exclusion", "hue", "saturation", "color", "luminosity", "plus-lighter"};

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
                case AtomVectorEffect: {
                    // none, or any effects once each; non-scaling-size keeps
                    // the stroke as non-scaling-stroke does.
                    static const char* const effects[] = {"non-scaling-stroke", "non-scaling-size", "non-rotation", "fixed-position"};
                    const u8* p = value.begin();
                    u8 buffer[64];
                    u32 seen = 0;
                    bool valid = true;

                    if (keyword || isKeyword(value, SV("none"))) {
                        style.nonScaling = keyword && source.nonScaling;
                        style.vectorEffects = keyword ? source.vectorEffects : 0;
                        break;
                    }

                    for (StringView word = mediaWord(p, value.end(), buffer); !word.empty(); word = mediaWord(p, value.end(), buffer)) {
                        u32 bit = 0;

                        for (u32 k = 0; k < 4; k++) {
                            bit = word == StringView(effects[k]) ? 1u << k : bit;
                        }

                        valid = valid && bit && !(seen & bit);
                        seen |= bit;
                    }

                    skipSpaces(p, value.end());

                    if (valid && p == value.end()) {
                        style.nonScaling = (seen & 3) != 0;
                        style.vectorEffects = (u8)(seen >> 1);
                    }

                    break;
                }
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

        style.visible = style.visible && !style.forceHidden;

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

    static void emitFill(Lowering& l, const Flat& flat, u8 rule, DevicePaint* paint, bool aliased, bool linear = false) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Fill;
        op.rule = rule;
        op.aliased = aliased;
        op.linear = linear;
        op.paint = paint;
        op.first = (u32)l.edges.length();
        op.bounds = emptyBox();
        emitEdges(l, flat, op.bounds);
        op.count = (u32)l.edges.length() - op.first;

        if (op.count && boxValid(op.bounds)) {
            if (paint->type == PaintType::Pattern) {
                paint->sampled = op.bounds;
            }

            if (l.patternDepth == 0 && (paint->type == PaintType::Pattern || paint->type == PaintType::Image)) {
                paint->once = true;
            }

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
        storeColor(paint->color16, paint->color);

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

    // What the clip property leaves of a box of the given size, as offsets
    // from its top left: a side auto is the box's own. False for no clip.
    static bool clipCut(const ComputedStyle& style, float w, float h, Viewport vp, Box& out) {
        if (!style.clipRect) {
            return false;
        }

        const BasicShape& rect = *style.clipRect;
        float sides[4];

        for (u32 side = 0; side < 4; side++) {
            sides[side] = resolveLength(rect.values[side], Axis::Other, vp, style.fontSize);
        }

        out = Box{rect.autos & 8 ? 0.f : sides[3], rect.autos & 1 ? 0.f : sides[0], rect.autos & 2 ? w : sides[1], rect.autos & 4 ? h : sides[2]};

        return true;
    }

    static Op groupOp(float opacity);
    static void addRectClip(Lowering& l, const Matrix& m, float x, float y, float w, float h);
    static void endGroup(Lowering& l, u32 begin);

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

        if (transform) {
            parseTransform(textOf(doc, transform), local, TransformGrammar::Either);

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
        // Content taken from a template is cloned under the pattern, and
        // inherits from it (pservers.md, PaintServerTemplates).
        const ComputedStyle& contentStyle = documentStyle(l, pattern, vp);

        Matrix outerHost = l.host;
        Matrix screenInverse;

        u32 outerWidth = l.width;
        u32 outerHeight = l.height;

        l.out = &program;
        l.active.pushBack(pattern);
        l.host = invert(l.screen, screenInverse) ? multiply(multiply(scaling(tw / w, th / h), inner), screenInverse) : identity();
        l.width = paint->imageWidth;
        l.height = paint->imageHeight;
        l.patternDepth++;

        // The clip property cuts the tile at its offsets.
        Box cut;
        bool cutting = clipCut(documentStyle(l, pattern, vp), w, h, vp, cut);
        u32 cutIndex = (u32)l.out->length();

        if (cutting) {
            Op op = groupOp(1.f);

            op.clipped = true;
            op.clipFirst = (u32)l.clips.length();
            op.clipCount = 1;
            addRectClip(l, scaling(tw / w, th / h), cut.x0, cut.y0, maxf(cut.x1 - cut.x0, 0.f), maxf(cut.y1 - cut.y0, 0.f));
            l.out->pushBack(op);
        }

        lowerChildren(l, content, contentStyle, multiply(scaling(tw / w, th / h), inner), vp);

        if (cutting) {
            endGroup(l, cutIndex);
        }

        l.patternDepth--;
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

    // The paint of a gradient, none where it paints nothing; invalid where
    // it is in error, no paint server then.
    static DevicePaint* gradientPaint(Lowering& l, Node* gradient, float opacity, Box bbox, const Matrix& ctm, Viewport vp, bool& invalid) {
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

                // Stops taken from a template are cloned under the gradient,
                // and inherit from it (pservers.md, PaintServerTemplates).
                ComputedStyle style = stopsNode == gradient ? documentStyle(l, child, vp) : computeStyle(l, child, documentStyle(l, gradient, vp), vp);
                Color color = style.stopColor;

                color.a *= style.stopOpacity * opacity;
                stops.pushBack(GradientStop{offset, color, {}});
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

            parseTransform(textOf(doc, transform), m, TransformGrammar::Either);

            if (u32 origin = attribute(gradient, AtomTransformOrigin)) {
                Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

                m = multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
            }

            gradientMatrix = m;
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
            paint->fr = gradientLength(l, gradient, AtomFr, boxUnits, Axis::Other, vp, 0.f);

            // A negative radius is an error (pservers.md,
            // RadialGradientElementRAttribute), the gradient no paint server.
            if (paint->r < 0.f || paint->fr < 0.f) {
                invalid = true;

                return nullptr;
            }

            if (paint->r <= 1.f / 32768.f) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }

            float apart = hypotf(paint->cx - paint->fx, paint->cy - paint->fy);

            // A start circle over the whole end circle draws nothing; a focal
            // circle touching the end circle from inside paints past it.
            if (apart + paint->r <= paint->fr * (1.f + 1e-6f)) {
                return nullptr;
            }

            paint->beyond = paint->r > paint->fr && fabsf(apart - (paint->r - paint->fr)) <= 1e-6f * paint->r;
        }

        for (u32 i = 0; i < paint->stopCount; i++) {
            storeColor(paint->stops[i].stored, paint->stops[i].color);
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
        Matrix m = identity();
        Matrix inverse;

        if (value) {
            parseTransform(textOf(doc, value), m, TransformGrammar::Either);
        }

        return !invert(m, inverse);
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

    // The side of (px, py) from the edge u v, taken from the edge's points in
    // one order whichever triangle holds it: two triangles sharing an edge
    // see exactly opposite numbers, so no pixel centre falls between them.
    static float edgeSide(Point u, Point v, float px, float py) {
        bool swap = v.x < u.x || (v.x == u.x && v.y < u.y);
        Point s = swap ? v : u;
        Point t = swap ? u : v;
        float side = (t.x - s.x) * (py - s.y) - (t.y - s.y) * (px - s.x);

        return swap ? -side : side;
    }

    // A centre on an edge goes to one of the two triangles: the one that,
    // turned the positive way, runs the edge downwards, or rightwards when
    // the edge is level.
    static bool edgeOwns(Point u, Point v, float turn) {
        float dx = (v.x - u.x) * turn;
        float dy = (v.y - u.y) * turn;

        return dy > 0.f || (dy == 0.f && dx > 0.f);
    }

    static void meshTriangle(u16* buffer, int width, int height, const MeshVertex& a, const MeshVertex& b, const MeshVertex& c) {
        float area = (b.p.x - a.p.x) * (c.p.y - a.p.y) - (c.p.x - a.p.x) * (b.p.y - a.p.y);

        if (fabsf(area) < 1e-12f) {
            return;
        }

        float turn = area > 0.f ? 1.f : -1.f;

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
                float ea = edgeSide(b.p, c.p, px, py) * turn;
                float eb = edgeSide(c.p, a.p, px, py) * turn;
                float ec = edgeSide(a.p, b.p, px, py) * turn;

                if (ea < 0.f || eb < 0.f || ec < 0.f || (ea == 0.f && !edgeOwns(b.p, c.p, turn)) || (eb == 0.f && !edgeOwns(c.p, a.p, turn)) || (ec == 0.f && !edgeOwns(a.p, b.p, turn))) {
                    continue;
                }

                float sum = ea + eb + ec;
                float d[4];

                for (int k = 0; k < 4; k++) {
                    d[k] = (ea * a.c[k] + eb * b.c[k] + ec * c.c[k]) / sum;
                }

                storePixel(buffer + ((size_t)y * width + x) * 4, d);
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

        // A mesh's gradientTransform comes from its attribute alone, which
        // the parser keeps only when it reads.
        if (u32 transform = meshAttribute(doc, mesh, AtomGradientTransform)) {
            Matrix m = identity();

            parseTransform(textOf(doc, transform), m, TransformGrammar::Either);
            space = multiply(space, m);
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
        u16* buffer = (u16*)l.pool->allocate(sizeof(u16) * (size_t)width * height * 4);
        Matrix local = multiply(translation(-(float)x0, -(float)y0), space);

        memset(buffer, 0, sizeof(u16) * (size_t)width * height * 4);

        // Patches sharing an edge cut it at the same points, or a pixel could
        // fall between their two chords: a row of patches takes one count down
        // its sides, a column one along its tops and bottoms.
        Vector<int> along;
        Vector<int> down;

        along.zero(columns);
        down.zero(rows);

        for (u32 r = 0; r < rows; r++) {
            for (u32 c = 0; c < columns; c++) {
                const Point* edges[4] = {horizontal.data() + ((size_t)r * columns + c) * 4, horizontal.data() + ((size_t)(r + 1) * columns + c) * 4, vertical.data() + ((size_t)r * stride + c) * 4, vertical.data() + ((size_t)r * stride + c + 1) * 4};
                float extent = 0.f;

                for (int k = 0; k < 3; k++) {
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

                along.mutData()[c] = along[c] > n ? along[c] : n;
                down.mutData()[r] = down[r] > n ? down[r] : n;
            }
        }

        for (u32 r = 0; r < rows; r++) {
            for (u32 c = 0; c < columns; c++) {
                const Point* top = horizontal.data() + ((size_t)r * columns + c) * 4;
                const Point* bottom = horizontal.data() + ((size_t)(r + 1) * columns + c) * 4;
                const Point* left = vertical.data() + ((size_t)r * stride + c) * 4;
                const Point* right = vertical.data() + ((size_t)r * stride + c + 1) * 4;
                Color corner[4] = {colors[r * stride + c], colors[r * stride + c + 1], colors[(r + 1) * stride + c], colors[(r + 1) * stride + c + 1]};
                Color smooth[81];

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

                int nu = along[c];
                int nv = down[r];
                Vector<MeshVertex> grid;

                grid.zero((size_t)(nv + 1) * (nu + 1));

                for (int a = 0; a <= nv; a++) {
                    float v = (float)a / (float)nv;
                    Point l0 = bezierAt(left, v);
                    Point r0 = bezierAt(right, v);

                    for (int b = 0; b <= nu; b++) {
                        float u = (float)b / (float)nu;
                        Point t0 = bezierAt(top, u);
                        Point b0 = bezierAt(bottom, u);
                        Point p{
                            (1.f - v) * t0.x + v * b0.x + (1.f - u) * l0.x + u * r0.x - ((1.f - u) * (1.f - v) * top[0].x + u * (1.f - v) * top[3].x + (1.f - u) * v * bottom[0].x + u * v * bottom[3].x),
                            (1.f - v) * t0.y + v * b0.y + (1.f - u) * l0.y + u * r0.y - ((1.f - u) * (1.f - v) * top[0].y + u * (1.f - v) * top[3].y + (1.f - u) * v * bottom[0].y + u * v * bottom[3].y),
                        };

                        // On its sides a patch is its edges, and its corners
                        // the mesh's: the sum above cancels in floats only
                        // nearly, differently in the patch across.
                        bool side = b == 0 || b == nu;

                        if (side && (a == 0 || a == nv)) {
                            p = corners[(r + (a == nv)) * stride + c + (b == nu)];
                        } else if (side) {
                            p = b == 0 ? l0 : r0;
                        } else if (a == 0 || a == nv) {
                            p = a == 0 ? t0 : b0;
                        }
                        MeshVertex& vertex = grid.mutData()[(size_t)a * (nu + 1) + b];
                        float w[4] = {(1.f - u) * (1.f - v), u * (1.f - v), (1.f - u) * v, u * v};
                        float color[4] = {0.f, 0.f, 0.f, 0.f};

                        for (int k = 0; k < 4; k++) {
                            color[0] += corner[k].r * w[k];
                            color[1] += corner[k].g * w[k];
                            color[2] += corner[k].b * w[k];
                            color[3] += corner[k].a * w[k];
                        }

                        if (bicubic) {
                            int x = b == nu ? 7 : b * 8 / nu;
                            int y = a == nv ? 7 : a * 8 / nv;
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

                for (int a = 0; a < nv; a++) {
                    for (int b = 0; b < nu; b++) {
                        const MeshVertex& p00 = grid[(size_t)a * (nu + 1) + b];
                        const MeshVertex& p01 = grid[(size_t)a * (nu + 1) + b + 1];
                        const MeshVertex& p10 = grid[(size_t)(a + 1) * (nu + 1) + b];
                        const MeshVertex& p11 = grid[(size_t)(a + 1) * (nu + 1) + b + 1];

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

                // A paint server is used whatever its display and its
                // conditions, or its ancestors' (pservers.md, Introduction).
                if ((gradient || pattern) && !singularTransform(*l.doc, server)) {
                    bool invalid = false;
                    DevicePaint* paint = gradient ? gradientPaint(l, server, opacity, bbox, ctm, vp, invalid) : patternPaint(l, server, opacity, bbox, ctm, vp);

                    // A gradient that paints nothing paints nothing; one in
                    // error, a pattern that cannot draw, or a box needed and
                    // missing, is no paint server: the fallback goes instead
                    // (tst/spec/decisions.md, D8).
                    if (paint || (gradient && !invalid && !degenerateBox(*l.doc, server, bbox))) {
                        return paint;
                    }
                }

                if (server && server->name == AtomMeshGradient) {
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
        // Shapes count their strokes as the stroke bounding box has them,
        // in boxes that are not tight.
        bool stroke;
    };

    static void elementExtent(Pass& p, Node* node, const ComputedStyle& parent, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box);
    static void markerExtent(Pass& p, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box);
    static bool imageReachable(const Document& doc, const Node* node);
    static bool isAuto(const Document& doc, const Node* node, u32 name);
    static bool imageRect(Pass& p, Node* node, const ComputedStyle& style, Viewport vp, ImageSource& source, float& x, float& y, float& w, float& h);
    static void releaseImage(ImageSource& source);
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

    // How far a stroke grows a shape's box, as CSS Masking computes the
    // stroke bounding box: by half its width, for a shape other than a
    // rectangle, circle or ellipse times a miter join's limit, though the
    // root of two at least for a square cap, or that root for a square cap
    // alone. A miter-clip join, and arcs drawn as one, reach as far as a
    // miter; a limit under one counts as one, past which no miter shows.
    static float strokeGrowth(const Node* node, const ComputedStyle& style) {
        float half = style.strokeWidth / 2.f;
        float root2 = 1.41421356f;

        if (style.stroke.kind == PaintKind::None || !(half > 0.f)) {
            return 0.f;
        }

        if (node->name == AtomRect || node->name == AtomCircle || node->name == AtomEllipse) {
            return half;
        }

        if (style.join == JoinMiter || style.join == JoinMiterClip) {
            return half * (style.miterLimit < root2 && style.cap == CapSquare ? root2 : maxf(style.miterLimit, 1.f));
        }

        return style.cap == CapSquare ? half * root2 : half;
    }

    // Adds to a box, in the space a matrix maps to, what an element draws:
    // the exact bounds of its shape, the rectangle of its image, or its
    // children placed in it. It is a box of geometry: hidden content counts,
    // clipped content counts whole, filters and masks do not, strokes only
    // when asked.
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

            // A built path begins with a move, so its box is never empty;
            // a stroke grows it in the shape's own space, as it is.
            Box shape = pathBox(path, m);
            float grow = how.stroke ? strokeGrowth(node, style) : 0.f;

            shape = Box{shape.x0 - grow, shape.y0 - grow, shape.x1 + grow, shape.y1 + grow};

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

                // Measuring takes the size alone, unless the pass draws the image too.
                if (!p.draws) {
                    releaseImage(source);
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

                if ((!targetStyle.displayNone || !svg) && passesConditions(doc, target)) {
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

        elementExtent(p, node, parent, identity(), vp, Extent{false, false, false}, depth, box);
        out = box;

        return boxValid(box);
    }

    // The stroke bounding box of an element of geometry, in its own user
    // space: its shapes' boxes grown by their strokes, mapped up as boxes.
    static Box strokeBox(Pass& p, Node* node, const ComputedStyle& parent, Viewport vp) {
        Box box = emptyBox();

        elementExtent(p, node, parent, identity(), vp, Extent{false, false, true}, 0, box);

        return box;
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

        if (!clip || clip->name != AtomClipPath) {
            return false;
        }

        for (Node* active : l.active) {
            if (active == clip) {
                return false;
            }
        }

        // A use in it names a shape or text itself; one naming anything
        // else is an error that drops the clipPath whole.
        for (Node* child = clip->first; child; child = child->next) {
            Node* target = child->name == AtomUse ? hrefTarget(doc, child) : nullptr;

            if (target && !isShape(target->name) && target->name != AtomText) {
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

            // Bounding box units on an element with no width or height
            // ignore the clip (coords.md, ObjectBoundingBoxUnits).
            if (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0) {
                return false;
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

    // A rectangle with elliptical corners, the radii resolved against the
    // reference box and all scaled down alike where two on a side pass it.
    static void roundedRectPath(Path& path, float x0, float y0, float x1, float y1, const Length* radii, float w, float h, Viewport vp, float fs) {
        float rx[4];
        float ry[4];
        float width = x1 - x0;
        float height = y1 - y0;
        float scale = 1.f;

        for (int k = 0; k < 4; k++) {
            rx[k] = shapeLength(radii[k], w, vp, fs);
            ry[k] = shapeLength(radii[4 + k], h, vp, fs);
        }

        // The corners by sides: top 0 1, right 1 2, bottom 2 3, left 3 0.
        for (int k = 0; k < 4; k++) {
            float sum = k % 2 ? ry[k] + ry[(k + 1) % 4] : rx[k] + rx[(k + 1) % 4];
            float side = k % 2 ? height : width;

            if (sum > side) {
                scale = minf(scale, side / sum);
            }
        }

        for (int k = 0; k < 4; k++) {
            rx[k] *= scale;
            ry[k] *= scale;
        }

        const float c = 0.5522847498f;

        moveTo(path, Point{x0 + rx[0], y0});
        lineTo(path, Point{x1 - rx[1], y0});
        cubicTo(path, Point{x1 - rx[1] + rx[1] * c, y0}, Point{x1, y0 + ry[1] - ry[1] * c}, Point{x1, y0 + ry[1]});
        lineTo(path, Point{x1, y1 - ry[2]});
        cubicTo(path, Point{x1, y1 - ry[2] + ry[2] * c}, Point{x1 - rx[2] + rx[2] * c, y1}, Point{x1 - rx[2], y1});
        lineTo(path, Point{x0 + rx[3], y1});
        cubicTo(path, Point{x0 + rx[3] - rx[3] * c, y1}, Point{x0, y1 - ry[3] + ry[3] * c}, Point{x0, y1 - ry[3]});
        lineTo(path, Point{x0, y0 + ry[0]});
        cubicTo(path, Point{x0, y0 + ry[0] - ry[0] * c}, Point{x0 + rx[0] - rx[0] * c, y0}, Point{x0 + rx[0], y0});
        closePath(path);
    }

    // The path of shape()'s commands in the reference box of the size given,
    // from its top left: a quadratic raised to a cubic, a smooth curve's
    // first control the last one of the curve before of its kind, mirrored
    // about the start, or the start.
    static void shapeCommandsPath(const BasicShape& shape, float w, float h, Viewport vp, float fs, Path& path) {
        Point current{0.f, 0.f};
        Point subpath{0.f, 0.f};
        Point quadratic{0.f, 0.f};
        Point cubic{0.f, 0.f};
        u8 previous = 0;
        auto point = [&](const ShapePoint& p, Point start, Point stop) {
            Point base = p.anchor == ShapeAnchor::Start ? start : (p.anchor == ShapeAnchor::End ? stop : Point{0.f, 0.f});

            return Point{base.x + shapeLength(p.x, w, vp, fs), base.y + shapeLength(p.y, h, vp, fs)};
        };
        auto mirrored = [&](Point p) {
            return Point{2.f * current.x - p.x, 2.f * current.y - p.y};
        };

        for (u32 i = 0; i < shape.commandCount; i++) {
            const ShapeCommand& c = shape.commands[i];
            Point end = point(c.end, current, current);
            u8 kind = c.kind;

            if (kind == 'H' || kind == 'V') {
                Point at = kind == 'H' ? Point{end.x, current.y} : Point{current.x, end.y};

                lineTo(path, at);
                end = at;
            } else if (kind == 'M') {
                moveTo(path, end);
                subpath = end;
            } else if (kind == 'L') {
                lineTo(path, end);
            } else if (kind == 'Z') {
                closePath(path);
                end = subpath;
            } else if (kind == 'A') {
                // One radius is of the box's diagonal over the root of two.
                float rx = shapeLength(c.radius[0], c.radii == 2 ? w : sqrtf((w * w + h * h) / 2.f), vp, fs);
                float ry = c.radii == 2 ? shapeLength(c.radius[1], h, vp, fs) : rx;

                arcTo(path, current, rx, ry, c.angle, c.large, c.sweep, end);
            } else {
                // C or S: a quadratic of one control, a cubic of two.
                bool smooth = kind == 'S';
                bool quad = c.controls == (smooth ? 0 : 1);
                Point first = smooth ? (previous == (quad ? 'Q' : 'C') ? mirrored(quad ? quadratic : cubic) : current) : point(c.control[0], current, end);

                if (quad) {
                    quadratic = first;
                    cubicTo(path, Point{current.x + 2.f / 3.f * (first.x - current.x), current.y + 2.f / 3.f * (first.y - current.y)}, Point{end.x + 2.f / 3.f * (first.x - end.x), end.y + 2.f / 3.f * (first.y - end.y)}, end);
                } else {
                    cubic = point(c.control[smooth ? 0 : 1], current, end);
                    cubicTo(path, first, cubic, end);
                }

                kind = quad ? 'Q' : 'C';
            }

            previous = kind;
            current = end;
        }
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

            if (shape.box == GeometryBox::Stroke || shape.box == GeometryBox::HalfBorder) {
                Box stroke = strokeBox(l, element, parent, vp);
                float part = shape.box == GeometryBox::Stroke ? 1.f : 0.5f;

                box = Box{box.x0 + (stroke.x0 - box.x0) * part, box.y0 + (stroke.y0 - box.y0) * part, box.x1 + (stroke.x1 - box.x1) * part, box.y1 + (stroke.y1 - box.y1) * part};
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
        } else if (shape.kind == ShapeKind::Inset || shape.kind == ShapeKind::Rect || shape.kind == ShapeKind::Xywh) {
            float v[4];

            for (int k = 0; k < 4; k++) {
                v[k] = shapeLength(shape.values[k], k % 2 ? w : h, vp, fs);
            }

            float x0;
            float y0;
            float x1;
            float y1;

            if (shape.kind == ShapeKind::Inset) {
                x0 = box.x0 + v[3];
                y0 = box.y0 + v[0];
                x1 = box.x1 - v[1];
                y1 = box.y1 - v[2];
            } else if (shape.kind == ShapeKind::Rect) {
                // Every side from the top or left edge; auto is the box's side.
                y0 = box.y0 + (shape.autos & 1 ? 0.f : v[0]);
                x1 = box.x0 + (shape.autos & 2 ? w : v[1]);
                y1 = box.y0 + (shape.autos & 4 ? h : v[2]);
                x0 = box.x0 + (shape.autos & 8 ? 0.f : v[3]);
            } else {
                x0 = box.x0 + shapeLength(shape.values[0], w, vp, fs);
                y0 = box.y0 + shapeLength(shape.values[1], h, vp, fs);
                x1 = x0 + shapeLength(shape.values[2], w, vp, fs);
                y1 = y0 + shapeLength(shape.values[3], h, vp, fs);
            }

            x1 = maxf(x0, x1);
            y1 = maxf(y0, y1);
            roundedRectPath(path, x0, y0, x1, y1, shape.radii, w, h, vp, fs);
        } else if (shape.kind == ShapeKind::Path) {
            parsePathData(shape.data, path);
            addClipShape(l, path, multiply(m, translation(box.x0, box.y0)), shape.rule);
            op.clipCount = (u32)l.clips.length() - op.clipFirst;

            return;
        } else if (shape.kind == ShapeKind::Commands) {
            shapeCommandsPath(shape, w, h, vp, fs, path);
            addClipShape(l, path, multiply(m, translation(box.x0, box.y0)), shape.rule);
            op.clipCount = (u32)l.clips.length() - op.clipFirst;

            return;
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

    // A mask element as a mask layer: its region in the user space of the
    // element it masks, its content a program, its type the mask-type, or
    // the mode's other than match-source. False where bounding box units
    // meet an element with no width or height: the mask is ignored
    // (coords.md, ObjectBoundingBoxUnits).
    static bool maskElementLayer(Lowering& l, Node* element, const ComputedStyle& parent, Node* mask, u8 mode, const Matrix& m, Viewport vp, MaskLayerOp& out) {
        const Document& doc = *l.doc;
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

        out.type = (u8)((mode ? mode == 1 : maskStyle.maskType) | (maskStyle.linearColors ? 2 : 0));
        out.first = (u32)l.programs.length();
        out.count = 0;
        out.region = (u32)l.clips.length();

        if ((boxRegion || boxContent) && (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0)) {
            return false;
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

        // The clip property cuts the region at its offsets.
        Box cut{0.f, 0.f, w, h};

        clipCut(maskStyle, w, h, vp, cut);

        float x0 = maxf(cut.x0, 0.f);
        float y0 = maxf(cut.y0, 0.f);

        w = minf(cut.x1, w) - x0;
        h = minf(cut.y1, h) - y0;
        x += x0;
        y += y0;

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
        out.first = (u32)l.programs.length();
        out.count = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return true;
    }

    // The stops of a CSS gradient along a line of the given length, placed
    // as CSS Images places them, a hint's curve and the mix of colours of
    // differing alpha, which CSS takes premultiplied, sampled into stops in
    // between; offsets over the line, perhaps past 0 to 1. The line is
    // never empty: a box has width and height.
    static void cssStops(const CssGradient& g, float length, Viewport vp, float fs, Vector<GradientStop>& out) {
        Vector<float> at;
        size_t n = g.stops.length();

        at.zero(n);

        for (size_t i = 0; i < n; i++) {
            const CssStop& s = g.stops[i];

            at.mut(i) = s.placed ? (s.at.unit == Unit::Percent ? s.at.value / 100.f : resolveLength(s.at, Axis::Other, vp, fs) / length) : (i == 0 ? 0.f : (i + 1 == n ? 1.f : -1e30f));
        }

        // No place before the largest before it; the unplaced spread evenly.
        float largest = -1e30f;

        for (size_t i = 0; i < n; i++) {
            if (at[i] > -1e29f) {
                at.mut(i) = maxf(at[i], largest);
                largest = at[i];
            }
        }

        for (size_t i = 0; i < n;) {
            size_t j = i + 1;

            while (j < n && at[j] < -1e29f) {
                j++;
            }

            for (size_t k = i + 1; k < j; k++) {
                at.mut(k) = at[i] + (at[j] - at[i]) * (float)(k - i) / (float)(j - i);
            }

            i = j;
        }

        // Between two colours, a hint's curve and premultiplied mixing.
        for (size_t i = 0; i < n; i++) {
            const CssStop& s = g.stops[i];

            if (s.hint) {
                continue;
            }

            if (!out.length()) {
                out.pushBack(GradientStop{at[i], s.color, {}});
                continue;
            }

            size_t before = i - 1;
            float hint = g.stops[before].hint ? at[before] : -1.f;

            before -= g.stops[before].hint;

            Color a = g.stops[before].color;
            Color b = s.color;
            float from = at[before];
            float to = at[i];
            int steps = hint >= 0.f || a.a != b.a ? 16 : 1;

            for (int k = 1; k <= steps; k++) {
                float t = (float)k / (float)steps;
                float w = t;

                if (hint >= 0.f && to > from) {
                    float h = (hint - from) / (to - from);

                    w = h <= 0.f ? 1.f : (h >= 1.f ? 0.f : powf(t, logf(0.5f) / logf(h)));
                }

                float alpha = a.a + (b.a - a.a) * w;
                float wa = (1.f - w) * a.a;
                float wb = w * b.a;
                float sum = wa + wb;
                Color c = sum > 0.f ? Color{(a.r * wa + b.r * wb) / sum, (a.g * wa + b.g * wb) / sum, (a.b * wa + b.b * wb) / sum, alpha} : Color{a.r + (b.r - a.r) * w, a.g + (b.g - a.g) * w, a.b + (b.b - a.b) * w, alpha};

                out.pushBack(GradientStop{from + (to - from) * t, c, {}});
            }
        }
    }

    // The colour a CSS gradient of no extent draws, its stops placed along
    // a line of the length given: its last stop's; a repeating one's the
    // average of a period, premultiplied, each stretch between two stops
    // weighing as long as it is, or, the stops all at one place, its colour
    // stops spread evenly, as CSS Images has it.
    static DevicePaint* flatGradient(Lowering& l, const CssGradient& g, float length, Viewport vp, float fs) {
        Vector<GradientStop> stops;
        Vector<float> at;
        Vector<Color> colors;
        float sum[4] = {0.f, 0.f, 0.f, 0.f};

        cssStops(g, length, vp, fs, stops);

        Color last = stops.back().color;

        if (!g.repeating) {
            return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
        }

        // Spread evenly, the steps a hint or a change of alpha adds count
        // for nothing.
        if (!(stops.back().offset > stops[0].offset)) {
            for (const CssStop& stop : g.stops) {
                if (!stop.hint) {
                    at.pushBack((float)colors.length());
                    colors.pushBack(stop.color);
                }
            }
        } else {
            for (const GradientStop& stop : stops) {
                at.pushBack(stop.offset);
                colors.pushBack(stop.color);
            }
        }

        for (size_t i = 0; i + 1 < colors.length(); i++) {
            float weight = (at[i + 1] - at[i]) / (at.back() - at[0]) / 2.f;
            const Color& a = colors[i];
            const Color& b = colors[i + 1];

            sum[0] += (a.r * a.a + b.r * b.a) * weight;
            sum[1] += (a.g * a.a + b.g * b.a) * weight;
            sum[2] += (a.b * a.a + b.b * b.a) * weight;
            sum[3] += (a.a + b.a) * weight;
        }

        return sum[3] > 0.f ? solidPaint(l, Color{sum[0] / sum[3], sum[1] / sum[3], sum[2] / sum[3], 1.f}, sum[3]) : nullptr;
    }

    // A CSS gradient's paint for an image of w by h, its space the user
    // space the matrix takes to the device, from the image's top left.
    static DevicePaint* cssGradientPaint(Lowering& l, const CssGradient& g, float w, float h, const Matrix& space, Viewport vp, float fs) {
        Vector<GradientStop> stops;
        DevicePaint* paint = l.pool->make<DevicePaint>();
        Matrix ellipse = identity();
        float length;

        memset(paint, 0, sizeof(DevicePaint));

        if (g.conic) {
            // Turns about the centre from the start angle; the table spans
            // the circle round the box's far corner.
            length = 1.f;
            paint->type = PaintType::Conic;
            paint->cx = shapeLength(g.center[0], w, vp, fs);
            paint->cy = shapeLength(g.center[1], h, vp, fs);
            paint->x1 = g.angle;
            paint->x2 = 2.f * pi * hypotf(w, h);
        } else if (!g.radial) {
            // The line through the middle, as long as the box is across it.
            float dx = sinf(g.angle * pi / 180.f);
            float dy = -cosf(g.angle * pi / 180.f);

            if (g.corner) {
                float sx = g.corner & 1 ? -1.f : (g.corner & 2 ? 1.f : 0.f);
                float sy = g.corner & 4 ? -1.f : (g.corner & 8 ? 1.f : 0.f);
                bool both = sx != 0.f && sy != 0.f;
                float nx = both ? sx * h : sx;
                float ny = both ? sy * w : sy;
                float norm = hypotf(nx, ny);

                dx = nx / norm;
                dy = ny / norm;
            }

            length = fabsf(w * dx) + fabsf(h * dy);
            paint->type = PaintType::Linear;
            paint->x1 = w / 2.f - dx * length / 2.f;
            paint->y1 = h / 2.f - dy * length / 2.f;
            paint->x2 = w / 2.f + dx * length / 2.f;
            paint->y2 = h / 2.f + dy * length / 2.f;
        } else {
            float cx = shapeLength(g.center[0], w, vp, fs);
            float cy = shapeLength(g.center[1], h, vp, fs);
            float near[2] = {minf(fabsf(cx), fabsf(w - cx)), minf(fabsf(cy), fabsf(h - cy))};
            float far[2] = {maxf(fabsf(cx), fabsf(w - cx)), maxf(fabsf(cy), fabsf(h - cy))};
            bool farthest = g.extent == 1 || g.extent == 3;
            float side[2] = {farthest ? far[0] : near[0], farthest ? far[1] : near[1]};
            float rx;
            float ry;

            if (g.extent == 4) {
                rx = shapeLength(g.radius[0], w, vp, fs);
                ry = g.circle ? rx : shapeLength(g.radius[1], h, vp, fs);
            } else if (g.extent < 2) {
                rx = g.circle ? (farthest ? maxf(side[0], side[1]) : minf(side[0], side[1])) : side[0];
                ry = g.circle ? rx : side[1];
            } else {
                // Through the corner, a circle, or an ellipse of the sides'
                // ratio.
                float corner = hypotf(side[0], side[1]);

                rx = g.circle ? corner : side[0] * 1.41421356f;
                ry = g.circle ? corner : side[1] * 1.41421356f;
            }

            // CSS Images draws a circle of no radius as one a hair across,
            // which percentages put no stop past; an ellipse of no width as
            // one a hair wide and as tall as can be, a line across mirrored
            // at the centre; and one of no height, as wide as can be, flat,
            // which percentages outweigh.
            if (!(rx > 0.f)) {
                rx = 1.f / 1024.f;
                ry = g.circle ? rx : rx * 16777216.f;
            } else if (!(ry > 0.f)) {
                return flatGradient(l, g, 1048576.f, vp, fs);
            }

            length = rx;
            ellipse = multiply(multiply(translation(cx, cy), scaling(1.f, ry / rx)), translation(-cx, -cy));
            paint->type = PaintType::Radial;
            paint->cx = cx;
            paint->cy = cy;
            paint->fx = cx;
            paint->fy = cy;
            paint->r = rx;
        }

        cssStops(g, length, vp, fs, stops);

        float lo = stops[0].offset;
        float hi = stops.back().offset;

        // Repeating, the stops' range is a period, of a radial one past its
        // centre by whole periods; else the line covers the stops past it,
        // a radial one's from its centre, a conic's whole turn.
        if (!g.repeating) {
            lo = g.radial ? 0.f : minf(lo, 0.f);
            hi = maxf(hi, 1.f);
        }

        if (!(hi > lo)) {
            return flatGradient(l, g, length, vp, fs);
        }

        if (g.radial && lo < 0.f) {
            float shift = ceilf(-lo / (hi - lo)) * (hi - lo);

            for (size_t i = 0; i < stops.length(); i++) {
                stops.mut(i).offset += shift;
            }

            lo += shift;
            hi += shift;
        }

        for (size_t i = 0; i < stops.length(); i++) {
            stops.mut(i).offset = clampUnit((stops[i].offset - lo) / (hi - lo));
        }

        if (g.conic) {
            paint->fr = lo;
            paint->r = hi;
        } else if (!g.radial) {
            float x1 = paint->x1;
            float y1 = paint->y1;

            paint->x1 = x1 + (paint->x2 - x1) * lo;
            paint->y1 = y1 + (paint->y2 - y1) * lo;
            paint->x2 = x1 + (paint->x2 - x1) * hi;
            paint->y2 = y1 + (paint->y2 - y1) * hi;
        } else {
            paint->fr = paint->r * lo;
            paint->r *= hi;
        }

        paint->spread = g.repeating ? SpreadRepeat : SpreadPad;
        paint->stopCount = (u32)stops.length();
        paint->stops = (GradientStop*)l.pool->allocate(sizeof(GradientStop) * stops.length());
        memcpy(paint->stops, stops.data(), sizeof(GradientStop) * stops.length());

        for (u32 i = 0; i < paint->stopCount; i++) {
            storeColor(paint->stops[i].stored, paint->stops[i].color);
        }

        // An ellipse too thin for a float draws as one of no height.
        return invert(multiply(space, ellipse), paint->inverse) ? paint : flatGradient(l, g, 1048576.f, vp, fs);
    }

    // A box filled with a paint under the matrix.
    static void fillBox(Lowering& l, Box box, const Matrix& m, DevicePaint* paint) {
        Path path;
        Flat flat;

        if (!paint) {
            return;
        }

        moveTo(path, Point{box.x0, box.y0});
        lineTo(path, Point{box.x1, box.y0});
        lineTo(path, Point{box.x1, box.y1});
        lineTo(path, Point{box.x0, box.y1});
        closePath(path);
        flatten(path, m, tolerance, flat);
        emitFill(l, flat, WindingNonZero, paint, false);
    }

    // A paint repeating a tile, of the period given in the user space the
    // matrix takes to the device from the tile's origin, drawn by draw in
    // the tile's pixels from the matrix it is given.
    template <typename Draw>
    static DevicePaint* tilePaint(Lowering& l, const Matrix& space, float pw, float ph, Draw draw) {
        float tw = minf(maxf(ceilf(pw * hypotf(space.a, space.b)), 1.f), 2048.f);
        float th = minf(maxf(ceilf(ph * hypotf(space.c, space.d)), 1.f), 2048.f);
        DevicePaint* paint = l.pool->make<DevicePaint>();
        Vector<Op> program;
        Vector<Op>* saved = l.out;
        u32 width = l.width;
        u32 height = l.height;

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Pattern;
        paint->imageWidth = (u32)tw;
        paint->imageHeight = (u32)th;
        paint->opacity = 1.f;

        if (!invert(multiply(space, scaling(pw / tw, ph / th)), paint->inverse)) {
            return nullptr;
        }

        l.out = &program;
        l.width = (u32)tw;
        l.height = (u32)th;
        l.patternDepth++;
        draw(scaling(tw / pw, th / ph));
        l.patternDepth--;
        l.width = width;
        l.height = height;
        l.out = saved;
        paint->programFirst = (u32)l.programs.length();
        paint->programCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return paint;
    }

    static void drawImage(Lowering& l, Node* node, const ImageSource& source, const Matrix& m, Viewport vp, float x, float y, float w, float h, u8 rendering);
    static bool explicitSide(const Document& doc, const Node* root, u32 name);
    static DevicePaint* rasterPaint(Lowering& l, CachedImage* cached, const Matrix& place, u8 rendering, bool tent, u8 wraps, const Box& part);

    // An image's own size and ratio, none for a side or a ratio it has
    // not: a document's size is the sides it gives, the rest its ratio's,
    // as CSS Images sizes an SVG image.
    static void imageNatural(const ImageSource& image, float (&natural)[2], float& ratio) {
        float vw = image.viewBox.x1 - image.viewBox.x0;
        float vh = image.viewBox.y1 - image.viewBox.y0;

        natural[0] = !image.nested || explicitSide(*image.nested, image.nested->root, AtomWidth) ? image.width : 0.f;
        natural[1] = !image.nested || explicitSide(*image.nested, image.nested->root, AtomHeight) ? image.height : 0.f;
        ratio = natural[0] > 0.f && natural[1] > 0.f ? natural[0] / natural[1] : (vw > 0.f && vh > 0.f ? vw / vh : 0.f);

        if (ratio > 0.f && natural[0] > 0.f) {
            natural[1] = natural[0] / ratio;
        } else if (ratio > 0.f && natural[1] > 0.f) {
            natural[0] = natural[1] * ratio;
        }
    }

    // A mask layer of an image, a gradient or the one given: its image
    // sized in the positioning area as mask-size has it, placed by
    // mask-position, repeated over the painting area by mask-repeat, whole
    // numbers for round and spread apart for space, as CSS Backgrounds lays
    // images out; the painting area bounds it. The boxes are the fill box,
    // the stroke box and the viewport.
    static void imageMaskLayer(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const MaskLayer& layer, const ImageSource* image, const Matrix& m, Viewport vp, MaskLayerOp& out) {
        CssGradient gradient;
        Box fill;
        float fs = style.fontSize;
        // The image's own size and ratio, none a gradient's.
        float natural[2] = {0.f, 0.f};
        float ratio = 0.f;

        out.type = layer.mode != 2;
        out.first = (u32)l.programs.length();
        out.count = 0;
        out.region = (u32)l.clips.length();

        if (image) {
            imageNatural(*image, natural, ratio);
        } else {
            // The cascade took the image whole.
            parseCssGradient(layer.image, style.color, gradient);
        }

        if (!elementBox(l, element, parent, vp, fill, 0)) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return;
        }

        Box stroke = layer.origin == GeometryBox::Stroke || layer.clip == (u8)GeometryBox::Stroke ? strokeBox(l, element, parent, vp) : fill;
        auto boxOf = [&](u8 kind) {
            return kind == (u8)GeometryBox::View ? Box{0.f, 0.f, vp.width, vp.height} : (kind == (u8)GeometryBox::Fill ? fill : stroke);
        };
        Box area = boxOf((u8)layer.origin);
        Box painted = layer.clip == 255 ? Box{-3e7f, -3e7f, 3e7f, 3e7f} : boxOf(layer.clip);
        float span[2] = {area.x1 - area.x0, area.y1 - area.y0};
        float start[2] = {area.x0, area.y0};
        float size[2];
        float gap[2] = {0.f, 0.f};
        float origin[2];
        bool repeats[2];

        // A side given is its length, a side auto the image's own or the
        // area's; cover and contain, or auto for an image of a ratio alone,
        // the largest of its ratio inside the area, or the smallest over
        // it; one side given, the other keeps the ratio.
        bool given[2] = {!layer.size && !layer.fit[0], !layer.size && !layer.fit[1]};
        u8 fit = layer.size ? layer.size : (!given[0] && !given[1] && ratio > 0.f && !(natural[0] > 0.f) ? 2 : 0);

        for (int axis = 0; axis < 2; axis++) {
            size[axis] = given[axis] ? shapeLength(layer.extent[axis], span[axis], vp, fs) : (natural[axis] > 0.f ? natural[axis] : span[axis]);
        }

        if (fit) {
            float r = ratio > 0.f ? ratio : span[0] / span[1];

            size[0] = span[0];
            size[1] = span[0] / r;

            if ((fit == 2) == (size[1] > span[1])) {
                size[1] = span[1];
                size[0] = span[1] * r;
            }
        } else if (given[0] != given[1] && ratio > 0.f) {
            size[given[0] ? 1 : 0] = given[0] ? size[0] / ratio : size[1] * ratio;
        }

        for (int axis = 0; axis < 2; axis++) {
            float was = size[axis];

            if (layer.repeat[axis] == 2 && size[axis] > 0.f) {
                size[axis] = span[axis] / maxf(1.f, roundf(span[axis] / size[axis]));

                // The other side of auto keeps the ratio.
                if (layer.fit[1 - axis] && layer.repeat[1 - axis] != 2) {
                    size[1 - axis] *= size[axis] / was;
                }
            }
        }

        if (!(size[0] > 0.f) || !(size[1] > 0.f)) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return;
        }

        for (int axis = 0; axis < 2; axis++) {
            const Length& at = layer.position[axis];
            float offset = at.unit == Unit::Percent ? (span[axis] - size[axis]) * at.value / 100.f : resolveLength(at, Axis::Other, vp, fs);
            float count = floorf(span[axis] / size[axis]);

            origin[axis] = start[axis] + (layer.far[axis] ? span[axis] - size[axis] - offset : offset);
            repeats[axis] = layer.repeat[axis] != 3 && !(layer.repeat[axis] == 1 && count < 2.f);

            if (layer.repeat[axis] == 1 && count >= 2.f) {
                gap[axis] = (span[axis] - count * size[axis]) / (count - 1.f);
                origin[axis] = start[axis];
            }
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;
        Matrix space = multiply(m, translation(origin[0], origin[1]));

        l.out = &program;

        auto draw = [&](const Matrix& tile) {
            if (image) {
                drawImage(l, nullptr, *image, tile, vp, 0.f, 0.f, size[0], size[1], style.imageRendering);
            } else {
                fillBox(l, Box{0.f, 0.f, size[0], size[1]}, tile, cssGradientPaint(l, gradient, size[0], size[1], tile, vp, fs));
            }
        };

        Box strip = painted;

        // An axis that does not repeat holds one row of tiles.
        if (!repeats[0]) {
            strip.x0 = origin[0];
            strip.x1 = origin[0] + size[0];
        }

        if (!repeats[1]) {
            strip.y0 = origin[1];
            strip.y1 = origin[1] + size[1];
        }

        // The axes the painting area passes a tile along: with neither the
        // tile is drawn alone, as browsers draw it.
        bool across = origin[0] > strip.x0 || origin[0] + size[0] < strip.x1;
        bool down = origin[1] > strip.y0 || origin[1] + size[1] < strip.y1;

        if (!across && !down) {
            draw(space);
        } else if (image && image->cached && gap[0] == 0.f && gap[1] == 0.f) {
            // A raster tiled edge to edge is one image sampled round its
            // sides, which blends each tile's edge with the next one's, as
            // browsers draw it; along an axis one tile spans, as Chromium
            // does, not.
            fillBox(l, strip, m, rasterPaint(l, image->cached, multiply(space, scaling(size[0] / image->width, size[1] / image->height)), style.imageRendering, true, (u8)(across | down << 1), Box{0.f, 0.f, image->width, image->height}));
        } else {
            fillBox(l, strip, m, tilePaint(l, space, size[0] + gap[0], size[1] + gap[1], draw));
        }

        l.out = saved;
        out.first = (u32)l.programs.length();
        out.count = (u32)program.length();
        l.programs.append(program.data(), program.length());
        // A document drawn adds clips of its own first.
        out.region = (u32)l.clips.length();
        addRectClip(l, m, painted.x0, painted.y0, painted.x1 - painted.x0, painted.y1 - painted.y0);
    }

    // The layer of a mask border, as CSS Backgrounds draws a border image:
    // the image at its own size, or the area's, cut by the slices into nine
    // parts, each scaled into its region of the border image area, the
    // stroke box past the outsets. A width of auto is its slice's for an
    // image of a size of its own, else the border's, and a number a
    // multiple of the border's, which SVG has none of; widths wider than
    // the area shrink together. A corner fills its region, an edge is as
    // wide as the border and as long as its scale makes it, the middle as
    // the top's or bottom's scale across and the left's or right's down
    // make it, then each tiled along as the repeats say, centred for
    // repeat, spread apart with spaces around for space. The middle
    // without fill is opaque white.
    static void borderMaskLayer(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const MaskBorder& border, const ImageSource* image, const Matrix& m, Viewport vp, MaskLayerOp& out) {
        CssGradient gradient;
        Box fill;
        float fs = style.fontSize;
        float natural[2] = {0.f, 0.f};
        float ratio = 0.f;

        out.type = border.mode;
        out.first = (u32)l.programs.length();
        out.count = 0;
        out.region = (u32)l.clips.length();

        if (image) {
            imageNatural(*image, natural, ratio);
        } else {
            parseCssGradient(border.image.image, style.color, gradient);
        }

        if (!elementBox(l, element, parent, vp, fill, 0)) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return;
        }

        Box box = strokeBox(l, element, parent, vp);
        float outset[4];

        for (int k = 0; k < 4; k++) {
            outset[k] = border.outset[k].unit == Unit::Number ? 0.f : resolveLength(border.outset[k], Axis::Other, vp, fs);
        }

        Box area{box.x0 - outset[3], box.y0 - outset[0], box.x1 + outset[1], box.y1 + outset[2]};
        float span[2] = {area.x1 - area.x0, area.y1 - area.y0};
        float size[2];

        for (int axis = 0; axis < 2; axis++) {
            size[axis] = natural[axis] > 0.f ? natural[axis] : span[axis];
        }

        // An image of a ratio alone fits inside the area.
        if (ratio > 0.f && !(natural[0] > 0.f)) {
            size[0] = minf(span[0], span[1] * ratio);
            size[1] = size[0] / ratio;
        }

        // By side: the slice, the image's at most, and the width; a top or
        // bottom one measures down, a left or right one across.
        float slice[4];
        float width[4];

        for (int k = 0; k < 4; k++) {
            int axis = k % 2 ? 0 : 1;
            const Length& at = border.slice[k];
            const Length& given = border.width[k];

            slice[k] = minf(at.unit == Unit::Percent ? size[axis] * at.value / 100.f : at.value, size[axis]);
            width[k] = (border.autos >> k) & 1 ? (natural[axis] > 0.f ? slice[k] : 0.f) : (given.unit == Unit::Number ? 0.f : (given.unit == Unit::Percent ? span[axis] * given.value / 100.f : resolveLength(given, Axis::Other, vp, fs)));
        }

        float shrink = minf(1.f, minf(span[0] / maxf(width[1] + width[3], 1e-30f), span[1] / maxf(width[0] + width[2], 1e-30f)));

        for (float& w : width) {
            w *= shrink;
        }

        // The regions' and the parts' edges, and the edges' scales.
        float dx[4] = {area.x0, area.x0 + width[3], area.x1 - width[1], area.x1};
        float dy[4] = {area.y0, area.y0 + width[0], area.y1 - width[2], area.y1};
        float sx[4] = {0.f, slice[3], size[0] - slice[1], size[0]};
        float sy[4] = {0.f, slice[0], size[1] - slice[2], size[1]};
        float scale[4];

        for (int k = 0; k < 4; k++) {
            scale[k] = width[k] / slice[k];
        }

        auto usable = [](float k) {
            return k > 0.f && k < 1e30f;
        };
        // A part of the image scaled onto a tile of w by h the matrix
        // places.
        auto drawPart = [&](const Matrix& at, const Box& part, float w, float h) {
            Matrix space = multiply(at, multiply(scaling(w / (part.x1 - part.x0), h / (part.y1 - part.y0)), translation(-part.x0, -part.y0)));

            if (!image) {
                fillBox(l, part, space, cssGradientPaint(l, gradient, size[0], size[1], space, vp, fs));
            } else if (image->cached) {
                float sx = image->width / size[0];
                float sy = image->height / size[1];

                fillBox(l, part, space, rasterPaint(l, image->cached, multiply(space, scaling(1.f / sx, 1.f / sy)), style.imageRendering, true, 0, Box{part.x0 * sx, part.y0 * sy, part.x1 * sx, part.y1 * sy}));
            } else {
                Op op = groupOp(1.f);
                u32 index = (u32)l.out->length();

                op.clipped = true;
                op.clipFirst = (u32)l.clips.length();
                op.clipCount = 1;
                addRectClip(l, space, part.x0, part.y0, part.x1 - part.x0, part.y1 - part.y0);
                l.out->pushBack(op);
                drawImage(l, nullptr, *image, space, vp, 0.f, 0.f, size[0], size[1], style.imageRendering);
                endGroup(l, index);
            }
        };
        Vector<Op> program;
        Vector<Op>* saved = l.out;

        l.out = &program;

        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                Box region{dx[c], dy[r], dx[c + 1], dy[r + 1]};
                Box part{sx[c], sy[r], sx[c + 1], sy[r + 1]};
                float length[2] = {region.x1 - region.x0, region.y1 - region.y0};

                if (!(length[0] > 0.f) || !(length[1] > 0.f)) {
                    continue;
                }

                if (r == 1 && c == 1 && !border.fill) {
                    fillBox(l, region, m, solidPaint(l, Color{1.f, 1.f, 1.f, 1.f}, 1.f));
                    continue;
                }

                // A part past the slices across is empty.
                if (!(part.x1 > part.x0) || !(part.y1 > part.y0)) {
                    continue;
                }

                float across = c != 1 ? length[0] : (part.x1 - part.x0) * (r != 1 ? scale[r] : (usable(scale[0]) ? scale[0] : (usable(scale[2]) ? scale[2] : 1.f)));
                float down = r != 1 ? length[1] : (part.y1 - part.y0) * (c != 1 ? scale[3 - c] : (usable(scale[3]) ? scale[3] : (usable(scale[1]) ? scale[1] : 1.f)));
                float tile[2] = {across, down};
                float start[2] = {region.x0, region.y0};
                float gap[2] = {0.f, 0.f};
                u8 repeat[2] = {c == 1 ? border.repeat[0] : (u8)0, r == 1 ? border.repeat[1] : (u8)0};
                bool empty = false;

                for (int axis = 0; axis < 2; axis++) {
                    float count = floorf(length[axis] / tile[axis]);

                    if (repeat[axis] == 0) {
                        tile[axis] = length[axis];
                    } else if (repeat[axis] == 1) {
                        start[axis] += (length[axis] - tile[axis]) / 2.f;
                    } else if (repeat[axis] == 2) {
                        tile[axis] = length[axis] / maxf(1.f, roundf(length[axis] / tile[axis]));
                    } else if (count >= 1.f) {
                        gap[axis] = (length[axis] - count * tile[axis]) / (count + 1.f);
                        start[axis] += gap[axis];
                    } else {
                        empty = true;
                    }
                }

                if (empty) {
                    continue;
                }

                // A tile its region's size is drawn alone.
                if (tile[0] == length[0] && tile[1] == length[1]) {
                    drawPart(multiply(m, translation(region.x0, region.y0)), part, tile[0], tile[1]);
                } else {
                    fillBox(l, region, m, tilePaint(l, multiply(m, translation(start[0], start[1])), tile[0] + gap[0], tile[1] + gap[1], [&](const Matrix& at) {
                        drawPart(at, part, tile[0], tile[1]);
                    }));
                }
            }
        }

        l.out = saved;
        out.first = (u32)l.programs.length();
        out.count = (u32)program.length();
        l.programs.append(program.data(), program.length());
        out.region = (u32)l.clips.length();
        addRectClip(l, m, area.x0, area.y0, span[0], span[1]);
    }

    const char* const maskInitials[MaskParts] = {"none", "match-source", "repeat", "0% 0%", "border-box", "border-box", "auto", "add"};

    // The mask layers of a style: one a value of mask-image, each of the
    // other longhands' values in turn, the initial one where not set.
    static void maskLayersOf(const Document& doc, const ComputedStyle& style, Vector<MaskLayer>& out) {
        for (u32 k = 0; k < MaskParts; k++) {
            StringView list = style.maskParts[k] ? textOf(doc, style.maskParts[k]) : StringView(maskInitials[k]);
            Vector<StringView> items;
            const u8* p = list.begin();

            for (;;) {
                const u8* comma = findOutside(p, list.end(), ',');

                items.pushBack(trim(StringView(p, comma ? comma : list.end())));

                if (!comma) {
                    break;
                }

                p = comma + 1;
            }

            if (k == MaskPartImage) {
                out.zero(items.length());
            }

            for (size_t i = 0; i < out.length(); i++) {
                maskPartOf(k, items[i % items.length()], out.mut(i));
            }
        }
    }

    static bool loadImageFrom(Pass& l, const void* key, StringView reference, ImageSource& out);

    // The mask layers of an element. none is no layer, nor a mask element
    // in no usable place or that refers back; a reference to no element or
    // to one not a mask, and an image that does not load, are transparent
    // black, as CSS Masking has it, alone too.
    static void resolveMask(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const Document& doc = *l.doc;
        Vector<MaskLayer> layers;
        u32 count = 0;

        maskLayersOf(doc, style, layers);

        // The border's layer goes first, over the others.
        MaskLayerOp* ops = (MaskLayerOp*)l.pool->allocate(sizeof(MaskLayerOp) * (layers.length() + 1));

        for (const MaskLayer& layer : layers) {
            MaskLayerOp& out = ops[1 + count];
            Node* found = layer.source == MaskSource::Element ? elementById(doc, fragmentAtom(doc, layer.url)) : nullptr;
            Node* mask = found && found->name == AtomMask ? found : nullptr;
            ImageSource image;

            if (layer.source == MaskSource::None || (mask && closesCycle(l, element, AtomMask, mask))) {
                continue;
            }

            out.composite = layer.composite;

            if (mask) {
                if (!maskElementLayer(l, element, parent, mask, layer.mode, m, vp, out)) {
                    continue;
                }
            } else if (layer.source == MaskSource::Gradient) {
                imageMaskLayer(l, element, parent, style, layer, nullptr, m, vp, out);
            } else if (layer.source == MaskSource::Image && loadImageFrom(l, layer.url.data(), layer.url, image)) {
                imageMaskLayer(l, element, parent, style, layer, &image, m, vp, out);
            } else {
                out.type = 1;
                out.first = (u32)l.programs.length();
                out.count = 0;
                out.region = (u32)l.clips.length();
                addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);
            }

            count++;
        }

        // The border masks after the layers, as CSS Masking has it:
        // intersecting them, or alone.
        MaskBorder border;
        ImageSource image;

        for (u32 k = 0; k < MaskBorderParts; k++) {
            maskBorderPartOf(k, style.maskBorder[k] ? textOf(doc, style.maskBorder[k]) : StringView(maskBorderInitials[k]), border);
        }

        op.masked = count > 0;
        op.maskLayers = ops + 1;
        op.maskLayerCount = count;

        if (border.image.source == MaskSource::Gradient || (border.image.source == MaskSource::Image && loadImageFrom(l, border.image.url.data(), border.image.url, image))) {
            borderMaskLayer(l, element, parent, style, border, border.image.source == MaskSource::Image ? &image : nullptr, m, vp, ops[0]);
            ops[0].composite = 2;
            op.masked = true;
            op.maskLayers = ops;
            op.maskLayerCount = count + 1;
        }
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

    static Filter* lowerFilter(Lowering& l, Node* element, const ComputedStyle& parent, const Matrix& m, Viewport vp, Node* filterNode) {
        const Document& doc = *l.doc;

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

        // Bounding box units on an element with no width or height ignore
        // the filter (coords.md, ObjectBoundingBoxUnits).
        if ((boxRegion || filter->boxPrimitives) && !hasBox) {
            return nullptr;
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

        // An empty region, or one of no number where a box past the floats
        // met its own infinity, draws nothing.
        if (!(region.x0 < region.x1) || !(region.y0 < region.y1)) {
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
            primitive.clip = user.x1 > user.x0 && user.y1 > user.y0 ? meetBoxes(deviceBox(m, user), filter->device) : emptyBox();
            primitive.box = pixelBox(primitive.clip, filter->region);

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
        primitive->clip = Box{0.f, 0.f, (float)l.width, (float)l.height};
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
        Vector<Node*> targets;

        // A reference to anything but a filter element drops the whole list
        // (filter-effects-1.md, typedef-filter-url).
        for (u32 i = 0; i < style.filterCount; i++) {
            bool reference = style.filters[i].kind == FilterFunction::Reference;
            Node* target = reference ? elementById(*l.doc, style.filters[i].reference) : nullptr;

            if (reference && (!target || target->name != AtomFilter)) {
                return nullptr;
            }

            targets.pushBack(target);
        }

        for (u32 i = 0; i < style.filterCount; i++) {
            const FilterItem& item = style.filters[i];
            bool linear = last && last->count && last->primitives[last->count - 1].linear;
            Filter* filter = item.kind == FilterFunction::Reference ? lowerFilter(l, element, parent, m, vp, targets[i]) : functionFilter(l, item, m, linear);

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

            // A paint a filter reads is sampled over the whole canvas.
            DevicePaint* const read[] = {filter->fillPaint, filter->strokePaint};

            for (DevicePaint* paint : read) {
                if (paint && paint->type == PaintType::Pattern) {
                    paint->sampledAll = true;
                }
            }

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

    // A rect, circle or ellipse ends its outline with a close that only
    // completes its last segment, no segment of its own (shapes.md,
    // TermSegment-CompletingClosePath): where it has no length, it adds no
    // vertex.
    static bool completes(const Node* node) {
        return node->name == AtomRect || node->name == AtomCircle || node->name == AtomEllipse;
    }

    static void markerVertices(const Path& path, bool completing, Vector<MarkerVertex>& out) {
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

                    if (completing && !nonZero(d)) {
                        MarkerVertex& last = out.mutData()[out.length() - 1];

                        last.out = first.out;
                        last.hasOut = first.hasOut;
                        out.mutData()[start].in = last.in;
                        out.mutData()[start].hasIn = true;
                        arc = false;
                        break;
                    }

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

    // An angle in degrees, a number with or without its unit; unit tells
    // which.
    static bool readAngle(StringView text, float& out, bool& unit) {
        const u8* p = text.begin();

        if (!readNumber(p, text.end(), out)) {
            return false;
        }

        StringView name(p, text.end());

        unit = !name.empty();

        if (!unit || equalsIgnoreCase(name, SV("deg"))) {
            return true;
        }

        if (equalsIgnoreCase(name, SV("rad"))) {
            out *= 180.f / pi;
        } else if (equalsIgnoreCase(name, SV("grad"))) {
            out *= 0.9f;
        } else if (equalsIgnoreCase(name, SV("turn"))) {
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
        bool unit;
        float autoAngle = vertexAngle(shapeStyle.nonScaling ? deviceVertex : userVertex);
        u32 orient = attribute(marker, AtomOrient);

        if (orient) {
            StringView text = trim(textOf(doc, orient));

            if (text == SV("auto")) {
                angle = autoAngle;
            } else if (text == SV("auto-start-reverse")) {
                angle = start ? autoAngle + 180.f : autoAngle;
            } else if (!readAngle(text, angle, unit)) {
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

        // A percentage of refX or refY is of the viewBox, a keyword its 0%,
        // 50% or 100%, as SVG 2 has them; with no viewBox, of the marker's
        // viewport. Chromium, Firefox and resvg take a percentage of the
        // viewport outside, librsvg as SVG 2.
        float rx;
        float ry;

        if (!referenceOf(doc, marker, AtomRefX, Axis::X, inside, fs, inside.width, rx)) {
            rx = 0.f;
        }

        if (!referenceOf(doc, marker, AtomRefY, Axis::Y, inside, fs, inside.height, ry)) {
            ry = 0.f;
        }

        Point ref = apply(content, Point{rx, ry});

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
        // A marker is no viewport: percentages in its content are of the
        // shape's (coords.md, ElementsThatEstablishViewports).
        lowerChildren(l, marker, markerStyle, multiply(origin, content), vp);
        l.active.popBack();
        l.hasContext = hadContext;
        l.contextFill = outerFill;
        l.contextStroke = outerStroke;

        if (clip) {
            endGroup(l, clipIndex);
        }
    }

    // The markers a shape's style names, those that are markers; false
    // when it names none.
    static bool shapeMarkers(const Pass& p, const ComputedStyle& style, Node** markers) {
        bool any = false;

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

        if (!shapeMarkers(l, style, markers)) {
            return;
        }

        Vector<MarkerVertex> vertices;
        Vector<MarkerVertex> deviceVertices;
        Matrix hostInverse;

        if (!invert(l.host, hostInverse)) {
            hostInverse = identity();
        }

        markerVertices(path, completes(node), vertices);
        markerVertices(style.nonScaling ? transformedPath(path, multiply(hostInverse, m)) : path, completes(node), deviceVertices);

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
        Box drawn = emptyBox();

        p.active.pushBack(marker);

        for (Node* child = marker->first; child; child = child->next) {
            childExtent(p, child, multiply(origin, content), markerStyle, vp, how, depth, drawn);
        }

        p.active.popBack();

        // A marker that clips its overflow draws inside its viewport alone,
        // as lowerMarker clips it.
        if (!markerStyle.overflowVisible) {
            Box view = deviceBox(origin, Box{0.f, 0.f, w, h});

            drawn = Box{maxf(drawn.x0, view.x0), maxf(drawn.y0, view.y0), minf(drawn.x1, view.x1), minf(drawn.y1, view.y1)};
        }

        if (drawn.x0 <= drawn.x1 && drawn.y0 <= drawn.y1) {
            extend(box, Point{drawn.x0, drawn.y0});
            extend(box, Point{drawn.x1, drawn.y1});
        }
    }

    static void markerExtent(Pass& p, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp, Extent how, u32 depth, Box& box) {
        Node* markers[3];

        if (!shapeMarkers(p, style, markers)) {
            return;
        }

        Vector<MarkerVertex> vertices;
        Vector<MarkerVertex> deviceVertices;

        markerVertices(path, completes(node), vertices);
        markerVertices(style.nonScaling ? transformedPath(path, m) : path, completes(node), deviceVertices);

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

    // A path's length as its dashes are measured: a line and a straight
    // cubic by their chords, any other cubic along it, in the steps that
    // keep it within a hundredth of its chords.
    static float pathLength(const Path& path) {
        const Point* pts = path.points.data();
        size_t pi = 0;
        Point start{0.f, 0.f};
        Point last{0.f, 0.f};
        double total = 0.0;

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    start = pts[pi++];
                    last = start;
                    break;
                case VerbLine:
                    total += hypot((double)pts[pi].x - last.x, (double)pts[pi].y - last.y);
                    last = pts[pi++];
                    break;
                case VerbCubic:
                case VerbArcCubic: {
                    Point p[4] = {last, pts[pi], pts[pi + 1], pts[pi + 2]};

                    if (straightCubic(p[0], p[1], p[2], p[3])) {
                        total += hypot((double)p[3].x - p[0].x, (double)p[3].y - p[0].y);
                    } else {
                        Powers q = powers(p[0], p[1], p[2], p[3]);
                        int n = 1 + (int)fmin(curveRate(p, 0.01f), 1024.0);

                        for (int k = 0; k < n; k++) {
                            total += arcLength(q, (double)k / n, (double)(k + 1) / n);
                        }
                    }

                    last = p[3];
                    pi += 3;
                    break;
                }
                default:
                    // VerbClose.
                    total += hypot((double)start.x - last.x, (double)start.y - last.y);
                    last = start;
                    break;
            }
        }

        return (float)total;
    }

    // The path-length property, whose attribute pathLength is a number of
    // px, scales the dashes to the path's own length; none scales nothing.
    static ComputedStyle authorDashes(Lowering& l, const Node* node, const Path& path, const ComputedStyle& style, Viewport vp) {
        ComputedStyle dashed = style;
        u32 value = attribute(node, AtomPathLength);
        Length length;

        if (!style.dashCount || !value || !parseLength(textOf(*l.doc, value), length) || length.value < 0.f) {
            return dashed;
        }

        float author = resolveLength(length, Axis::Other, vp, style.fontSize);

        float total = pathLength(path);

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

        Box box = pathBox(path, identity());

        for (u8 layer : style.paintOrder) {
            if (layer == 0 && style.visible) {
                if (DevicePaint* fill = devicePaint(l, node, AtomFill, style.fill, style.fillOpacity, style, style.fill.context ? l.contextBox : box, style.fill.context ? l.contextMatrix : m, vp)) {
                    Flat flat;

                    flatten(path, m, tolerance, flat);
                    emitFill(l, flat, style.fillRule, fill, style.aliased, style.linearColors);
                }
            } else if (layer == 1 && style.visible && style.strokeWidth > 0.f) {
                if (DevicePaint* stroke = devicePaint(l, node, AtomStroke, style.stroke, style.strokeOpacity, style, style.stroke.context ? l.contextBox : box, style.stroke.context ? l.contextMatrix : m, vp)) {
                    ComputedStyle dashed = authorDashes(l, node, path, style, vp);
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

                    emitFill(l, device, WindingNonZero, stroke, style.aliased, style.linearColors);
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

        Matrix boxed = identity();
        Viewport inside{w, h, vp.screenWidth, vp.screenHeight, l.rootFontSize};
        Box viewBox;

        if (parseViewBox(*l.doc, node, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return;
            }

            boxed = viewBoxTransform(*l.doc, node, viewBox, w, h);
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, vp.screenWidth, vp.screenHeight, l.rootFontSize};
        }

        // A symbol's refX and refY put that point of its content at x and
        // y, the viewport with it; a side they leave out keeps the
        // viewport's edge there.
        if (node->name == AtomSymbol) {
            float rx;
            float ry;
            bool hasX = referenceOf(*l.doc, node, AtomRefX, Axis::X, inside, style.fontSize, inside.width, rx);
            bool hasY = referenceOf(*l.doc, node, AtomRefY, Axis::Y, inside, style.fontSize, inside.height, ry);
            Point ref = apply(boxed, Point{hasX ? rx : 0.f, hasY ? ry : 0.f});

            x -= hasX ? ref.x : 0.f;
            y -= hasY ? ref.y : 0.f;
        }

        Matrix inner = multiply(multiply(m, translation(x, y)), boxed);

        // The clip property cuts the viewport, overflow or not; not the
        // outermost svg's, whose CSS box is not absolutely positioned.
        Box cut{0.f, 0.f, w, h};
        bool cutting = node != l.doc->root && clipCut(style, w, h, vp, cut);
        Box kept = clip ? Box{maxf(cut.x0, 0.f), maxf(cut.y0, 0.f), minf(cut.x1, w), minf(cut.y1, h)} : cut;
        u32 clipIndex = (u32)l.out->length();

        clip = clip || cutting;

        if (clip) {
            u32 first = (u32)l.clips.length();

            Op op = groupOp(1.f);

            addRectClip(l, m, x + kept.x0, y + kept.y0, maxf(kept.x1 - kept.x0, 0.f), maxf(kept.y1 - kept.y0, 0.f));
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
            l.contextBox = deviceBox(boxTransform(l, target, style, vp), targetBox);
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

    // Reads up to four numbers apart by commas or spaces.
    static u32 readNumbers(StringView text, float* out, u32 most) {
        const u8* p = text.begin();
        u32 count = 0;

        while (count < most) {
            while (p < text.end() && (isSpace(*p) || *p == ',')) {
                p++;
            }

            if (!readNumber(p, text.end(), out[count])) {
                break;
            }

            count++;
        }

        return count;
    }

    // The view an image's fragment gives the document, viewBox its own
    // before (linking.md, SVGFragmentIdentifiersDefinitions): svgView()
    // with its viewBox() and transform(), preserveAspectRatio() being the
    // image's own; or the media fragment xywh=, in user units or a percent
    // of that viewBox (tst/spec/decisions.md, D19). A time segment after &
    // is no view. False where the view given has no width or height, so
    // nothing draws.
    static bool fragmentView(StringView fragment, Box& viewBox, Matrix& transform) {
        for (const u8* p = fragment.begin(); p < fragment.end(); p++) {
            if (*p == '&') {
                fragment = StringView(fragment.begin(), p);
                break;
            }
        }

        fragment = trim(fragment);

        float v[4];
        bool given = false;

        if (startsWith(fragment, SV("svgView(")) && fragment.back() == ')') {
            StringView rest(fragment.begin() + 8, fragment.end() - 1);

            while (!rest.empty()) {
                const u8* end = rest.begin();

                while (end < rest.end() && *end != ';') {
                    end++;
                }

                StringView part = trim(StringView(rest.begin(), end));

                rest = StringView(end < rest.end() ? end + 1 : end, rest.end());

                if (part.empty() || part.back() != ')') {
                    continue;
                }

                if (startsWith(part, SV("viewBox(")) && readNumbers(StringView(part.begin() + 8, part.end() - 1), v, 4) == 4) {
                    viewBox = Box{v[0], v[1], v[0] + v[2], v[1] + v[3]};
                    given = true;
                } else if (startsWith(part, SV("transform("))) {
                    parseTransform(StringView(part.begin() + 10, part.end() - 1), transform, TransformGrammar::Either);
                }
            }
        } else if (startsWith(fragment, SV("xywh="))) {
            StringView rest(fragment.begin() + 5, fragment.end());
            bool percent = startsWith(rest, SV("percent:"));

            if (percent || startsWith(rest, SV("pixel:"))) {
                rest = StringView(rest.begin() + (percent ? 8 : 6), rest.end());
            }

            if (readNumbers(rest, v, 4) == 4) {
                float sx = percent ? (viewBox.x1 - viewBox.x0) / 100.f : 1.f;
                float sy = percent ? (viewBox.y1 - viewBox.y0) / 100.f : 1.f;
                float ox = percent ? viewBox.x0 : 0.f;
                float oy = percent ? viewBox.y0 : 0.f;

                viewBox = Box{ox + v[0] * sx, oy + v[1] * sy, ox + (v[0] + v[2]) * sx, oy + (v[1] + v[3]) * sy};
                given = true;
            }
        }

        return !given || (viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0);
    }

    // The image a reference names, its raster decoded once for the key.
    static bool loadImageFrom(Pass& l, const void* key, StringView reference, ImageSource& out) {
        for (CachedImage* cached : l.images) {
            if (cached->key == key) {
                out = cached->source;

                return true;
            }
        }

        StringView mime;
        Buffer bytes;

        out.nested = nullptr;
        out.raster = nullptr;
        out.decoded = nullptr;
        out.cached = nullptr;
        out.ratio = nullptr;
        out.transform = identity();

        bool remote = startsWithIgnoreCase(reference, SV("http:")) || startsWithIgnoreCase(reference, SV("https:"));
        // A fragment alone names the document itself, drawn again as an
        // image (embedded.md, ImageElement); an image document references
        // nothing so, and the copy draws once.
        bool self = !reference.empty() && reference[0] == '#';
        StringView fragment = self ? StringView(reference.begin() + 1, reference.end()) : StringView();

        if (self && l.imageDepth > 0) {
            return false;
        }

        if (startsWithIgnoreCase(reference, SV("data:"))) {
            for (const u8* p = reference.end(); p > reference.begin(); p--) {
                if (p[-1] == '#') {
                    fragment = StringView(p, reference.end());
                    reference = StringView(reference.begin(), p - 1);
                    break;
                }
            }
        }

        bool broken = !self && (remote || (decodeDataUri(reference, mime, bytes) && bytes.length() == 0));

        if (!self && !remote && !broken && bytes.length() == 0) {
            return false;
        }

        StringView data = keep(*l.pool, bytes.data(), bytes.length());

        if (self || (equalsIgnoreCase(mime, SV("image/svg+xml")) && !broken)) {
            if (l.imageDepth >= 16) {
                return false;
            }

            Document* nested = self ? l.doc : l.pool->make<Document>();

            if (!self) {
                loadDocument(*nested, *l.pool, data, l.fail);
            }

            if (nested->root && nested->root->name == AtomSvg) {
                if (!self) {
                    cascade(*nested);
                }

                out.nested = nested;

                // The document's size, a side it leaves open zero: the
                // image's box fills it in, or, for an image of no size of
                // its own, a measure of what the document draws.
                givenSize(*nested, nested->root, out.width, out.height);

                Node* view = fragment.empty() ? nullptr : elementById(*nested, lookup(nested->atoms, fragment));
                Node* boxed = view && view->name == AtomView && parseViewBox(*nested, view, out.viewBox) ? view : nullptr;

                // The view's viewBox, else the root's, else the size; one
                // of no width or height draws nothing, as Chromium and
                // Firefox draw it.
                if (!boxed && !parseViewBox(*nested, nested->root, out.viewBox)) {
                    out.viewBox = Box{0.f, 0.f, out.width, out.height};
                } else if (!(out.viewBox.x1 > out.viewBox.x0) || !(out.viewBox.y1 > out.viewBox.y0)) {
                    return false;
                }

                out.ratio = boxed;

                if (!boxed && !fragmentView(fragment, out.viewBox, out.transform)) {
                    return false;
                }

                return true;
            }

            broken = true;
        }

        SvgImage* raster = !broken && l.decode ? l.decode(l.context, data.data(), data.length()) : nullptr;

        if (!raster) {
            return false;
        }

        out.decoded = l.pool->make<DecodedImage>(raster);

        u32 width = svg_image_width(raster);
        u32 height = svg_image_height(raster);

        out.raster = raster;
        out.width = (float)width;
        out.height = (float)height;
        out.viewBox = Box{0.f, 0.f, out.width, out.height};

        CachedImage* cached = (CachedImage*)l.pool->allocate(sizeof(CachedImage));

        out.cached = cached;
        *cached = CachedImage{key, out, width, height, 0};
        l.images.pushBack(cached);

        return true;
    }

    // The image of an element's href, which its callers see first.
    static bool loadImage(Pass& l, Node* node, ImageSource& out) {
        return loadImageFrom(l, node, trim(textOf(*l.doc, attribute(node, AtomHref))), out);
    }

    // Lets the decoded raster of a source go once nothing reads it.
    static void releaseImage(ImageSource& source) {
        if (source.decoded && source.decoded->image) {
            svg_image_destroy(source.decoded->image);
            source.decoded->image = nullptr;
        }

        source.raster = nullptr;
    }

    // The image averaged over blocks of 2^kx by 2^ky pixels, its last
    // column and row extended to fill them, the colours weighed by their
    // alpha: what halving it level by level comes to, without the levels
    // between or their rounding. Each output row sums its source rows into
    // one row of sums.
    static u8* shrinkImage(const u8* pixels, u32 width, u32 height, u32 kx, u32 ky) {
        u64 bx = (u64)1 << kx;
        u64 by = (u64)1 << ky;
        u32 w = (u32)((width + bx - 1) >> kx);
        u32 h = (u32)((height + by - 1) >> ky);
        u8* out = new u8[(size_t)w * h * 4];
        Vector<u64> sums;
        // The columns past the image that the last column stands for.
        u64 extra = ((u64)w << kx) - width;

        sums.zero((size_t)w * 4);

        for (u32 y = 0; y < h; y++) {
            u64* sum = sums.mutData();
            u64 first = (u64)y << ky;
            u64 last = first + by < height ? first + by : height;

            memset(sum, 0, sizeof(u64) * 4 * (size_t)w);

            for (u64 sy = first; sy < last; sy++) {
                const u8* row = pixels + (size_t)sy * width * 4;
                // The last row stands for the rows past the image.
                u64 weight = sy + 1 == height ? first + by - sy : 1;
                u32 x = 0;

                for (u32 block = 0; block < w; block++) {
                    u32 end = ((u64)(block + 1) << kx) < width ? (u32)((u64)(block + 1) << kx) : width;
                    u64 r = 0;
                    u64 g = 0;
                    u64 b = 0;
                    u64 a = 0;

                    for (; x < end; x++) {
                        const u8* t = row + (size_t)x * 4;
                        u32 alpha = t[3];

                        r += t[0] * alpha;
                        g += t[1] * alpha;
                        b += t[2] * alpha;
                        a += alpha;
                    }

                    u64* d = sum + (size_t)block * 4;

                    d[0] += r * weight;
                    d[1] += g * weight;
                    d[2] += b * weight;
                    d[3] += a * weight;
                }

                const u8* t = row + (size_t)(width - 1) * 4;
                u64* d = sum + (size_t)(w - 1) * 4;
                u64 alpha = (u64)t[3] * extra * weight;

                d[0] += t[0] * alpha;
                d[1] += t[1] * alpha;
                d[2] += t[2] * alpha;
                d[3] += alpha;
            }

            for (u32 x = 0; x < w; x++) {
                const u64* d = sum + (size_t)x * 4;
                u8* o = out + ((size_t)y * w + x) * 4;

                for (int c = 0; c < 3; c++) {
                    o[c] = d[3] ? (u8)((d[c] + d[3] / 2) / d[3]) : 0;
                }

                o[3] = (u8)((d[3] + (bx * by) / 2) / (bx * by));
            }
        }

        return out;
    }

    static bool usesBackground(const Document& doc, const Node* node, u32 depth = 0);

    // The paint of a raster under the matrix that takes its pixels to the
    // device: of a CSS image, sampled so and round the sides wraps names;
    // reading the part's pixels alone; null for a matrix of no inverse.
    static DevicePaint* rasterPaint(Lowering& l, CachedImage* cached, const Matrix& place, u8 rendering, bool tent, u8 wraps, const Box& part) {
        DevicePaint* paint = l.pool->make<DevicePaint>();
        Matrix inverse;

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Image;
        paint->imageWidth = cached->width;
        paint->imageHeight = cached->height;
        paint->tent = tent;
        paint->wraps = wraps;

        if (!invert(place, inverse)) {
            return nullptr;
        }

        float scale = minf(sqrtf(place.a * place.a + place.b * place.b), sqrtf(place.c * place.c + place.d * place.d));
        int levels = scale < 1.f ? (int)floorf(maxf(-log2f(scale) - 0.5f, 0.f) + 0.5f) : 0;

        paint->smooth = rendering == 0 || (rendering == 2 && scale < 1.f);
        levels = paint->smooth ? levels : 0;

        // Each side halves as many of the levels as it has pixels to: a side
        // of one stays.
        u32 kx = 0;
        u32 ky = 0;

        while (kx < (u32)levels && ((u64)1 << kx) < paint->imageWidth) {
            kx++;
        }

        while (ky < (u32)levels && ((u64)1 << ky) < paint->imageHeight) {
            ky++;
        }

        // The paint samples the image shrunk so, its pixels read when a fill
        // first draws it.
        paint->source = cached;
        paint->kx = kx;
        paint->ky = ky;
        paint->imageWidth = (u32)((paint->imageWidth + ((u64)1 << kx) - 1) >> kx);
        paint->imageHeight = (u32)((paint->imageHeight + ((u64)1 << ky) - 1) >> ky);
        cached->paintsLeft++;

        paint->inverse = multiply(scaling(ldexpf(1.f, -(int)kx), ldexpf(1.f, -(int)ky)), inverse);
        paint->texels = IntBox{(int)floorf(ldexpf(part.x0, -(int)kx)), (int)floorf(ldexpf(part.y0, -(int)ky)), (int)ceilf(ldexpf(part.x1, -(int)kx)), (int)ceilf(ldexpf(part.y1, -(int)ky))};

        return paint;
    }

    // An image in its box: of an image element, as its preserveAspectRatio
    // fits it; of none, a CSS image, a raster stretched over the box and a
    // document as its own root fits it.
    static void drawImage(Lowering& l, Node* node, const ImageSource& source, const Matrix& m, Viewport vp, float x, float y, float w, float h, u8 rendering) {
        const Document& doc = *l.doc;
        Box viewBox = source.viewBox;

        // A side a nested document leaves open is the box's, as it is.
        if (viewBox.x1 <= viewBox.x0) {
            viewBox.x1 = viewBox.x0 + w;
        }

        if (viewBox.y1 <= viewBox.y0) {
            viewBox.y1 = viewBox.y0 + h;
        }

        // An image element fits what it draws by its own
        // preserveAspectRatio, a document's root and view taking none
        // (embedded.md, ImageElement); a CSS image of a document by the
        // document's.
        Matrix fit = multiply(translation(x, y), node ? viewBoxTransform(doc, node, viewBox, w, h) : (source.nested ? viewBoxTransform(*source.nested, source.ratio ? source.ratio : source.nested->root, viewBox, w, h) : scaling(w / viewBox.x1, h / viewBox.y1)));
        Matrix place = multiply(m, fit);

        CachedImage* cached = source.cached;

        if (cached) {
            DevicePaint* paint = rasterPaint(l, cached, place, rendering, !node, 0, Box{0.f, 0.f, source.width, source.height});

            if (!paint) {
                return;
            }

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
        lowerChildren(l, nested->root, rootStyle, multiply(place, source.transform), Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, source.width > 0.f ? source.width : w, source.height > 0.f ? source.height : h, rootStyle.fontSize});
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

    // Where an element's transform turns about: its transform-origin in the
    // box transform-box names, or in the viewport. False where it turns
    // about the user space's origin as it is.
    static bool transformAnchor(Pass& l, Node* node, const ComputedStyle& parent, Viewport vp, Point& o) {
        const Document& doc = *l.doc;
        u32 kind = attribute(node, AtomTransformBox);
        StringView box = kind ? trim(textOf(doc, kind)) : StringView();
        u32 origin = attribute(node, AtomTransformOrigin);
        Box bounds;

        // An SVG element has no CSS box: content-box is its fill box and
        // border-box its stroke box.
        bool fill = box == SV("fill-box") || box == SV("content-box");
        bool stroke = box == SV("stroke-box") || box == SV("border-box");

        if (!(fill || stroke)) {
            o = origin ? transformOrigin(textOf(doc, origin), vp.width, vp.height) : Point{vp.width / 2.f, vp.height / 2.f};

            return origin || node == doc.root;
        }

        if (!elementBox(l, node, parent, vp, bounds, 0)) {
            return false;
        }

        if (stroke) {
            bounds = strokeBox(l, node, parent, vp);
        }

        o = origin ? transformOrigin(textOf(doc, origin), bounds.x1 - bounds.x0, bounds.y1 - bounds.y0) : (node == doc.root ? Point{(bounds.x1 - bounds.x0) / 2.f, (bounds.y1 - bounds.y0) / 2.f} : Point{0.f, 0.f});
        o.x += bounds.x0;
        o.y += bounds.y0;

        return true;
    }

    static Matrix boxTransform(Pass& l, Node* node, const ComputedStyle& parent, Viewport vp) {
        u32 value = attribute(node, AtomTransform);
        Matrix m;
        Point o;

        if (!value) {
            return identity();
        }

        parseTransform(textOf(*l.doc, value), m, TransformGrammar::Either);

        if (!transformAnchor(l, node, parent, vp, o)) {
            return m;
        }

        return multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
    }

    // An element's transform in perspective, about its anchor; false for an
    // affine one, which may still be solid, of 3D functions.
    static bool boxProjective(Pass& l, Node* node, const ComputedStyle& parent, Viewport vp, Projective& out) {
        u32 value = attribute(node, AtomTransform);
        Matrix m;
        Point o;

        out.solid = false;

        if (!value) {
            return false;
        }

        parseTransform(textOf(*l.doc, value), m, TransformGrammar::Either, &out);

        if (!out.on) {
            return false;
        }

        if (transformAnchor(l, node, parent, vp, o)) {
            out = compose(compose(projectiveOf(translation(o.x, o.y)), out), projectiveOf(translation(-o.x, -o.y)));
        }

        return true;
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

                float x = lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f);
                float y = lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f);

                l.sized = nullptr;
                lowerViewport(l, node, style, m, vp, node, x, y, w, h, !style.overflowVisible);
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

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp);

    // Draws an element of a transform in perspective as Firefox draws it: on
    // a layer of its own user space, its extent and as far again around it,
    // at the scale the transform has at its middle, then through the
    // transform: each device pixel takes the layer between pixels where the
    // plane is in front of the eye.
    static void lowerProjectiveLayer(Lowering& l, Node* node, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& ctm, const Projective& perspective, Viewport vp) {
        Projective device = compose(projectiveOf(ctm), perspective);
        Projective fromDevice;
        Box local;

        if (!elementBox(l, node, parent, vp, local, 0) || !invertProjective(device, fromDevice)) {
            return;
        }

        float margin = maxf(local.x1 - local.x0, local.y1 - local.y0) / 2.f + style.strokeWidth;
        Box area{local.x0 - margin, local.y0 - margin, local.x1 + margin, local.y1 + margin};
        Point middle{(local.x0 + local.x1) / 2.f, (local.y0 + local.y1) / 2.f};
        Point at;

        if (!projectPoint(device, middle, at)) {
            return;
        }

        // The scale the device has at the middle: the columns of the map's
        // derivative there, the linear part less the point by the weight's
        // gradient, over the weight.
        const float* d = device.m;
        float weight = d[6] * middle.x + d[7] * middle.y + d[8];
        float columns[2][2] = {{(d[0] - at.x * d[6]) / weight, (d[3] - at.y * d[6]) / weight}, {(d[1] - at.x * d[7]) / weight, (d[4] - at.y * d[7]) / weight}};
        float scale = minf(maxf(maxf(hypotf(columns[0][0], columns[0][1]), hypotf(columns[1][0], columns[1][1])), 0.0625f), 16.f);
        float lw = ceilf((area.x1 - area.x0) * scale);
        float lh = ceilf((area.y1 - area.y0) * scale);
        float budget = 16.f * (float)l.width * (float)l.height;

        if (!(lw * lh <= budget)) {
            float fit = sqrtf(budget / (lw * lh));

            scale *= fit;
            lw = ceilf((area.x1 - area.x0) * scale);
            lh = ceilf((area.y1 - area.y0) * scale);
        }

        Matrix toLayer = multiply(scaling(scale, scale), translation(-area.x0, -area.y0));
        Matrix fromLayer;

        invert(toLayer, fromLayer);

        Projective* maps = (Projective*)l.pool->allocate(sizeof(Projective) * 2);

        maps[0] = compose(projectiveOf(toLayer), fromDevice);
        maps[1] = compose(device, projectiveOf(fromLayer));

        Vector<Op> content;
        Vector<Op>* saved = l.out;
        u32 width = l.width;
        u32 height = l.height;

        l.width = (u32)lw;
        l.height = (u32)lh;
        l.out = &content;
        l.flattened = node;
        lowerElement(l, node, parent, toLayer, vp);
        l.out = saved;
        l.width = width;
        l.height = height;

        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Layer;
        op.first = (u32)l.programs.length();
        op.count = (u32)content.length();
        op.layerWidth = (u32)lw;
        op.layerHeight = (u32)lh;
        op.layerMatrix = toLayer;
        op.perspective = maps;
        op.bounds = projectBox(maps[1], Box{-2.f, -2.f, lw + 2.f, lh + 2.f}, Box{0.f, 0.f, (float)width, (float)height});
        l.programs.append(content.data(), content.length());
        l.out->pushBack(op);
    }

    // vector-effect's constrained transformations, SVG 2's formulas in the
    // host's space, the device's: the user space keeps its origin where the
    // CTM puts it, or for fixed-position at the e and f of the element's
    // own transform; non-scaling-size takes the scale out of the CTM, by
    // the root of its determinant, non-rotation the rotation and skew,
    // both all of it. Of a CTM of no area non-scaling-size is none, and so
    // is every effect.
    static Matrix vectorEffectMatrix(const Matrix& host, const Matrix& m, const Matrix& own, u8 effects) {
        Matrix hostInverse;
        bool size = effects & 1;
        bool rotation = effects & 2;

        if (!invert(host, hostInverse)) {
            return m;
        }

        Matrix ctm = multiply(hostInverse, m);
        float area = fabsf(ctm.a * ctm.d - ctm.b * ctm.c);
        float root = sqrtf(area);
        Matrix linear{ctm.a, ctm.b, ctm.c, ctm.d, 0.f, 0.f};

        if (size && !(area > 0.f)) {
            return m;
        }

        if (size && rotation) {
            linear = identity();
        } else if (size) {
            linear = Matrix{ctm.a / root, ctm.b / root, ctm.c / root, ctm.d / root, 0.f, 0.f};
        } else if (rotation) {
            linear = scaling(root, root);
        }

        linear.e = effects & 4 ? own.e : ctm.e;
        linear.f = effects & 4 ? own.f : ctm.f;

        return multiply(host, linear);
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

        // A symbol a use instances displays inline, whatever its display
        // (struct.md, SymbolNotes).
        if ((style.displayNone && node != l.doc->root && !instance) || style.opacity <= 0.f) {
            return;
        }

        // A transform in perspective draws the element on a layer of its
        // own, where the element is again, its transform the layer's.
        Projective perspective{{}, false, false};
        bool flattened = l.flattened == node;

        l.flattened = nullptr;

        if (!flattened && boxProjective(l, node, parent, vp, perspective)) {
            lowerProjectiveLayer(l, node, parent, style, ctm, perspective, vp);

            return;
        }

        Matrix own = flattened ? identity() : boxTransform(l, node, parent, vp);
        Matrix m = multiply(ctm, own);
        Op op = groupOp(style.opacity);

        if (style.vectorEffects && (isShape(node->name) || node->name == AtomImage || node->name == AtomUse)) {
            m = vectorEffectMatrix(l.host, m, own, style.vectorEffects);
        }

        if (node->name == AtomUse) {
            m = multiply(m, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, style.fontSize, 0.f)));
        }

        if (style.clipPath) {
            op.clipped = resolveClip(l, node, parent, style, m, vp, op);
        } else if (style.clipShape) {
            shapeClip(l, node, parent, style, m, vp, op);
            op.clipped = true;
        }

        if (style.maskParts[MaskPartImage] || style.maskBorder[MaskBorderPartSource]) {
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
        op.linear = style.linearColors && (isShape(node->name) || node->name == AtomImage);

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

        // A shape lays its fills over in linear light itself; an image is
        // drawn apart to be.
        // A 3D transform isolates, as a layer, though it is affine; a 2D
        // one does not (compositing-1.md, csscompositingrules_SVG).
        bool layer = op.clipped || op.masked || op.filter || style.opacity < 1.f || style.blend || style.isolate || perspective.solid || (op.linear && node->name == AtomImage);

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

    // The pixels of a buffer of the given pixels, zero.
    static u16* makePixels(size_t count) {
        u16* data = new u16[count * 4];

        memset(data, 0, sizeof(u16) * count * 4);

        return data;
    }

    // A box is empty as boxInside leaves it, by its x.
    static Plane makePlane(IntBox box) {
        size_t count = box.x1 > box.x0 ? (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0) : 0;

        return Plane{count ? makePixels(count) : nullptr, box};
    }

    static void releasePlane(Plane& plane) {
        delete[] plane.data;
        plane.data = nullptr;
    }

    static int planeWidth(const Plane& plane) {
        return plane.box.x1 - plane.box.x0;
    }

    // The stored pixel at x, y of a plane whose box holds it.
    static u16* planeAt(const Plane& plane, int x, int y) {
        return plane.data + ((size_t)(y - plane.box.y0) * (size_t)planeWidth(plane) + (size_t)(x - plane.box.x0)) * 4;
    }

    // The stored pixel at x, y, zero outside the plane.
    static const u16* planeRead(const Plane& plane, int x, int y) {
        if (x < plane.box.x0 || x >= plane.box.x1 || y < plane.box.y0 || y >= plane.box.y1) {
            return zeroStored;
        }

        return planeAt(plane, x, y);
    }

    // Stores the pixels from x0 to x1 of row y of a plane whose box holds them.
    static void writeSpan(const Plane& plane, int x0, int x1, int y, const float* src) {
        storeSpan(planeAt(plane, x0, y), src, (size_t)(x1 - x0));
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

    // The stored pixels from x0 to x1 of row y of a plane, zero outside it.
    static void readStored(const Plane& plane, int x0, int x1, int y, u16* dst) {
        IntBox inside = boxInside(IntBox{x0, y, x1, y + 1}, plane.box);

        memset(dst, 0, sizeof(u16) * 4 * (size_t)(x1 - x0));

        if (inside.x1 > inside.x0) {
            memcpy(dst + (size_t)(inside.x0 - x0) * 4, planeAt(plane, inside.x0, y), sizeof(u16) * 4 * (size_t)(inside.x1 - inside.x0));
        }
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

    // A coverage in units of the accumulator, 65536 for a whole pixel,
    // rounded alike either side of zero.
    static i32 fixedCover(float v) {
        return (i32)rintf(v * 65536.f);
    }

    // A line's coverage of the rows it crosses, a cell at a time: each cell
    // takes the difference of the rounded running total of the row, so that
    // the cells sum to the line's rounded height in the row exactly.
    static void addLine(u32* acc, u32 stride, u32 width, float x0, float y0, float x1, float y1) {
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
            u32* line = acc + (size_t)y * stride;
            float dy = minf((float)(y + 1), y1) - maxf((float)y, y0);
            float xnext = x + dxdy * dy;
            float d = dy * dir;
            float xa = minf(maxf(minf(x, xnext), 0.f), w);
            float xb = minf(maxf(maxf(x, xnext), 0.f), w);
            float x0floor = floorf(xa);
            int x0i = (int)x0floor;
            float x1ceil = ceilf(xb);
            int x1i = (int)x1ceil;
            i32 total = fixedCover(d);

            if (x1i <= x0i + 1) {
                float xmf = 0.5f * (xa + xb) - x0floor;
                i32 first = fixedCover(d - d * xmf);

                line[x0i] += (u32)first;
                line[x0i + 1] += (u32)(total - first);
            } else {
                float s = 1.f / (xb - xa);
                float x0f = xa - x0floor;
                float a0 = 0.5f * s * (1.f - x0f) * (1.f - x0f);
                float x1f = xb - x1ceil + 1.f;
                float am = 0.5f * s * x1f * x1f;
                i32 done = fixedCover(d * a0);
                i32 next;

                line[x0i] += (u32)done;

                if (x1i > x0i + 2) {
                    float a1 = s * (1.5f - x0f);

                    // The running total through each cell before the last two.
                    for (int xi = x0i + 1; xi < x1i - 1; xi++) {
                        next = fixedCover(d * (a1 + (float)(xi - x0i - 1) * s));
                        line[xi] += (u32)(next - done);
                        done = next;
                    }
                }

                next = fixedCover(d * (1.f - am));
                line[x1i - 1] += (u32)(next - done);
                line[x1i] += (u32)(total - next);
            }

            x = xnext;
        }
    }

    static void addEdge(u32* acc, u32 stride, u32 width, Edge e) {
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

    // The stored coverage of a running sum of the accumulator: nonzero
    // clamps its magnitude to a whole pixel, even-odd folds it every two.
    static u32 coverStored(u32 sum, u8 rule) {
        u32 a = sum >> 31 ? 0u - sum : sum;

        if (rule == WindingEvenOdd) {
            a &= 0x1ffffu;
            a = a > 0x10000u ? 0x20000u - a : a;
        } else {
            a = a < 0x10000u ? a : 0x10000u;
        }

        return a - (a >> 16);
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

    // The colour of the gradient at t, premultiplied: the stored stops are
    // blended straight in fixed point and the result multiplied by its alpha.
    static void gradientColor(const DevicePaint& paint, float t, u16* out) {
        const GradientStop* stops = paint.stops;
        u32 count = paint.stopCount;
        u16 c[4];

        if (t <= stops[0].offset) {
            memcpy(c, stops[0].stored, sizeof(c));
        } else if (t >= stops[count - 1].offset) {
            memcpy(c, stops[count - 1].stored, sizeof(c));
        } else {
            // t lies past the first offset and before the last, so a stop
            // follows past it and the span to it is not empty: the first
            // one, as the offsets never decrease. Where stops inside share
            // an offset, the later one rules there, as SVG and CSS have it;
            // at the first offset and before, the first, as browsers draw.
            u32 i = 1;
            u32 n = count - 1;

            while (i < n) {
                u32 m = i + (n - i) / 2;

                if (stops[m].offset <= t) {
                    i = m + 1;
                } else {
                    n = m;
                }
            }

            const GradientStop& a = stops[i - 1];
            const GradientStop& b = stops[i];
            u32 f = toUnit16((t - a.offset) / (b.offset - a.offset));

            for (int k = 0; k < 4; k++) {
                c[k] = mix16(a.stored[k], b.stored[k], f);
            }
        }

        // Stops blended in linear light go back to sRGB on the curve.
        if (paint.linear) {
            float v[4];

            loadPixel(c, v);
            v[0] = linearToSrgb(v[0]);
            v[1] = linearToSrgb(v[1]);
            v[2] = linearToSrgb(v[2]);
            storePixel(c, v);
        }

        out[0] = (u16)div65535((u32)c[0] * c[3]);
        out[1] = (u16)div65535((u32)c[1] * c[3]);
        out[2] = (u16)div65535((u32)c[2] * c[3]);
        out[3] = c[3];
    }

    // The table of a gradient: its colours at as many evenly spaced t from 0
    // to 1 as the gradient spans device pixels twice over, from 16 to 4096.
    // A pixel blends the two entries around its t: the colour is linear in t
    // between stops, so only an entry step holding a stop, half a pixel at
    // most, departs from it. The device length of the gradient is at most
    // its length over the least scale of the map to its space.
    static void prepareGradient(const Lowering& l, DevicePaint& paint) {
        if (paint.table) {
            return;
        }

        const Matrix& m = paint.inverse;
        float extent = paint.type == PaintType::Linear ? hypotf(paint.x2 - paint.x1, paint.y2 - paint.y1) : (paint.type == PaintType::Conic ? paint.x2 : paint.r + hypotf(paint.cx - paint.fx, paint.cy - paint.fy));
        float frobenius = sqrtf(m.a * m.a + m.b * m.b + m.c * m.c + m.d * m.d);
        float length = extent * frobenius / fabsf(m.a * m.d - m.b * m.c);
        u32 size = 16;

        while (size < 4096 && (float)size < 2.f * length) {
            size *= 2;
        }

        paint.table = (u16*)l.pool->allocate(sizeof(u16) * 4 * size);
        paint.kinks = (u8*)l.pool->allocate(size);
        paint.tableSize = size;

        u64 sums[4] = {0, 0, 0, 0};

        for (u32 i = 0; i < size; i++) {
            gradientColor(paint, (float)i / (float)(size - 1), paint.table + (size_t)i * 4);

            for (int k = 0; k < 4; k++) {
                sums[k] += paint.table[(size_t)i * 4 + k];
            }
        }

        for (int k = 0; k < 4; k++) {
            paint.beyond16[k] = paint.spread == SpreadPad ? paint.table[(size_t)(size - 1) * 4 + k] : (u16)((sums[k] + size / 2) / size);
        }

        // A step holding a stop, at either end too, is not linear in t; nor is
        // any step of stops blended in linear light, which the sRGB curve bends.
        memset(paint.kinks, paint.linear ? 1 : 0, size);

        // A stop at an entry ends the step before it and starts the one after.
        for (u32 k = 0; k < paint.stopCount; k++) {
            float at = paint.stops[k].offset * (float)(size - 1);

            paint.kinks[(u32)minf(floorf(at), (float)(size - 2))] = 1;
            paint.kinks[(u32)maxf(ceilf(at) - 1.f, 0.f)] = 1;
        }
    }

    // Where t falls in the gradient's table: the step and how far into it.
    static u32 tableStep(const DevicePaint& paint, float t, u32& f) {
        float at = minf(maxf(t, 0.f), 1.f) * (float)(paint.tableSize - 1);
        float whole = minf(floorf(at), (float)(paint.tableSize - 2));

        f = toUnit16(at - whole);

        return (u32)whole;
    }

    // The colour of the gradient at t: the blend of the two entries around
    // it, or, in a step holding a stop, the stops themselves.
    static void gradientAt(const DevicePaint& paint, float t, u16* out) {
        u32 f;
        u32 i = tableStep(paint, t, f);
        const u16* a = paint.table + (size_t)i * 4;

        if (paint.kinks[i]) {
            gradientColor(paint, minf(maxf(t, 0.f), 1.f), out);

            return;
        }

        for (int k = 0; k < 4; k++) {
            out[k] = mix16(a[k], a[4 + k], f);
        }
    }

    // A cubic of the fraction t, 65536 a whole one, by Horner's rule, its
    // coefficients 2^30ths: the cubic in 2^30ths, each step shifted down
    // to that, and so less by under three of them.
    static i64 cubic(i64 t, i64 c0, i64 c1, i64 c2, i64 c3) {
        i64 v = (c3 * t >> 16) + c2;

        v = (v * t >> 16) + c1;

        return (v * t >> 16) + c0;
    }

    // Mitchell's filter at the fraction t of a texel, 65536 a whole one,
    // for the texels one before, at, one after and two after: times 18,
    // its cubics there are 1 - 9t + 15t^2 - 7t^3, 16 - 36t^2 + 21t^3,
    // 1 + 9t + 27t^2 - 21t^3 and 7t^3 - 6t^2. The weights are 16384ths;
    // the one at takes what rounding the others leaves, so the four sum
    // to one.
    static void mitchellWeights(i64 t, i32* w) {
        constexpr i64 unit = ((i64)1 << 30) / 18;
        constexpr i64 half = (i64)1 << 15;

        w[0] = (i32)((cubic(t, unit, -9 * unit, 15 * unit, -7 * unit) + half) >> 16);
        w[2] = (i32)((cubic(t, unit, 9 * unit, 27 * unit, -21 * unit) + half) >> 16);
        w[3] = (i32)((cubic(t, 0, 0, -6 * unit, 7 * unit) + half) >> 16);
        w[1] = 16384 - w[0] - w[2] - w[3];
    }

    // a modulo m, in 0 to m - 1 for a negative a too.
    static int floorMod(int a, int m) {
        return ((a % m) + m) % m;
    }

    // The t of a gradient at a point of its space, spread; false where a
    // radial gradient paints nothing.
    static bool gradientT(const DevicePaint& paint, Point p, float& t) {
        // A conic's turn from straight up, clockwise, past its start.
        if (paint.type == PaintType::Conic) {
            float turn = atan2f(p.x - paint.cx, paint.cy - p.y) / (2.f * pi) - paint.x1 / 360.f;

            turn -= floorf(turn);
            t = spreadT((turn - paint.fr) / (paint.r - paint.fr), paint.spread);

            return true;
        }

        if (paint.type == PaintType::Linear) {
            float dx = paint.x2 - paint.x1;
            float dy = paint.y2 - paint.y1;

            t = spreadT(((p.x - paint.x1) * dx + (p.y - paint.y1) * dy) / (dx * dx + dy * dy), paint.spread);

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

        t = spreadT(t, paint.spread);

        return true;
    }

    // The blend of four stored pixels around a point tx, ty past the first:
    // the weights sum to 65535, so the sum of the products fits.
    static void bilinear(const u16* c00, const u16* c10, const u16* c01, const u16* c11, float tx, float ty, u16* out) {
        u32 wx = toUnit16(tx);
        u32 wy = toUnit16(ty);
        u32 w11 = div65535(wx * wy);
        u32 w10 = wx - w11;
        u32 w01 = wy - w11;
        u32 w00 = 65535u - wx - wy + w11;

        for (int k = 0; k < 4; k++) {
            out[k] = (u16)div65535(c00[k] * w00 + c10[k] * w10 + c01[k] * w01 + c11[k] * w11);
        }
    }

    // A texel's column or row among those from lo to hi: clamped to them,
    // or round them.
    static int texel(int v, int lo, int hi, bool wrap) {
        return wrap ? lo + floorMod(v - lo, hi - lo) : (v < lo ? lo : (v >= hi ? hi - 1 : v));
    }

    // The paint at x, y stored premultiplied; false where it has none.
    static bool paintAt(const DevicePaint& paint, float x, float y, u16* out) {
        if (paint.type == PaintType::Solid) {
            memcpy(out, paint.color16, sizeof(u16) * 4);

            return true;
        }

        Point p = apply(paint.inverse, Point{x, y});

        if (paint.type == PaintType::Mesh) {
            int ix = (int)floorf(p.x);
            int iy = (int)floorf(p.y);

            if (ix < 0 || iy < 0 || ix >= (int)paint.imageWidth || iy >= (int)paint.imageHeight) {
                return false;
            }

            memcpy(out, paint.tile + ((size_t)iy * paint.imageWidth + ix) * 4, sizeof(u16) * 4);

            return out[3] > 0;
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
            // A tile pixel is in the window once, at its unwrapped place.
            const IntBox& window = paint.window;
            size_t ww = (size_t)(window.x1 - window.x0);
            auto at = [&](int x, int y) {
                int ux = floorMod(x - window.x0, tw);
                int uy = floorMod(y - window.y0, th);

                return paint.tile + ((size_t)uy * ww + (size_t)ux) * 4;
            };
            const u16* c00 = at(ix, iy);
            const u16* c10 = at(jx, iy);
            const u16* c01 = at(ix, jy);
            const u16* c11 = at(jx, jy);
            bilinear(c00, c10, c01, c11, tx, ty, out);

            for (int k = 0; k < 4; k++) {
                out[k] = (u16)div65535((u32)out[k] * paint.opacity16);
            }

            return out[3] > 0;
        }

        if (paint.type == PaintType::Image) {
            // The nearest texel: a byte b is b * 257 stored.
            if (!paint.smooth) {
                int x = texel((int)floorf(p.x), paint.texels.x0, paint.texels.x1, paint.wraps & 1);
                int y = texel((int)floorf(p.y), paint.texels.y0, paint.texels.y1, paint.wraps & 2);
                const u8* t = paint.image + ((size_t)y * paint.imageWidth + x) * 4;
                u32 a = t[3] * 257u;

                out[0] = (u16)div65535(t[0] * 257u * a);
                out[1] = (u16)div65535(t[1] * 257u * a);
                out[2] = (u16)div65535(t[2] * 257u * a);
                out[3] = (u16)a;

                return true;
            }

            // A CSS image's four texels, premultiplied, weighed by their
            // nearness.
            if (paint.tent) {
                float fx = p.x - 0.5f;
                float fy = p.y - 0.5f;
                float x0 = floorf(fx);
                float y0 = floorf(fy);
                u16 c[4][4];

                for (int k = 0; k < 4; k++) {
                    int x = texel((int)x0 + (k & 1), paint.texels.x0, paint.texels.x1, paint.wraps & 1);
                    int y = texel((int)y0 + (k >> 1), paint.texels.y0, paint.texels.y1, paint.wraps & 2);
                    const u8* t = paint.image + ((size_t)y * paint.imageWidth + x) * 4;
                    u32 a = t[3] * 257u;

                    for (int j = 0; j < 3; j++) {
                        c[k][j] = (u16)div65535(t[j] * 257u * a);
                    }

                    c[k][3] = (u16)a;
                }

                bilinear(c[0], c[1], c[2], c[3], fx - x0, fy - y0, out);

                return true;
            }

            // Mitchell's filter, in integers: each texel's colour times its
            // alpha and its alpha times 255, 65025 a whole one, by the
            // weights of its column, rounded once a row, then by the weights
            // of its row. The negative lobes can take a sum below none or
            // past the alpha, where it clamps.
            float fx = p.x - 0.5f;
            float fy = p.y - 0.5f;
            float x0 = floorf(fx);
            float y0 = floorf(fy);
            i32 wx[4];
            i32 wy[4];
            int xs[4];
            int ys[4];

            mitchellWeights((i64)((fx - x0) * 65536.f + 0.5f), wx);
            mitchellWeights((i64)((fy - y0) * 65536.f + 0.5f), wy);

            for (int i = 0; i < 4; i++) {
                int x = (int)x0 - 1 + i;
                int y = (int)y0 - 1 + i;

                xs[i] = x < 0 ? 0 : (x >= (int)paint.imageWidth ? (int)paint.imageWidth - 1 : x);
                ys[i] = y < 0 ? 0 : (y >= (int)paint.imageHeight ? (int)paint.imageHeight - 1 : y);
            }

            i32 sum[4] = {0, 0, 0, 0};

            for (int j = 0; j < 4; j++) {
                const u8* row = paint.image + (size_t)ys[j] * paint.imageWidth * 4;
                i32 line[4] = {0, 0, 0, 0};

                for (int i = 0; i < 4; i++) {
                    const u8* t = row + (size_t)xs[i] * 4;
                    i32 weight = t[3] * wx[i];
                    i32 by[4] = {weight, weight, weight, 255 * wx[i]};

                    for (int k = 0; k < 4; k++) {
                        line[k] += t[k] * by[k];
                    }
                }

                for (int k = 0; k < 4; k++) {
                    sum[k] += ((line[k] + 8192) >> 14) * wy[j];
                }
            }

            // 65025 is a whole one, 65535 a stored one: the sums scale by
            // 257 / 255.
            u32 c[4];

            for (int k = 0; k < 4; k++) {
                c[k] = clampStored((((sum[k] + 8192) >> 14) * 257 + 127) / 255);
            }

            for (int k = 0; k < 3; k++) {
                out[k] = (u16)(c[k] < c[3] ? c[k] : c[3]);
            }

            out[3] = (u16)c[3];

            return true;
        }

        float t;

        if (!gradientT(paint, p, t)) {
            memcpy(out, paint.beyond16, sizeof(paint.beyond16));

            return paint.beyond;
        }

        gradientAt(paint, t, out);

        return true;
    }

    static void addAliasedEdge(u32* acc, u32 stride, u32 width, u32 height, Edge e) {
        u32 dir = 0x10000u;

        if (e.y0 > e.y1) {
            e = Edge{e.x1, e.y1, e.x0, e.y0};
            dir = 0u - 0x10000u;
        }

        // Rows and columns clamp to the raster; one that is not a number
        // takes its start.
        float dxdy = (e.x1 - e.x0) / (e.y1 - e.y0);
        int y0 = (int)minf(maxf(floorf(e.y0 + 0.5f), 0.f), (float)height);
        int y1 = (int)minf(maxf(floorf(e.y1 + 0.5f), 0.f), (float)height);

        for (int y = y0; y < y1; y++) {
            int column = (int)minf(maxf(floorf(e.x0 + ((float)y + 0.5f - e.y0) * dxdy + 0.5f), 0.f), (float)width);

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
    // Only samples that cross come here, and one whose x is not a number
    // crosses none, so the keys are numbers.
    static void halfBandSamples(const BandEdge* edges, const BandSample* whole, size_t count, float a, float b, bool top, Vector<BandSample>& samples) {
        float mid = (a + b) * 0.5f;

        samples.clear();

        for (size_t i = 0; i < count; i++) {
            const BandSample& s = whole[i];

            samples.pushBack(BandSample{top ? s.x0 : s.key, top ? s.key : s.x1, bandX(edges[s.edge], mid), s.dir, s.order, s.edge});
        }
    }

    // The coverage of a band from a to b of what the rule fills, entered
    // with the winding of the samples left of them. A sample that crosses
    // none before it by more than a sixty-fourth of a pixel keeps its place
    // through the band and is drawn whole; each run of samples that do
    // cross is halved, up to six times or until the middle rounds onto an
    // end, and its halves entered with the winding left of the run.
    static void unionBand(u32* acc, u32 stride, u32 w, const BandEdge* edges, Vector<BandSample>* samples, float a, float b, u8 rule, int depth, bool comparable, int winding) {
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

        float mid = (a + b) * 0.5f;
        bool halve = depth < 6 && a < mid && mid < b;
        float reach0 = sorted[0].x0;
        float reach1 = sorted[0].x1;

        for (size_t k = 0; k < count;) {
            size_t end = k + 1;

            while (end < count && (reach0 > sorted[end].x0 + 1.f / 64.f || reach1 > sorted[end].x1 + 1.f / 64.f)) {
                reach0 = maxf(reach0, sorted[end].x0);
                reach1 = maxf(reach1, sorted[end].x1);
                end++;
            }

            if (end - k > 1 && halve) {
                Vector<BandSample>& half = samples[depth + 1];

                halfBandSamples(edges, sorted + k, end - k, a, mid, true, half);
                unionBand(acc, stride, w, edges, samples, a, mid, rule, depth + 1, true, winding);
                halfBandSamples(edges, sorted + k, end - k, mid, b, false, half);
                unionBand(acc, stride, w, edges, samples, mid, b, rule, depth + 1, true, winding);

                for (size_t i = k; i < end; i++) {
                    winding += sorted[i].dir;
                }
            } else {
                for (size_t i = k; i < end; i++) {
                    const BandSample& sample = sorted[i];
                    bool before = insideWinding(winding, rule);

                    winding += sample.dir;

                    bool after = insideWinding(winding, rule);

                    if (before != after) {
                        addEdge(acc, stride, w, after ? Edge{sample.x0, a, sample.x1, b} : Edge{sample.x1, b, sample.x0, a});
                    }
                }
            }

            if (end < count) {
                reach0 = maxf(reach0, sorted[end].x0);
                reach1 = maxf(reach1, sorted[end].x1);
            }

            k = end;
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

    static void addSpan(u32* acc, u32 w, float x0, float x1, i32 weight) {
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

        // Each end splits the span's weight between two cells, rounded.
        i32 a = (i32)rintf((1.f - (x0 - c0)) * (float)weight);
        i32 b = (i32)rintf((1.f - (x1 - c1)) * (float)weight);

        acc[i0] += (u32)a;
        acc[i0 + 1] += (u32)(weight - a);
        acc[i1] -= (u32)b;
        acc[i1 + 1] -= (u32)(weight - b);
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

    // Whether an edge of the lowering spans rows, not level in y nor of a
    // y that is not a number.
    static bool spansRows(const Edge& e) {
        return e.y0 < e.y1 || e.y1 < e.y0;
    }

    // The band edge of an edge of the lowering, its ends in order of y and
    // its direction kept, in the raster's coordinates.
    static BandEdge bandEdge(Edge e, float ox, float oy) {
        if (e.y0 < e.y1) {
            return BandEdge{e.x0 - ox, e.y0 - oy, e.x1 - ox, e.y1 - oy, 1};
        }

        return BandEdge{e.x1 - ox, e.y1 - oy, e.x0 - ox, e.y0 - oy, -1};
    }

    static void rasterizeUnion(u32* acc, u32 stride, u32 w, u32 h, const Lowering& l, u32 first, u32 count, float ox, float oy, u8 rule) {
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

            if (spansRows(e)) {
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

            if (spansRows(e)) {
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

                        unionBand(acc, stride, w, edges.data(), samples, cuts[k], cuts[k + 1], rule, 0, comparable, 0);

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

            u32* line = acc + (size_t)row * stride;
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
                        addSpan(line, w, start, c.x, 0x10000 / subs);
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

    // Rasterizes the edges within the clip, the pixels the fill can reach.
    static bool rasterize(Raster& r, const Lowering& l, u32 first, u32 count, Box bounds, bool aliased, const IntBox& clip, u32& ix0, u32& iy0, u32& w, u32& h, u8 rule) {
        float bx0 = floorf(maxf(bounds.x0, (float)clip.x0));
        float by0 = floorf(maxf(bounds.y0, (float)clip.y0));
        float bx1 = ceilf(minf(bounds.x1, (float)clip.x1));
        float by1 = ceilf(minf(bounds.y1, (float)clip.y1));

        if (!(bx1 > bx0) || !(by1 > by0)) {
            return false;
        }

        ix0 = (u32)bx0;
        iy0 = (u32)by0;
        w = (u32)bx1 - ix0;
        h = (u32)by1 - iy0;

        u32 stride = w + 2;

        r.accumulation.zero((size_t)stride * h);

        u32* acc = r.accumulation.mutData();

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

    // The tile of a pattern is drawn once, where its fill samples it: over
    // the window of tile pixels under the fill's bounds, widened by the
    // bilinear taps and by what rounding can move a sample, when that is
    // less than a period each way; else over the whole tile. The window is
    // unwrapped, so it is drawn in up to four pieces of the tile. A paint a
    // filter reads is sampled everywhere.
    static void prepareTile(const Lowering& l, DevicePaint& paint) {
        if (paint.tile) {
            return;
        }

        paint.opacity16 = (u16)toUnit16(paint.opacity);

        int tw = (int)paint.imageWidth;
        int th = (int)paint.imageHeight;
        IntBox window{0, 0, tw, th};
        bool windowed = false;

        if (!paint.sampledAll) {
            // The fill samples pixel centres within its bounds rounded out.
            const Matrix& m = paint.inverse;
            Box s{floorf(paint.sampled.x0), floorf(paint.sampled.y0), ceilf(paint.sampled.x1), ceilf(paint.sampled.y1)};
            Point corners[4] = {{s.x0, s.y0}, {s.x1, s.y0}, {s.x0, s.y1}, {s.x1, s.y1}};
            Box b = emptyBox();

            for (const Point& c : corners) {
                extend(b, apply(m, c));
            }

            // A sample and a corner each round by a few units in the last
            // place of the terms of the map.
            float ax = maxf(fabsf(s.x0), fabsf(s.x1));
            float ay = maxf(fabsf(s.y0), fabsf(s.y1));
            float mx = ceilf(0x1p-21f * (fabsf(m.a) * ax + fabsf(m.c) * ay + fabsf(m.e))) + 1.f;
            float my = ceilf(0x1p-21f * (fabsf(m.b) * ax + fabsf(m.d) * ay + fabsf(m.f))) + 1.f;
            float x0 = floorf(b.x0 - 0.5f) - mx;
            float y0 = floorf(b.y0 - 0.5f) - my;
            float x1 = floorf(b.x1 - 0.5f) + 2.f + mx;
            float y1 = floorf(b.y1 - 0.5f) + 2.f + my;

            // Far from the origin a pixel is no longer exact in a float.
            windowed = x1 - x0 < (float)tw && y1 - y0 < (float)th && fabsf(x0) + fabsf(y0) < 16777216.f;

            if (windowed) {
                window = IntBox{(int)x0, (int)y0, (int)x1, (int)y1};
            }
        }

        size_t size = (size_t)(window.x1 - window.x0) * (size_t)(window.y1 - window.y0);
        Raster tile;

        paint.window = window;
        paint.tile = makePixels(size);
        l.held.paints.pushBack(&paint);
        tile.width = paint.imageWidth;
        tile.height = paint.imageHeight;

        Plane whole{paint.tile, window};

        if (!windowed) {
            runProgram(tile, l, l.programs.data() + paint.programFirst, paint.programCount, whole);

            return;
        }

        // The window's columns and rows split where they pass a multiple of
        // the period; a piece lies within one period, so it is drawn as the
        // part of the tile it shows and copied to its place.
        int splitX = window.x0 + tw - floorMod(window.x0, tw);
        int splitY = window.y0 + th - floorMod(window.y0, th);
        int xs[3] = {window.x0, splitX < window.x1 ? splitX : window.x1, window.x1};
        int ys[3] = {window.y0, splitY < window.y1 ? splitY : window.y1, window.y1};

        for (int j = 0; j < 2; j++) {
            for (int i = 0; i < 2; i++) {
                if (xs[i + 1] <= xs[i] || ys[j + 1] <= ys[j]) {
                    continue;
                }

                int px = floorMod(xs[i], tw);
                int py = floorMod(ys[j], th);
                IntBox shown{px, py, px + xs[i + 1] - xs[i], py + ys[j + 1] - ys[j]};
                Plane part = makePlane(shown);

                runProgram(tile, l, l.programs.data() + paint.programFirst, paint.programCount, part);

                for (int y = shown.y0; y < shown.y1; y++) {
                    memcpy(planeAt(whole, xs[i], ys[j] + y - shown.y0), planeAt(part, shown.x0, y), sizeof(u16) * 4 * (size_t)(shown.x1 - shown.x0));
                }

                releasePlane(part);
            }
        }
    }

    static void clearSpan(const Plane& plane, int y, int x0, int x1) {
        if (x1 > x0) {
            memset(planeAt(plane, x0, y), 0, sizeof(u16) * 4 * (size_t)(x1 - x0));
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

    static void blendLinear(Raster& r, int mode, u16* d, const u16* s, size_t count);

    // The source pixels from x0 on row y over every layer from first up,
    // within the box of each: the layer of a filtered group holds its
    // filters' region, which a drawing may pass. linear lays them over in
    // linear light.
    static void overLayers(Raster& r, size_t first, int x0, int y, const u16* source, int count, bool linear = false) {
        for (size_t level = first; level < r.layers.length(); level++) {
            const Plane& plane = r.layers[level];
            int a = x0 > plane.box.x0 ? x0 : plane.box.x0;
            int b = x0 + count < plane.box.x1 ? x0 + count : plane.box.x1;

            if (y < plane.box.y0 || y >= plane.box.y1 || b <= a) {
                continue;
            }

            if (linear) {
                blendLinear(r, 0, planeAt(plane, a, y), source + (size_t)(a - x0) * 4, (size_t)(b - a));
            } else {
                overSpan(planeAt(plane, a, y), source + (size_t)(a - x0) * 4, (size_t)(b - a));
            }
        }
    }

    // The rows a paint is sampled over: the two table entries around each
    // pixel's t and how far between them.
    struct PaintRows {
        Vector<u16> below;
        Vector<u16> above;
        Vector<u16> between;
    };

    // The paint of count pixels from x on row y, stored premultiplied; a
    // pixel the paint leaves out gets no cover. A gradient takes the t of
    // each pixel, the two entries around it, and blends them all at once.
    static void paintSpan(const DevicePaint& paint, int x, int y, u32 count, u16* cover, u16* out, PaintRows& rows) {
        if (paint.type != PaintType::Linear && paint.type != PaintType::Radial && paint.type != PaintType::Conic) {
            for (u32 i = 0; i < count; i++) {
                if (!paintAt(paint, (float)(x + (int)i) + 0.5f, (float)y + 0.5f, out + (size_t)i * 4)) {
                    cover[i] = 0;
                }
            }

            return;
        }

        rows.below.zero((size_t)count * 4);
        rows.above.zero((size_t)count * 4);
        rows.between.zero(count);

        u16* below = rows.below.mutData();
        u16* above = rows.above.mutData();
        u16* between = rows.between.mutData();

        // The row's part of the map to the gradient's space, the products
        // apply forms, taken once.
        const Matrix& m = paint.inverse;
        float py = (float)y + 0.5f;
        float rowX = m.c * py;
        float rowY = m.d * py;

        for (u32 i = 0; i < count; i++) {
            float t;
            u32 f;
            float px = (float)(x + (int)i) + 0.5f;

            if (!gradientT(paint, Point{m.a * px + rowX + m.e, m.b * px + rowY + m.f}, t)) {
                if (paint.beyond) {
                    memcpy(below + (size_t)i * 4, paint.beyond16, sizeof(paint.beyond16));
                    memcpy(above + (size_t)i * 4, paint.beyond16, sizeof(paint.beyond16));
                    between[i] = 0;
                } else {
                    cover[i] = 0;
                }

                continue;
            }

            u32 step = tableStep(paint, t, f);

            // A step holding a stop takes the stops themselves.
            if (paint.kinks[step]) {
                gradientColor(paint, minf(maxf(t, 0.f), 1.f), below + (size_t)i * 4);
                memcpy(above + (size_t)i * 4, below + (size_t)i * 4, sizeof(u16) * 4);
                continue;
            }

            memcpy(below + (size_t)i * 4, paint.table + (size_t)step * 4, sizeof(u16) * 4);
            memcpy(above + (size_t)i * 4, paint.table + (size_t)step * 4 + 4, sizeof(u16) * 4);
            between[i] = (u16)f;
        }

        mixSpan(out, below, above, between, count);
    }

    // The pixels a raster paint samples, read when a fill first draws it:
    // the decoded image, or it shrunk.
    static void prepareImage(const Lowering& l, DevicePaint& paint) {
        if (paint.image) {
            return;
        }

        const u8* pixels = (const u8*)svg_image_data(paint.source->source.raster);

        if (paint.kx || paint.ky) {
            paint.shrunk = shrinkImage(pixels, paint.source->width, paint.source->height, paint.kx, paint.ky);
            pixels = paint.shrunk;
            l.held.paints.pushBack(&paint);
        }

        paint.image = pixels;
    }

    // Readies what a paint samples: the tile of a pattern, the pixels of an
    // image, the table of a gradient.
    static void preparePaint(const Lowering& l, DevicePaint& paint) {
        if (paint.type == PaintType::Pattern) {
            prepareTile(l, paint);
        }

        if (paint.type == PaintType::Image) {
            prepareImage(l, paint);
        }

        if (paint.type == PaintType::Linear || paint.type == PaintType::Radial || paint.type == PaintType::Conic) {
            prepareGradient(l, paint);
        }
    }

    // After its fill outside patterns' content, a paint lets its tile or
    // pixels go, and an image the last paint of it.
    static void finishPaint(DevicePaint& paint) {
        if (!paint.once) {
            return;
        }

        delete[] paint.tile;
        paint.tile = nullptr;
        delete[] paint.shrunk;
        paint.shrunk = nullptr;
        paint.image = nullptr;

        if (paint.source && --paint.source->paintsLeft == 0) {
            releaseImage(paint.source->source);
        }
    }

    static void fillOp(Raster& r, const Lowering& l, const Op& op) {
        preparePaint(l, *op.paint);

        u32 ix0;
        u32 iy0;
        u32 w;
        u32 h;
        size_t first = firstTarget(r);
        // The fill reaches the layers it draws on, the base of its program or
        // the layers of the groups above it, which may pass the base.
        IntBox clip = r.layers[first].box;

        for (size_t level = first + 1; level < r.layers.length(); level++) {
            clip = boxJoin(clip, r.layers[level].box);
        }

        if (!rasterize(r, l, op.first, op.count, op.bounds, op.aliased, clip, ix0, iy0, w, h, op.rule)) {
            return;
        }

        const u32* acc = r.accumulation.data();
        u32 stride = w + 2;
        const DevicePaint& paint = *op.paint;
        IntBox fill{(int)ix0, (int)iy0, (int)(ix0 + w), (int)(iy0 + h)};
        bool solid = paint.type == PaintType::Solid;
        Vector<u16> cover;
        Vector<u16> colors;
        Vector<u16> source;
        PaintRows rows;

        for (size_t level = first; level < r.layers.length(); level++) {
            ensure(r.layers[level], r.dirty.mut(level), fill);
        }

        cover.zero(w);
        colors.zero((size_t)w * 4);
        source.zero((size_t)w * 4);

        // A row at a time: the coverage of each pixel stored, and for each
        // run of covered pixels the paint, its product with the coverage, and
        // that over the layers. Eight cells that change nothing keep the
        // coverage before them; a run ends on the first cell of none, so
        // none of them is in a run when that coverage is none.
        for (u32 y = 0; y < h; y++) {
            u32 sum = 0;
            u16 c = 0;
            const u32* line = acc + (size_t)y * stride;
            u16* coverRow = cover.mutData();
            u32 count = 0;
            auto draw = [&](u32 end) {
                u32 start = end - count;
                u16* out = source.mutData() + (size_t)start * 4;

                if (solid) {
                    solidSpan(out, paint.color16, coverRow + start, count);
                } else {
                    u16* paints = colors.mutData() + (size_t)start * 4;

                    paintSpan(paint, (int)(ix0 + start), (int)(iy0 + y), count, coverRow + start, paints, rows);
                    scaleSpan(out, paints, coverRow + start, count);
                }

                overLayers(r, first, (int)(ix0 + start), (int)(iy0 + y), out, (int)count, op.linear);
                count = 0;
            };

            for (u32 x = 0; x < w;) {
                u32 end = x + 8 < w ? x + 8 : w;
                u32 change = 0;

                for (u32 i = x; i < end; i++) {
                    change |= line[i];
                }

                if (change == 0) {
                    if (c) {
                        for (u32 i = x; i < end; i++) {
                            coverRow[i] = c;
                        }

                        count += end - x;
                    }

                    x = end;
                    continue;
                }

                for (; x < end; x++) {
                    sum += line[x];
                    c = (u16)coverStored(sum, op.rule);
                    coverRow[x] = c;

                    if (c) {
                        count++;
                    } else if (count) {
                        draw(x);
                    }
                }
            }

            if (count) {
                draw(w);
            }
        }
    }

    // Adds the coverage of the shape to a map of one stored unit a pixel over the box.
    static void accumulateShape(Raster& r, const Lowering& l, const ClipShape& shape, u16* target, const IntBox& box) {
        Box bounds = emptyBox();

        for (u32 i = shape.first; i < shape.first + shape.count; i++) {
            extend(bounds, Point{l.edges[i].x0, l.edges[i].y0});
            extend(bounds, Point{l.edges[i].x1, l.edges[i].y1});
        }

        u32 ix0;
        u32 iy0;
        u32 w;
        u32 h;

        if (!boxValid(bounds) || !rasterize(r, l, shape.first, shape.count, bounds, false, box, ix0, iy0, w, h, shape.rule)) {
            return;
        }

        const u32* acc = r.accumulation.data();
        u32 stride = w + 2;
        int width = box.x1 - box.x0;

        // The raster's window lies within the box it was clipped to.
        for (u32 y = 0; y < h; y++) {
            u32 sum = 0;
            const u32* line = acc + (size_t)y * stride;
            u16* values = target + (size_t)((int)(iy0 + y) - box.y0) * width + (size_t)((int)ix0 - box.x0);

            for (u32 x = 0; x < w; x++) {
                sum += line[x];

                u16 cover = (u16)coverStored(sum, shape.rule);

                values[x] = values[x] > cover ? values[x] : cover;
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base);

    // The clip of a group, one stored unit a pixel over the box of its layer.
    static u16* clipBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool, const IntBox& box) {
        size_t size = (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0);
        u16* mask = (u16*)pool.allocate(sizeof(u16) * size);

        memset(mask, 0, sizeof(u16) * size);

        if (op.clipProgram) {
            Plane content{(u16*)pool.allocate(sizeof(u16) * size * 4), box};

            memset(content.data, 0, sizeof(u16) * size * 4);
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

    // The sRGB curve one way over stored values: real valued, so each of the
    // 65536 is computed once in a render, as it is first met.
    struct Curve {
        u16* values;
        u8* known;
        bool toLinear;
    };

    static Curve curveOf(Raster& r, bool toLinear) {
        int k = toLinear ? 1 : 0;

        if (r.curves[k].empty()) {
            r.curves[k].zero(65536);
            r.curvesKnown[k].zero(65536);
        }

        return Curve{r.curves[k].mutData(), r.curvesKnown[k].mutData(), toLinear};
    }

    static u32 curveValue(const Curve& curve, u32 v) {
        if (!curve.known[v]) {
            float unit = (float)v / 65535.f;

            curve.values[v] = (u16)toUnit16(curve.toLinear ? srgbToLinear(unit) : linearToSrgb(unit));
            curve.known[v] = 1;
        }

        return curve.values[v];
    }

    // The mask of a group, one stored unit a pixel over the box of its layer:
    // the alpha or luminance of its content within its region. Luminance
    // weighs the stored channels by 13926, 46884 and 4725 of 65536: 0.2125,
    // 0.7154 and 0.0721, feColorMatrix's luminanceToAlpha (css-masking-1.md,
    // MaskValues).
    // The mask of an element: each layer's image in its painting area, as
    // alpha or luminance, from the bottom layer up, each composited over
    // the ones below: add, subtract, intersect or exclude.
    static u16* maskBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool, const IntBox& box) {
        size_t size = (size_t)(box.x1 - box.x0) * (size_t)(box.y1 - box.y0);
        Plane content{(u16*)pool.allocate(sizeof(u16) * size * 4), box};
        u16* mask = (u16*)pool.allocate(sizeof(u16) * size);
        u16* above = op.maskLayerCount > 1 ? (u16*)pool.allocate(sizeof(u16) * size) : mask;

        for (u32 k = op.maskLayerCount; k-- > 0;) {
            const MaskLayerOp& layer = op.maskLayers[k];
            u16* out = k + 1 == op.maskLayerCount ? mask : above;

            memset(content.data, 0, sizeof(u16) * size * 4);
            memset(out, 0, sizeof(u16) * size);
            runProgram(r, l, l.programs.data() + layer.first, layer.count, content);
            accumulateShape(r, l, l.clips[layer.region], out, box);

            for (size_t i = 0; i < size; i++) {
                const u16* c = content.data + i * 4;
                u32 value = (layer.type & 1) ? c[3] : (13926u * c[0] + 46884u * c[1] + 4725u * c[2] + 32768u) >> 16;

                // Luminance in linear light takes the colour along the sRGB curve.
                if ((layer.type & 3) == 2) {
                    Curve curve = curveOf(r, true);
                    u32 u[4];

                    unpremultiplyStored(c, u);
                    value = div65535(((13926u * curveValue(curve, u[0]) + 46884u * curveValue(curve, u[1]) + 4725u * curveValue(curve, u[2]) + 32768u) >> 16) * u[3]);
                }

                out[i] = (u16)div65535(out[i] * value);
            }

            if (out == mask) {
                continue;
            }

            for (size_t i = 0; i < size; i++) {
                u32 s = above[i];
                u32 d = mask[i];
                u32 both = div65535(s * d);

                switch (layer.composite) {
                    case 0:
                        mask[i] = (u16)(s + d - both);
                        break;
                    case 1:
                        mask[i] = (u16)(s - both);
                        break;
                    case 2:
                        mask[i] = (u16)both;
                        break;
                    default:
                        mask[i] = (u16)(s + d - 2 * both);
                        break;
                }
            }
        }

        return mask;
    }

    // A primitive's result, and where in pixels it can be other than
    // transparent: rectangles meet in fractions, and only that is rounded,
    // so that two abutting subregions keep nothing between them.
    // edges are the hard edges of what it holds: a subregion's side inside
    // them cuts it, and its pixels take their part; one on them leaves the
    // edge's own antialiasing alone.
    struct FilterResult {
        Plane plane;
        bool linear;
        Box support;
        Box edges;
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

    // Converts the rows of a plane into another, or the same, between sRGB
    // and linear light: each pixel not premultiplied, each colour channel
    // along the curve, premultiplied again. A pixel as stored as the one
    // before converts as it did, so a flat area converts once; the first
    // compares with opaque white, which either curve keeps.
    static void convertSpan(const Curve& curve, const u16* s, u16* d, size_t n) {
        u64 lastIn = ~(u64)0;
        u64 lastOut = ~(u64)0;

        for (size_t x = 0; x < n; x++) {
            u64 pixel;

            memcpy(&pixel, s + x * 4, sizeof(pixel));

            if (pixel != lastIn) {
                u32 c[4];
                u16 o[4];

                unpremultiplyStored(s + x * 4, c);

                for (int k = 0; k < 3; k++) {
                    o[k] = (u16)div65535(curveValue(curve, c[k]) * c[3]);
                }

                o[3] = (u16)c[3];
                lastIn = pixel;
                memcpy(&lastOut, o, sizeof(lastOut));
            }

            memcpy(d + x * 4, &lastOut, sizeof(lastOut));
        }
    }

    static void convertRows(Raster& r, const Plane& src, const Plane& out, const IntBox& box, bool toLinear) {
        Curve curve = curveOf(r, toLinear);

        for (int y = box.y0; y < box.y1; y++) {
            convertSpan(curve, planeAt(src, box.x0, y), planeAt(out, box.x0, y), (size_t)(box.x1 - box.x0));
        }
    }

    static void convertInPlace(Raster& r, const Plane& plane, bool toLinear) {
        convertRows(r, plane, plane, plane.box, toLinear);
    }

    static Plane convertBuffer(Raster& r, const Plane& src, bool toLinear) {
        Plane out = makePlane(src.box);

        convertRows(r, src, out, src.box, toLinear);

        return out;
    }

    // Copies the box, which the output plane holds, from one plane to a new
    // one, zero, a row at a time: what lies outside the input stays zero.
    static void copyBox(const Plane& in, const Plane& out, const IntBox& b) {
        IntBox inside = boxInside(b, in.box);

        for (int y = inside.y0; y < inside.y1; y++) {
            memcpy(planeAt(out, inside.x0, y), planeAt(in, inside.x0, y), sizeof(u16) * 4 * (size_t)(inside.x1 - inside.x0));
        }
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
            copyBox(r.backdrop, out, box);
        }

        // Ancestor opacity, masks and filters apply when that ancestor is
        // composited, not while its children form the current background.
        for (size_t level = first; level < r.layers.length(); level++) {
            IntBox drawn = boxInside(r.dirty[level], box);
            const Plane& layer = r.layers[level];

            for (int y = drawn.y0; y < drawn.y1; y++) {
                overSpan(planeAt(out, drawn.x0, y), planeAt(layer, drawn.x0, y), (size_t)(drawn.x1 - drawn.x0));
            }
        }

        if (scope && scope->backgroundClipped) {
            const Box& clip = scope->backgroundBox;

            for (int y = box.y0; y < box.y1; y++) {
                for (int x = box.x0; x < box.x1; x++) {
                    Point point = apply(scope->backgroundMatrix, Point{(float)x + .5f, (float)y + .5f});

                    if (point.x < clip.x0 || point.x >= clip.x1 || point.y < clip.y0 || point.y >= clip.y1) {
                        memset(planeAt(out, x, y), 0, sizeof(u16) * 4);
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
                    preparePaint(*run.lowering, *paint);

                    for (int y = run.canvas.y0; y < run.canvas.y1; y++) {
                        for (int x = run.canvas.x0; x < run.canvas.x1; x++) {
                            u16 color[4];

                            if (paintAt(*paint, (float)x + .5f, (float)y + .5f, color)) {
                                memcpy(planeAt(buffer, x, y), color, sizeof(color));
                            }
                        }
                    }
                }
            }

            if (linear && !run.paint[kind][1].data) {
                run.paint[kind][1] = convertBuffer(*run.raster, buffer, true);
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
                    convertInPlace(*run.raster, run.background, true);
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
                run.backgroundLinear = convertBuffer(*run.raster, run.background, true);
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
                run.sourceLinear = convertBuffer(*run.raster, run.source, true);
            }

            return run.sourceLinear;
        }

        const FilterResult& result = run.results[index];

        if (result.linear == linear) {
            return result.plane;
        }

        Vector<Plane>& cache = linear ? run.linearCache : run.srgbCache;

        if (!cache[index].data) {
            cache.mut(index) = convertBuffer(*run.raster, result.plane, linear);
        }

        return cache[index];
    }

    // Where an input can be other than transparent, in pixels.
    static Box supportOf(const FilterRun& run, int index) {
        return index >= 0 ? run.results[index].support : run.filter->device;
    }

    // An input's hard edges: the element's box for the source, as its
    // antialiasing has them.
    static Box edgesOf(const FilterRun& run, int index) {
        const Filter& f = *run.filter;
        bool source = index == InputSourceGraphic || index == InputSourceAlpha;
        bool boxed = fminf(f.bbox.x1 - f.bbox.x0, f.bbox.y1 - f.bbox.y0) > 0.f;

        return index >= 0 ? run.results[index].edges : (source && boxed ? deviceBox(f.ctm, f.bbox) : f.device);
    }

    // What a composite keeps of its inputs: where both are, a, b, or with a
    // constant term everything.
    enum : u8 {
        KeepsBoth = 1,
        KeepsA = 2,
        KeepsB = 4,
        KeepsAll = 8,
    };

    static Box movedBox(const Box& box, Point d) {
        return Box{box.x0 + d.x, box.y0 + d.y, box.x1 + d.x, box.y1 + d.y};
    }

    static Box keptBox(u8 keeps, const Box& a, const Box& b) {
        Box kept = joinBoxes(joinBoxes(keeps & KeepsBoth ? meetBoxes(a, b) : emptyBox(), keeps & KeepsA ? a : emptyBox()), keeps & KeepsB ? b : emptyBox());

        return keeps & KeepsAll ? allBox() : kept;
    }

    // Scales the pixels of a plane by the part of each the box covers: an
    // image cut by a box past the pixels' bounds keeps its area within
    // each. Only rows the box cuts are scaled whole; the rest at its sides.
    static void coverEdges(const Plane& plane, const Box& box) {
        const IntBox& b = plane.box;
        int whole = (int)fminf(fmaxf(ceilf(box.x0), (float)b.x0), (float)b.x1);
        int edge = (int)fminf(fmaxf(floorf(box.x1), (float)whole), (float)b.x1);
        auto cover = [&](int x0, int x1, int y, float cy) {
            for (int x = x0; x < x1; x++) {
                float c = cy * clampUnit(minf((float)x + 1.f, box.x1) - maxf((float)x, box.x0));
                u16* v = planeAt(plane, x, y);

                for (int k = 0; k < 4; k++) {
                    v[k] = (u16)((float)v[k] * c + 0.5f);
                }
            }
        };

        for (int y = b.y0; y < b.y1; y++) {
            float cy = clampUnit(minf((float)y + 1.f, box.y1) - maxf((float)y, box.y0));

            if (cy < 1.f) {
                cover(b.x0, b.x1, y, cy);
            } else {
                cover(b.x0, whole, y, cy);
                cover(edge, b.x1, y, cy);
            }
        }
    }

    // Clears a result past the pixels its support touches, all of it where
    // the support has no area.
    static void keepSupport(const Plane& plane, const Box& support) {
        IntBox keep = support.x1 > support.x0 && support.y1 > support.y0 ? pixelBox(support, plane.box) : IntBox{0, 0, 0, 0};
        const IntBox& b = plane.box;

        for (int y = b.y0; y < b.y1; y++) {
            bool row = y >= keep.y0 && y < keep.y1;
            int from = row ? keep.x0 : b.x1;
            int to = row ? keep.x1 : b.x1;

            memset(planeAt(plane, b.x0, y), 0, sizeof(u16) * 4 * (size_t)(from - b.x0));
            memset(planeAt(plane, to, y), 0, sizeof(u16) * 4 * (size_t)(b.x1 - to));
        }
    }

    static IntBox inputBox(const FilterRun& run, int index) {
        if (index >= 0) {
            return run.filter->primitives[index].box;
        }

        return run.filter->region;
    }

    // How a convolution or a blur extends its input past the input's box.
    enum class EdgeMode : u8 {
        None,
        Duplicate,
        Wrap,
        Mirror,
    };

    static EdgeMode edgeModeOf(const Document& doc, const Node* node, EdgeMode fallback) {
        u32 value = attribute(node, AtomEdgeMode);
        StringView text = value ? trim(textOf(doc, value)) : StringView();

        if (text == SV("none")) {
            return EdgeMode::None;
        }

        if (text == SV("duplicate")) {
            return EdgeMode::Duplicate;
        }

        if (text == SV("wrap")) {
            return EdgeMode::Wrap;
        }

        return text == SV("mirror") ? EdgeMode::Mirror : fallback;
    }

    // The coordinate from lo to hi an edge mode reads for v; -1 for none,
    // or past an empty input. A mirror repeats the edge: a period of twice
    // the size.
    static int edgeFold(int v, int lo, int hi, EdgeMode mode) {
        int size = hi - lo;

        if (v >= lo && v < hi) {
            return v;
        }

        if (mode == EdgeMode::None || size <= 0) {
            return -1;
        }

        int period = mode == EdgeMode::Mirror ? 2 * size : size;
        int folded = (((v - lo) % period) + period) % period;

        if (mode == EdgeMode::Duplicate) {
            return v < lo ? lo : hi - 1;
        }

        return lo + (folded < size ? folded : period - 1 - folded);
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

        float d[4] = {c.r * c.a, c.g * c.a, c.b * c.a, c.a};
        u16 stored[4];

        storePixel(stored, d);

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                memcpy(planeAt(out, x, y), stored, sizeof(stored));
            }
        }
    }

    // A shift by a fraction of a pixel: the whole part, and how far past it.
    struct Shift {
        int ox;
        int oy;
        float fx;
        float fy;
    };

    static Shift shiftBy(float dx, float dy) {
        float floorX = floorf(dx);
        float floorY = floorf(dy);

        return Shift{(int)floorX, (int)floorY, dx - floorX, dy - floorY};
    }

    // Shifts the input over the box into a new plane, zero: a whole shift
    // copies rows; past a fraction, each pixel blends the four input pixels
    // around, from two input rows read at once.
    static void shiftBuffer(const Plane& in, const Plane& out, const IntBox& b, float dx, float dy) {
        Shift shift = shiftBy(dx, dy);
        int n = b.x1 - b.x0;

        if (shift.fx == 0.f && shift.fy == 0.f) {
            for (int y = b.y0; y < b.y1; y++) {
                readStored(in, b.x0 - shift.ox, b.x1 - shift.ox, y - shift.oy, planeAt(out, b.x0, y));
            }

            return;
        }

        Vector<u16> upper;
        Vector<u16> lower;

        upper.zero((size_t)(n + 1) * 4);
        lower.zero((size_t)(n + 1) * 4);

        // The rows reach one pixel further left: index i is x0 - ox - 1 + i.
        for (int y = b.y0; y < b.y1; y++) {
            readStored(in, b.x0 - shift.ox - 1, b.x1 - shift.ox, y - shift.oy, upper.mutData());
            readStored(in, b.x0 - shift.ox - 1, b.x1 - shift.ox, y - shift.oy - 1, lower.mutData());

            u16* o = planeAt(out, b.x0, y);

            for (int x = 0; x < n; x++) {
                const u16* a = upper.data() + (size_t)x * 4;
                const u16* c = lower.data() + (size_t)x * 4;

                bilinear(a + 4, a, c + 4, c, shift.fx, shift.fy, o + (size_t)x * 4);
            }
        }
    }

    // Returns the shift in pixels.
    static Point filterOffset(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        float dx = userX(f, numberOf(doc, p.node, AtomDx, 0.f));
        float dy = userY(f, numberOf(doc, p.node, AtomDy, 0.f));
        float sx = f.ctm.a * dx + f.ctm.c * dy;
        float sy = f.ctm.b * dx + f.ctm.d * dy;
        Plane in = filterInputBuffer(run, p.in1, p.linear);

        shiftBuffer(in, out, p.box, sx, sy);

        return Point{sx, sy};
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

    // A line of a blur is count positions of a few groups of four lanes:
    // pixels of some rows, or of some columns of a plane, so that a step
    // goes over many lanes at once. The lanes are held in 32 bits.

    // One box pass over a line: each output is the mean of the window from
    // left before to right after, rounded through a multiplier of 2^32 over
    // the window; the line is zero past its ends.
    static void boxPass(const u32* __restrict in, u32* __restrict out, int count, size_t lanes, int left, int right, u32* __restrict sum) {
        u32 window = (u32)(left + right + 1);
        u64 multiplier = ((1ull << 32) + window / 2) / window;

        for (size_t c = 0; c < lanes; c++) {
            sum[c] = 0;
        }

        // The padding of a line passes the window, so i stays below count.
        for (int i = 0; i <= right; i++) {
            for (size_t c = 0; c < lanes; c++) {
                sum[c] += in[(size_t)i * lanes + c];
            }
        }

        for (int i = 0; i < count; i++) {
            for (size_t c = 0; c < lanes; c++) {
                out[(size_t)i * lanes + c] = (u32)(((u64)sum[c] * multiplier + (1ull << 31)) >> 32);
            }

            int gone = i - left;
            int next = i + right + 1;

            // gone stays below count, and next above zero.
            if (gone >= 0) {
                for (size_t c = 0; c < lanes; c++) {
                    sum[c] -= in[(size_t)gone * lanes + c];
                }
            }

            if (next < count) {
                for (size_t c = 0; c < lanes; c++) {
                    sum[c] += in[(size_t)next * lanes + c];
                }
            }
        }
    }

    // The scratch of the blur of a line: the line padded, and two passes.
    struct BlurScratch {
        Vector<u32> first;
        Vector<u32> second;
        Vector<u32> sum;
    };

    // Where the lanes of a line lie: a position and a group step apart.
    struct LineLayout {
        ptrdiff_t position;
        ptrdiff_t group;
    };

    // The scratch of a morphology line: the line padded, and its running
    // extremes forward and backward.
    struct MorphologyScratch {
        Vector<u16> line;
        Vector<u16> g;
        Vector<u16> h;
    };

    // Blurs a line of groups of four lanes read from src and written to dst.
    // Deviations below 2 take a Gaussian of weights summing to 65536; a
    // window as long as twice the line takes the quadratic the three boxes
    // leave in its middle; others take the three boxes.
    static void blurLine(const u16* src, LineLayout from, u16* dst, LineLayout to, int count, int groups, float sigma, BlurScratch& scratch) {
        size_t lanes = (size_t)groups * 4;
        auto store = [&](int i, size_t c, u32 value) {
            dst[i * to.position + (ptrdiff_t)(c >> 2) * to.group + (ptrdiff_t)(c & 3)] = (u16)value;
        };

        if (!(sigma > 0.f)) {
            for (int i = 0; i < count; i++) {
                for (int g = 0; g < groups; g++) {
                    memcpy(dst + i * to.position + g * to.group, src + i * from.position + g * from.group, sizeof(u16) * 4);
                }
            }

            return;
        }

        double window = floorf(sigma * 3.f * sqrtf(2.f * pi) / 4.f + 0.5f);
        bool boxes = sigma >= 2.f && window < 2.0 * count;
        // The boxes' length, which a window past the line's needs not, nor
        // could an int hold.
        int d = boxes ? (int)window : 0;
        int pad = boxes ? 3 * (d / 2 + 1) : 0;
        int total = count + 2 * pad;

        scratch.first.zero((size_t)total * lanes);
        scratch.second.zero((size_t)total * lanes);
        scratch.sum.zero(lanes);

        u32* first = scratch.first.mutData();
        u32* second = scratch.second.mutData();

        for (int i = 0; i < count; i++) {
            for (int g = 0; g < groups; g++) {
                const u16* in = src + i * from.position + g * from.group;
                u32* lane = first + (size_t)(i + pad) * lanes + (size_t)g * 4;

                lane[0] = in[0];
                lane[1] = in[1];
                lane[2] = in[2];
                lane[3] = in[3];
            }
        }

        if (sigma < 2.f) {
            // A sigma below 2 reaches 6 pixels at most.
            int radius = (int)ceilf(sigma * 3.f);
            float kernel[13];
            u32 weights[13];
            float total = 0.f;
            u32 sum = 0;

            for (int k = -radius; k <= radius; k++) {
                kernel[k + radius] = expf(-(float)(k * k) / (2.f * sigma * sigma));
                total += kernel[k + radius];
            }

            for (int k = 0; k <= 2 * radius; k++) {
                weights[k] = (u32)(kernel[k] / total * 65536.f + 0.5f);
                sum += weights[k];
            }

            // The middle weight takes what rounding left, so the weights sum to 65536.
            weights[radius] += 65536u - sum;

            for (int i = 0; i < count; i++) {
                // The taps within the line.
                int k0 = i < radius ? -i : -radius;
                int k1 = i + radius < count ? radius : count - 1 - i;
                u32* acc = second + (size_t)i * lanes;

                for (int k = k0; k <= k1; k++) {
                    const u32* tap = first + (size_t)(i + k) * lanes;
                    u32 w = weights[k + radius];

                    for (size_t c = 0; c < lanes; c++) {
                        acc[c] += tap[c] * w;
                    }
                }

                for (size_t c = 0; c < lanes; c++) {
                    store(i, c, (acc[c] + 32768u) >> 16);
                }
            }

            return;
        }

        if (!boxes) {
            // The central part of three convolved boxes is a quadratic. Integrate
            // it from the input moments instead of allocating padding proportional to sigma.
            bool even = fmod(window, 2.0) == 0.0;
            double center = (3.0 * window * window + (even ? 2.0 * window : 1.0)) / 4.0;
            double divisor = window * window * (even ? window + 1.0 : window);

            for (size_t c = 0; c < lanes; c++) {
                double moments[3] = {};

                for (int i = 0; i < count; i++) {
                    double value = first[(size_t)i * lanes + c];

                    moments[0] += value;
                    moments[1] += i * value;
                    moments[2] += (double)i * i * value;
                }

                for (int i = 0; i < count; i++) {
                    double value = ((center - (double)i * i) * moments[0] + 2.0 * i * moments[1] - moments[2]) / divisor;

                    store(i, c, (u32)(minf(maxf((float)value, 0.f), 65535.f) + 0.5f));
                }
            }

            return;
        }

        u32* sum = scratch.sum.mutData();

        if (d % 2 == 1) {
            int r = (d - 1) / 2;

            boxPass(first, second, total, lanes, r, r, sum);
            boxPass(second, first, total, lanes, r, r, sum);
            boxPass(first, second, total, lanes, r, r, sum);
        } else {
            boxPass(first, second, total, lanes, d / 2, d / 2 - 1, sum);
            boxPass(second, first, total, lanes, d / 2 - 1, d / 2, sum);
            boxPass(first, second, total, lanes, d / 2, d / 2, sum);
        }

        for (int i = 0; i < count; i++) {
            for (size_t c = 0; c < lanes; c++) {
                store(i, c, second[(size_t)(i + pad) * lanes + c]);
            }
        }
    }

    // Blurs the input over the box: groups of eight rows into a plane of the
    // horizontal pass, then groups of eight of its columns into the box, 32
    // lanes a position either way.
    // With an edge mode other than none the input extends past src, past
    // the raster too: its rows and columns fold into src. A deviation is
    // held there to twice src's size, which bounds the lines; past it the
    // extended input is about as flat.
    static void blurBuffer(FilterRun& run, const Plane& in, const Plane& out, const IntBox& box, float sigmaX, float sigmaY, bool consume, EdgeMode edge, const IntBox& src) {
        bool edged = edge != EdgeMode::None;
        float limit = 2.f * (float)(src.x1 - src.x0 > src.y1 - src.y0 ? src.x1 - src.x0 : src.y1 - src.y0);

        sigmaX = edged ? minf(sigmaX, limit) : sigmaX;
        sigmaY = edged ? minf(sigmaY, limit) : sigmaY;

        int reachX = blurReach(sigmaX);
        int reachY = blurReach(sigmaY);
        IntBox bound = edged ? IntBox{box.x0 - reachX, box.y0 - reachY, box.x1 + reachX, box.y1 + reachY} : IntBox{0, 0, (int)run.raster->width, (int)run.raster->height};
        int x0 = box.x0 - reachX > bound.x0 ? box.x0 - reachX : bound.x0;
        int x1 = box.x1 + reachX < bound.x1 ? box.x1 + reachX : bound.x1;
        int y0 = box.y0 - reachY > bound.y0 ? box.y0 - reachY : bound.y0;
        int y1 = box.y1 + reachY < bound.y1 ? box.y1 + reachY : bound.y1;
        BlurScratch scratch;
        Vector<u16> rows;
        Vector<u16> columns;

        // An empty box is empty by its x, as boxInside leaves it.
        if (box.x1 <= box.x0) {
            return;
        }

        // The horizontal pass goes over the input itself when the input is
        // dead after the blur: a row is read whole before it is written.
        bool inPlace = consume && !edged && x0 >= in.box.x0 && y0 >= in.box.y0 && x1 <= in.box.x1 && y1 <= in.box.y1;
        Plane horizontal = inPlace ? in : makePlane(IntBox{x0, y0, x1, y1});

        size_t rowLength = (size_t)(x1 - x0) * 4;
        ptrdiff_t across = (ptrdiff_t)planeWidth(horizontal) * 4;

        rows.zero(rowLength * 8);
        columns.zero((size_t)(y1 - y0) * 32);

        // The rows are read first: the horizontal plane may be the input.
        for (int y = y0; y < y1; y += 8) {
            int group = y1 - y < 8 ? y1 - y : 8;

            for (int g = 0; g < group; g++) {
                u16* row = rows.mutData() + rowLength * (size_t)g;
                int sy = edgeFold(y + g, src.y0, src.y1, edge);

                if (!edged) {
                    readStored(in, x0, x1, y + g, row);
                    continue;
                }

                for (int x = x0; x < x1; x++) {
                    memcpy(row + (size_t)(x - x0) * 4, planeRead(in, edgeFold(x, src.x0, src.x1, edge), sy), sizeof(u16) * 4);
                }
            }

            blurLine(rows.data(), LineLayout{4, (ptrdiff_t)rowLength}, planeAt(horizontal, x0, y), LineLayout{4, across}, x1 - x0, group, sigmaX, scratch);
        }

        for (int x = box.x0; x < box.x1; x += 8) {
            int group = box.x1 - x < 8 ? box.x1 - x : 8;
            size_t lanes = (size_t)group * 4;

            blurLine(planeAt(horizontal, x, y0), LineLayout{across, 4}, columns.mutData(), LineLayout{(ptrdiff_t)lanes, 4}, y1 - y0, group, sigmaY, scratch);

            for (int y = box.y0; y < box.y1; y++) {
                memcpy(planeAt(out, x, y), columns.data() + (size_t)(y - y0) * lanes, sizeof(u16) * lanes);
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

        // The input's box is where its edge mode extends it; an empty input
        // has nothing to extend.
        IntBox src = inputBox(run, p.in1);
        EdgeMode edge = p.node && src.x1 > src.x0 ? edgeModeOf(*f.doc, p.node, EdgeMode::None) : EdgeMode::None;

        blurBuffer(run, in, out, p.box, sx, sy, consumesInput(run, p.in1, in), edge, src);
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

        // The deviation goes to a blur, where a negative one is zero: no
        // blur along that axis.
        sx = maxf(sx, 0.f);
        sy = maxf(sy, 0.f);

        Color c = toSpace(p.flood, p.linear);
        IntBox all = run.filter->region;
        Plane shadow = makePlane(all);

        if (userX(f, sx) * run.scaleX > 0.f || userY(f, sy) * run.scaleY > 0.f) {
            blurBuffer(run, in, shadow, all, userX(f, sx) * run.scaleX, userY(f, sy) * run.scaleY, false, EdgeMode::None, all);
        } else {
            copyBox(in, shadow, all);
        }

        // The shadow moved over the box, coloured by the flood, and the input
        // over it, a row at a time.
        const IntBox& b = p.box;
        size_t n = (size_t)(b.x1 - b.x0);
        Plane moved = makePlane(b);
        u16 flood[4];
        Vector<u16> row;
        Vector<u16> shade;

        shiftBuffer(shadow, moved, b, f.ctm.a * dx + f.ctm.c * dy, f.ctm.b * dx + f.ctm.d * dy);
        storeColor(flood, Color{c.r * c.a, c.g * c.a, c.b * c.a, c.a});
        row.zero(n * 4);
        shade.zero(n * 4);

        for (int y = b.y0; y < b.y1; y++) {
            const u16* m = planeAt(moved, b.x0, y);
            u16* sh = shade.mutData();

            for (size_t x = 0; x < n; x++) {
                for (int k = 0; k < 4; k++) {
                    sh[x * 4 + (size_t)k] = (u16)div65535((u32)flood[k] * m[x * 4 + 3]);
                }
            }

            readStored(in, b.x0, b.x1, y, row.mutData());
            overSpan(sh, row.data(), n);
            memcpy(planeAt(out, b.x0, y), sh, sizeof(u16) * 4 * n);
        }

        releasePlane(moved);
        releasePlane(shadow);
    }

    static void storePremultiplied(float* d, const float* c) {
        float a = clampUnit(c[3]);

        d[0] = clampUnit(c[0]) * a;
        d[1] = clampUnit(c[1]) * a;
        d[2] = clampUnit(c[2]) * a;
        d[3] = a;
    }

    // Pixels premultiplied from floats not premultiplied, each channel clamped.
    static void premultiplySpan(float* __restrict d, const float* __restrict c, size_t count) {
        for (size_t p = 0; p < count; p++) {
            storePremultiplied(d + p * 4, c + p * 4);
        }
    }

    // A sum of stored channels, each times a real coefficient, plus a real
    // constant, in 31 bits: the coefficients carry as many bits of fraction
    // as the sum at its largest leaves room for, up to 16, so that what
    // they lose to rounding is less than a stored unit or two of the
    // channels; a sum too large even for whole coefficients drops low bits
    // of the channels instead. A coefficient past 2^24 saturates any channel
    // it takes, and one that is not a number counts for nothing.
    struct FixedSum {
        i32 constant;
        u32 shift;
        u32 drop;
        u32 cap;
        u32 half;
    };

    static double fixedCoefficient(float v) {
        return v == v ? (double)minf(maxf(v, -0x1p24f), 0x1p24f) : 0.0;
    }

    // Fixes the coefficients into factors, and the constant, in stored
    // units, into the sum.
    static FixedSum fixSum(const float* coefficients, size_t count, float constant, i32* factors) {
        for (int level = 16;; level--) {
            u32 shift = level > 0 ? (u32)level : 0u;
            u32 drop = level < 0 ? (u32)-level : 0u;
            double scale = ldexp(1.0, (int)shift);
            double fixedConstant = floor(fixedCoefficient(constant) * 65535.0 * ldexp(1.0, (int)shift - (int)drop) + 0.5);
            double bound = fabs(fixedConstant);

            for (size_t k = 0; k < count; k++) {
                bound += fabs(floor(fixedCoefficient(coefficients[k]) * scale + 0.5)) * (double)(65535u >> drop);
            }

            if (bound < 0x1p30) {
                for (size_t k = 0; k < count; k++) {
                    factors[k] = (i32)floor(fixedCoefficient(coefficients[k]) * scale + 0.5);
                }

                return FixedSum{(i32)fixedConstant, shift, drop, drop ? 65536u >> drop : 0x40000000u, shift ? 1u << (shift - 1) : 0u};
            }
        }
    }

    // The stored value of a fixed sum: its constant and the factors times
    // the channels shifted down by its drop, clamped.
    static u32 fixedValue(const FixedSum& f, i32 sum) {
        u32 v = sum > 0 ? (u32)sum : 0u;

        v = (((v < f.cap ? v : f.cap) << f.drop) + f.half) >> f.shift;

        return v < 65535u ? v : 65535u;
    }

    // Returns whether a transparent pixel comes out other than transparent.
    static bool filterColorMatrix(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
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

        // A row at a time: each pixel not premultiplied, each row of the
        // matrix a fixed sum over it, the colours premultiplied again by the
        // alpha they come to.
        i32 factors[4][4];
        FixedSum sums[4];

        for (int k = 0; k < 4; k++) {
            sums[k] = fixSum(m + k * 5, 4, m[k * 5 + 4], factors[k]);
        }

        const IntBox& b = p.box;
        size_t n = (size_t)(b.x1 - b.x0);
        Vector<u16> row;

        row.zero(n * 4);

        for (int y = b.y0; y < b.y1; y++) {
            u16* o = planeAt(out, b.x0, y);

            readStored(in, b.x0, b.x1, y, row.mutData());

            for (size_t x = 0; x < n; x++) {
                u32 c[4];
                u32 v[4];

                unpremultiplyStored(row.data() + x * 4, c);

                for (int k = 0; k < 4; k++) {
                    const i32* q = factors[k];
                    u32 drop = sums[k].drop;

                    v[k] = fixedValue(sums[k], sums[k].constant + q[0] * (i32)(c[0] >> drop) + q[1] * (i32)(c[1] >> drop) + q[2] * (i32)(c[2] >> drop) + q[3] * (i32)(c[3] >> drop));
                }

                for (int k = 0; k < 3; k++) {
                    o[x * 4 + (size_t)k] = (u16)div65535(v[k] * v[3]);
                }

                o[x * 4 + 3] = (u16)v[3];
            }
        }

        return m[19] > 0.f;
    }

    // A transfer function over a stored channel not premultiplied: a table
    // in stored units, interpolated or stepped in integers, a line as a
    // fixed sum; the gamma is real valued, computed once for each value in
    // a run of it.
    struct TransferFunction {
        int type;
        Vector<float> table;
        Vector<i64> values;
        float slope;
        float intercept;
        float amplitude;
        float exponent;
        float offset;
        i32 factor;
        FixedSum line;
        u32 lastIn;
        u32 lastOut;
    };

    static u32 transfer(TransferFunction& f, u32 c) {
        size_t n = f.values.length();

        switch (f.type) {
            case 1: {
                if (n == 0) {
                    return c;
                }

                u64 position = (u64)c * (n - 1);
                size_t k = (size_t)(position / 65535u);

                // A table of one value, or the top of the channel, is the last value.
                if (k >= n - 1) {
                    return clampStored(f.values[n - 1]);
                }

                i64 step = (f.values[k + 1] - f.values[k]) * (i64)(position - (u64)k * 65535u);

                return clampStored(f.values[k] + (step >= 0 ? (step + 32767) / 65535 : -((32767 - step) / 65535)));
            }
            case 2: {
                if (n == 0) {
                    return c;
                }

                size_t k = (size_t)((u64)c * n / 65535u);

                return clampStored(f.values[k < n ? k : n - 1]);
            }
            case 3:
                return fixedValue(f.line, f.line.constant + f.factor * (i32)(c >> f.line.drop));
            case 4:
                if (c != f.lastIn) {
                    f.lastIn = c;
                    f.lastOut = toUnit16(f.amplitude * powf((float)c / 65535.f, f.exponent) + f.offset);
                }

                return f.lastOut;
            default:
                return c;
        }
    }

    // Returns whether a transparent pixel comes out other than transparent.
    static bool filterComponentTransfer(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane in = filterInputBuffer(run, p.in1, p.linear);
        TransferFunction functions[4];
        u32 names[4] = {AtomFeFuncR, AtomFeFuncG, AtomFeFuncB, AtomFeFuncA};

        // A channel without a function is passed as it is.
        for (TransferFunction& f : functions) {
            f.type = 0;
            f.slope = 1.f;
            f.intercept = 0.f;
            f.amplitude = 1.f;
            f.exponent = 1.f;
            f.offset = 0.f;
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

        for (TransferFunction& f : functions) {
            f.values.zero(f.table.length());

            for (size_t k = 0; k < f.table.length(); k++) {
                f.values.mut(k) = (i64)floor(fixedCoefficient(f.table[k]) * 65535.0 + 0.5);
            }

            f.line = fixSum(&f.slope, 1, f.intercept, &f.factor);
            f.lastIn = 0x10000;
            f.lastOut = 0;
        }

        // A row at a time: each pixel not premultiplied, each channel
        // through its function, the colours premultiplied again by the alpha
        // it comes to.
        const IntBox& b = p.box;
        size_t n = (size_t)(b.x1 - b.x0);
        Vector<u16> row;

        row.zero(n * 4);

        for (int y = b.y0; y < b.y1; y++) {
            u16* o = planeAt(out, b.x0, y);

            readStored(in, b.x0, b.x1, y, row.mutData());

            for (size_t x = 0; x < n; x++) {
                u32 c[4];

                unpremultiplyStored(row.data() + x * 4, c);

                u32 alpha = transfer(functions[3], c[3]);

                for (int k = 0; k < 3; k++) {
                    o[x * 4 + (size_t)k] = (u16)div65535(transfer(functions[k], c[k]) * alpha);
                }

                o[x * 4 + 3] = (u16)alpha;
            }
        }

        return transfer(functions[3], 0) > 0;
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

    // Each lane of a times the factor of its pixel plus each lane of b times
    // its pixel's factor, saturated: over, in, out, atop, xor and lighter.
    static void twoTermSpan(u16* __restrict o, const u16* __restrict a, const u16* __restrict b, const u16* __restrict fa, const u16* __restrict fb, size_t count) {
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            for (int i = 0; i < 8; i++) {
                size_t k = p + (size_t)(i >> 2);

                o[p * 4 + i] = saturate(div65535((u32)a[p * 4 + i] * fa[k]) + div65535((u32)b[p * 4 + i] * fb[k]));
            }
        }

        for (; p < count; p++) {
            for (int i = 0; i < 4; i++) {
                o[p * 4 + i] = saturate(div65535((u32)a[p * 4 + i] * fa[p]) + div65535((u32)b[p * 4 + i] * fb[p]));
            }
        }
    }

    // The composite of the two inputs. The Porter-Duff operators weigh each
    // input by the other's alpha, its complement, 0 or 1: a factor is a
    // constant plus or minus that alpha, taken by masks. The arithmetic one
    // has real coefficients and goes through floats.
    // Returns what its operator keeps of the inputs.
    static u8 filterComposite(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
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
        size_t n = (size_t)(box.x1 - box.x0);
        bool arithmetic = kind == SV("arithmetic");
        // A colour past its alpha is kept where a reader takes it so.
        u32 ceiling = arithmetic && keepsInvalidColor(*run.filter, p) ? 65535u : 0u;
        int mode = kind == SV("in") ? 1 : (kind == SV("out") ? 2 : (kind == SV("atop") ? 3 : (kind == SV("xor") ? 4 : (kind == SV("lighter") ? 5 : 0))));
        // The factor of a: 65535, plus the alpha of b, or minus it.
        static const u32 baseA[6] = {65535, 0, 65535, 0, 65535, 65535};
        static const u32 plusA[6] = {0, 0xffff, 0, 0xffff, 0, 0};
        static const u32 minusA[6] = {0, 0, 0xffff, 0, 0xffff, 0};
        // The factor of b: the same of the alpha of a.
        static const u32 baseB[6] = {65535, 0, 0, 65535, 65535, 65535};
        static const u32 minusB[6] = {0xffff, 0, 0, 0xffff, 0xffff, 0};
        // The arithmetic's sum over the product of the channels, each
        // channel and one, fixed.
        float coefficients[3] = {k1, k2, k3};
        i32 q[3];
        FixedSum sum = fixSum(coefficients, 3, k4, q);
        Vector<u16> rowA;
        Vector<u16> rowB;
        Vector<u16> fa;
        Vector<u16> fb;

        rowA.zero(n * 4);
        rowB.zero(n * 4);
        fa.zero(n);
        fb.zero(n);

        for (int y = box.y0; y < box.y1; y++) {
            readStored(a, box.x0, box.x1, y, rowA.mutData());
            readStored(b, box.x0, box.x1, y, rowB.mutData());

            if (arithmetic) {
                u16* o = planeAt(out, box.x0, y);

                for (size_t c = 0; c < n * 4; c++) {
                    u32 s = rowA[c] >> sum.drop;
                    u32 d = rowB[c] >> sum.drop;
                    u32 product = div65535((u32)rowA[c] * rowB[c]) >> sum.drop;

                    o[c] = (u16)fixedValue(sum, sum.constant + q[0] * (i32)product + q[1] * (i32)s + q[2] * (i32)d);
                }

                // A colour is no more than its alpha, or the ceiling.
                for (size_t x = 0; x < n; x++) {
                    u32 limit = o[x * 4 + 3] > ceiling ? o[x * 4 + 3] : ceiling;

                    for (int c = 0; c < 3; c++) {
                        o[x * 4 + (size_t)c] = (u16)(o[x * 4 + (size_t)c] < limit ? o[x * 4 + (size_t)c] : limit);
                    }
                }

                continue;
            }

            for (size_t x = 0; x < n; x++) {
                u32 sa = rowA[x * 4 + 3];
                u32 da = rowB[x * 4 + 3];

                fa.mut(x) = (u16)(baseA[mode] + (da & plusA[mode]) - (da & minusA[mode]));
                fb.mut(x) = (u16)(baseB[mode] - (sa & minusB[mode]));
            }

            twoTermSpan(planeAt(out, box.x0, y), rowA.data(), rowB.data(), fa.data(), fb.data(), n);
        }

        // Over, in, out, atop, xor and lighter.
        static const u8 kept[6] = {KeepsA | KeepsB, KeepsBoth, KeepsA, KeepsB, KeepsA | KeepsB, KeepsA | KeepsB};

        return arithmetic ? (u8)((k1 != 0.f ? KeepsBoth : 0) | (k2 != 0.f ? KeepsA : 0) | (k3 != 0.f ? KeepsB : 0) | (k4 > 0.f ? KeepsAll : 0)) : kept[mode];
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

    // A separable blend in stored premultiplied channels: each lane is
    // s (1 - da) + d (1 - sa) plus the term of the mode, the mode's function
    // of the colours not premultiplied times both alphas, written in the
    // premultiplied ones. On the alpha lanes, where s and d are the alphas,
    // the term is sa da, which gives sa + da - sa da; the modes whose term
    // falls short there add it where told the lane is alpha. Without
    // compositing the d (1 - sa) is left out: the colours mix, and the
    // alpha is sa. Two pixels, eight lanes, a step.
    template <bool composite, typename Term>
    static void blendLanes(u16* __restrict o, const u16* __restrict s, const u16* __restrict d, size_t count, Term term) {
        auto lane = [&](size_t base, int i) {
            i32 sc = s[base + (size_t)i];
            i32 dc = d[base + (size_t)i];
            i32 sa = s[base + (size_t)((i & 4) | 3)];
            i32 da = d[base + (size_t)((i & 4) | 3)];
            // A product, not a choice: coverage takes each instantiation
            // apart, and either would leave one side of a choice unrun.
            i32 under = (i32)div65535((u32)dc * (u32)(65535 - sa)) * (i32)composite;
            i32 v = (i32)div65535((u32)sc * (u32)(65535 - da)) + under + term(sc, dc, sa, da, (i & 3) == 3);

            // Rounding the terms apart can take a sum just below zero.
            o[base + (size_t)i] = saturate((u32)(v * (v > 0)));
        };
        size_t p = 0;

        for (; p + 2 <= count; p += 2) {
            for (int i = 0; i < 8; i++) {
                lane(p * 4, i);
            }
        }

        for (; p < count; p++) {
            for (int i = 0; i < 4; i++) {
                lane(p * 4, i);
            }
        }
    }

    static i32 product(i32 a, i32 b) {
        return (i32)div65535((u32)a * (u32)b);
    }

    // n / d rounded to the nearest, a half up.
    static u64 quotient(u64 n, u64 d) {
        return (n + d / 2) / d;
    }

    // Square roots of a quarter to a whole for soft light, 65536 a whole
    // one: values[i] is the root of 1/4 + i/1024, rounded, found a bit at a
    // step; between two of them a root is the straight line, off there by
    // under a fiftieth of a 65536th. The last is doubled for the line out
    // of a whole.
    struct Roots {
        u32 values[770];
    };

    constexpr Roots roots = [] {
        Roots table{};

        for (u32 i = 0; i <= 768; i++) {
            u64 v = (u64)(16384 + 64 * i) << 16;
            u64 root = 0;

            for (u64 bit = (u64)1 << 32; bit; bit >>= 2) {
                u64 take = 0 - (u64)(v >= root + bit);

                v -= (root + bit) & take;
                root = (root >> 1) + (bit & take);
            }

            table.values[i] = (u32)(root + (v > root));
        }

        table.values[769] = table.values[768];

        return table;
    }();

    // The luminosity of a colour times 100.
    static i32 luminosity(const i32* c) {
        return 30 * c[0] + 59 * c[1] + 11 * c[2];
    }

    static i32 saturation(const i32* c) {
        i32 x = c[0];
        i32 n = c[0];

        for (int k = 1; k < 3; k++) {
            x = c[k] > x ? c[k] : x;
            n = c[k] < n ? c[k] : n;
        }

        return x - n;
    }

    // The colour with its largest channel less its smallest set to s: the
    // largest becomes s, the smallest none and the middle one keeps its
    // place between them; a grey becomes black.
    static void setSaturation(i32* c, i32 s) {
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
            c[0] = 0;
            c[1] = 0;
            c[2] = 0;

            return;
        }

        int imid = 3 - imax - imin;
        u32 range = (u32)(c[imax] - c[imin]);

        c[imid] = (i32)(((u32)(c[imid] - c[imin]) * (u32)s + range / 2) / range);
        c[imax] = s;
        c[imin] = 0;
    }

    // The colour moved by a grey to the luminosity l, then, where a channel
    // falls outside none to all, pulled toward the grey of l until it fits;
    // l and the channels out are times 100. Pulled channels are written so
    // that every factor is positive, the ratio once in 2^32ths: c - n times
    // l / (l - n) from below none, all less x - c times
    // (all - l) / (x - l) from past all.
    static void setLuminosity(const i32* c, i32 l, i64* out) {
        const i64 whole = 100 * 65535;
        i64 shift = l - luminosity(c);
        i64 n = whole;
        i64 x = 0;

        for (int k = 0; k < 3; k++) {
            out[k] = 100 * (i64)c[k] + shift;
            n = out[k] < n ? out[k] : n;
            x = out[k] > x ? out[k] : x;
        }

        if (n < 0) {
            u64 ratio = ((u64)l << 32) / (u64)(l - n);

            for (int k = 0; k < 3; k++) {
                out[k] = (i64)(((u64)(out[k] - n) * ratio + ((u64)1 << 31)) >> 32);
            }
        } else if (x > whole) {
            u64 ratio = ((u64)(whole - l) << 32) / (u64)(x - l);

            for (int k = 0; k < 3; k++) {
                out[k] = whole - (i64)(((u64)(x - out[k]) * ratio + ((u64)1 << 31)) >> 32);
            }
        }
    }

    // The hue, saturation, colour and luminosity modes, which mix the three
    // channels of the colours not premultiplied: each is its channel, at
    // most its alpha, times the reciprocal of its alpha, 65535 a whole one,
    // and the mix times both alphas is the term of the mode. Where either
    // has no alpha the terms are none.
    static void blendColours(int mode, const u16* s, const u16* d, i32* t) {
        u32 sa = s[3];
        u32 da = d[3];
        i64 mixed[3] = {0, 0, 0};

        if (sa && da) {
            u64 inverseS = (65535u * 65536u + sa / 2) / sa;
            u64 inverseD = (65535u * 65536u + da / 2) / da;
            i32 cs[3];
            i32 cb[3];

            for (int k = 0; k < 3; k++) {
                cs[k] = (i32)(((s[k] < sa ? s[k] : sa) * inverseS + 32768) >> 16);
                cb[k] = (i32)(((d[k] < da ? d[k] : da) * inverseD + 32768) >> 16);
            }

            // Hue and colour set the source's, saturation and luminosity
            // the backdrop's; all but luminosity to the backdrop's
            // luminosity.
            i32 l = mode == 15 ? luminosity(cs) : luminosity(cb);
            i32* c = mode == 12 || mode == 14 ? cs : cb;

            if (mode == 12) {
                setSaturation(cs, saturation(cb));
            }

            if (mode == 13) {
                setSaturation(cb, saturation(cs));
            }

            setLuminosity(c, l, mixed);
        }

        for (int k = 0; k < 3; k++) {
            t[k] = (i32)quotient((u64)mixed[k] * sa * da, (u64)100 * 65535 * 65535);
        }
    }

    // Soft light: times both alphas, a source up to a half darkens the
    // backdrop to sa dc - (sa - 2 sc) dc (1 - cb); one past it lightens it
    // to sa dc + (2 sc - sa) (da D - dc), where D is the root of cb, or
    // below a quarter the cubic ((16 cb - 12) cb + 4) cb. cb is 65536ths
    // through one reciprocal of the backdrop's alpha, 2^31ths cut down so
    // that cb is at most a whole one, a colour past it taken as all of it;
    // a channel of none stays none, and a backdrop of none, all of whose
    // channels are none, divides by one.
    static void softLight(const u16* s, const u16* d, i32* t) {
        u32 sa = s[3];
        u32 a = d[3];
        u64 inverse = (1u << 31) / (a + (a == 0));

        for (int k = 0; k < 3; k++) {
            u32 sc = s[k];
            u32 c = d[k] < a ? d[k] : a;
            u32 x = (u32)((c * inverse + 16384) >> 15);

            if (c == 0) {
                t[k] = 0;
                continue;
            }

            if (2 * sc <= sa) {
                u32 kept = (c * (65536 - x) + 32768) >> 16;

                t[k] = (i32)div65535(sa * c - (sa - 2 * sc) * kept);
                continue;
            }

            i64 lifted;

            if (x <= 16384) {
                lifted = ((i64)c * (((x * x) >> 12) - 12 * x + 262144) + 32768) >> 16;
            } else {
                u32 i = (x - 16384) >> 6;
                u32 f = (x - 16384) & 63;
                u32 root = roots.values[i] + (((roots.values[i + 1] - roots.values[i]) * f + 32) >> 6);

                lifted = ((i64)a * root + 32768) >> 16;
            }

            // Below none only for a source past its alpha, where it
            // truncates.
            t[k] = (i32)(((i64)sa * c + (i64)(2 * sc - sa) * (lifted - c) + 32767) / 65535);
        }
    }

    // A blend whose terms come a pixel at a time: terms(s, d, t) writes the
    // three colour terms of a source pixel over a backdrop pixel, and each
    // lane is s (1 - da) + d (1 - sa) plus its term, the alpha's sa da;
    // without compositing the d (1 - sa) is left out, as in blendLanes.
    template <bool composite, typename Terms>
    static void blendPixels(u16* __restrict o, const u16* __restrict s, const u16* __restrict d, size_t count, Terms terms) {
        for (size_t p = 0; p < count * 4; p += 4) {
            u32 sa = s[p + 3];
            u32 da = d[p + 3];
            i32 t[4];

            terms(s + p, d + p, t);
            t[3] = (i32)div65535(sa * da);

            for (size_t k = 0; k < 4; k++) {
                i32 v = (i32)div65535(s[p + k] * (65535 - da)) + (i32)div65535(d[p + k] * (65535 - sa)) * (i32)composite + t[k];

                o[p + k] = saturate((u32)(v * (v > 0)));
            }
        }
    }

    // The blend of a source span over a destination span into an output
    // span, in stored channels. The modes whose function is a product, a
    // minimum or maximum, a difference or a choice on a half are exact; the
    // dodge, the burn, soft light and the modes that mix the channels
    // divide or take roots in integers, to about a stored unit.
    template <bool composite = true>
    static void blendSpan(int mode, u16* __restrict o, const u16* __restrict s, const u16* __restrict d, size_t count) {
        switch (mode) {
            case 0:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32, i32, i32 da, bool) {
                    return product(sc, da);
                });
                return;
            case 1:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32, i32, bool) {
                    return product(sc, dc);
                });
                return;
            case 2:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    return product(sc, da) + product(dc, sa) - product(sc, dc);
                });
                return;
            case 3:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    return 2 * dc <= da ? 2 * product(sc, dc) : product(sa, da) - 2 * product(da - dc, sa - sc);
                });
                return;
            case 4:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    i32 u = product(dc, sa);
                    i32 v = product(sc, da);

                    return u < v ? u : v;
                });
                return;
            case 5:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    i32 u = product(dc, sa);
                    i32 v = product(sc, da);

                    return u > v ? u : v;
                });
                return;
            case 6:
                // cb / (1 - cs), at most all, times both alphas is sa / 65535
                // times dc sa / (sa - sc), at most da: none on a backdrop of
                // none, all from a source of all.
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    i32 full = product(sa, da);

                    if (dc == 0) {
                        return 0;
                    }

                    if (sc >= sa) {
                        return full;
                    }

                    u32 n = (u32)dc * (u32)sa;
                    u32 m = (u32)(sa - sc);

                    return n < (u32)da * m ? product(sa, (i32)((n + m / 2) / m)) : full;
                });
                return;
            case 7:
                // 1 - (1 - cb) / cs, at least none, times both alphas is
                // sa / 65535 times da less (da - dc) sa / sc, at least none:
                // all on a backdrop of all, none from a source of none.
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    if (dc >= da) {
                        return product(sa, da);
                    }

                    if (sc == 0) {
                        return 0;
                    }

                    u32 n = (u32)(da - dc) * (u32)sa;
                    u32 m = (u32)sc;

                    return n < (u32)da * m ? product(sa, da - (i32)((n + m / 2) / m)) : 0;
                });
                return;
            case 8:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    return 2 * sc <= sa ? 2 * product(sc, dc) : product(sa, da) - 2 * product(da - dc, sa - sc);
                });
                return;
            case 9:
                blendPixels<composite>(o, s, d, count, softLight);
                return;
            case 16:
                // plus-lighter: the sum, which the lanes saturate.
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool) {
                    return product(sc, da) + product(dc, sa);
                });
                return;
            case 10:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool alpha) {
                    i32 v = product(dc, sa) - product(sc, da);

                    return (v < 0 ? -v : v) + alpha * product(sa, da);
                });
                return;
            case 11:
                blendLanes<composite>(o, s, d, count, [](i32 sc, i32 dc, i32 sa, i32 da, bool alpha) {
                    return product(sc, da) + product(dc, sa) - 2 * product(sc, dc) + alpha * product(sa, da);
                });
                return;
            default:
                blendPixels<composite>(o, s, d, count, [mode](const u16* sp, const u16* dp, i32* t) {
                    blendColours(mode, sp, dp, t);
                });
        }
    }

    // The source pixels blended into the stored ones in linear light, by
    // the blend mode, 0 laying them over: a clear source pixel leaves the
    // stored one, an opaque one laid over is itself, the rest go to linear
    // light and back, a pair as the one before as it did.
    static void blendLinear(Raster& r, int mode, u16* d, const u16* s, size_t count) {
        Curve in = curveOf(r, true);
        Curve back = curveOf(r, false);
        u64 lastS = ~(u64)0;
        u64 lastD = ~(u64)0;
        u64 lastOut = 0;

        for (size_t x = 0; x < count; x++, s += 4, d += 4) {
            u64 source;
            u64 stored;
            u16 ls[4];
            u16 ld[4];
            u16 lo[4];

            if (!s[3] || (s[3] == 65535 && !mode)) {
                memcpy(d, s[3] ? s : d, sizeof(ls));
                continue;
            }

            memcpy(&source, s, sizeof(source));
            memcpy(&stored, d, sizeof(stored));

            if (source != lastS || stored != lastD) {
                convertSpan(in, s, ls, 1);
                convertSpan(in, d, ld, 1);

                if (mode) {
                    blendSpan(mode, lo, ls, ld, 1);
                } else {
                    memcpy(lo, ld, sizeof(lo));
                    overSpan(lo, ls, 1);
                }

                convertSpan(back, lo, ld, 1);
                lastS = source;
                lastD = stored;
                memcpy(&lastOut, ld, sizeof(lastOut));
            }

            memcpy(d, &lastOut, sizeof(lastOut));
        }
    }

    // The blend of the two inputs, by rows of stored channels.
    static void filterBlend(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const Document& doc = *run.filter->doc;
        Plane a = filterInputBuffer(run, p.in1, p.linear);
        Plane b = filterInputBuffer(run, p.in2, p.linear);
        u32 modeValue = attribute(p.node, AtomMode);
        int mode = modeValue ? blendMode(trim(textOf(doc, modeValue))) : 0;
        // no-composite, present, blends without compositing over in2.
        bool composite = !attribute(p.node, AtomNoComposite);
        const IntBox& box = p.box;
        size_t n = (size_t)(box.x1 - box.x0);
        Vector<u16> rowA;
        Vector<u16> rowB;

        rowA.zero(n * 4);
        rowB.zero(n * 4);

        for (int y = box.y0; y < box.y1; y++) {
            readStored(a, box.x0, box.x1, y, rowA.mutData());
            readStored(b, box.x0, box.x1, y, rowB.mutData());
            if (composite) {
                blendSpan(mode, planeAt(out, box.x0, y), rowA.data(), rowB.data(), n);
            } else {
                blendSpan<false>(mode, planeAt(out, box.x0, y), rowA.data(), rowB.data(), n);
            }
        }
    }

    static void filterMerge(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        const IntBox& box = p.box;

        // Each input over the ones before, where it has pixels.
        for (u32 k = 0; k < p.inputCount; k++) {
            Plane in = filterInputBuffer(run, p.inputs[k], p.linear);
            IntBox inside = boxInside(box, in.box);

            for (int y = inside.y0; y < inside.y1; y++) {
                overSpan(planeAt(out, inside.x0, y), planeAt(in, inside.x0, y), (size_t)(inside.x1 - inside.x0));
            }
        }
    }

    // The running extreme of windows of w = 2r + 1 values over a line of
    // groups of four stored lanes padded by r zeros on each side, forward in
    // g and backward in h, so that the extreme of the window at x is the
    // extreme of h[x] and g[x + 2r]; the outputs from first to last of the
    // line go to dst.
    template <typename Extreme>
    static void slideExtremeBy(const u16* src, LineLayout from, u16* dst, LineLayout to, int n, int groups, int r, int first, int last, MorphologyScratch& scratch, Extreme extreme) {
        size_t lanes = (size_t)groups * 4;
        int m = n + 2 * r;
        int w = 2 * r + 1;
        int run = 0;

        scratch.line.zero((size_t)m * lanes);
        scratch.g.zero((size_t)m * lanes);
        scratch.h.zero((size_t)m * lanes);

        u16* line = scratch.line.mutData();
        u16* pg = scratch.g.mutData();
        u16* ph = scratch.h.mutData();

        for (int i = 0; i < n; i++) {
            for (int k = 0; k < groups; k++) {
                memcpy(line + (size_t)(i + r) * lanes + (size_t)k * 4, src + i * from.position + k * from.group, sizeof(u16) * 4);
            }
        }

        for (int i = 0; i < m; i++) {
            const u16* v = line + (size_t)i * lanes;
            u16* out = pg + (size_t)i * lanes;

            if (run == 0) {
                memcpy(out, v, sizeof(u16) * lanes);
            } else {
                const u16* previous = out - lanes;

                for (size_t c = 0; c < lanes; c++) {
                    out[c] = extreme(previous[c], v[c]);
                }
            }

            run = run + 1 == w ? 0 : run + 1;
        }

        // Backward, a window starts at the last value and wherever i + 1 is
        // a multiple of w; phase counts i + 1 down modulo w.
        int phase = m % w;

        for (int i = m - 1; i >= 0; i--) {
            const u16* v = line + (size_t)i * lanes;
            u16* out = ph + (size_t)i * lanes;

            if (i == m - 1 || phase == 0) {
                memcpy(out, v, sizeof(u16) * lanes);
            } else {
                for (size_t c = 0; c < lanes; c++) {
                    out[c] = extreme(out[c + lanes], v[c]);
                }
            }

            phase = phase == 0 ? w - 1 : phase - 1;
        }

        for (int x = first; x < last; x++) {
            const u16* a = ph + (size_t)x * lanes;
            const u16* b = pg + (size_t)(x + 2 * r) * lanes;
            u16 v[32];

            for (size_t c = 0; c < lanes; c++) {
                v[c] = extreme(a[c], b[c]);
            }

            for (int k = 0; k < groups; k++) {
                memcpy(dst + (x - first) * to.position + k * to.group, v + k * 4, sizeof(u16) * 4);
            }
        }
    }

    static void slideExtreme(const u16* src, LineLayout from, u16* dst, LineLayout to, int n, int groups, int r, int first, int last, bool dilate, MorphologyScratch& scratch) {
        if (dilate) {
            slideExtremeBy(src, from, dst, to, n, groups, r, first, last, scratch, [](u16 a, u16 b) {
                return a > b ? a : b;
            });
        } else {
            slideExtremeBy(src, from, dst, to, n, groups, r, first, last, scratch, [](u16 a, u16 b) {
                return a < b ? a : b;
            });
        }
    }

    // Erodes or dilates the input over the box: the extremes are those of
    // the stored channels, groups of eight rows into a pass over the box's
    // columns, then groups of eight of its columns into the box.
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
        MorphologyScratch scratch;
        Vector<u16> rows;

        // An empty box is empty by its x, as boxInside leaves it.
        if (box.x1 <= box.x0) {
            return;
        }

        int x0 = box.x0 - ix > 0 ? box.x0 - ix : 0;
        int x1 = box.x1 + ix < width ? box.x1 + ix : width;
        int y0 = box.y0 - iy > 0 ? box.y0 - iy : 0;
        int y1 = box.y1 + iy < height ? box.y1 + iy : height;
        Plane pass = makePlane(IntBox{box.x0, y0, box.x1, y1});
        size_t rowLength = (size_t)(x1 - x0) * 4;
        ptrdiff_t passAcross = (ptrdiff_t)planeWidth(pass) * 4;
        ptrdiff_t outAcross = (ptrdiff_t)planeWidth(out) * 4;

        rows.zero(rowLength * 8);

        for (int y = y0; y < y1; y += 8) {
            int group = y1 - y < 8 ? y1 - y : 8;

            for (int g = 0; g < group; g++) {
                readStored(in, x0, x1, y + g, rows.mutData() + rowLength * (size_t)g);
            }

            slideExtreme(rows.data(), LineLayout{4, (ptrdiff_t)rowLength}, planeAt(pass, box.x0, y), LineLayout{4, passAcross}, x1 - x0, group, ix, box.x0 - x0, box.x1 - x0, dilate, scratch);
        }

        for (int x = box.x0; x < box.x1; x += 8) {
            int group = box.x1 - x < 8 ? box.x1 - x : 8;

            slideExtreme(planeAt(pass, x, y0), LineLayout{passAcross, 4}, planeAt(out, x, box.y0), LineLayout{outAcross, 4}, y1 - y0, group, iy, box.y0 - y0, box.y1 - y0, dilate, scratch);
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

                memcpy(planeAt(out, x, y), planeRead(in, sx, sy), sizeof(u16) * 4);
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

        // A kernel sums to zero, and its divisor is then one, when its sum is
        // lost in the rounding of its values to binary: within a millionth
        // of their magnitude.
        double total = 0.0;
        double magnitude = 0.0;

        for (float k : kernel) {
            total += k;
            magnitude += fabs((double)k);
        }

        float sum = fabs(total) <= magnitude * 0x1p-20 ? 0.f : (float)total;
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

        EdgeMode edge = edgeModeOf(doc, p.node, EdgeMode::Duplicate);
        // The kernel unit in pixels: a pixel, or kernelUnitLength in
        // primitive units, which must be above zero.
        float unitX = 1.f;
        float unitY = 1.f;

        if (attribute(p.node, AtomKernelUnitLength) && numberPair(doc, p.node, AtomKernelUnitLength, unitX, unitY) && unitX > 0.f && unitY > 0.f) {
            unitX = userX(*run.filter, unitX) * run.scaleX;
            unitY = userY(*run.filter, unitY) * run.scaleY;
        } else {
            unitX = 1.f;
            unitY = 1.f;
        }

        bool spaced = unitX != 1.f || unitY != 1.f;
        u32 preserveValue = attribute(p.node, AtomPreserveAlpha);
        bool preserve = preserveValue && trim(textOf(doc, preserveValue)) == SV("true");
        IntBox src = inputBox(run, p.in1);
        const IntBox& box = p.box;
        int n = box.x1 - box.x0;
        int span = n + orderX - 1;
        // The input column each column of the taps reads, after the edge
        // mode; -1 for none.
        Vector<int> columns;
        // The rows of taps: each input pixel as a tap reads it, once, in a
        // ring of the kernel's rows; which row each slot holds.
        Vector<u16> ring;
        Vector<int> held;
        // The kernel turned to the order of the taps and over the divisor,
        // fixed with the bias as a sum; a tap of no factor is not read.
        Vector<float> weights;
        Vector<i32> factors;
        Vector<i32> acc;
        Vector<u16> row;
        Vector<u16> spacedTaps;

        columns.zero((size_t)span);
        ring.zero((size_t)span * 4 * (size_t)orderY);
        held.zero((size_t)orderY);
        weights.zero(kernel.length());
        factors.zero(kernel.length());
        acc.zero((size_t)n * 4);
        row.zero((size_t)n * 4);
        spacedTaps.zero((size_t)n * 4);

        for (int j = 0; j < orderY; j++) {
            for (int i = 0; i < orderX; i++) {
                weights.mut((size_t)(j * orderX + i)) = kernel[(size_t)((orderY - 1 - j) * orderX + (orderX - 1 - i))] / divisor;
            }
        }

        FixedSum fixed = fixSum(weights.data(), weights.length(), bias, factors.mutData());

        // The edge mode maps a coordinate past the input back into it, or
        // leaves it out.
        for (int i = 0; i < span; i++) {
            columns.mut((size_t)i) = edgeFold(box.x0 - targetX + i, src.x0, src.x1, edge);
        }

        // No row of taps is held yet: the row before the first asked for.
        for (int k = 0; k < orderY; k++) {
            held.mut((size_t)k) = box.y0 - targetY - 1;
        }

        // A tap is the input as stored, or not premultiplied when alpha is
        // preserved.
        auto taps = [&](int r) {
            size_t slot = (size_t)(((r % orderY) + orderY) % orderY);
            u16* out = ring.mutData() + slot * (size_t)span * 4;

            if (held[slot] == r) {
                return (const u16*)out;
            }

            held.mut(slot) = r;

            int sy = edgeFold(r, src.y0, src.y1, edge);

            for (int i = 0; i < span; i++) {
                u16* t = out + (size_t)i * 4;
                int sx = columns[(size_t)i];

                if (sx < 0 || sy < 0) {
                    memset(t, 0, sizeof(u16) * 4);
                    continue;
                }

                const u16* s = planeRead(in, sx, sy);

                if (preserve) {
                    u32 c[4];

                    unpremultiplyStored(s, c);

                    for (int k = 0; k < 4; k++) {
                        t[k] = (u16)c[k];
                    }

                    continue;
                }

                memcpy(t, s, sizeof(u16) * 4);
            }

            return (const u16*)out;
        };

        // Taps a kernel unit apart read between pixels: the four around a
        // tap, each by the edge mode, mixed by their distance, as Firefox
        // reads them.
        auto spacedRow = [&](float tx, float ty) {
            int y0 = (int)floorf(ty);
            float wy = ty - (float)y0;

            for (int x = 0; x < n; x++) {
                float fx = (float)(box.x0 + x) + tx;
                int x0 = (int)floorf(fx);
                float wx = fx - (float)x0;
                float mixed[4] = {0.f, 0.f, 0.f, 0.f};

                for (int k = 0; k < 4; k++) {
                    int sx = edgeFold(x0 + (k & 1), src.x0, src.x1, edge);
                    int sy = edgeFold(y0 + (k >> 1), src.y0, src.y1, edge);
                    float w = (k & 1 ? wx : 1.f - wx) * (k >> 1 ? wy : 1.f - wy);
                    u32 c[4];

                    if (sx < 0 || sy < 0) {
                        continue;
                    }

                    const u16* s = planeRead(in, sx, sy);

                    if (preserve) {
                        unpremultiplyStored(s, c);
                    } else {
                        for (int ch = 0; ch < 4; ch++) {
                            c[ch] = s[ch];
                        }
                    }

                    for (int ch = 0; ch < 4; ch++) {
                        mixed[ch] += w * (float)c[ch];
                    }
                }

                for (int ch = 0; ch < 4; ch++) {
                    spacedTaps.mut((size_t)x * 4 + (size_t)ch) = (u16)(mixed[ch] + 0.5f);
                }
            }

            return spacedTaps.data();
        };

        for (int y = box.y0; y < box.y1; y++) {
            i32* sums = acc.mutData();
            size_t lanes = (size_t)n * 4;

            for (size_t c = 0; c < lanes; c++) {
                sums[c] = fixed.constant;
            }

            for (int j = 0; j < orderY; j++) {
                const u16* t = nullptr;

                for (int i = 0; i < orderX; i++) {
                    i32 q = factors[(size_t)(j * orderX + i)];

                    if (q == 0) {
                        continue;
                    }

                    if (!t && !spaced) {
                        t = taps(y - targetY + j);
                    }

                    const u16* tap = spaced ? spacedRow((float)(i - targetX) * unitX, (float)y + (float)(j - targetY) * unitY) : t + (size_t)i * 4;

                    for (size_t c = 0; c < lanes; c++) {
                        sums[c] += q * (i32)(tap[c] >> fixed.drop);
                    }
                }
            }

            u16* o = planeAt(out, box.x0, y);

            if (preserve) {
                readStored(in, box.x0, box.x1, y, row.mutData());
            }

            for (int x = 0; x < n; x++) {
                const i32* a = sums + (size_t)x * 4;
                u16* d = o + (size_t)x * 4;

                // The colours not premultiplied, over the input's alpha.
                if (preserve) {
                    u32 alpha = row[(size_t)x * 4 + 3];

                    for (int ch = 0; ch < 3; ch++) {
                        d[ch] = (u16)div65535(fixedValue(fixed, a[ch]) * alpha);
                    }

                    d[3] = (u16)alpha;
                    continue;
                }

                u32 alpha = fixedValue(fixed, a[3]);
                // A colour takes the bias times the alpha, the sum and bias
                // of the alpha channel, as SVG and Filter Effects have it:
                // the sum carries the bias whole, so the rest of it goes.
                i64 unbiased = (i64)fixed.constant * (i64)(65535u - alpha) / 65535;

                for (int ch = 0; ch < 3; ch++) {
                    u32 v = fixedValue(fixed, (i32)((i64)a[ch] - unbiased));

                    d[ch] = (u16)(v < alpha ? v : alpha);
                }

                d[3] = (u16)alpha;
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

        // The noise of a row at the pixel centres in its space, premultiplied
        // and stored at once.
        size_t n = (size_t)(box.x1 - box.x0);
        Vector<float> noise;
        Vector<float> row;

        noise.zero(n * 4);
        row.zero(n * 4);

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                Point point = apply(inverse, Point{(float)x + 0.5f, (float)y + 0.5f});

                turbulenceAt(*tb, point.x, point.y, frequencyX, frequencyY, octaves, fractal, stitching ? &stitch : nullptr, noise.mutData() + (size_t)(x - box.x0) * 4);
            }

            premultiplySpan(row.mutData(), noise.data(), n);
            writeSpan(out, box.x0, box.x1, y, row.data());
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
        size_t n = (size_t)(box.x1 - box.x0);
        Vector<u16> row;

        row.zero(n * 4);

        // A pixel of the image, none past the raster.
        auto tap = [&](int tx, int ty) {
            return tx < 0 || ty < 0 || tx >= (int)run.raster->width || ty >= (int)run.raster->height ? zeroStored : planeRead(in, tx, ty);
        };

        // The map's pixel not premultiplied moves each pixel's centre; the
        // image there is read between the four pixel centres about it, so a
        // move of a fraction of a pixel moves the image by as much.
        for (int y = box.y0; y < box.y1; y++) {
            readStored(map, box.x0, box.x1, y, row.mutData());

            for (int x = box.x0; x < box.x1; x++) {
                u32 m[4];

                unpremultiplyStored(row.data() + (size_t)(x - box.x0) * 4, m);

                // Far past the raster is far enough, and an int holds it.
                float fx = maxf(minf((float)x + sx * ((float)m[cx] / 65535.f - 0.5f), 1e9f), -1e9f);
                float fy = maxf(minf((float)y + sy * ((float)m[cy] / 65535.f - 0.5f), 1e9f), -1e9f);
                float x0 = floorf(fx);
                float y0 = floorf(fy);
                int tx = (int)x0;
                int ty = (int)y0;

                bilinear(tap(tx, ty), tap(tx + 1, ty), tap(tx, ty + 1), tap(tx + 1, ty + 1), fx - x0, fy - y0, planeAt(out, x, y));
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

        // The box lies on the raster. Its stored alphas, and a pixel around
        // it taking the nearest in it, feed the slopes in integers.
        const IntBox& box = p.box;
        int columns = box.x1 - box.x0;
        int rows = box.y1 - box.y0;
        size_t across = (size_t)columns + 2;
        Vector<u16> alpha;
        Vector<u16> row;
        Vector<float> colors;

        alpha.zero(across * (size_t)(rows + 2));
        row.zero((size_t)columns * 4);
        colors.zero((size_t)columns * 4);

        for (int y = 0; y < rows; y++) {
            u16* a = alpha.mutData() + (size_t)(y + 1) * across;

            readStored(in, box.x0, box.x1, box.y0 + y, row.mutData());

            for (int x = 0; x < columns; x++) {
                a[x + 1] = row[(size_t)x * 4 + 3];
            }

            a[0] = a[1];
            a[columns + 1] = a[columns];
        }

        memcpy(alpha.mutData(), alpha.data() + across, sizeof(u16) * across);
        memcpy(alpha.mutData() + (size_t)(rows + 1) * across, alpha.data() + (size_t)rows * across, sizeof(u16) * across);

        // The kernel unit: a pixel apart the slopes take neighbours; with
        // kernelUnitLength, in primitive units and above zero, they take
        // the alpha that far, between pixels, the nearest in the box past
        // it, and a slope over that length is the normal's, as Firefox
        // reads it.
        float kernelX = 1.f;
        float kernelY = 1.f;
        float spanX = 1.f;
        float spanY = 1.f;

        if (attribute(p.node, AtomKernelUnitLength) && numberPair(doc, p.node, AtomKernelUnitLength, kernelX, kernelY) && kernelX > 0.f && kernelY > 0.f) {
            kernelX = userX(f, kernelX);
            kernelY = userY(f, kernelY);
            spanX = kernelX * run.scaleX;
            spanY = kernelY * run.scaleY;
        } else {
            kernelX = 1.f;
            kernelY = 1.f;
        }

        bool spaced = spanX != 1.f || spanY != 1.f;
        auto between = [&](float bx, float by) {
            bx = minf(maxf(bx, 1.f), (float)columns);
            by = minf(maxf(by, 1.f), (float)rows);

            int x0 = (int)bx;
            int y0 = (int)by;
            int x1 = x0 + (x0 < columns);
            int y1 = y0 + (y0 < rows);
            float wx = bx - (float)x0;
            float wy = by - (float)y0;
            const u16* a = alpha.data();
            float top = (float)a[(size_t)y0 * across + (size_t)x0] * (1.f - wx) + (float)a[(size_t)y0 * across + (size_t)x1] * wx;
            float bottom = (float)a[(size_t)y1 * across + (size_t)x0] * (1.f - wx) + (float)a[(size_t)y1 * across + (size_t)x1] * wx;

            return (i32)(top * (1.f - wy) + bottom * wy + 0.5f);
        };

        for (int y = box.y0; y < box.y1; y++) {
            const u16* above = alpha.data() + (size_t)(y - box.y0) * across;
            const u16* middle = above + across;
            const u16* below = middle + across;

            for (int x = box.x0; x < box.x1; x++) {
                // Spaced, a sample is the box's nearest past its edges,
                // and every pixel takes the inner kernel.
                bool left = x == box.x0 && !spaced;
                bool right = x == box.x1 - 1 && !spaced;
                bool top = y == box.y0 && !spaced;
                bool bottom = y == box.y1 - 1 && !spaced;
                int i = x - box.x0;
                float cx = (float)(i + 1);
                float cy = (float)(y - box.y0 + 1);
                i32 nx;
                i32 ny;
                float factorX;
                float factorY;
                i32 a00 = spaced ? between(cx - spanX, cy - spanY) : above[i];
                i32 a01 = spaced ? between(cx, cy - spanY) : above[i + 1];
                i32 a02 = spaced ? between(cx + spanX, cy - spanY) : above[i + 2];
                i32 a10 = spaced ? between(cx - spanX, cy) : middle[i];
                i32 a11 = middle[i + 1];
                i32 a12 = spaced ? between(cx + spanX, cy) : middle[i + 2];
                i32 a20 = spaced ? between(cx - spanX, cy + spanY) : below[i];
                i32 a21 = spaced ? between(cx, cy + spanY) : below[i + 1];
                i32 a22 = spaced ? between(cx + spanX, cy + spanY) : below[i + 2];

                if (box.x1 - box.x0 == 1 || box.y1 - box.y0 == 1) {
                    nx = 0;
                    ny = 0;
                    factorX = 0.f;
                    factorY = 0.f;
                } else if (top && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2 * a11 + 2 * a12 - a21 + a22;
                    ny = -2 * a11 - a12 + 2 * a21 + a22;
                } else if (top && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2 * a10 + 2 * a11 - a20 + a21;
                    ny = -a10 - 2 * a11 + a20 + 2 * a21;
                } else if (bottom && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a01 + a02 - 2 * a11 + 2 * a12;
                    ny = -2 * a01 - a02 + 2 * a11 + a12;
                } else if (bottom && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a00 + a01 - 2 * a10 + 2 * a11;
                    ny = -a00 - 2 * a01 + a10 + 2 * a11;
                } else if (top) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -2 * a10 + 2 * a12 - a20 + a22;
                    ny = -a10 - 2 * a11 - a12 + a20 + 2 * a21 + a22;
                } else if (bottom) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -a00 + a02 - 2 * a10 + 2 * a12;
                    ny = -a00 - 2 * a01 - a02 + a10 + 2 * a11 + a12;
                } else if (left) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a01 + a02 - 2 * a11 + 2 * a12 - a21 + a22;
                    ny = -2 * a01 - a02 + 2 * a21 + a22;
                } else if (right) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a00 + a01 - 2 * a10 + 2 * a11 - a20 + a21;
                    ny = -a00 - 2 * a01 + a20 + 2 * a21;
                } else {
                    factorX = 1.f / 4.f;
                    factorY = 1.f / 4.f;
                    nx = -a00 + a02 - 2 * a10 + 2 * a12 - a20 + a22;
                    ny = -a00 - 2 * a01 - a02 + a20 + 2 * a21 + a22;
                }

                // The slopes in alpha units: a stored alpha is 65535 for 1.
                constexpr float unit = 1.f / 65535.f;
                float dx = factorX * (float)nx * unit / spanX;
                float dy = factorY * (float)ny * unit / spanY;
                // The height samples are in device space; the light is in user space.
                Vector3 normal = normalize3(Vector3{-surfaceScale * kernelX * (f.ctm.a * dx + f.ctm.b * dy), -surfaceScale * kernelY * (f.ctm.c * dx + f.ctm.d * dy), 1.f});
                Vector3 toLight = direction;
                Color color = lightColor;

                if (light->name != AtomFeDistantLight) {
                    Point surface = apply(inverse, Point{(float)x + 0.5f, (float)y + 0.5f});

                    toLight = normalize3(Vector3{lightPosition.x - surface.x, lightPosition.y - surface.y, lightPosition.z - surfaceScale * (float)a11 * unit});
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
                float* o = colors.mutData() + (size_t)i * 4;

                o[0] = r;
                o[1] = g;
                o[2] = b;
                o[3] = specular ? maxf(r, maxf(g, b)) : 1.f;
            }

            writeSpan(out, box.x0, box.x1, y, colors.data());
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, const Plane& base);

    static void filterImage(FilterRun& run, const FilterPrimitive& p, const Plane& out) {
        Plane content = makePlane(p.box);

        runProgram(*run.raster, *run.lowering, run.lowering->programs.data() + p.programFirst, p.programCount, content);

        // The image drawn over the box is the result: as it is, or with its
        // colours taken along the sRGB curve to linear light.
        if (p.linear) {
            convertRows(*run.raster, content, out, p.box, true);
        } else {
            copyBox(content, out, p.box);
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
            memset(layer.data, 0, sizeof(u16) * size * 4);

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
            convertInPlace(r, run.source, true);
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

        // The stack holds primitives not done, each reading the one above
        // it, so lower in index: none is on it twice, and the top is popped
        // as it is done.
        while (!stack.empty()) {
            u32 k = stack.back();
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
            // Where the result reaches and the hard edges of what it holds,
            // before its subregion cuts them: everywhere for a primitive that
            // makes its pixels anew; its inputs' where it keeps transparent
            // what they leave so. An image is cut by nothing past its own.
            Box reach = allBox();
            Box hard = allBox();
            Point shift{0.f, 0.f};
            u8 keeps = 0;

            switch (p.kind) {
                case AtomFeFlood:
                    filterFlood(p, out);
                    break;
                case AtomFeOffset:
                    shift = filterOffset(run, p, out);
                    reach = movedBox(supportOf(run, p.in1), shift);
                    hard = movedBox(edgesOf(run, p.in1), shift);
                    break;
                case AtomFeGaussianBlur:
                    filterBlur(run, p, out);
                    break;
                case AtomFeDropShadow:
                    filterDropShadow(run, p, out);
                    break;
                case AtomFeColorMatrix:
                case AtomFeComponentTransfer:
                    if (!(p.kind == AtomFeColorMatrix ? filterColorMatrix(run, p, out) : filterComponentTransfer(run, p, out))) {
                        reach = supportOf(run, p.in1);
                        hard = edgesOf(run, p.in1);
                    }

                    break;
                case AtomFeComposite:
                    keeps = filterComposite(run, p, out);
                    reach = keptBox(keeps, supportOf(run, p.in1), supportOf(run, p.in2));
                    hard = keptBox(keeps, edgesOf(run, p.in1), edgesOf(run, p.in2));
                    break;
                case AtomFeBlend:
                    filterBlend(run, p, out);
                    reach = joinBoxes(supportOf(run, p.in1), supportOf(run, p.in2));
                    hard = joinBoxes(edgesOf(run, p.in1), edgesOf(run, p.in2));
                    break;
                case AtomFeMerge:
                    filterMerge(run, p, out);
                    reach = emptyBox();
                    hard = emptyBox();

                    for (u32 i = 0; i < p.inputCount; i++) {
                        reach = joinBoxes(reach, supportOf(run, p.inputs[i]));
                        hard = joinBoxes(hard, edgesOf(run, p.inputs[i]));
                    }

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
                    hard = p.clip;
                    break;
            }

            Box support = meetBoxes(p.clip, reach);
            // A side of the subregion inside the hard edges cuts what it
            // holds there; a hundredth of a pixel is the same edge.
            Box cut{p.clip.x0 > hard.x0 + 1e-2f ? p.clip.x0 : -INFINITY, p.clip.y0 > hard.y0 + 1e-2f ? p.clip.y0 : -INFINITY, p.clip.x1 < hard.x1 - 1e-2f ? p.clip.x1 : INFINITY, p.clip.y1 < hard.y1 - 1e-2f ? p.clip.y1 : INFINITY};

            coverEdges(out, cut);
            keepSupport(out, support);
            run.results.mut(k) = FilterResult{out, p.linear, support, meetBoxes(p.clip, hard)};
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

        memset(layer.data, 0, sizeof(u16) * size * 4);

        if (last.plane.data && last.linear) {
            convertRows(r, last.plane, layer, kept, false);
        }

        for (int y = kept.y0; last.plane.data && !last.linear && y < kept.y1; y++) {
            memcpy(planeAt(layer, kept.x0, y), planeAt(last.plane, kept.x0, y), sizeof(u16) * 4 * (size_t)(kept.x1 - kept.x0));
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
        Plane content{(u16*)pool.allocate(sizeof(u16) * size * 4), IntBox{0, 0, lw, lh}};
        Matrix toDevice;

        // A layer's frame inverts. Seen in perspective, a layer takes the
        // maps both ways, and a point behind the eye is nothing.
        const Projective* maps = op.perspective;
        Box canvasBox{0.f, 0.f, (float)r.width, (float)r.height};
        Box layerBox{0.f, 0.f, (float)lw, (float)lh};
        auto mapped = [&](int way, Point in, Point& out) {
            if (maps) {
                return projectPoint(maps[way], in, out);
            }

            out = apply(way ? toDevice : op.layerMatrix, in);

            return true;
        };
        auto mappedBox = [&](int way, const Box& box) {
            return maps ? projectBox(maps[way], box, way ? canvasBox : layerBox) : deviceBox(way ? toDevice : op.layerMatrix, box);
        };

        invert(op.layerMatrix, toDevice);
        sub.width = op.layerWidth;
        sub.height = op.layerHeight;

        if (l.backgroundInputs) {
            // The layer inherits the background under its footprint, resampled
            // from the pixels around it.
            IntBox canvas{0, 0, (int)r.width, (int)r.height};
            Box footprint = mappedBox(1, layerBox);
            Plane background = backgroundBuffer(r, pixelBox(Box{footprint.x0 - 2.f, footprint.y0 - 2.f, footprint.x1 + 2.f, footprint.y1 + 2.f}, canvas));

            if (background.data) {
                Plane inherited{(u16*)pool.allocate(sizeof(u16) * size * 4), content.box};

                for (u32 y = 0; y < sub.height; y++) {
                    for (u32 x = 0; x < sub.width; x++) {
                        Point point;

                        if (!mapped(1, Point{(float)x + .5f, (float)y + .5f}, point)) {
                            continue;
                        }

                        float fx = point.x - .5f;
                        float fy = point.y - .5f;
                        int ix = (int)floorf(fx);
                        int iy = (int)floorf(fy);
                        float tx = fx - ix;
                        float ty = fy - iy;

                        bilinear(planeRead(background, ix, iy), planeRead(background, ix + 1, iy), planeRead(background, ix, iy + 1), planeRead(background, ix + 1, iy + 1), tx, ty, planeAt(inherited, (int)x, (int)y));
                    }
                }

                sub.backdrop = inherited;
            }

            releasePlane(background);
        }

        memset(content.data, 0, sizeof(u16) * size * 4);
        runProgram(sub, l, l.programs.data() + op.first, op.count, content);

        // The device canvas and the taps around it, in layer pixels: the
        // part of the last result that is drawn.
        Box footprint = mappedBox(0, canvasBox);
        IntBox visible = pixelBox(Box{footprint.x0 - 2.f, footprint.y0 - 2.f, footprint.x1 + 2.f, footprint.y1 + 2.f}, content.box);
        IntBox region{0, 0, lw, lh};

        for (const Filter* filter = op.filter; filter; filter = filter->next) {
            applyFilter(sub, l, *filter, content, pool, filter->next ? content.box : visible);
            region = filter->region;
        }

        // A filter's result is zero outside its region, so only the device
        // pixels whose bilinear taps reach the region of the last one can draw.
        Box reach = mappedBox(1, Box{(float)region.x0 - 1.f, (float)region.y0 - 1.f, (float)region.x1 + 1.f, (float)region.y1 + 1.f});
        IntBox drawn{reach.x0 - 2.f > 0.f ? (int)(reach.x0 - 2.f) : 0, reach.y0 - 2.f > 0.f ? (int)(reach.y0 - 2.f) : 0, reach.x1 + 3.f < (float)r.width ? (int)(reach.x1 + 3.f) : (int)r.width, reach.y1 + 3.f < (float)r.height ? (int)(reach.y1 + 3.f) : (int)r.height};

        // A layer shifted by whole pixels is drawn row by row as it is: not
        // in perspective, its matrix passes all eight tests of a whole
        // shift. Else each device pixel blends the four layer pixels around
        // its centre.
        const Matrix& lm = op.layerMatrix;
        int tests = !maps + (lm.a == 1.f) + (lm.b == 0.f) + (lm.c == 0.f) + (lm.d == 1.f) + (lm.e == floorf(lm.e)) + (lm.f == floorf(lm.f)) + (fabsf(lm.e) < 16777216.f) + (fabsf(lm.f) < 16777216.f);
        // An empty box is all zero, as boxInside makes it.
        drawn = boxInside(drawn, IntBox{0, 0, (int)r.width, (int)r.height});

        int count = drawn.x1 - drawn.x0;
        size_t first = firstTarget(r);
        Vector<u16> sample;

        if (tests == 9) {
            int shiftX = (int)lm.e;
            int shiftY = (int)lm.f;
            int x0 = drawn.x0 > -shiftX ? drawn.x0 : -shiftX;
            int x1 = drawn.x1 < lw - shiftX ? drawn.x1 : lw - shiftX;

            for (int y = drawn.y0; y < drawn.y1; y++) {
                int iy = y + shiftY;

                if (iy < 0 || iy >= lh || x1 <= x0) {
                    continue;
                }

                overLayers(r, first, x0, y, content.data + ((size_t)iy * lw + (size_t)(x0 + shiftX)) * 4, x1 - x0);
            }

            return;
        }

        sample.zero((size_t)count * 4);

        // A tap of the layer, zero outside it.
        auto texel = [&](int x, int y) {
            return x < 0 || y < 0 || x >= lw || y >= lh ? zeroStored : content.data + ((size_t)y * lw + x) * 4;
        };

        for (int y = drawn.y0; y < drawn.y1; y++) {
            u16* out = sample.mutData();

            for (int x = drawn.x0; x < drawn.x1; x++, out += 4) {
                Point p{0.f, 0.f};
                bool front = mapped(0, Point{(float)x + 0.5f, (float)y + 0.5f}, p);
                float fx = p.x - 0.5f;
                float fy = p.y - 0.5f;
                float bx = floorf(fx);
                float by = floorf(fy);

                if (!front || bx < -1.f || by < -1.f || bx >= (float)lw || by >= (float)lh) {
                    memset(out, 0, sizeof(u16) * 4);
                    continue;
                }

                int ix = (int)bx;
                int iy = (int)by;

                bilinear(texel(ix, iy), texel(ix + 1, iy), texel(ix, iy + 1), texel(ix + 1, iy + 1), fx - bx, fy - by, out);
            }

            overLayers(r, first, drawn.x0, y, sample.data(), count);
        }
    }

    // The factor of each pixel of a group's row: its opacity times its clip
    // and mask, where it has them.
    static void factorRow(u16* factor, u32 opacity, const u16* clip, const u16* mask, size_t count) {
        for (size_t k = 0; k < count; k++) {
            factor[k] = (u16)opacity;
        }

        if (clip) {
            for (size_t k = 0; k < count; k++) {
                factor[k] = (u16)div65535((u32)factor[k] * clip[k]);
            }
        }

        if (mask) {
            for (size_t k = 0; k < count; k++) {
                factor[k] = (u16)div65535((u32)factor[k] * mask[k]);
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
                    finishPaint(*op.paint);
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
                    Plane layer{area ? (u16*)pool->allocate(sizeof(u16) * area * 4) : nullptr, box};

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

                    const u16* clip = begin->clipped ? clipBuffer(r, l, *begin, *pool, layer.box) : nullptr;
                    const u16* mask = begin->masked ? maskBuffer(r, l, *begin, *pool, layer.box) : nullptr;
                    int width = planeWidth(layer);
                    u32 opacity = toUnit16(begin->opacity);
                    // The layer is scaled by its opacity, clip and mask, a factor a pixel.
                    bool scaled = opacity < 65535 || clip || mask;
                    Vector<u16> factor;
                    Vector<u16> scaledRow;
                    Vector<u16> blended;

                    factor.zero((size_t)width);
                    scaledRow.zero((size_t)width * 4);
                    blended.zero((size_t)width * 4);

                    for (size_t level = firstTarget(r); level < r.layers.length(); level++) {
                        const Plane& parent = r.layers[level];
                        IntBox target = boxInside(box, parent.box);
                        int n = target.x1 - target.x0;

                        ensure(parent, r.dirty.mut(level), target);

                        for (int y = target.y0; y < target.y1; y++) {
                            size_t i = (size_t)(y - layer.box.y0) * width + (size_t)(target.x0 - layer.box.x0);
                            const u16* row = planeAt(layer, target.x0, y);
                            u16* out = planeAt(parent, target.x0, y);

                            if (scaled) {
                                factorRow(factor.mutData(), opacity, clip ? clip + i : nullptr, mask ? mask + i : nullptr, (size_t)n);
                                scaleSpan(scaledRow.mutData(), row, factor.data(), (size_t)n);
                                row = scaledRow.data();
                            }

                            if (begin->linear) {
                                blendLinear(r, begin->blend, out, row, (size_t)n);
                            } else if (!begin->blend) {
                                overSpan(out, row, (size_t)n);
                            } else {
                                blendSpan(begin->blend, blended.mutData(), row, out, (size_t)n);
                                memcpy(out, blended.data(), sizeof(u16) * 4 * (size_t)n);
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
        p.draws = false;
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
            childExtent(p, child, m, style, vp, Extent{true, true, false}, 0, box);
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

HeldPaints::~HeldPaints() {
    for (DevicePaint* paint : paints) {
        delete[] paint->tile;
        paint->tile = nullptr;
        delete[] paint->shrunk;
        paint->shrunk = nullptr;
    }
}

DecodedImage::~DecodedImage() {
    if (image) {
        svg_image_destroy(image);
    }
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
    u16* base = (u16*)scratch.allocate(sizeof(u16) * size * 4);

    memset(base, 0, sizeof(u16) * size * 4);

    {
        Lowering l;

        beginPass(l, doc, scratch, document->decode, document->fail, document->context);
        l.width = width;
        l.height = height;
        l.docWidth = document->width;
        l.docHeight = document->height;
        l.depth = 0;
        l.patternDepth = 0;
        l.draws = true;
        l.out = &l.ops;
        l.backgroundInputs = usesBackground(doc, doc.root);
        l.sized = nullptr;
        l.flattened = nullptr;
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

    // Bytes not premultiplied, by runs over the rows end to end: a run of
    // no alpha is none; an opaque run takes every channel times 65281, with
    // which it comes out rounded as c 255 / 65535 does, the alpha too; any
    // other pixel has each channel, clamped to its alpha, over it through
    // 255 2^24 / alpha, divided once for a run of one alpha.
    u32 last = 0;
    u32 inverse = 0;

    for (size_t p = 0; p < size;) {
        const u16* s = base + p * 4;
        u8* d = pixels + p * 4;
        u32 a = s[3];

        if (a == 0 || a == 65535) {
            size_t end = p + 1;

            // Four alphas a step while they all hold, then one.
            while (end + 4 <= size && ((base[end * 4 + 3] ^ a) | (base[end * 4 + 7] ^ a) | (base[end * 4 + 11] ^ a) | (base[end * 4 + 15] ^ a)) == 0) {
                end += 4;
            }

            while (end < size && base[end * 4 + 3] == a) {
                end++;
            }

            if (a == 0) {
                memset(d, 0, (end - p) * 4);
            } else {
                for (size_t i = p * 4; i < end * 4; i++) {
                    pixels[i] = (u8)((base[i] * 65281u + (1u << 23)) >> 24);
                }
            }

            p = end;
            continue;
        }

        if (a != last) {
            last = a;
            inverse = (255u << 24) / a;
        }

        for (int c = 0; c < 3; c++) {
            u32 v = s[c] < a ? s[c] : a;

            d[c] = (u8)((v * inverse + (1u << 23)) >> 24);
        }

        d[3] = (u8)((a * 65281u + (1u << 23)) >> 24);
        p++;
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
