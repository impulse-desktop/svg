"""css-link-state: the link and dynamic pseudo-classes of Selectors 4 in a
document drawn as an image (D33)."""
from unitgen import TARGET as T, Unit, squares

unit = Unit("css-link-state", 0, 0, """Each lime square fills its black
outline, and nothing red shows. The document draws as an image, whose
links nothing follows: :any-link and :link match no a element, with an
href in either namespace or none, and :visited, :hover, :focus, :active and
:target nothing either, yet each is valid, the other selectors of its list
applying (D33).""")
squares(unit, [
    ("never", [("{c} :any-link > .t, {c} :link > .t, {c} :visited > .t { fill: red } {c} .t { fill: lime }", f'<a href="#x">{T}</a>'),
               ("{c} a:any-link > .t, {c} a:link > .t { fill: red } {c} .t { fill: lime }", f'<a xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#x">{T}</a>'),
               ("{c} :any-link > .t { fill: red } {c} .t { fill: lime }", f"<a>{T}</a>"), ("{c} > :link { fill: red } {c} > .t { fill: lime }", T)]),
    ("valid", [("{c} :link > .t, {c} a > .t { fill: lime }", f'<a href="#x">{T}</a>'),
               ("{c} :visited > .t, {c} :hover > .t, {c} :focus > .t, {c} :active > .t, {c} :target > .t, {c} a > .t { fill: lime }", f'<a href="#x" id="x">{T}</a>')]),
])
unit.write()
