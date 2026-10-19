"""root-effects-*: a clip-path, a mask and a mask border on the outermost
svg, whose CSS box is its viewport and whose viewBox halves its user units
here, so a box in the wrong space shows. One unit each, as each takes the
root."""
from unitgen import Unit

LIME = '<rect x="2" y="2" width="12" height="6" fill="lime"/>'
RED = '<rect x="2" y="9" width="12" height="5" fill="red"/>'
VIEW = 'viewBox="0 0 16 16"'

for name, root, defs, test, plain, description in [
    ("root-effects-clip-shape", 'style="clip-path: inset(0 0 50% 0) fill-box"', "", LIME + RED, LIME,
     """A basic shape clips the outermost svg against its CSS box, the
viewport, whichever box it names: the top half of it, though the root's
content is smaller and its viewBox halves its units."""),
    ("root-effects-clip-units", 'clip-path="url(#c)"', '<clipPath id="c" clipPathUnits="objectBoundingBox"><rect width="1" height=".5"/></clipPath>', LIME + RED, LIME,
     """A clipPath in bounding box units clips the outermost svg against its
content's bounding box mapped into its viewport by its viewBox, where the
clip applies (D46): the top half of the content."""),
    ("root-effects-mask-units", 'mask="url(#m)"', '<mask id="m" maskContentUnits="objectBoundingBox"><rect width="1" height=".5" fill="white"/></mask>', LIME + RED, LIME,
     """A mask in bounding box units masks the outermost svg against its
content's bounding box mapped into its viewport by its viewBox, where the
mask applies (D46): the top half of the content."""),
    ("root-effects-mask-image", 'style="mask-image: linear-gradient(black, black); mask-size: 100% 50%; mask-repeat: no-repeat"', "", LIME + RED, LIME,
     """A mask image's positioning area on the outermost svg is its CSS box,
the viewport: a mask the size of its top half shows that half, though the
root's content is smaller and its viewBox halves its units."""),
    ("root-effects-mask-border", 'style="mask-border: linear-gradient(transparent, transparent) 30% / 8px"', "",
     '<rect width="16" height="16" fill="red"/><rect x="4" y="4" width="8" height="8" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>',
     """A mask border on the outermost svg lies on its CSS box, the viewport: a
transparent border 8 px wide hides all but the middle 16 px of it, though
its viewBox halves its units."""),
]:
    # No outline, which the root's effect would cut too.
    unit = Unit(name, 32, 32, "A lime rect shows, and nothing red. " + description, root=f"{VIEW} {root}", plain_root=VIEW, ground=False)

    if defs:
        unit.common(defs, "")

    unit.cell("root", (0, 0, 32, 32), test, plain)
    unit.write()
