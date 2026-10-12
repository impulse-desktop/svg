#!/usr/bin/env python3
"""Check rectangle blur coverage, composition and the accepted document scope."""
import copy
import xml.etree.ElementTree as ET

import blur_reference as blur
from noise_reference import NS


def main():
    print(blur.check())
    _,data,_,_,_,_ = next(blur.probes())
    template = ET.fromstring(data)
    prefix,layers = blur.prepare(data,64,48)
    assert not list(ET.fromstring(prefix).iter(NS+'rect')) and len(layers) == 1
    assert layers[0]['sigma'] == [2,3]
    checked = 1
    for tag,key,value in [
        ('svg','viewBox','0 0 0 48'), ('svg','viewBox','0 0 64 nan'),
        ('svg','preserveAspectRatio','none'), ('svg','transform','translate(1)'),
        ('svg','opacity','.5'), ('svg','stroke','red'), ('svg','filter','url(#f)'),
        ('rect','fill','currentColor'), ('rect','fill','url(#paint)'), ('rect','stroke','red'),
        ('rect','width','0'), ('rect','height','-1'), ('rect','x','10%'), ('rect','y','nan'),
        ('rect','rx','1'), ('rect','ry','1'), ('rect','opacity','1.1'), ('rect','fill-opacity','-.1'),
        ('rect','transform','rotate(10)'), ('rect','style','x:0'), ('rect','style','opacity:1 !important'),
        ('rect','clip-path','url(#clip)'), ('rect','mask','url(#mask)'), ('rect','filter','url(#absent)'),
        ('filter','filterUnits','userSpaceOnUse'), ('filter','primitiveUnits','objectBoundingBox'),
        ('filter','color-interpolation-filters','linearRGB'), ('filter','filterRes','64'),
        ('filter','x','.1'), ('filter','width','.5'), ('filter','height','-1'), ('filter','style','x:0'),
        ('feGaussianBlur','in','SourceAlpha'), ('feGaussianBlur','stdDeviation','1'),
        ('feGaussianBlur','stdDeviation','-1 2'), ('feGaussianBlur','stdDeviation','nan'),
        ('feGaussianBlur','stdDeviation','2 3 4'), ('feGaussianBlur','x','0'),
    ]:
        root = copy.deepcopy(template)
        next(root.iter(NS+tag)).set(key,value)
        try:
            blur.prepare(ET.tostring(root),64,48)
        except ValueError:
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported rectangle blur {tag}/{key}')
    for tag in ['path','style','svg']:
        root = copy.deepcopy(template)
        ET.SubElement(root,NS+tag)
        try:
            blur.prepare(ET.tostring(root),64,48)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted later or unsupported rectangle content')
    for sigma in [0,1,-2,float('nan'),float('inf'),100000]:
        try:
            blur.kernel(sigma)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted invalid or excessive blur sigma')
    print(f'checked {checked} rectangle blur scope cases, plus kernel/coverage and rational composition controls')


if __name__ == '__main__':
    main()
