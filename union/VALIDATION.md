# Validation of the initial port

Validated with the installed KDE Union 6.8.0 runtime in an isolated temporary
XDG data/config directory. The desktop's installed style and settings were not
changed.

- Root CMake configuration with `WITH_UNION_ONLY=ON` and standalone configuration
  with `-S union` succeeded. Both installed the style and twelve color schemes.
  Neither configuration enabled a compiler or required Qt/KDE development files.
- `union-styletool inspect`, installation, and `.unionstyle` packaging succeeded.
  The archive contains the CSS, SVG indicators, metadata, and both license texts.
  On Union 6.8.0, installing the archive directly failed because that version's
  install command only handles directories. Extracting it with `tar` and
  installing the extracted directory succeeded; the guide documents this path.
- Eighty queries against Union's native rule engine passed. Checks covered
  button padding/shadows/focus/disabled state, editable comboboxes, checked and
  indeterminate indicators and their image paths, disabled checked indicators,
  scrollbar dimensions, slider/progress dimensions, menu opacity, and selected
  tabs in all four orientations. All relative imports and image references were
  also checked for existing files.
- Qt Widgets controls were rendered offscreen with the light and dark Vinyl
  color settings. This is a rendering smoke check, not a visual comparison
  against a built C++ Vinyl style.
- A Qt Quick control gallery loaded with `org.kde.union`, but its custom
  backgrounds did not render with the software scene graph. An attempted
  Vulkan render also failed in this headless environment. **Qt Quick visual
  equivalence remains unverified on a desktop with working graphics.**

The stock Union 6.8 rule inspector combines states with `&=` starting from an
empty state set, so its `--state` option cannot validate state-specific rules.
For the state checks above, only a temporary copy of the inspector executable
was corrected to use `|=`. No installed Union binary was changed. Use an
upstream-corrected inspector or a running application to reproduce state checks.
The source of the issue is `tools/ruleinspector/main.cpp` in KDE Union.

Before calling this port visually equivalent, compare it to C++ Vinyl in real
Qt Widgets and Qt Quick applications, with keyboard navigation, light/dark
schemes, right-to-left layout, and fractional scaling. The behavioral gaps are
listed in [README.md](README.md).

## Refinement against the installed C++ style

Rendered the same Qt Widgets gallery with `QStyleFactory::create("Vinyl")` and
`QStyleFactory::create("Union")`, using the same font, palette, icon theme,
window size, and controls. Checked light and dark Vinyl palettes. The resulting
size hints after refinement were:

| Control | Installed C++ Vinyl | Union |
| --- | --- | --- |
| Checkbox | 63×21 | 63×21 |
| Radio button | 115×21 | 115×21 |
| Push button | 92×41 | 92×41 |
| Slider | 84×20 | 84×20 |
| Progress bar | 91×25 | 91×25 |
| Combo box | 131×41 | 139×41 |
| Spin box | 53×34 | 39×34 |

These are measurements of that gallery, not universal sizes for all fonts or
applications. The installed Vinyl plugin reports an 18px indicator metric.
The later source build below confirms that the effective enum in
`kstyle/vinyl.h` also uses 18px; the unused `vinylmetrics.h` is not the reference.

The original eighty rule queries passed again with the updated expected
metrics. Twenty-seven additional queries covered icon-only button sizing,
stacked spin arrows, and first/last immutable tabs in all orientations,
including selected-hovered and selected-disabled states. These use the same
temporary correction to the rule inspector described above.

The Qt Widgets comparison still shows a shorter tab strip than C++ Vinyl and a
progress groove that extends beneath the trailing label area. Union's output
controls those geometries; CSS changes have not resolved them. The screenshot
comparison also does not replace a desktop check of the Qt Quick preview.

## Quick geometry and footer colors

The subsequent System Settings screenshots exposed a limitation of the earlier
Widgets-only calibration: Quick adds the background insets to its implicit
size. The previous 41px background plus 10px insets made buttons 51px tall.
Quick buttons now use a 31px background, while Widgets keeps its 41px hint.

Measured real controls using `org.kde.desktop` backed by C++ Vinyl and
`org.kde.union`, with the same application, font, palette and content:

| Quick control | Native Vinyl | Union after correction |
| --- | --- | --- |
| Text/icon button outer height | 41px | 41px |
| Icon-only button outer height | 40px | 40px |
| Dialog button outer height | 41px | 41px |
| Radio button outer height | 21.67px | 21px |
| Slider outer height | 20px | 20px |

The slider has a 14px surface inside its 20px interaction area; the follow-up
below corrects the offset introduced by control padding. Quick check/radio
surfaces are 16px without a painter inset; the Widgets
surfaces continue to use their existing inset. Explicit Quick highlights still
use the accent color, and disabled highlighted controls use disabled colors.
These are live object/property measurements; Quick visual rendering still
needs desktop verification.

The footer color issue was reproduced with real `QDialogButtonBox` buttons.
System Settings sets `autoDefault=true` on its footer buttons; Union 6.8 maps
that feature to `Highlighted`, even when the button is neither the current
default nor focused. The old CSS therefore painted both enabled buttons with
the accent color. With the corrected CSS their sampled surfaces matched a
neutral control (`#323b49` in the test palette), instead of the accent
(`#1b91d5`). Focused buttons still changed color and disabled buttons matched
the disabled control. [The regression test](tests/README.md) passed via CTest.
The original eighty rule queries also passed with the new slider dimensions.

Navigation delegates use a translucent selected background to approach the
reference sidebar; text/table selection rules are unchanged.

Source evidence for the footer mapping:
[Union ButtonElement](https://github.com/KDE/union/blob/v6.8.0/src/output/qtwidgets/elements/ButtonElement.cpp)
and [System Settings ModuleView](https://github.com/KDE/systemsettings/blob/master/app/ModuleView.cpp).

## Slider alignment and context-menu separation

The padding used to reserve slider interaction space shifted the Quick handle
3px from the groove's center: the output adds the padding when placing the
handle but omits it when placing the groove. The updated style uses zero
control padding and a 20px handle with a centered 14px SVG surface instead.
Live measurements changed from a 3px cross-axis offset to zero for horizontal,
vertical, and mirrored sliders, including controls with an explicit height.

Menus and popups now use an opaque button-palette surface, a contrasting 1px
border, and a stronger shadow declaration. The surface and border provide
separation even if the output clips or does not draw the external shadow.
Qt Widgets menu captures confirmed distinct edge/surface colors with full
surface alpha: dark `#585f6a` / `#323b49`, light `#c1c1c1` / `#f0f0f0`.

Both CTest regressions (`footer-colors` and `slider-menus`) passed with the light
and dark Vinyl schemes in isolated config/data directories. The latter checks
real Quick geometry and popup properties without depending on software-backend
visual rendering. The eighty rule queries also passed with the updated opaque
menu surface and handle dimensions, and all local SVG/import paths resolved.

## State regressions and a source-built reference

Built this repository's C++ `vinyl6` target in a temporary build directory and
loaded its `bin/vinyl6.so` directly with QPluginLoader. This confirms that the
application style's effective `Metrics` enum is in `kstyle/vinyl.h`: indicators
are 18px, including a 1px painter inset. `kstyle/vinylmetrics.h` is not included
by the application style, so its 20px value must not drive the Union port.

New checks reproduced the missing checked surface on flat buttons, the 2px
focus outline left on Widgets' `itemviewitem`, and visible Quick toolbar
separators before the CSS fixes. Checked flat buttons now use the native
resting color (window text at 20% alpha), disabled flat buttons remain clear,
item-view focus outlines are suppressed, and Quick ToolSeparators are clear
inside toolbars, including header toolbars. Standalone separators remain visible.

The reference comparison also found a 2px shortfall in the line editor and,
with this gallery's font, the combo's height. Line editors now use 7px padding
and a 33px Quick background minimum. Combo vertical padding now preserves a
41px Widgets hint without increasing its 41px Quick hint. A live desktop Quick
control backed by the source-built Vinyl plugin also reports 33px for TextField
and 41px for ComboBox.

Measured Widgets size hints with the same application and Sans Serif 9 font:

| Control/content | Source-built C++ Vinyl | Union |
| --- | --- | --- |
| Checkbox, “Check” | 61×21 | 61×21 |
| Radio, “Radio” | 58×21 | 58×21 |
| Push button, “Button” | 92×41 | 92×41 |
| Line editor, “Text” | 120×33 | 120×33 |
| Slider | 84×20 | 84×20 |
| Progress bar | 91×25 | 91×25 |
| Combo, “First item” / “Second item” | 108×41 | 116×41 |
| Integer spinbox, 0–999 | 60×34 | 46×34 |
| Decimal spinbox, 0–999 | 77×34 | 63×34 |

Actual checkbox/radio indicator rectangles (18×18), slider handles (20×20)
and scrollbar extents (21) match as well. Null `pixelMetric` queries in Union
can return QCommonStyle fallback values; the comparison uses actual controls,
options and subelement/subcontrol rectangles instead.

All five CTest checks (`footer-colors`, `flat-buttons`, `reference-metrics`,
`slider-menus`, `control-states`) passed with Vinyl Blue Light and Dark using
Union 6.8.2 and Qt 6.12.0 in isolated data/config/cache directories. The reference
check is optional and requires a source-built plugin path; the other tests do
not need a C++ Vinyl installation. No desktop installation or settings were
changed. Width differences above, full-width tabs, progress-label layout, the
remaining Breeze controls and Quick rendering on a working GPU remain open.

## Menu and radio refinements from desktop screenshots

Loaded Dolphin 26.08.3's actual `setfoldericonitemaction` plugin in a temporary
Qt Widgets host. Its QWidgetAction contains five icon-only QPushButtons and an
“Other” button with a menu. The 92px text-button minimum made those icon buttons
expand the menu to 697px, compared with 406px using the source-built C++ Vinyl.
Scoped compact button rules reduce the Union menu to 417px in that host. The
icon-button hints change from 92×41 to 40×40; the menu button changes from
112×41 to 92×41. These widths include this host's font and layout margins.

The former menu and button surfaces both used the button palette, hiding the
button backgrounds. Menus now use the opaque window palette with a subtle
contrast edge, while embedded buttons keep their button-palette surfaces.
Other popups retain the earlier button-palette surface. Menu opacity stays at
100%; compositor-dependent external shadows are still a visual approximation.

Radio labels now have 8px of space beyond the indicator in Widgets and Quick,
including mirrored and constrained controls and Kirigami forms. Radio surfaces
gain a subtle 1px border and stronger shadow declaration. A separate GPL Vinyl
marker supplies the native 8px center mark without modifying the BSD fallback.
The extra spacing deliberately adds 4px to the radio's former width hint;
indicator size and height still match the reference. Disabled colors and
keyboard focus retain dedicated rules.

The new menu regression reproduced oversized, indistinguishable buttons before
these fixes. All seven CTest checks then passed with Vinyl Blue Light and Dark
on Union 6.8.2. Widgets captures were inspected, and actual radio/text geometry
was measured in Quick and the installed workspace KCM. Quick GPU appearance
still needs desktop confirmation. The Widgets output's missing icon column
for iconless menu entries remains a known difference; it is reported by the
menu regression rather than hidden by an equality claim.
