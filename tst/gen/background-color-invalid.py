"""Invalid background colours cannot displace a valid declaration."""
from unitgen import Unit


cases = [
    ("inline", "background-color:lime;background-color:rgba(1)", "", "lime"),
    ("important", "background-color:lime;background-color:rgba(1)!important", "", "lime"),
    ("sheet", "background-color:rgba(1)!important", "svg{background-color:lime}", "lime"),
    ("layer", "", "@layer a,b;@layer a{svg{background-color:lime}}@layer b{svg{background-color:rgba(1)}}", "lime"),
    ("shorthand", "background:lime;background-color:rgba(1)", "", "lime"),
    ("current", "color:lime;background-color:currentColor;background-color:rgba(1)", "", "lime"),
    ("variable", "--c:rgba(1);background-color:lime;background-color:var(--c)", "", None),
    ("fallback", "background-color:var(--missing,lime)", "", "lime"),
    ("hashless", "background-color:11ee11", "", None),
    ("hashless-cascade", "background-color:lime;background-color:11ee11", "", "lime"),
]

for name, style, sheet, color in cases:
    unit = Unit("background-color-invalid" + ("" if name == "inline" else "-" + name), 8, 8, """A background-color outside its colour
 grammar is discarded before the cascade, preserving the valid declaration
 before or below it. A variable's invalid value is instead invalid at
 computed-value time and gives transparent. The expected drawing uses a
 plain rectangle, with no background declarations.""", root=f'style="{style}"', plain_root="", ground=False)
    if sheet:
        unit.common(f'<style>{sheet}</style>', "")
    unit.cell(name, (0, 0, 8, 8), "", '<rect width="8" height="8" fill="lime"/>' if color else "")
    unit.write()
