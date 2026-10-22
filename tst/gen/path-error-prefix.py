"""A malformed suffix cannot remove a completely specified path segment."""
from unitgen import Unit

cases=[
    ('vertical-dot', 'M12 2H4V10..h--2', 'M12 2H4V10'),
    ('relative-dot', 'M12 2H4v8..h--2', 'M12 2H4V10'),
    ('horizontal-dot', 'M2 12V4H10..v--2', 'M2 12V4H10'),
    ('valid-second-dot', 'M12 2H4v8..0h--2', 'M12 2H4V10'),
    ('partial-pair', 'M2 2L12 2 12 12 .Z', 'M2 2L12 2L12 12'),
    ('partial-curve', 'M2 2H12V12C5 5 .', 'M2 2H12V12'),
    ('bad-command', 'M2 2H12V12X2 12Z', 'M2 2H12V12'),
    ('after-close', 'M2 2H12V12ZX', 'M2 2H12V12Z'),
    ('bad-sign', 'M2 2H12V12h--2', 'M2 2H12V12'),
    ('partial-arc', 'M2 2H12V12A4 4 0 0 1 2', 'M2 2H12V12'),
    ('line-pair', 'M2 4L12 4 12', 'M2 4H12'),
    ('complete-control', 'M2 2L12 2 12 12Z', 'M2 2H12V12Z'),
]
unit=Unit('path-error-prefix',64,48,
          'SVG 2 PathDataErrorHandling preserves every completely defined segment before an invalid suffix, including within a compound command. A second decimal point starts a new number; a lone point cannot erase the preceding complete segment. Literal valid paths define the expected prefix, with a stroke for the incomplete line-pair case.', ground=False)
for i,(name,test,expected) in enumerate(cases):
    x,y=i%4*16,i//4*16
    paint='fill="none" stroke="lime" stroke-width="2"' if name=='line-pair' else 'fill="lime"'
    unit.cell(name,(x,y,16,16),f'<g transform="translate({x} {y})"><path d="{test}" {paint}/></g>',f'<g transform="translate({x} {y})"><path d="{expected}" {paint}/></g>')
unit.write()
