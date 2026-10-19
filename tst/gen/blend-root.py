"""blend-root: the root element group of Compositing 1."""
from unitgen import Unit

unit = Unit("blend-root", 40, 16, """The drawing is lime at half opacity, with no
outline, which the root's opacity would fade. The root element's group
takes its opacity as one, after its content: the overlapping squares are
no darker where they overlap.""", root='opacity=".5"', ground=False, plain_root="")
unit.cell("root-group", (1, 1, 38, 14), '<rect x="2" y="2" width="12" height="12" fill="lime"/><rect x="8" y="2" width="12" height="12" fill="lime"/><rect x="24" y="2" width="12" height="12" fill="lime"/>',
          '<rect x="2" y="2" width="18" height="12" fill="lime" fill-opacity=".5"/><rect x="24" y="2" width="12" height="12" fill="lime" fill-opacity=".5"/>')
unit.write()
