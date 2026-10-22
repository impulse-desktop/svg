"""The root's outer CSS box must not outline its content bounding box.

A transparent, partly empty viewport exposes the root error that a white
full-viewport ground would hide. Explicitly inherited outline values still
apply to descendants, and only explicit offset inheritance keeps the
root's negative offset. Plain rectangles are the independent expectations.
"""
from unitgen import Unit

u = Unit('outline-root-box', 128, 64,
         'The outermost SVG has no surrounding CSS box in this API (D39). Its outline must not become an outline of its content bounding box, even with a negative offset. A transparent partial viewport exposes that error. Descendants still obtain explicitly inherited outline and offset values; both ordinary and inherited-offset rectangles are constructed plainly.',
         root='viewBox="0 0 64 32" style="outline:3px solid red;outline-offset:-1px"',
         plain_root='', ground=False)
u.cell('partial-viewport', (0, 0, 40, 64),
       '<rect x="8" y="8" width="8" height="8" fill="lime"/>',
       '<rect x="16" y="16" width="16" height="16" fill="lime"/>')
u.cell('inherit-outline', (40, 0, 44, 64),
       '<rect x="28" y="8" width="8" height="8" fill="lime" style="outline:inherit"/>',
       '<rect x="50" y="10" width="28" height="28" fill="red"/><rect x="56" y="16" width="16" height="16" fill="lime"/>')
u.cell('inherit-offset', (84, 0, 44, 64),
       '<rect x="48" y="8" width="8" height="8" fill="lime" style="outline:inherit;outline-offset:inherit"/>',
       '<rect x="92" y="12" width="24" height="24" fill="red"/><rect x="98" y="18" width="12" height="12" fill="lime"/>')
u.write()
