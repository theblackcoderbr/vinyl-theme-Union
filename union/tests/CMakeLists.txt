cmake_minimum_required(VERSION 3.16)
project(vinyl-union-checks LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_executable(footer-colors footer-colors.cpp)
target_link_libraries(footer-colors PRIVATE Qt6::Widgets)
target_compile_features(footer-colors PRIVATE cxx_std_17)

enable_testing()
add_test(NAME footer-colors COMMAND footer-colors)
set_tests_properties(footer-colors PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;UNION_STYLE_NAME=org.ekaaty.vinyl"
)

add_executable(flat-buttons flat-buttons.cpp)
target_link_libraries(flat-buttons PRIVATE Qt6::Widgets)
target_compile_features(flat-buttons PRIVATE cxx_std_17)
add_test(NAME flat-buttons COMMAND flat-buttons)
set_tests_properties(flat-buttons PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;UNION_STYLE_NAME=org.ekaaty.vinyl"
)

add_executable(menu-controls menu-controls.cpp)
target_link_libraries(menu-controls PRIVATE Qt6::Widgets)
target_compile_features(menu-controls PRIVATE cxx_std_17)
add_test(NAME menu-controls COMMAND menu-controls)
set_tests_properties(menu-controls PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;UNION_STYLE_NAME=org.ekaaty.vinyl"
)

add_executable(reference-metrics reference-metrics.cpp)
target_link_libraries(reference-metrics PRIVATE Qt6::Widgets)
target_compile_features(reference-metrics PRIVATE cxx_std_17)
set(VINYL_REFERENCE_PLUGIN "" CACHE FILEPATH "Vinyl C++ plugin built from this source tree")
if(VINYL_REFERENCE_PLUGIN)
    add_test(NAME reference-metrics COMMAND reference-metrics "${VINYL_REFERENCE_PLUGIN}")
    set_tests_properties(reference-metrics PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;UNION_STYLE_NAME=org.ekaaty.vinyl"
    )
endif()

find_package(Qt6 QUIET COMPONENTS Quick Qml QuickControls2)
if(TARGET Qt6::Quick AND TARGET Qt6::Qml AND TARGET Qt6::QuickControls2)
    add_executable(slider-menus slider-menus.cpp)
    target_link_libraries(slider-menus PRIVATE Qt6::Quick Qt6::Qml Qt6::QuickControls2)
    target_compile_features(slider-menus PRIVATE cxx_std_17)
    add_test(NAME slider-menus COMMAND slider-menus "${CMAKE_CURRENT_SOURCE_DIR}/slider-menus.qml")
    add_test(NAME control-states COMMAND slider-menus "${CMAKE_CURRENT_SOURCE_DIR}/control-states.qml")
    add_test(NAME radio-labels COMMAND slider-menus "${CMAKE_CURRENT_SOURCE_DIR}/radio-labels.qml")
    add_test(NAME input-geometry COMMAND slider-menus "${CMAKE_CURRENT_SOURCE_DIR}/input-geometry.qml")
    set_tests_properties(slider-menus control-states radio-labels input-geometry PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_BACKEND=software;UNION_STYLE_NAME=org.ekaaty.vinyl"
    )
endif()
