"""clip-path-path-validity: path() of CSS Shapes 1 in clip-path and d."""
from unitgen import Unit

unit = Unit("clip-path-path-validity", 74, 34, """Each lime drawing fills its
black outline. path() takes a string of path data good to its end and
drawing something: an empty string, one of no path data, or one with an
error after good commands makes the whole path() invalid, for clip-path,
which then clips nothing, and for d, whose attribute then stays; a good
one is closed for clip-path and takes its fill rule, nonzero unless it
names one; in d, a path() without one takes the fill-rule property.""")


def clip(name, x, y, value, plain):
    unit.cell(name, (x - 1, y - 1, 14, 14), f'<rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="clip-path: path({value})"/>',
              f'<g fill="lime" transform="translate({x} {y})">{plain}</g>', f'<g transform="translate({x} {y})">{plain}</g>')


square = '<rect width="12" height="12"/>'
clip("empty", 2, 2, "''", square)
clip("no-data", 16, 2, "'X'", square)
clip("error", 30, 2, "'M0 0 h12 v12 X'", square)
clip("closed", 44, 2, "'M0 0 h12 v12'", '<path d="M0 0h12v12z"/>')
clip("fill-rule", 58, 2, "evenodd, 'M0 0h12v12h-12z M3 3h6v6h-6z'", '<path fill-rule="evenodd" d="M0 0h12v12h-12z M3 3h6v6h-6z"/>')
clip("nonzero", 2, 20, "'M0 0h12v12h-12z M3 3h6v6h-6z'", square)
unit.cell("d-invalid", (1, 14, 30, 4), '<path d="M2 15h12v2h-12z" fill="lime" style="d: path(\'M2 15 h70 v2 h-70 z X\')"/><path d="M16 15h12v2h-12z" fill="lime" style="d: path(\'\')"/>',
          '<rect x="2" y="15" width="12" height="2" fill="lime"/><rect x="16" y="15" width="12" height="2" fill="lime"/>', '<rect x="2" y="15" width="12" height="2"/><rect x="16" y="15" width="12" height="2"/>')
unit.cell("d-fill-rule", (15, 19, 14, 14), '<path d="M0 0" fill="lime" fill-rule="evenodd" style="d: path(\'M16 20h12v12h-12z M19 23h6v6h-6z\')"/>',
          '<path fill="lime" fill-rule="evenodd" d="M16 20h12v12h-12z M19 23h6v6h-6z"/>', '<path d="M16 20h12v12h-12z M19 23h6v6h-6z"/>')
unit.write()
