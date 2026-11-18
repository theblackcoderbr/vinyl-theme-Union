# Vinyl (Union)

An initial declarative port of Vinyl's C++ application style to KDE Union.
The theme is CSS and SVG data: changing or installing it does not compile any
Vinyl code. The **Union 6.8 or newer runtime**, including its CSS input, Plasma
platform integration, and Qt Widgets/Qt Quick outputs, must already be installed.

## Install

From the repository root, install the style for the current user:

```sh
union-styletool install union/org.ekaaty.vinyl
```

To update an installed copy while developing, use:

```sh
union-styletool update --force union/org.ekaaty.vinyl
```

Without `--force`, the update command expects a newer version number.

To install the style **and all twelve existing Vinyl color schemes**, use the
data-only CMake path. It needs CMake and a build tool such as Make or Ninja,
but no C++ compiler, Qt development packages, or KDE development packages:

```sh
cmake -S . -B build/union -DWITH_UNION_ONLY=ON \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local"
cmake --install build/union
```

`cmake -S union -B build/union-standalone` is also supported. For a distribution
package, set `CMAKE_INSTALL_PREFIX` and, if necessary, `CMAKE_INSTALL_DATADIR`.
The normal C++ build also installs this style; use `-DWITH_UNION=OFF` to opt out.

## Try and select

Select a Vinyl color scheme in System Settings → Colors & Themes → Colors.
The CSS uses KDE palette roles, so a single style works with all Vinyl colors
and both light and dark schemes.

Test a Qt Widgets application in a separate process:

```sh
UNION_STYLE_NAME=org.ekaaty.vinyl QT_STYLE_OVERRIDE=Union dolphin --new-window
```

Test a Qt Quick application:

```sh
UNION_STYLE_NAME=org.ekaaty.vinyl QT_QUICK_CONTROLS_STYLE=org.kde.union systemsettings
```

Close existing instances first if the application reuses its current process.
For desktop-wide use, select **Vinyl (Union)** under System Settings → Colors &
Themes → Application Style and restart applications. These commands only set
environment variables for the launched process.

The existing Vinyl global themes still select the C++ style and window
decoration. Applying one of those global themes may therefore require selecting
Vinyl (Union) again afterward.

## Configuration and equivalence

The theme entry point is `org.ekaaty.vinyl/contents/css/style.css`. Edit the
files in `contents/css/vinyl/`, then reinstall the style and restart the test
application. `vinylrc` remains the configuration of the C++ implementation;
Union does not read it or migrate per-user changes automatically.

| C++ reference/default | Union configuration | Status |
| --- | --- | --- |
| `Common/CornerRadius=4` | `--vinyl-radius`, `--vinyl-button-radius`, `--vinyl-check-radius` | 4px frames, 3px buttons, 2px checkboxes |
| Button margins 6, `ButtonSize=8`, icon spacing 4 | `buttons.css`, 10px padding, 4px spacing | Approximation; Union owns content sizing |
| Text-button minimum plus frame/shadow space | `--vinyl-button-min-width`, `--vinyl-button-height` | 92×41px outer hint, checked against the source-built C++ style |
| Icon-only action buttons | `buttons.css` | Compact 40px Quick outer hint and Widgets menu buttons; regular text-button minimum retained elsewhere |
| Check/radio indicators | `--vinyl-check-size`, `inputs.css` | 18px slot, 1px surface inset, matching the compiled C++ reference |
| Radio labels and marker | `inputs.css`, `radio-marker.svg` | 8px label gap, subtle 1px edge and 8px center mark; label gap deliberately 4px wider than C++ |
| Line editor frame width 7 | `--vinyl-input-padding`, `inputs.css` | 7px padding; 33px Quick minimum height |
| Scrollbar extent 21, handle slot width 8 | `bars.css` | 21px extent, 8px slot, 2px surface inset |
| Scrollbar line buttons 0/0 | Union's buttonless scrollbar | Equivalent default |
| Progress/slider groove 6, slider handle 20 | `bars.css`, `slider-handle.svg` | 6px groove; 14px circular surface centered in a 20px handle, with zero control padding |
| Progress size hint | `progressbar > chunk` | 9px chunk; avoids the fallback's oversized preview |
| Fixed tabs with rounded selected surface | `tabs.css` | Neutral selected pill, no pane outline or accent stripe, including immutable tabs |
| Spinbox arrows | `inputs.css` | Trailing stacked arrows, including the constrained variant |
| Quick buttons and shadow space | `buttons.css` display/role selectors | 31px painted background + 10px insets = 41px outer height, matching the native Quick reference |
| Dialog button colors | `buttons.css` highlighted selectors | Widgets auto-default buttons stay neutral until focused; explicit Quick highlight remains accented |
| Navigation selection | `delegates.css` | Translucent accent rather than a solid selection fill |
| Context-menu separation | `--vinyl-menu-background`, `--vinyl-menu-border`, `--vinyl-popup-shadow` | Opaque window-palette surface, contrast edge and shadow; embedded buttons remain distinct. Other popups use the button palette |
| `MenuItemDrawStrongFocus=true` | Filled selection in `containers.css` | Highlight and highlighted text palette roles |
| Toolbar separators false, view focus indicator false | `containers.css` | Widgets and Quick toolbar separators and extra view outlines suppressed; standalone Quick separators retained |
| Checked flat buttons | `buttons.css` | Resting selection uses window text at 20% alpha; disabled buttons stay transparent |
| Palette colors | `custom-color("kcolorscheme", ...)` | Active/disabled colors, light and dark |
| Button/check shadows and pressed appearance | `buttons.css`, `inputs.css` | Approximation of C++ painting |

Quick check/radio indicators use a 16px painted surface because Quick does not
remove the indicator inset like the Widgets painter does. Labels stay on one
line, as in the native style. See [tests/README.md](tests/README.md) for the
rendered footer-color regression check.

The slider's smaller painted circle is an SVG inside its 20px interaction
rectangle. Cross-axis padding is kept at zero because Union 6.8 positions the
Slider groove without that padding but positions the handle with it, causing
an offset. Menu and popup separation is guaranteed by their surface and border;
external shadow rendering depends on the output/compositor.

This is a starting implementation, not a claim of pixel-for-pixel equivalence.
Union and applications ultimately control layout and may interpret size hints
differently. Rounded tabs, popup shadows, and focus behavior need comparison in
real applications, including keyboard navigation, RTL, and different scales.

The C++ application style uses the `Metrics` enum in `kstyle/vinyl.h`, including
an 18px indicator slot and a 7px line-editor frame. `kstyle/vinylmetrics.h` is
not included by the application style; its 20px indicator is not the effective
reference. A build from this source tree confirms the 18px slot. The optional
`reference-metrics` check loads that build by absolute filename rather than
selecting the desktop's installed plugin; see [tests/README.md](tests/README.md).

Union 6.8's Qt Widgets output still controls the width of the tab strip and
draws progress across the full groove. The CSS moves progress text to the
trailing edge, but cannot reproduce C++ Vinyl's separate label area or force
tabs to span the full pane width. Spinbox arrow rendering also depends on the
output and available icon theme. These remain visual differences to compare.
Combo and spinbox width hints still differ from the C++ reference. Dial, switch,
roundbutton and header controls still use the Breeze fallback and need their own
Vinyl comparison before being considered equivalent.
The Widgets menu output also omits the icon column for iconless entries such as
“Open Terminal Here”, shifting their text left. That layout difference remains;
menu width and embedded button contrast are covered by rendered regressions.

The following C++ behavior is not ported by these rules: the 250ms animations,
press ripple, busy-indicator timing, hue-shifting slider gradients, automatic
light/dark painter outline adjustments, Dolphin sidebar opacity/blur and
transparency exceptions, Konsole-specific tabs, window dragging allow/deny
lists, splitter hit-area proxy, tree branch-line settings, and mnemonic policy.
No CSS animation properties are added because Union 6.8 does not support them.
Window decoration, Plasma shell style, and the other suite components are
separate from this application-style package.

## Inspect and distribute

```sh
union-styletool inspect union/org.ekaaty.vinyl
UNION_STYLE_NAME=org.ekaaty.vinyl QT_QPA_PLATFORM=offscreen \
    union-ruleinspector --type button
union-styletool package union/org.ekaaty.vinyl
```

Packaging creates `org.ekaaty.vinyl.unionstyle` in the current directory.
It contains the style and licenses; color schemes are installed separately.
The package includes a frozen Breeze fallback to cover controls not yet
customized by Vinyl; see [UPSTREAM.md](UPSTREAM.md) for provenance and licensing.

Union 6.8.0 can create the archive but its install command expects an unpacked
directory. On that version, extract the package before installing it:

```sh
mkdir -p vinyl-package
tar -xzf org.ekaaty.vinyl.unionstyle -C vinyl-package
union-styletool install vinyl-package/org.ekaaty.vinyl
```

Union 6.8's rule inspector has an upstream `currentStates &= value` bug that
prevents `--state` from changing the query. State checks need a corrected tool
or a running application. Its `--style` argument is also consumed by Qt in this
environment, so the examples use `UNION_STYLE_NAME`.

References: [Union style packages](https://api.kde.org/working-with-styles.html),
[supported CSS](https://api.kde.org/css-support.html),
[element reference](https://api.kde.org/elements.html),
[rule inspector source](https://invent.kde.org/plasma/union/-/blob/v6.8.0/tools/ruleinspector/main.cpp).
