"""transform-3d-contexts: 3D rendering contexts, transform-style, the
perspective and perspective-origin properties and transform-origin's z, of
CSS Transforms 2. The expected drawing draws each plane as the plain path of
its corners projected, the pieces of crossing planes as the polygons each
side of the line they cross on, from the back; with no perspective a plane
is drawn on the device's own pixels, so its edges are a path's, and the
crossings are put on whole pixels."""
from matrices import P, R3, T, chain, corners, project, visible
from unitgen import Unit

W, H = 130, 266
unit = Unit("transform-3d-contexts", W, H, """Each drawing matches its plain
twin in the expected drawing. In a 3D rendering context, a g with
transform-style preserve-3d, 3D transformed elements are planes that cross
and hide one another by their z, not their order, and planes in one plane
keep their order; a g with preserve-3d inside one extends it, a flat one
flattens what it holds into its own plane. backface-visibility goes by the
matrix accumulated through the context, for an element of no plane of its
own the plane's; opacity, isolation, filter, clip-path, clip, mask,
mask-border, mix-blend-mode and a viewport's hidden overflow flatten a
preserve-3d, a mask-image of none does not; a clipPath, mask or pattern
flattens what it holds; vector-effect does nothing in a context, of a
shape too. Only what of a plane is in front of the eye draws. The perspective property puts its children, not deeper ones, in
perspective about perspective-origin, the reference box's centre unless
given, of the box transform-box names; none puts none, below a px is a px;
a value that is none of these is dropped, and attributes of their names do
nothing. transform-origin's third value
moves the origin along z.""")

CTX = "transform-style: preserve-3d"
LIME, NAVY, RED = "lime", "navy", "red"


def rect(fill, style, x=0, y=0, w=20, h=12):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}" style="{style}"/>'


def turned(degrees):
    return f"transform-origin: 10px 6px; transform: rotateY({degrees}deg)"


def path(m, fill, x=0, y=0, w=20, h=12):
    return f'<path fill="{fill}" d="{corners(m, x, y, w, h)}"/>'


def about(m, ox=10, oy=6, oz=0):
    return chain(T(ox, oy, oz), m, T(-ox, -oy, -oz))


def polygon(points, fill):
    return f'<path fill="{fill}" d="M' + " ".join("%.4f %.4f" % p for p in points) + 'Z"/>'


def side(m, keep, fill, x=0, y=0, w=20, h=12, line=10):
    """The part of a rect through m on one side of the vertical line x =
    line on the device: keep < 0 its left, > 0 its right."""
    points = [project(m, a, b) for a, b in [(x, y), (x + w, y), (x + w, y + h), (x, y + h)]]
    kept = []

    for i, p in enumerate(points):
        q = points[(i + 1) % 4]
        a, b = (p[0] - line) * keep, (q[0] - line) * keep

        if a >= 0:
            kept.append(p)

        if (a >= 0) != (b >= 0):
            t = a / (a - b)
            kept.append((p[0] + (q[0] - p[0]) * t, p[1] + (q[1] - p[1]) * t))

    return polygon(kept, fill)


A45, B45 = about(R3(0, 1, 0, 45)), about(R3(0, 1, 0, -45))


def narrow(fill, style):
    """The navy plane narrower, its edges inside the lime one's and never on
    them: edges of planes that meet are each antialiased, a pixel taking
    both."""
    return rect(fill, style, x=3, w=14)


# The box a g's perspective-origin is of, where transform-box names its
# fill or stroke box, is this unturned rect's, which no fill draws.
BOX = '<rect width="20" height="12" fill="none"/>'
cells = [
    # Turned either way about their shared middle, the two cross on its
    # vertical line: each is in front on the side it turns towards the eye.
    ("intersect", f'<g style="{CTX}">{rect(LIME, turned(45))}{narrow(NAVY, turned(-45))}</g>', path(A45, LIME) + side(B45, 1, NAVY, x=3, w=14)),
    ("order-by-z", f'<g style="{CTX}">{rect(LIME, "transform: translateZ(10px)")}{rect(RED, "transform: translateZ(-10px)")}</g>', rect(LIME, "")),
    ("coplanar-order", f'<g style="{CTX}">{rect(NAVY, turned(45))}{rect(LIME, turned(45))}</g>', path(A45, NAVY) + path(A45, LIME)),
    # Turned 45 degrees, the plane's z passes 4 where it shows at x = 6: left
    # of that it stands before the one moved 4 towards the eye.
    ("extends", f'<g style="{CTX}"><g style="{CTX}">{rect(LIME, turned(45))}</g>{rect(NAVY, "transform: translateZ(4px)")}</g>',
     rect(NAVY, "") + side(A45, -1, LIME, line=6)),
    ("flat-child", f'<g style="{CTX}"><g style="transform-style: flat">{rect(LIME, turned(45))}</g>{rect(NAVY, "transform: translateZ(4px)")}</g>', rect(NAVY, "")),
    ("backface-accumulated", f'<g style="{CTX}; {turned(180)}">{rect(LIME, turned(180) + "; backface-visibility: hidden")}</g>', rect(LIME, "")),
    ("backface-hidden", rect(LIME, "") + f'<g style="{CTX}">{rect(RED, turned(180) + "; backface-visibility: hidden")}</g>', rect(LIME, "")),
    ("grouping-opacity", f'<g style="{CTX}; opacity: .5">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>',
     f'<g opacity=".5">{rect(NAVY, "")}</g>'),
    ("grouping-isolation", f'<g style="{CTX}; isolation: isolate">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("grouping-filter", f'<g style="{CTX}; filter: opacity(1)">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("grouping-clip-path", f'<g style="{CTX}; clip-path: inset(0)">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("grouping-clip-path-url", f'<clipPath id="whole"><rect width="20" height="12"/></clipPath><g style="{CTX}; clip-path: url(#whole)">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("grouping-clip", f'<svg width="20" height="12" overflow="visible" style="{CTX}; clip: rect(0, 20px, 12px, 0)">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</svg>', rect(NAVY, "")),
    ("grouping-mask", f'<mask id="all" maskUnits="userSpaceOnUse" x="0" y="0" width="20" height="12"><rect width="20" height="12" fill="white"/></mask><g style="{CTX}; mask: url(#all)">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("grouping-mask-border", f'<g style="{CTX}; mask-border-source: linear-gradient(white, white); mask-border-slice: 0 fill; mask-border-width: 0">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>',
     rect(NAVY, "mask-border-source: linear-gradient(white, white); mask-border-slice: 0 fill; mask-border-width: 0")),
    ("grouping-blend", f'<g style="{CTX}; mix-blend-mode: multiply">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(NAVY, "")),
    ("mask-none", f'<g style="{CTX}; mask-image: none; mask-border-source: none">{rect(LIME, "transform: translateZ(10px)")}{rect(RED, "transform: translateZ(-10px)")}</g>', rect(LIME, "")),
    ("viewport-hidden", f'<svg width="20" height="12" style="{CTX}">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</svg>', rect(NAVY, "")),
    ("viewport-visible", f'<svg width="20" height="12" overflow="visible" style="{CTX}">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</svg>', rect(LIME, "")),
    # In a context vector-effect does nothing: the stroke scales with its rect.
    ("vector-effect", f'<g style="{CTX}"><rect x="1" y="1" width="8" height="4" fill="none" stroke="navy" vector-effect="non-scaling-stroke" transform="scale(2)"/></g>',
     '<rect x="2" y="2" width="16" height="8" fill="none" stroke="navy" stroke-width="2"/>'),
    # The origin 10 along z: turned 60 degrees, the rect moves 10 sin 60 left.
    ("origin-z", rect(LIME, "transform-origin: 20px 6px 10px; transform: rotateY(60deg)", x=10), path(about(R3(0, 1, 0, 60), ox=20, oz=10), LIME, x=10)),
    ("perspective-property", f'<g style="perspective: 40px; perspective-origin: 10px 6px; transform-box: stroke-box">{BOX}{rect(LIME, turned(40))}</g>', path(chain(about(P(40)), about(R3(0, 1, 0, 40))), LIME)),
    ("perspective-origin-default", f'<g style="perspective: 40px; transform-box: content-box">{rect(LIME, turned(40))}</g>', path(chain(about(P(40)), about(R3(0, 1, 0, 40))), LIME)),
    # An empty g has no box to take perspective-origin of: the viewport is.
    ("perspective-none", f'<g style="perspective: none">{rect(LIME, turned(40))}</g><g style="perspective: 10px; transform-box: border-box"/>', path(about(R3(0, 1, 0, 40)), LIME)),
    ("perspective-below-px", f'<g style="perspective: 0; perspective-origin: 0 0">{rect(LIME, "transform: translateZ(-1px)")}</g>', path(chain(P(1), T(0, 0, -1)), LIME)),
    ("perspective-children-only", f'<g style="perspective: 40px; perspective-origin: 10px 6px"><g style="perspective: initial; perspective-origin: inherit; transform-style: initial">{rect(LIME, turned(40))}</g></g>', path(about(R3(0, 1, 0, 40)), LIME)),
    ("perspective-origin-keywords", f'<g style="perspective: 40px; perspective-origin: right bottom; transform-box: fill-box">{BOX}{rect(LIME, turned(40))}</g>',
     path(chain(about(P(40), ox=20, oy=12), about(R3(0, 1, 0, 40))), LIME)),
    # Declarations of no valid value are dropped: the ones before them hold.
    ("perspective-invalid", f'<g style="perspective: 40px; perspective: -10px; perspective: 30; perspective: 10%; perspective: calc(0); perspective: bogus; perspective-origin: 10px 6px; perspective-origin: middle; transform-box: fill-box">{BOX}{rect(LIME, turned(40))}</g>',
     path(chain(about(P(40)), about(R3(0, 1, 0, 40))), LIME)),
    ("transform-style-invalid", f'<g style="{CTX}; transform-style: preserve">{rect(RED, "transform: translateZ(-10px)")}{rect(LIME, "transform: translateZ(10px)")}</g>', rect(LIME, "")),
    ("origin-z-invalid", rect(LIME, "transform-origin: 20px 6px 10px; transform-origin: 20px 6px 10%; transform-origin: 20px 6px left; transform: rotateY(60deg)", x=10), path(about(R3(0, 1, 0, 60), ox=20, oz=10), LIME, x=10)),
    # A pattern flattens what it holds: the later rect covers the nearer.
    ("pattern-flattens", f'<pattern id="flat" patternUnits="userSpaceOnUse" width="20" height="12"><g style="{CTX}">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g></pattern>'
     '<rect width="20" height="12" fill="url(#flat)"/>', rect(NAVY, "")),
    # A participant of no plane of its own is turned as the plane it is on.
    ("backface-participant-hidden", rect(LIME, "") + f'<g style="{CTX}; {turned(180)}">{rect(RED, "backface-visibility: hidden")}</g>', rect(LIME, "")),
    ("backface-participant-shown", f'<g style="{CTX}">{rect(LIME, "backface-visibility: hidden")}</g>', rect(LIME, "")),
    # The perspective of an element taking part reaches its grandchildren
    # through a child that only extends the context.
    ("perspective-through-participant", f'<g style="perspective: 40px; perspective-origin: 10px 6px; {CTX}"><g style="{CTX}">{rect(LIME, turned(40))}</g></g>',
     path(chain(about(P(40)), about(R3(0, 1, 0, 40))), LIME)),
    # Turned 120 degrees the lime plane shows its back, mirrored: the two
    # cross on the middle line, the navy one in front on the left.
    ("intersect-back-facing", f'<g style="perspective: 40px; perspective-origin: 10px 6px"><g style="{CTX}">{rect(LIME, turned(120))}{narrow(NAVY, turned(-45))}</g></g>',
     path(chain(about(P(40)), about(R3(0, 1, 0, 120))), LIME) + side(chain(about(P(40)), B45), -1, NAVY, x=3, w=14)),
    # Only the part of a plane in front of the eye draws, here none of what
    # it holds, a red rect far down a plane tipped towards the eye.
    ("behind-eye", '<g style="perspective: 20px; perspective-origin: 10px 0"><g style="transform-style: preserve-3d"><g style="transform-origin: 10px 0; transform: rotateX(80deg)">'
     '<rect width="20" height="40" fill="none"/><rect y="35" width="20" height="5" fill="red"/></g></g></g>', ""),
    ("use-context", f'<defs><g id="pair" style="{CTX}">{rect(LIME, "transform: translateZ(10px)")}{rect(RED, "transform: translateZ(-10px)")}</g></defs><use href="#pair"/>', rect(LIME, "")),
    # A shape with preserve-3d is a context of itself: vector-effect does
    # nothing to it.
    ("shape-context", f'<rect x="1" y="1" width="8" height="4" fill="none" stroke="navy" vector-effect="non-scaling-stroke" transform="scale(2)" style="{CTX}"/>',
     '<rect x="2" y="2" width="16" height="8" fill="none" stroke="navy" stroke-width="2"/>'),
    ("mask-flattens", rect(LIME, "") + f'<mask id="flat-mask" maskUnits="userSpaceOnUse" x="0" y="0" width="20" height="12"><g style="{CTX}">{rect("white", "transform: translateZ(10px)")}{rect("black", "transform: translateZ(-10px)")}</g></mask>'
     f'{rect(RED, "mask: url(#flat-mask)")}', rect(LIME, "")),
    ("clip-path-flattens", f'<clipPath id="flat-clip"><use href="#clip-shape" style="{CTX}"/></clipPath><rect id="clip-shape" x="2" y="2" width="16" height="8" fill="none"/>'
     f'{rect(LIME, "clip-path: url(#flat-clip)")}', rect(LIME, "", x=2, y=2, w=16, h=8)),
    # A plane of no area, its x scaled to none, draws nothing.
    ("plane-no-area", f'<g style="{CTX}">{rect(LIME, "")}{rect(RED, "transform: scale3d(0, 1, 1)")}</g>', rect(LIME, "")),
    # A plane tipped towards the eye draws as far as the horizon, which the
    # viewport cuts.
    ("partly-behind", '<svg width="20" height="12"><g style="perspective: 20px; perspective-origin: 10px 0"><g style="transform-style: preserve-3d"><g style="transform-origin: 10px 0; transform: rotateX(80deg)">'
     '<rect width="20" height="40" fill="lime"/></g></g></g></svg>', f'<svg width="20" height="12"><path fill="lime" d="{visible(chain(about(P(20), oy=0), about(R3(1, 0, 0, 80), oy=0)), 0, 0, 20, 40)}"/></svg>'),
    # transform-style, perspective and perspective-origin are properties
    # alone: attributes of their names do nothing.
    ("attribute-transform-style", f'<g transform-style="preserve-3d">{rect(LIME, "transform: translateZ(10px)")}{rect(NAVY, "transform: translateZ(-10px)")}</g>', rect(LIME, "") + rect(NAVY, "")),
    ("attribute-perspective", f'<g perspective="40px" perspective-origin="10px 6px">{rect(LIME, turned(40))}</g>', path(about(R3(0, 1, 0, 40)), LIME)),
    ("perspective-in-context", f'<g style="perspective: 40px; perspective-origin: 10px 6px"><g style="{CTX}">{rect(LIME, turned(45))}{narrow(NAVY, turned(-45))}</g></g>',
     path(chain(about(P(40)), A45), LIME) + side(chain(about(P(40)), B45), 1, NAVY, x=3, w=14)),
]

COLUMNS = 4
# A plane in perspective is drawn from its layer resampled, its edges softer
# than a path's, the more where the plane comes nearer.
TOLERANCES = {name: 24 for name in ("perspective-property", "perspective-origin-default", "perspective-invalid", "perspective-through-participant", "perspective-in-context")}
TOLERANCES["intersect-back-facing"] = 40
TOLERANCES["perspective-origin-keywords"] = 48
TOLERANCES["partly-behind"] = 64

for k, (name, test, plain) in enumerate(cells):
    x, y = 4 + k % COLUMNS * 32, 4 + k // COLUMNS * 20

    unit.cell(name, (x - 3, y - 3, 30, 18), f'<g transform="translate({x} {y})">{test}</g>', f'<g transform="translate({x} {y})">{plain}</g>', "", TOLERANCES.get(name))

unit.write()
