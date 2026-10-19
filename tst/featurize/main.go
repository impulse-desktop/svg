// featurize: tst/features.py and the cleaning of tst/import.py in Go, for
// tst/pick.py: many files at once, on every core.
//
// usage: featurize [-clean] [-jobs N] [-root REPO] < PATHS
//
// Reads a path a line and writes a JSON line a file, [path, digest, size,
// features]: with -clean the file as tst/import.py cleans it, a file it
// refuses left out, else the file as it is; the digest is the sha256 of its
// words joined by single spaces, comments left out, as tst/pick.py's. The features are
// tst/features.py's, which this follows rule for rule.
package main

import (
	"bufio"
	"bytes"
	"crypto/sha256"
	"encoding/base64"
	"encoding/hex"
	"encoding/json"
	"encoding/xml"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"sort"
	"strings"
	"sync"
)

// The cleaning of tst/import.py.

const maxBytes = 1 << 20

var (
	forbidden = regexp.MustCompile(`(?i)<(?:svg:)?(?:script|foreignObject|animate|animateMotion|animateTransform|animateColor|set|discard|tspan|textPath|tref|altGlyph|video|audio|iframe|canvas)\b|@(?:-[a-z]+-)?keyframes|<html\b|<body\b`)
	comment   = regexp.MustCompile(`(?s)<!--.*?-->`)
	svgRoot   = regexp.MustCompile(`(?i)<(?:svg:)?svg\b`)
	dataURI   = regexp.MustCompile(`(?i)data:([^,;"')\t\n\v\f\r ]*)((?:;[^,"')\t\n\v\f\r ]*)*),([^"')\t\n\v\f\r ]*)`)
	packing   = regexp.MustCompile(`[\t\n\v\f\r ]|&#1[03];`)
)

func isWord(c byte) bool {
	return c == '_' || (c >= '0' && c <= '9') || (c|0x20 >= 'a' && c|0x20 <= 'z')
}

func hasPrefixFold(s []byte, prefix string) bool {
	return len(s) >= len(prefix) && strings.EqualFold(string(s[:len(prefix)]), prefix)
}

// cutText removes text and flowRoot elements, as TEXT_ELEMENT of
// tst/import.py: one closing itself, or up to the first closing tag of its
// name.
func cutText(data []byte) []byte {
	var out []byte
	last := 0

	for i := 0; i < len(data); i++ {
		if data[i] != '<' {
			continue
		}

		j := i + 1

		if hasPrefixFold(data[j:], "svg:") {
			j += 4
		}

		name := ""

		for _, n := range []string{"text", "flowRoot"} {
			if hasPrefixFold(data[j:], n) && (j+len(n) >= len(data) || !isWord(data[j+len(n)])) {
				name = string(data[j : j+len(n)])
				break
			}
		}

		if name == "" {
			continue
		}

		k := j + len(name)
		end := -1

		// A tag closing itself has a slash before its first '>'.
		if gt := bytes.IndexByte(data[k:], '>'); gt > 0 && data[k+gt-1] == '/' {
			end = k + gt + 1
		}

		if end < 0 {
			for p := k; p < len(data); p++ {
				if data[p] != '<' || p+1 >= len(data) || data[p+1] != '/' {
					continue
				}

				q := p + 2

				if hasPrefixFold(data[q:], "svg:") {
					q += 4
				}

				if !hasPrefixFold(data[q:], name) {
					continue
				}

				q += len(name)

				for q < len(data) && isSpace(data[q]) {
					q++
				}

				if q < len(data) && data[q] == '>' {
					end = q + 1
					break
				}
			}
		}

		if end < 0 {
			continue
		}

		out = append(out, data[last:i]...)
		last = end
		i = end - 1
	}

	return append(out, data[last:]...)
}

func isSpace(c byte) bool {
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'
}

func b64decode(s []byte) ([]byte, bool) {
	var kept []byte

	for _, c := range s {
		if c|0x20 >= 'a' && c|0x20 <= 'z' || c >= '0' && c <= '9' || c == '+' || c == '/' {
			kept = append(kept, c)
		}
	}

	if len(kept)%4 == 1 {
		return nil, false
	}

	out, err := base64.RawStdEncoding.DecodeString(string(kept))

	return out, err == nil
}

func unquoteBytes(s []byte) []byte {
	var out []byte

	for i := 0; i < len(s); i++ {
		if s[i] == '%' && i+2 < len(s) {
			if b, err := hex.DecodeString(string(s[i+1 : i+3])); err == nil {
				out = append(out, b[0])
				i += 2
				continue
			}
		}

		out = append(out, s[i])
	}

	return out
}

func quoteBytes(s []byte) []byte {
	var out []byte

	for _, c := range s {
		if isWord(c) || c == '.' || c == '-' || c == '~' {
			out = append(out, c)
		} else {
			out = append(out, []byte(fmt.Sprintf("%%%02X", c))...)
		}
	}

	return out
}

func inside(data []byte) ([]byte, bool) {
	data = cutText(data)

	if forbidden.Match(data) {
		return nil, false
	}

	var out []byte
	last := 0

	for _, m := range dataURI.FindAllSubmatchIndex(data, -1) {
		whole := data[m[0]:m[1]]
		mime, params, payload := data[m[2]:m[3]], data[m[4]:m[5]], data[m[6]:m[7]]
		replacement := whole

		if bytes.Contains(bytes.ToLower(mime), []byte("svg")) {
			encoded := bytes.Contains(bytes.ToLower(params), []byte("base64"))
			var original []byte

			if encoded {
				decoded, ok := b64decode(packing.ReplaceAll(payload, nil))

				if !ok {
					return nil, false
				}

				original = decoded
			} else {
				original = unquoteBytes(payload)
			}

			inner, ok := inside(original)

			if !ok {
				return nil, false
			}

			if !bytes.Equal(inner, original) {
				var body []byte

				if encoded {
					body = []byte(base64.StdEncoding.EncodeToString(inner))
				} else {
					body = quoteBytes(inner)
				}

				replacement = append(append(append(append([]byte("data:"), mime...), params...), ','), body...)
			}
		}

		out = append(out, data[last:m[0]]...)
		out = append(out, replacement...)
		last = m[1]
	}

	return append(out, data[last:]...), true
}

func cleaned(data []byte) ([]byte, bool) {
	if len(data) > maxBytes || !svgRoot.Match(data) {
		return nil, false
	}

	return inside(data)
}

func digest(data []byte) string {
	h := sha256.Sum256(bytes.Join(bytes.FieldsFunc(comment.ReplaceAll(data, nil), func(r rune) bool { return r < 128 && isSpace(byte(r)) }), []byte(" ")))

	return hex.EncodeToString(h[:])
}

// The vocabulary of tst/spec_coverage.py.

var (
	tagRe         = regexp.MustCompile(`<([A-Za-z_][\w:.-]*)((?:\s+[^\s=/>]+\s*=\s*(?:"[^"]*"|'[^']*'))*)\s*/?>`)
	attributeRe   = regexp.MustCompile(`([^\s=/>]+)\s*=\s*(?:"([^"]*)"|'([^']*)')`)
	declarationRe = regexp.MustCompile(`([A-Za-z-]+)\s*:\s*([^;{}]*)`)
	commentRe     = regexp.MustCompile(`(?s)<!--.*?-->`)
	styleBlockRe  = regexp.MustCompile(`(?s)<style[^>]*>(.*?)</style>`)
	ruleRe        = regexp.MustCompile(`\{([^{}]*)\}`)
	embeddedRe    = regexp.MustCompile(`(?s)^\s*data:image/svg\+xml([^,]*),(.*)`)
)

func local(name string) string {
	if strings.HasPrefix(name, "xlink:") || strings.HasPrefix(name, "xml:") {
		return name
	}

	return name[strings.LastIndex(name, ":")+1:]
}

func wordChar(c byte) bool {
	return isWord(c) || c >= 0x80
}

// words follows WORD, (?<![\w.#-])([A-Za-z][\w-]*)(\()?, a call marked by
// its parenthesis.
func words(value string) map[string]bool {
	found := map[string]bool{}

	for i := 0; i < len(value); i++ {
		c := value[i]

		if !(c|0x20 >= 'a' && c|0x20 <= 'z') {
			continue
		}

		if i > 0 {
			p := value[i-1]

			if wordChar(p) || p == '.' || p == '#' || p == '-' {
				continue
			}
		}

		j := i + 1

		for j < len(value) && (wordChar(value[j]) || value[j] == '-') {
			j++
		}

		word := value[i:j]

		if j < len(value) && value[j] == '(' {
			word += "()"
		}

		found[word] = true
		i = j - 1
	}

	return found
}

func embedded(value string) (string, bool) {
	m := embeddedRe.FindStringSubmatch(value)

	if m == nil {
		return "", false
	}

	data := strings.SplitN(m[2], "#", 2)[0]

	if strings.HasSuffix(m[1], ";base64") {
		decoded, ok := b64decode([]byte(data))

		if !ok {
			return "", false
		}

		return strings.ToValidUTF8(string(decoded), "�"), true
	}

	return strings.ToValidUTF8(string(unquoteBytes([]byte(data))), "�"), true
}

func usesText(source string, properties map[string]bool, found map[string]bool, depth int) {
	source = commentRe.ReplaceAllString(source, " ")

	declare := func(name, value string) {
		name = strings.ToLower(strings.TrimSpace(name))
		found["property "+name] = true

		for word := range words(value) {
			found["keyword "+name+":"+strings.ToLower(word)] = true
		}
	}

	for _, m := range tagRe.FindAllStringSubmatch(source, -1) {
		element := local(m[1])
		found["element "+element] = true

		for _, a := range attributeRe.FindAllStringSubmatch(m[2], -1) {
			value := a[2]

			if value == "" {
				value = a[3]
			}

			name := local(a[1])
			found["attribute "+element+"/"+name] = true
			found["attribute */"+name] = true

			for word := range words(value) {
				found["value "+element+"/"+name+"="+word] = true
				found["value */"+name+"="+word] = true
			}

			if inner, ok := embedded(value); ok && depth < 8 {
				usesText(inner, properties, found, depth+1)
			}

			if name == "style" {
				for _, d := range declarationRe.FindAllStringSubmatch(value, -1) {
					declare(d[1], d[2])
				}
			} else if properties[name] {
				declare(name, value)
			}
		}
	}

	for _, block := range styleBlockRe.FindAllStringSubmatch(source, -1) {
		text := strings.ReplaceAll(strings.ReplaceAll(block[1], "<![CDATA[", " "), "]]>", " ")

		for _, rule := range ruleRe.FindAllStringSubmatch(text, -1) {
			for _, d := range declarationRe.FindAllStringSubmatch(rule[1], -1) {
				declare(d[1], d[2])
			}
		}
	}
}

// The inventory and the exclusions.

type inventory struct {
	Categories map[string]map[string][]string `json:"categories"`
	Elements   map[string]struct {
		Attributes map[string][]string `json:"attributes"`
	} `json:"elements"`
	Properties map[string][]string `json:"properties"`
}

var (
	properties = map[string]bool{}
	elements   = map[string]bool{}
	attributes = map[string]bool{}
	vocabulary = map[string]bool{}
	excluded   []*regexp.Regexp
)

func fnmatch(pattern string) *regexp.Regexp {
	var b strings.Builder

	b.WriteString(`(?s)^`)

	for i := 0; i < len(pattern); i++ {
		switch c := pattern[i]; c {
		case '*':
			b.WriteString(`.*`)
		case '?':
			b.WriteString(`.`)
		case '[':
			j := strings.IndexByte(pattern[i+1:], ']')

			if j < 0 {
				b.WriteString(`\[`)
				continue
			}

			set := pattern[i+1 : i+1+j]

			if strings.HasPrefix(set, "!") {
				set = "^" + set[1:]
			}

			b.WriteString("[" + strings.ReplaceAll(set, `\`, `\\`) + "]")
			i += j + 1
		default:
			b.WriteString(regexp.QuoteMeta(string(c)))
		}
	}

	b.WriteString(`$`)

	return regexp.MustCompile(b.String())
}

func load(root string) {
	raw, err := os.ReadFile(filepath.Join(root, "tst", "spec", "inventory.json"))

	if err != nil {
		panic(err)
	}

	var inv inventory

	if err := json.Unmarshal(raw, &inv); err != nil {
		panic(err)
	}

	for p, keywords := range inv.Properties {
		properties[p] = true
		attributes[p] = true
		vocabulary["property "+p] = true

		for _, k := range keywords {
			vocabulary["keyword "+p+":"+strings.ToLower(k)] = true
		}
	}

	for _, category := range inv.Categories {
		for a, keywords := range category {
			vocabulary["attribute */"+a] = true

			for _, k := range keywords {
				vocabulary["value */"+a+"="+k] = true
			}
		}
	}

	for e, entry := range inv.Elements {
		elements[e] = true
		vocabulary["element "+e] = true

		for a, keywords := range entry.Attributes {
			attributes[a] = true
			vocabulary["attribute "+e+"/"+a] = true

			for _, k := range keywords {
				vocabulary["value */"+a+"="+k] = true
			}
		}
	}

	text, err := os.ReadFile(filepath.Join(root, "tst", "spec", "excluded.txt"))

	if err != nil {
		panic(err)
	}

	for _, line := range strings.Split(string(text), "\n") {
		line = strings.TrimSpace(line)

		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}

		key := line

		if i := strings.Index(line, ": "); i >= 0 {
			key = line[:i]
		}

		excluded = append(excluded, fnmatch(strings.TrimSpace(key)))
	}
}

// The structure of tst/features.py.

const (
	svgNS   = "http://www.w3.org/2000/svg"
	xlinkNS = "http://www.w3.org/1999/xlink"
)

var (
	urlRe       = regexp.MustCompile(`url\(\s*['"]?#([^'")\s]+)`)
	numberRe    = regexp.MustCompile(`[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?`)
	lengthRe    = regexp.MustCompile(`^\s*([-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?)\s*([a-zA-Z%]*)\s*$`)
	functionRe  = regexp.MustCompile(`([A-Za-z][\w-]*)\s*\(`)
	schemeRe    = regexp.MustCompile(`^([A-Za-z][\w+.-]+):`)
	dotRe       = regexp.MustCompile(`^[-+]?\.`)
	pathCommand = regexp.MustCompile(`[MmLlHhVvCcSsQqTtAaZz]`)
	cssComment  = regexp.MustCompile(`(?s)/\*.*?\*/`)
	cssAt       = regexp.MustCompile(`@([\w-]+)`)
	cssRule     = regexp.MustCompile(`([^{}@;]+)\{`)
	cssPseudo   = regexp.MustCompile(`::?([\w-]+)`)
	cssDesc     = regexp.MustCompile(`[\w\])]\s+[\w.#\[*:]`)
	cssKinds    = []struct {
		name string
		re   *regexp.Regexp
	}{{"class", regexp.MustCompile(`\.[\w-]`)}, {"id", regexp.MustCompile(`#[\w-]`)}, {"attribute", regexp.MustCompile(`\[`)}, {"child", regexp.MustCompile(`>`)},
		{"next", regexp.MustCompile(`\+`)}, {"sibling", regexp.MustCompile(`~`)}, {"universal", regexp.MustCompile(`\*`)}}
	importantRe = regexp.MustCompile(`!important`)
)

var (
	lengths    = set("x y width height cx cy r rx ry x1 y1 x2 y2 fx fy fr dx dy stroke-width stroke-dashoffset font-size refX refY markerWidth markerHeight startOffset")
	sizes      = set("width height r rx ry markerWidth markerHeight")
	colors     = set("fill stroke stop-color flood-color lighting-color color")
	transforms = set("transform gradientTransform patternTransform")
	referring  = set("fill stroke clip-path mask filter marker-start marker-mid marker-end marker")
	units      = unitSet()
	tfunctions = set("matrix matrix3d translate translatex translatey translatez translate3d scale scalex scaley scalez scale3d rotate rotatex rotatey rotatez rotate3d skew skewx skewy perspective")
	schemes    = set("data http https file")
	colorFns   = set("rgb rgba hsl hsla hwb lab lch oklab oklch color color-mix var icc-color device-cmyk light-dark")
	inputs     = set("SourceGraphic SourceAlpha BackgroundImage BackgroundAlpha FillPaint StrokePaint")
)

func set(words string) map[string]bool {
	m := map[string]bool{}

	for _, w := range strings.Fields(words) {
		m[w] = true
	}

	return m
}

func unitSet() map[string]bool {
	m := set("number % px cm mm q in pt pc em ex ch rem cap ic lh rlh rcap rch rex ric")

	for _, p := range []string{"", "s", "l", "d"} {
		for _, a := range []string{"w", "h", "i", "b", "min", "max"} {
			m[p+"v"+a] = true
		}
	}

	for _, a := range []string{"w", "h", "i", "b", "min", "max"} {
		m["cq"+a] = true
	}

	return m
}

type node struct {
	raw      xml.Name
	name     xml.Name
	attrs    []xml.Attr
	parent   *node
	children []*node
	text     strings.Builder
	closed   bool
}

func (n *node) get(space, local string) (string, bool) {
	for _, a := range n.attrs {
		if a.Name.Space == space && a.Name.Local == local {
			return a.Value, true
		}
	}

	return "", false
}

// svgName is an element's name in the SVG namespace, or "".
func svgName(n *node) string {
	if n != nil && n.name.Space == svgNS {
		return n.name.Local
	}

	return ""
}

var (
	doctypeRe = regexp.MustCompile(`(?s)<!DOCTYPE[^\[>]*\[(.*?)\]\s*>`)
	entityRe  = regexp.MustCompile(`<!ENTITY\s+([A-Za-z_][\w.-]*)\s+(?:"([^"]*)"|'([^']*)')\s*>`)
)

// expandEntities puts the general entities of the internal subset in place,
// as expat expands them, markup and all.
func expandEntities(source string) string {
	m := doctypeRe.FindStringSubmatchIndex(source)

	if m == nil {
		return source
	}

	values := map[string]string{}

	for _, e := range entityRe.FindAllStringSubmatch(source[m[2]:m[3]], -1) {
		values[e[1]] = e[2] + e[3]
	}

	body := source[m[1]:]

	for pass := 0; pass < 4 && len(values) > 0 && strings.Contains(body, "&"); pass++ {
		changed := false

		for name, value := range values {
			if strings.Contains(body, "&"+name+";") {
				body = strings.ReplaceAll(body, "&"+name+";", value)
				changed = true
			}
		}

		if !changed {
			break
		}
	}

	return source[:m[0]] + body
}

// parse reads the document as ElementTree does, nil where it fails: an
// unbound prefix, a mismatched end tag, a second root or text past it are
// errors, as in expat.
func parse(source string) *node {
	source = expandEntities(strings.TrimPrefix(source, "\uFEFF"))
	d := xml.NewDecoder(strings.NewReader(source))
	d.Strict = true
	d.CharsetReader = func(_ string, r io.Reader) (io.Reader, error) { return r, nil }

	var root, current *node
	var scopes []map[string]string

	resolve := func(prefix string, element bool) (string, bool) {
		if prefix == "xml" {
			return "http://www.w3.org/XML/1998/namespace", true
		}

		if prefix == "" && !element {
			return "", true
		}

		for i := len(scopes) - 1; i >= 0; i-- {
			if uri, ok := scopes[i][prefix]; ok {
				return uri, true
			}
		}

		return "", prefix == ""
	}

	for {
		token, err := d.RawToken()

		if err == io.EOF {
			break
		}

		if err != nil {
			return nil
		}

		switch t := token.(type) {
		case xml.StartElement:
			if root != nil && current == nil {
				return nil
			}

			scope := map[string]string{}

			for _, a := range t.Attr {
				if a.Name.Space == "xmlns" {
					scope[a.Name.Local] = a.Value
				} else if a.Name.Space == "" && a.Name.Local == "xmlns" {
					scope[""] = a.Value
				}
			}

			scopes = append(scopes, scope)
			space, ok := resolve(t.Name.Space, true)

			if !ok {
				return nil
			}

			n := &node{name: xml.Name{Space: space, Local: t.Name.Local}, parent: current}
			n.raw = t.Name

			for _, a := range t.Attr {
				if a.Name.Space == "xmlns" || (a.Name.Space == "" && a.Name.Local == "xmlns") {
					continue
				}

				space, ok := resolve(a.Name.Space, false)

				if !ok {
					return nil
				}

				n.attrs = append(n.attrs, xml.Attr{Name: xml.Name{Space: space, Local: a.Name.Local}, Value: a.Value})
			}

			if current == nil {
				root = n
			} else {
				current.children = append(current.children, n)
				current.closed = true
			}

			current = n
		case xml.EndElement:
			if current == nil || t.Name != current.raw {
				return nil
			}

			scopes = scopes[:len(scopes)-1]
			current = current.parent
		case xml.CharData:
			if current != nil && !current.closed {
				current.text.Write(t)
			} else if current == nil && len(bytes.TrimSpace(t)) > 0 {
				return nil
			}
		}
	}

	if root == nil || current != nil {
		return nil
	}

	return root
}

func band(n int, bounds []int) string {
	for _, b := range bounds {
		if n <= b {
			return fmt.Sprintf("<=%d", b)
		}
	}

	return fmt.Sprintf(">%d", bounds[len(bounds)-1])
}

func colour(value string) string {
	v := strings.ToLower(strings.TrimSpace(value))

	if strings.HasPrefix(v, "#") {
		switch len(v) - 1 {
		case 3, 4, 6, 8:
			return fmt.Sprintf("hex%d", len(v)-1)
		}

		return "hex-bad"
	}

	if strings.HasPrefix(v, "url(") {
		if i := strings.Index(v, ")"); i >= 0 && strings.TrimSpace(v[i+1:]) != "" {
			return "url-fallback"
		}

		return "url"
	}

	if m := functionRe.FindStringSubmatchIndex(v); m != nil && m[0] == 0 {
		if colorFns[v[m[2]:m[3]]] {
			return v[m[2]:m[3]]
		}

		return "function-other"
	}

	switch v {
	case "none", "currentcolor", "transparent", "inherit", "context-fill", "context-stroke":
		return v
	}

	return "keyword"
}

func css(text string, found map[string]bool) {
	text = cssComment.ReplaceAllString(strings.ReplaceAll(strings.ReplaceAll(text, "<![CDATA[", " "), "]]>", " "), " ")

	for _, m := range cssAt.FindAllStringSubmatch(text, -1) {
		found["css @"+strings.ToLower(m[1])] = true
	}

	if strings.Contains(text, "!important") {
		found["css !important"] = true
	}

	for _, m := range cssRule.FindAllStringSubmatch(text, -1) {
		for _, selector := range strings.Split(m[1], ",") {
			s := strings.TrimSpace(selector)

			if s == "" {
				continue
			}

			for _, p := range cssPseudo.FindAllStringSubmatch(s, -1) {
				found["css :"+strings.ToLower(p[1])] = true
			}

			for _, k := range cssKinds {
				if k.re.MatchString(s) {
					found["css "+k.name] = true
				}
			}

			if cssDesc.MatchString(s) {
				found["css descendant"] = true
			}
		}
	}
}

func declarations(n *node) [][2]string {
	var found [][2]string
	index := map[string]int{}

	put := func(name, value string) {
		if i, ok := index[name]; ok {
			found[i][1] = value
			return
		}

		index[name] = len(found)
		found = append(found, [2]string{name, value})
	}

	for _, a := range n.attrs {
		if a.Name.Space == "" && properties[a.Name.Local] {
			put(a.Name.Local, a.Value)
		}
	}

	style, _ := n.get("", "style")

	for _, d := range declarationRe.FindAllStringSubmatch(style, -1) {
		put(strings.ToLower(strings.TrimSpace(d[1])), strings.TrimSpace(d[2]))
	}

	return found
}

func structure(source string, found map[string]bool) {
	root := parse(source)

	if root == nil {
		return
	}

	var all []*node
	var walk func(*node)

	walk = func(n *node) {
		all = append(all, n)

		for _, c := range n.children {
			walk(c)
		}
	}
	walk(root)

	ids := map[string]*node{}

	for _, n := range all {
		if id, ok := n.get("", "id"); ok && id != "" {
			if _, seen := ids[id]; !seen {
				ids[id] = n
			}
		}
	}

	deepest := 0

	for _, e := range all {
		tag := svgName(e)

		if !elements[tag] {
			continue
		}

		if e.parent != nil && elements[svgName(e.parent)] && tag != "desc" && tag != "title" && tag != "metadata" {
			found["nest "+svgName(e.parent)+">"+tag] = true
		}

		depth := 0

		for p := e.parent; p != nil; p = p.parent {
			depth++
		}

		if depth > deepest {
			deepest = depth
		}

		for _, d := range declarations(e) {
			prop, value := d[0], d[1]

			for _, m := range urlRe.FindAllStringSubmatch(value, -1) {
				if t, ok := ids[m[1]]; ok && referring[prop] && elements[svgName(t)] {
					found["ref "+prop+">"+svgName(t)] = true
				}
			}

			if colors[prop] {
				found["color "+prop+":"+colour(value)] = true
			}
		}

		href, ok := e.get("", "href")

		if !ok || href == "" {
			href, ok = e.get(xlinkNS, "href")
		}

		if ok && href != "" {
			if strings.HasPrefix(href, "#") && ids[href[1:]] != nil && elements[svgName(ids[href[1:]])] {
				target := svgName(ids[href[1:]])
				found["ref href:"+tag+">"+target] = true

				switch tag {
				case "linearGradient", "radialGradient", "pattern", "use":
					found["chain "+tag+">"+target] = true
				}
			} else {
				kind := "relative"

				if m := schemeRe.FindStringSubmatch(href); m != nil {
					if schemes[strings.ToLower(m[1])] {
						kind = "scheme-" + strings.ToLower(m[1])
					} else {
						kind = "scheme-other"
					}
				} else if strings.Contains(href, "#") {
					kind = "fragment"
				}

				found["ref href:"+tag+">"+kind] = true
			}
		}

		for _, a := range e.attrs {
			attr, value := a.Name.Local, a.Value

			if lengths[attr] {
				if m := lengthRe.FindStringSubmatch(value); m != nil {
					unit := strings.ToLower(m[2])

					if unit == "" {
						unit = "number"
					}

					if !units[unit] {
						unit = "bad"
					}

					found["len "+attr+":"+unit] = true

					if sizes[attr] && strings.HasPrefix(m[1], "-") {
						found["num "+attr+":neg"] = true
					}
				}
			}

			if attributes[attr] {
				for _, number := range numberRe.FindAllString(value, -1) {
					if strings.ContainsAny(number, "eE") {
						found["num "+attr+":e"] = true
					}

					if dotRe.MatchString(number) {
						found["num "+attr+":dot"] = true
					}
				}
			}

			if transforms[attr] {
				for _, m := range functionRe.FindAllStringSubmatch(value, -1) {
					f := strings.ToLower(m[1])

					if !tfunctions[f] {
						f = "bad"
					}

					found["tf "+attr+":"+f] = true
				}
			}
		}

		if tag == "path" {
			d, _ := e.get("", "d")

			for _, c := range pathCommand.FindAllString(d, -1) {
				found["path "+c] = true
			}
		}

		if strings.HasPrefix(tag, "fe") {
			for _, attr := range []string{"in", "in2"} {
				if v, ok := e.get("", attr); ok && v != "" {
					if !inputs[v] {
						v = "result"
					}

					found["in "+tag+":"+v] = true
				}
			}
		}

		if tag == "filter" {
			count := 0

			for _, c := range e.children {
				if strings.HasPrefix(svgName(c), "fe") {
					count++
				}
			}

			found["prims "+band(count, []int{1, 3, 7})] = true
		}

		if tag == "style" {
			css(e.text.String(), found)
		}

		if style, ok := e.get("", "style"); ok && style != "" && importantRe.MatchString(style) {
			found["css !important"] = true
		}
	}

	found["depth "+band(deepest, []int{5, 10, 20})] = true

	if svgName(root) == "svg" {
		_, hasBox := root.get("", "viewBox")

		if hasBox {
			found["root viewBox:yes"] = true
		} else {
			found["root viewBox:no"] = true
		}

		for _, side := range []string{"width", "height"} {
			v, ok := root.get("", side)

			if !ok || v == "" {
				found["root size:none"] = true
				continue
			}

			m := lengthRe.FindStringSubmatch(v)

			switch {
			case m == nil:
				found["root size:none"] = true
			case m[2] == "":
				found["root size:number"] = true
			default:
				found["root size:"+strings.ToLower(m[2])] = true
			}
		}

		if par, ok := root.get("", "preserveAspectRatio"); ok && strings.TrimSpace(par) != "" {
			found["root aspect:"+strings.Fields(par)[0]] = true
		}
	}
}

func features(data []byte) []string {
	source := strings.ToValidUTF8(string(data), "�")
	words := map[string]bool{}

	usesText(source, properties, words, 0)

	found := map[string]bool{}

	for w := range words {
		if !vocabulary[w] {
			continue
		}

		skip := false

		for _, e := range excluded {
			if e.MatchString(w) {
				skip = true
				break
			}
		}

		if !skip {
			found[w] = true
		}
	}

	structure(source, found)

	keys := make([]string, 0, len(found))

	for k := range found {
		keys = append(keys, k)
	}

	sort.Strings(keys)

	return keys
}

func main() {
	clean := flag.Bool("clean", false, "clean each file as tst/import.py does, leaving out what it refuses")
	jobs := flag.Int("jobs", 0, "workers, the cores by default")
	root := flag.String("root", ".", "the repository")
	flag.Parse()

	if *jobs <= 0 {
		*jobs = 2 * runtime.NumCPU()
	}

	load(*root)

	paths := make(chan string, 1024)
	out := bufio.NewWriterSize(os.Stdout, 1<<20)
	var lock sync.Mutex
	var wg sync.WaitGroup

	for k := 0; k < *jobs; k++ {
		wg.Add(1)

		go func() {
			defer wg.Done()

			for path := range paths {
				data, err := os.ReadFile(path)

				if err != nil {
					continue
				}

				if *clean {
					var ok bool

					if data, ok = cleaned(data); !ok {
						continue
					}
				}

				line, _ := json.Marshal([]any{path, digest(data), len(data), features(data)})

				lock.Lock()
				out.Write(line)
				out.WriteByte('\n')
				lock.Unlock()
			}
		}()
	}

	scanner := bufio.NewScanner(os.Stdin)
	scanner.Buffer(make([]byte, 1<<20), 1<<20)

	for scanner.Scan() {
		if line := scanner.Text(); line != "" {
			paths <- line
		}
	}

	close(paths)
	wg.Wait()
	out.Flush()
}
