"""A long open stroke's short returning tip keeps its inner opening (D49)."""
from unitgen import Unit

# Black on transparent, captured at the unit's 128x64 pixels from Chromium;
# resvg and librsvg agree on the opening and its outline. Keeping the fixed
# PNG here makes the expected drawing independent of this renderer's stroke
# construction, while the two cells test both path directions.
REFERENCE = "iVBORw0KGgoAAAANSUhEUgAAAIAAAABACAYAAADS1n9/AAADZUlEQVR4AeycO6wNURSGh4aEwiMkGoJOIvGoBQ0FolAQiRAVUUhQ0dJ5JCK0okGjQKFCJKKRICQKCgoSj9AodHyr2DfXyJwze2bNYWb9J+s/e2bP2mv2+td/Zs7Zd3JnFnqFZkACCF3+opAAJIDgDARPX1cACSA4A8HT1xVAAgjOQND0U9q6AiQmgrYSQNDCp7QlgMRE0FYCCFr4lLYEkJgI2koAQQuf0pYAEhNBWwkgWOHL6UoAZUaC7UsAwQpeTlcCKDMSbF8CCFbwcroSQJmRYPsSQLCCl9OVAMqMBNuXAIIUvCpNCaCKmSD9EkCQQlelKQFUMROkXwIIUuiqNCWAKmYm17+VU50BD8AP8BE8AqfACtCpSQCd0jsy+DKO3gf3wEmwCcwBS8AGcBq8BZeB9dP4WxcC+MU0haIYx8E7eNoMRtkMDh4GL4GJgsbXuhCA7wwVzRhYzttDsB5k2ThnCWAcQ//PcavVTabjejuwoMSU9YSBlczzInAzCcCNyokFOsiZFgMXkwBcaJx4kO1eZ5QAvJicbBxbO3A5owTgQmOtIHfwegY8bItHEIshARgLk8FZTrMOLAQnwCvQ1OYxcDZobRJAawprB7BlXnP+xts5sBqYIOxb/Ve2c8xi/Rw1oO4xCaAuU+397CdcOYrdEo7SuQjsAbdBHftUx6mOjwRQhyUfn11jwtgiz0587BZxnPYFqLLPVQdy+yWAXMaa+29jaJ1VPLtFnMd3DVgFLoAvYLp9mL7TZlsCaMNe3ti5uF8COfYa52PAFn520N4CZo/tzQMSgAeL9WMcwNXu9TTZdpcR6Yuf/WGI3fYmAbTnMDfCdQYcArl2hAF7wXfwHLiYBOBCY3aQK4y4BmaBcTYfhxsg3T7cPv3E1D+LNhL+EfZx3iegal1/Kcf2gzdgN0hmTxGl7b/a3A5dAXIZ8/VfSzhbIrbLun2yDU/ps6eJ3tNeBfazkGbKzGdqp+2GBNCWQZ/xtrS7kVCGUU/9mFDaLCFzij+tCwHYc2xCUXTBwYLC+dWFAJynqHBdMiABdMluD2JLAD0oUpdTlAC6ZLcHsSWAHhSpzhSb+kgATZkbyDgJYCCFbJqGBNCUuYGMkwAGUsimaUgATZkbyDgJYCCFbJqGBNCUuYGMkwB6Xsi20/8NAAD//zVgYuUAAAAGSURBVAMAGbyLgRX/hjkAAAAASUVORK5CYII="

unit = Unit("stroke-hairpin-tip", 128, 128, """A long open stroke with a
sub-width reversal by its tip leaves a small opening inside the tip, whether
the path runs towards or away from it. The opening is not a line cap: it
comes from the returning inner offset of the stroke outline (D49).""")

end = 'M 10 20 H 50 C 50.2 19.2 49.9 18.8 49.8 19.4 L 49.5 20.2'
start = 'M 49.5 20.2 L 49.8 19.4 C 49.9 18.8 50.2 19.2 50 20 H 10'
for name, path, y in (("end", end, 0), ("start", start, 32)):
    unit.cell(name, (0, 2 * y, 128, 64),
              f'<path transform="translate(0 {2 * y}) scale(2)" d="{path}" fill="none" stroke="black" stroke-width="10"/>',
              f'<image x="0" y="{2 * y}" width="128" height="64" href="data:image/png;base64,{REFERENCE}"/>',
              tolerance=64)

unit.write()
