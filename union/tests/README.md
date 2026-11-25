# Control regression checks

When Qt Quick development files are available, CMake also builds
`slider-menus`. It measures actual Quick slider handle/groove centers for
horizontal, vertical, and mirrored controls, requiring zero cross-axis offset
and a 20px interaction area. It also checks opaque menu/popup surfaces, a menu
edge that contrasts with its surface, and a distinct popup surface. This geometry/property test runs with
the software backend; it does not require working GPU rendering.

## Flat buttons, focus and separators

`flat-buttons` renders the actual Qt Widgets button primitive onto a transparent
image. An enabled, checked flat button must have a visible surface, while an
unchecked or disabled flat button must remain transparent.

`control-states` uses the Quick runner with real flat Buttons and ToolSeparators,
including a header toolbar and a separator outside any toolbar. Toolbar
separators must be transparent, and the standalone separator must remain visible.
The test queries Union's actual rules for Widgets' `itemviewitem` with both focus
flags set, requiring no outline while preserving the selected surface. It also
checks the Quick TextField and ComboBox minimum heights. These checks use
properties and rendered Widgets pixels rather than software Quick screenshots.

## Source-built C++ reference

Build the C++ style from this repository without installing it, then pass its
plugin by absolute path when configuring these checks:

```sh
cmake -S . -B build/native-reference -DWITH_APPSTYLE_ONLY=ON -DWITH_DECORATIONS=OFF
cmake --build build/native-reference --target vinyl6
cmake -S union/tests -B build/union-tests \
    -DVINYL_REFERENCE_PLUGIN="$PWD/build/native-reference/bin/vinyl6.so"
cmake --build build/union-tests
ctest --test-dir build/union-tests --output-on-failure
```

`reference-metrics` loads that exact plugin with QPluginLoader. It compares real
checkbox/radio indicator rectangles, the slider handle and scrollbar extent,
plus the size hints for checkboxes, buttons, text fields, sliders and
progress bars, and radio/combo heights. Radio label spacing must leave at least
6px of clear space; its width intentionally grows with the new 8px gap. It reports combo and spinbox widths
without asserting equality, since those remain different. Both styles use the
same application and font. This is a geometry comparison, not visual approval.

Union needs the widget and style option to identify a control: queries such as
`pixelMetric(PM_IndicatorWidth)` with null arguments can return QCommonStyle
fallback values instead of the real indicator size. The test uses the actual
control rectangles to avoid that false comparison.

## Menus and radio labels

`menu-controls` reproduces Dolphin's folder-color row with five icon-only
QPushButtons and a menu button inside a QWidgetAction. Each icon button must
have a compact size hint and a painted surface distinct from the menu. It also
reports the remaining icon-column difference for iconless menu entries without
asserting that this backend-controlled alignment has been fixed.

`radio-labels` measures actual Quick text and indicator positions after layout,
including mirrored and constrained controls and a Kirigami form. The label gap
must remain at least 6px, and both checked and unchecked radios must retain an
edge. Disabled radios must not inherit the focus outline. These property checks
do not substitute for a GPU screenshot comparison.

## Combo and spinbox geometry

`input-geometry` checks actual Quick ComboBox text/arrow clearance from the
painted background's shadow insets. It also checks that normal, constrained and
mirrored SpinBox arrows share one column, stack without overlap and remain
inside the control. This catches the more specific Breeze increase/decrease
rules overriding a generic indicator alignment.

## Footer colors

This test uses real Qt Widgets buttons and the installed Union runtime. It
reproduces System Settings' enabled Help and Restore Defaults buttons with
`autoDefault=true`, while keyboard focus is outside the footer. Their rendered
surface must match a normal button's surface, including the disabled state.
Moving keyboard focus to the Help button must still change its surface.

Union 6.8 maps both `AutoDefaultButton` and `DefaultButton` to `Highlighted`.
Treating that state as an unconditional accent fill made several unrelated
footer buttons blue. The test fails with the earlier Vinyl CSS and passes with
the corrected rules. It does not depend on a particular color scheme.

Install or update the style being tested, then run:

```sh
cmake -S union/tests -B build/union-tests
cmake --build build/union-tests
ctest --test-dir build/union-tests --output-on-failure
```

Only these developer checks require a C++ compiler and Qt Widgets development
files. Installing the Union theme remains data-only. An isolated test install
can be selected with `XDG_DATA_HOME` and `XDG_CONFIG_HOME`; the test does not
modify those directories or desktop settings.
