"""transform-functions: the transform property and attribute of CSS
Transforms 1, each rect drawn in the expected drawing as the path of its
corners through the matrix it should have."""
from matrices import K, R, S, T, affine, chain, corners
from unitgen import Unit, rows

W = 122
unit = Unit("transform-functions", W, 0, """Each lime drawing fills its black
outline, and nothing red shows. A transform makes a coordinate system for
an element and its content within its parent's, through viewports too:
transform-origin moved, the functions left to right, moved back. The
origin is 0 0 for SVG, in lengths or the reference box's percentages and
keywords, one value centring the other, a third one along z; the box is
the viewport, of its viewBox's size at its user space's origin, unless fill-box or stroke-box, or
content-box and border-box, which are those, and a gradient's is its
bounding box. The property overrides the attribute; the attribute takes
white space and commas between its numbers and functions, may be empty,
takes rotate about a point and only SVG's six functions, of numbers in px
and deg. matrix(), translate() with y 0 if none, translateX/Y, scale() with
y as x if none, scaleX/Y, rotate() clockwise, skew() not skewX then skewY,
skewX/Y; a zero angle needs no unit, another does; a scale takes
percentages, a translation any length.""", root='font-size="8"')


def rect(x, y, w, h, m, attributes=""):
    """A lime rect with attributes, and its expected path through m."""
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime" {attributes}/>', corners(m, x, y, w, h)


def item(x, y, value):
    test, expected = value
    plain = f'<path d="{expected}"/>' if expected.startswith("M") else expected

    return f'<g transform="translate({x} {y})">{test}</g>', f'<g fill="lime" transform="translate({x} {y})">{plain}</g>', f'<g transform="translate({x} {y})">{plain}</g>'


def wrapped(before, after, x, y, w, h, m, attributes=""):
    test, expected = rect(x, y, w, h, m, attributes)

    return before + test + after, expected


stroked = '<rect x="7" y="5" width="6" height="6" fill="lime" stroke="lime" stroke-width="2"/>'
gradient = '<linearGradient id="gb" style="transform: translate(-25%)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="white"/></linearGradient>'
y = rows(unit, 2, [
    ("local", [wrapped('<g style="transform: translate(4px, 2px)">', "</g>", 2, 2, 8, 6, T(4, 2))]),
    ("origin-matrix", [rect(4, 4, 8, 6, chain(T(8, 7), R(30), T(-8, -7)), 'style="transform: rotate(30deg); transform-origin: 8px 7px"')]),
    ("cumulative", [wrapped('<g transform="translate(2 1)"><g style="transform: scale(1.5)">', "</g></g>", 2, 2, 6, 6, chain(T(2, 1), S(1.5)))]),
    ("ctm", [('<svg x="2" y="2" width="16" height="16" viewBox="0 0 8 8"><rect x="1" y="1" width="2" height="2" fill="lime" transform="translate(1 1)"/></svg>', corners(chain(T(2, 2), S(2), T(1, 1)), 1, 1, 2, 2))]),
    ("none", [rect(4, 4, 8, 8, chain(), 'transform="translate(100 0)" style="transform: none"')]),
    ("not-inherited", [wrapped('<g style="transform: translate(2px)">', "</g>", 2, 4, 8, 8, T(2, 0))]),
    ("percent", [rect(2, 2, 8, 8, T(4, 2), 'style="transform: translate(50%, 25%); transform-box: fill-box"'), rect(2, 2, 4, 3, T(8, 12), 'style="transform: translate(1em, .125in); font-size: 8px"')]),
    ("origin-initial", [rect(0, 0, 6, 4, chain(T(10, 4), R(90)), 'transform="translate(10 4)" style="transform: translate(10px, 4px) rotate(90deg)"')]),
    ("origin-percent", [rect(4, 4, 8, 6, chain(T(8, 10), R(-90), T(-8, -10)), 'style="transform-box: fill-box; transform-origin: 50% 100%; transform: rotate(-90deg)"')]),
    ("origin-one", [rect(4, 4, 8, 6, chain(T(4, 7), R(90), T(-4, -7)), 'style="transform-box: fill-box; transform-origin: left; transform: rotate(90deg)"'),
                    rect(4, 4, 8, 6, chain(T(8, 4), R(90), T(-8, -4)), 'style="transform-box: fill-box; transform-origin: top; transform: rotate(90deg)"')]),
    ("origin-two", [rect(4, 4, 8, 6, chain(T(6, 8), R(90), T(-6, -8)), 'style="transform-origin: 6px 8px; transform: rotate(90deg)"'),
                    rect(4, 4, 8, 6, chain(T(6, 8), R(90), T(-6, -8)), 'style="transform-origin: 6px 8px 5px; transform: rotate(90deg)"')]),
    ("origin-keywords", [rect(4, 4, 8, 6, chain(T(12, 10), R(90), T(-12, -10)), 'style="transform-box: fill-box; transform-origin: right bottom; transform: rotate(90deg)"'),
                         rect(4, 4, 8, 6, chain(T(4, 7), R(90), T(-4, -7)), 'style="transform-box: fill-box; transform-origin: center left; transform: rotate(90deg)"'),
                         rect(4, 4, 8, 6, chain(T(12, 4), R(90), T(-12, -4)), 'style="transform-box: fill-box; transform-origin: top right; transform: rotate(90deg)"')]),
    ("box-view", [rect(2, 2, 4, 4, T(W * .1, 0), 'style="transform: translate(10%)"'),
                  ('<svg width="20" height="20" viewBox="10 0 20 20" overflow="visible"><rect x="12" y="2" width="8" height="4" fill="lime" style="transform-box: view-box; transform-origin: 50% 50%; transform: rotate(90deg)"/></svg>',
                   corners(chain(T(-10, 0), T(10, 10), R(90), T(-10, -10)), 12, 2, 8, 4))]),
    ("box-stroke", [('<rect x="3" y="5" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" style="transform-box: stroke-box; transform: translate(50%)"/>', stroked),
                    ('<rect x="3" y="5" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" style="transform-box: border-box; transform: translate(50%)"/>', stroked),
                    ('<rect x="4" y="5" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" style="transform-box: content-box; transform: translate(50%)"/>', stroked)]),
    ("box-gradient", [(gradient + '<rect x="2" y="2" width="16" height="8" fill="url(#gb)"/>', '<rect x="2" y="2" width="16" height="8" fill="white"/><rect x="2" y="2" width="4" height="8"/>')]),
    ("attribute-overridden", [rect(4, 4, 6, 6, T(2, 0), 'transform="scale(2)" style="transform: translate(2px)"')]),
    ("attribute-separators", [rect(2, 2, 6, 6, T(1, 2), 'transform="translate(1,&#10;2)&#9;rotate(0)"'), rect(2, 2, 6, 6, chain(T(2, 1), S(1.5)), 'transform=" translate( 2 , 1 ) , scale (1.5) "'),
                              rect(2, 2, 6, 6, affine(1, 0, 0, 1, 3, 2), 'transform="matrix(1,0,0,1,3,2)"')]),
    ("attribute-no-comma", [rect(0, 0, 6, 6, chain(T(4, 2), R(10)), 'transform="translate(4,2)rotate(10)"')]),
    ("attribute-empty", [rect(4, 4, 8, 8, chain(), 'transform="  "'), rect(4, 4, 8, 8, chain(), 'transform=""')]),
    ("attribute-functions", [rect(4, 4, 8, 8, chain(), 'transform="translateX(5)"'), rect(4, 4, 8, 8, chain(), 'transform="scaleX(2)"'), rect(4, 4, 8, 8, chain(), 'transform="skew(10)"')]),
    ("attribute-units", [rect(4, 4, 8, 8, chain(), 'transform="translate(10%)"'), rect(4, 4, 8, 8, chain(), 'transform="translate(3px)"'), rect(4, 4, 8, 8, chain(), 'transform="rotate(10deg)"'),
                         rect(4, 4, 8, 6, chain(T(4, 4), R(30), T(-4, -4)), 'transform="translate(4 4) rotate(30) translate(-4 -4)"')]),
    ("attribute-rotate-center", [rect(4, 4, 6, 6, chain(T(7, 7), R(45), T(-7, -7)), 'transform="rotate(45 7 7)"'), rect(4, 4, 6, 6, chain(T(7, 7), R(45), T(-7, -7)), 'transform="rotate(45,7,7)"')]),
    ("matrix", [rect(2, 2, 8, 8, affine(1, .2, .3, 1, 2, 1), 'style="transform: matrix(1, .2, .3, 1, 2, 1)"')]),
    ("translate", [rect(2, 2, 8, 8, T(4, 0), 'style="transform: translate(4px)"'), rect(2, 2, 8, 8, T(4, 2), 'style="transform: translate(4px, 2px)"')]),
    ("translate-axis", [rect(2, 2, 8, 6, chain(T(3, 2)), 'style="transform: translateX(3px) translateY(2px)"')]),
    ("scale", [rect(2, 2, 8, 8, S(1.5), 'style="transform: scale(1.5)"'), rect(2, 2, 8, 8, S(1.5, .5), 'style="transform: scale(1.5, .5)"'), rect(2, 2, 8, 8, S(.8, 1), 'style="transform: scale(80%, 1)"')]),
    ("scale-axis", [rect(2, 2, 8, 6, S(.5, 1.5), 'style="transform: scaleX(.5) scaleY(1.5)"')]),
    ("rotate", [rect(4, 4, 8, 4, chain(T(8, 6), R(30), T(-8, -6)), 'style="transform-box: fill-box; transform-origin: center; transform: rotate(30deg)"')]),
    ("zero-angle", [rect(2, 2, 6, 6, T(2, 0), 'style="transform: rotate(0) translate(2px)"'), rect(2, 2, 6, 6, chain(), 'style="transform: rotate(10) translate(30px)"')]),
    ("skew", [rect(2, 2, 6, 6, K(20, 10), 'style="transform: skew(20deg, 10deg)"'), rect(2, 2, 6, 6, K(20), 'style="transform: skew(20deg)"')]),
    ("skew-axis", [rect(2, 2, 6, 6, chain(K(20), K(0, 10)), 'style="transform: skewX(20deg) skewY(10deg)"')]),
], 6, 20, 20, item)
unit.height = y + 1
unit.write()
