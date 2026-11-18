# Union fallback provenance

`org.ekaaty.vinyl/contents/css/base/` and the `checkmark.svg`,
`indeterminate.svg`, and `radio.svg` files in `contents/images/` contain the
Breeze style shipped by KDE Union **6.8.0**:

- Repository: https://invent.kde.org/plasma/union
- Release: https://download.kde.org/stable/plasma/6.8.0/union-6.8.0.tar.xz
- License: BSD-2-Clause; copyright notices are retained in individual files.

This snapshot provides complete control layouts, indicators, disabled palettes,
and Qt Quick/Kirigami fallbacks. Bundling it makes the style self-contained:
imports do not depend on where a distribution installs Breeze or whether the
user has a Breeze package in their own data directory.

The only local changes to this snapshot are the three image URLs in
`base/check.css`, adjusted from `../images/` to `../../images/` to account for
the additional `base/` directory. Vinyl rules live separately in `css/vinyl/`.
`base/global.css` is retained from upstream but is not imported by upstream's
`base/style.css`.

`contents/images/slider-handle.svg` and `contents/images/radio-marker.svg` are
separate GPL-2.0-or-later Vinyl assets, not part of the Breeze snapshot. The
radio marker follows the 8px center of Vinyl's native 16px radio surface.

When updating the snapshot, review the Vinyl overrides with
`union-ruleinspector` before changing the supported Union version.
