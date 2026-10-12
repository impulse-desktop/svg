#!/usr/bin/env python3
"""Fractional straight-edge coverage controls for Inkscape references."""


def probes():
    # The viewBox fits the viewport with a fractional margin. This includes
    # the top edge of openclipart/321288 at its rounded 355 x 512 viewport.
    for margin in [.01, .0625, .125, .2, .3, .37, .4375, .4383557928091193, .45, .49]:
        extent = 32 - 2 * margin
        for layers in [1, 4]:
            for scale in [1, 2, 4]:
                path = f'<path fill="red" d="M0 0H32V{extent}H0Z"/>'
                svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="{extent}" '
                       f'viewBox="0 0 32 {extent}">' + path * layers + '</svg>')
                # Integrate the horizontal boundary over each device pixel,
                # composite the repeated fills, then average the device rows.
                alpha = []
                for y in range(32):
                    total = 0
                    for row in range(y * scale, (y + 1) * scale):
                        coverage = max(0, min(row + 1, (32 - margin) * scale) - max(row, margin * scale))
                        total += 1 - (1 - coverage) ** layers
                    alpha.append(255 * total / scale)
                yield svg, scale, alpha
