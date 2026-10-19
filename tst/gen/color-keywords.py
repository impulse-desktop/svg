"""color-keywords: currentcolor and transparent of CSS Color 4."""
from unitgen import TARGET as T, Unit, squares


def rect(attributes):
    return f'<rect class="t" width="6" height="6" {attributes}/>'


unit = Unit("color-keywords", 0, 0, """Each lime square fills its black
outline, and nothing red shows. currentcolor is the element's own color,
in any case and in any property, fill, stroke, a stop's or a flood's; in
color itself it is the inherited color. transparent is transparent black,
which leaves the lime under it.""")
squares(unit, [
    ("currentcolor", [("", rect('color="lime" fill="currentcolor"')), ("", rect('color="lime" fill="red" style="fill: CurrentColor"')), ("{c} > .t { color: lime; fill: currentColor }", T),
                      ("", f'<g color="red">{rect("color=\"lime\" fill=\"currentcolor\"")}</g>'), ("", '<path d="M3 0v6" color="lime" stroke="currentcolor" stroke-width="6"/>'),
                      ("", '<linearGradient id="gc" color="lime"><stop stop-color="currentcolor"/></linearGradient>' + rect('fill="url(#gc)"')),
                      ("", '<filter id="fc" x="0" y="0" width="1" height="1" color="lime"><feFlood flood-color="currentcolor"/></filter>' + rect('fill="red" filter="url(#fc)"'))]),
    ("currentcolor-color", [("", f'<g color="lime">{rect("color=\"currentcolor\" fill=\"currentcolor\"")}</g>'), ("", f'<g color="lime">{rect("style=\"color: currentcolor\" fill=\"currentcolor\"")}</g>')]),
    ("transparent", [("", '<rect width="6" height="6" fill="lime"/>' + rect('fill="transparent"')), ("", '<rect width="6" height="6" fill="lime"/>' + rect('fill="red" style="fill: TRANSPARENT"'))]),
])
unit.write()
