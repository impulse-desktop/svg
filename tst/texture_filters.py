#!/usr/bin/env python3
"""Check texture rectangles, transfer equations, window halos and scope guards."""
import copy
import xml.etree.ElementTree as ET

import texture_reference as texture
from noise_reference import NS


def main():
    texture.check()
    checked = 0
    for drawing in ['M1 2H31V42H1Z', 'm1 2 30 0 0 40 -30 0z', 'M31 42L31 2 1 2 1 42 31 42z']:
        assert texture.rectangle(drawing) == (1,2,31,42)
        checked += 1
    for drawing in ['L1 2H31V42H1Z', 'M0 0h10v10h-10', 'M0 0h10v10h-10zM20 20h1v1h-1z',
                    'M0 0L10 0 0 10z', 'M0 0L10 10 0 10 10 0z', 'M0 0H10H0H10Z',
                    'M0 0C1 2 3 4 5 6Z', 'M0 0h10v0h-10z', 'M0 0h1e999v10h-10z']:
        try:
            texture.rectangle(drawing)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted non-rectangle '+drawing)
    template = ET.fromstring('''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64">
      <defs><filter id="f" color-interpolation-filters="sRGB"><feTurbulence numOctaves="0"/>
      <feComposite in2="SourceGraphic" operator="in"/></filter></defs>
      <g><path d="M-100 -100h400v400h-400z" fill="#112233" filter="url(#f)"/></g>
    </svg>''')

    def measured(data, names):
        return {name: {'bbox': [-100,-100,400,400], 'matrix': [1,0,0,1,0,0], 'clips': []} for name in names}

    result = texture.source(ET.tostring(template), 64, 64, measured)
    assert len(list(ET.fromstring(result).iter(NS+'image'))) == 1
    for tag, key, value in [('svg','height','32'), ('svg','viewBox','1 1 64 64'), ('svg','transform','translate(1)'),
                           ('path','d','M0 0L100 0 0 100z'), ('path','fill','url(#gradient)'),
                           ('path','stroke','red'), ('path','marker-start','url(#marker)'), ('path','style','d:path("M0 0H10V10Z")'), ('path','fill-opacity','.5'), ('path','opacity','.5'),
                           ('path','transform','translate(1)'), ('g','mask','url(#mask)'), ('g','opacity','.5'),
                           ('filter','color-interpolation-filters','linearRGB'), ('filter','filterUnits','userSpaceOnUse'),
                           ('filter','x','0.5'), ('feTurbulence','numOctaves','11'), ('feTurbulence','stitchTiles','stitch'),
                           ('feComposite','operator','over')]:
        root = copy.deepcopy(template)
        next(root.iter(NS+tag)).set(key,value)
        try:
            texture.source(ET.tostring(root),64,64,measured)
        except (ValueError, KeyError):
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported texture {tag}/{key}')
    for matrix in [[1,0,0,2,0,0], [1,.5,0,1,0,0], [-1,0,0,1,0,0], [1,0,0,1,float('inf'),0]]:
        def transformed(data,names):
            return {name: {'bbox': [-100,-100,400,400], 'matrix': matrix, 'clips': []} for name in names}
        try:
            texture.source(ET.tostring(template),64,64,transformed)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted nonuniform or invalid frame')
    f = next(template.iter(NS+'filter'))
    for primitive in ['<feGaussianBlur stdDeviation="2"/>', '<feImage/>', '<feComponentTransfer><feFuncA type="gamma"/></feComponentTransfer>',
                      '<feComponentTransfer><feFuncR/><feFuncR/></feComponentTransfer>']:
        changed = copy.deepcopy(f)
        changed.insert(0, ET.fromstring('<g xmlns="http://www.w3.org/2000/svg">'+primitive+'</g>')[0])
        try:
            texture.prepare(changed,1)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported neighborhood or transfer')
    # The required window grows with the complete dependency radius, not with
    # the final operation alone. A displacement can sample either direction.
    changed = copy.deepcopy(f)
    changed.insert(0, ET.Element(NS+'feDisplacementMap', {'scale':'100'}))
    changed.insert(1, ET.Element(NS+'feConvolveMatrix', {'order':'3', 'targetX':'0', 'kernelMatrix':'1 0 0 0 0 0 0 0 0'}))
    _, halo = texture.prepare(changed,2)
    assert halo >= 4+101+2
    checked += 1
    print(f'checked {checked} texture shapes, scope guards and halo bounds, plus numeric controls')


if __name__ == '__main__':
    main()
