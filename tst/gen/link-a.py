"""link-a: the a element as a container, of SVG 2's linking. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect

expect("link-a", [(name, (10 * k, 0, 10, 12), f'<rect x="{10 * k}" width="10" height="12" fill="lime"/>') for k, name in enumerate(["href", "no-href", "nested", "inherit"])])
