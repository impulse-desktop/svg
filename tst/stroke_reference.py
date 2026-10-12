"""Outline reviewed solid paths with Inkscape before reference rasterization."""
from pathlib import Path
import subprocess
import tempfile
import xml.etree.ElementTree as ET

NS = '{http://www.w3.org/2000/svg}'


def validate(data):
    root = ET.fromstring(data)
    if root.tag != NS + 'svg' or not root.get('viewBox'):
        raise ValueError('stroke outlining requires a root viewBox')
    allowed = {'fill', 'stroke', 'fill-rule', 'fill-opacity', 'stroke-width',
               'stroke-opacity', 'stroke-linecap', 'stroke-linejoin',
               'stroke-miterlimit', 'opacity', 'display', 'visibility'}

    def visit(node):
        if not node.tag.startswith(NS) or node.tag in [NS + 'metadata', NS + 'title', NS + 'desc']:
            return
        if node.tag not in [NS + 'svg', NS + 'g', NS + 'defs', NS + 'path'] or (node is not root and node.tag == NS + 'svg'):
            raise ValueError('stroke outlining only supports groups and paths')
        for key in node.attrib:
            if (not key.startswith('{') and key not in allowed | {'id', 'd', 'viewBox', 'width', 'height', 'version', 'transform', 'style'}) or key.endswith('}path-effect'):
                raise ValueError('unsupported stroke outlining attribute: ' + key)
        properties = node.attrib.copy()
        for entry in node.get('style', '').split(';'):
            if entry.strip():
                key, value = entry.split(':', 1)
                if key.strip() not in allowed:
                    raise ValueError('unsupported stroke outlining style: ' + key)
                properties[key.strip()] = value.strip()
        for key in ['filter', 'mask', 'clip-path', 'marker', 'marker-start', 'marker-mid', 'marker-end',
                    'vector-effect', 'stroke-dasharray', 'stroke-dashoffset', 'paint-order', 'transform-origin']:
            if key in properties:
                raise ValueError('unsupported stroke outlining property: ' + key)
        if any('url(' in value.lower() or '!important' in value.lower() for value in properties.values()):
            raise ValueError('stroke outlining requires solid paints without CSS overrides')
        for child in node:
            visit(child)

    visit(root)


def source(data, program):
    validate(data)
    with tempfile.TemporaryDirectory(prefix='svg-outline-') as temporary:
        source = Path(temporary) / 'source.svg'
        output = Path(temporary) / 'outlined.svg'
        source.write_bytes(data)
        subprocess.run([program, str(source), '--export-plain-svg', '--export-filename=' + str(output),
                        '--actions=select-all:all;object-stroke-to-path'],
                       check=True, capture_output=True, timeout=60)
        return output.read_bytes()
