"""Data URI stylesheets in XML prologs, without synthetic selector elements."""
from base64 import b64encode
from html import escape
from urllib.parse import quote
from unitgen import Unit


def uri(css):
    return 'data:text/css,' + quote(css, safe='')


def pi(css='rect{fill:lime}', extra='', href=None):
    return '<?xml-stylesheet href="' + escape(href or uri(css), quote=True) + '" ' + extra + '?>'


cases = []


def case(name, before, inside='', after='', color='lime', rect=''):
    cases.append((name, before, inside, after, color, rect))


case('percent', pi(extra='type="text/css"'))
case('type-omitted', pi())
case('base64', pi(href='data:text/css;base64,' + b64encode(b'rect{fill:lime}').decode()))
case('mime-parameters', pi(href='data:TEXT/CSS;charset=utf-8,rect%7Bfill:lime%7D'))
case('advisory-type', pi(extra='type="text/plain"'))
case('document-order', pi('rect{fill:red}') + pi())
case('style-order', pi('rect{fill:red}'), '<style>rect{fill:lime}</style>')
case('inline', pi('rect{fill:red}'), rect='style="fill:lime"')
case('important', pi('rect{fill:lime!important}'), rect='style="fill:red"')
case('structural', pi('svg>rect:first-child:last-child{fill:lime}'))
case('screen', pi(extra='media="screen and (min-width: 8px)"'))
case('print', pi(extra='media="print"'), color='red')
case('empty-media', pi(extra='media=""'))
case('preferred', pi(extra='title="main"') + pi('rect{fill:red}', 'title="other"'))
case('alternate', pi(extra='title="other" alternate="yes"'), color='red')
case('alternate-no-title', pi(extra='alternate="yes"'), color='red')
case('alternate-before-preferred', pi(extra='title="main" alternate="yes"') + pi('rect{stroke:none}', 'title="main"'))
case('alternate-after-preferred', pi('rect{fill:red}', 'title="main"') + pi(extra='title="main" alternate="yes"'))
case('alternate-before-style', pi(extra='title="main" alternate="yes"'), '<style title="main">rect{stroke:none}</style>')
case('quoted-attributes', "<?xml-stylesheet\n href = '" + uri('rect{fill:lime}') + "' type = 'text/css' ?>")
case('character-references', '<?xml-stylesheet href="data:text/css,rect{fill:li&#109;&#x65;}"?>')
case('predefined-entities', '<?xml-stylesheet href="data:text/css,[id=&quot;a&amp;b&apos;c&quot;]{fill:lime}"?>', rect='id="a&amp;b\'c"')
case('unknown-attribute', pi(extra='unused="x"'))
case('missing-href', '<?xml-stylesheet type="text/css"?>', color='red')
case('external', pi(href='missing.css'), color='red')
case('wrong-mime', pi(href='data:text/plain,rect%7Bfill:lime%7D'), color='red')
case('bad-data', pi(href='data:text/css;base64,%'), color='red')
case('duplicate', pi(extra='href="' + uri('rect{fill:lime}') + '"'), color='red')
case('duplicate-unknown', pi(extra='unused="a" unused="b"'), color='red')
case('unquoted', '<?xml-stylesheet href=' + uri('rect{fill:lime}') + '?>', color='red')
case('no-equals', '<?xml-stylesheet href "' + uri('rect{fill:lime}') + '"?>', color='red')
case('unclosed-quote', '<?xml-stylesheet href="' + uri('rect{fill:lime}') + '?>', color='red')
case('no-space', '<?xml-stylesheet href="' + uri('rect{fill:lime}') + '"type="text/css"?>', color='red')
case('bad-name', pi(extra='1bad="x"'), color='red')
case('wrong-target', pi().replace('xml-stylesheet', 'xml-stylesheet-other'), color='red')
case('target-case', pi().replace('xml-stylesheet', 'XML-STYLESHEET'), color='red')
case('inside-root', '', pi(), color='red')
case('after-root', '', after=pi(), color='red')
case('after-doctype', '<!DOCTYPE svg>' + pi())
case('inside-doctype', '<!DOCTYPE svg [' + pi() + ']>', color='red')
for name, ref in [('unknown-entity', '&custom;'), ('empty-reference', '&#;'),
                  ('invalid-digit', '&#xq;'), ('invalid-decimal', '&#1f;'),
                  ('zero', '&#0;'), ('control', '&#1;'), ('surrogate', '&#xd800;'),
                  ('non-character', '&#xffff;'), ('out-of-range', '&#x110000;'),
                  ('no-semicolon', '&amp'), ('literal-less-than', '<')]:
    case(name, '<?xml-stylesheet href="' + uri('rect{fill:lime}') + '" unused="' + ref + '"?>', color='red')
case('recovery', '<?xml-stylesheet href=bad?>' + pi())

unit = Unit('xml-stylesheet', 128, 112, '''Each embedded SVG has its own XML
prolog. Valid data: CSS sheets enter the cascade before style elements;
media and the preferred title select the sheets, while inline declarations
and importance keep their priorities. Processing instructions create no
selector elements. Malformed pseudo-attributes discard one instruction,
not the XML document. Expected rectangles use no stylesheet or image.''')
for i, (name, before, inside, after, color, rect) in enumerate(cases):
    x, y = i % 8 * 16 + 2, i // 8 * 16 + 2
    source = (before + '<svg xmlns="http://www.w3.org/2000/svg" width="12" height="12">'
              + inside + '<rect width="12" height="12" fill="red" ' + rect + '/></svg>' + after)
    unit.cell(name, (x-1, y-1, 14, 14),
              f'<image x="{x}" y="{y}" width="12" height="12" href="data:image/svg+xml,{quote(source, safe="")}"/>',
              f'<rect x="{x}" y="{y}" width="12" height="12" fill="{color}"/>')
unit.write()
