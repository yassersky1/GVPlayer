cmake_minimum_required(VERSION 3.21)

project(
    GVPlayer
    VERSION 1.0.0
    DESCRIPTION "A high-quality video player for macOS"
    LANGUAGES CXX
)

set(
    CMAKE_CXX_STANDARD
    17
)

set(
    CMAKE_CXX_STANDARD_REQUIRED
    ON
)

set(
    CMAKE_CXX_EXTENSIONS
    OFF
)

set(
    CMAKE_AUTOMOC
    ON
)

set(
    CMAKE_AUTOUIC
    ON
)

set(
    CMAKE_AUTORCC
    ON
)

find_package(
    Qt6
    REQUIRED
    COMPONENTS
        Widgets
        OpenGL
        OpenGLWidgets
)

find_package(
    PkgConfig
    REQUIRED
)

pkg_check_modules(
    MPV
    REQUIRED
    IMPORTED_TARGET
    mpv
)

set_source_files_properties(
    resources/GVPlayer.icns
    PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
)

qt_add_executable(
    GVPlayer
    MACOSX_BUNDLE

    src/main.cpp

    src/MainWindow.h
    src/MainWindow.cpp

    src/MpvWidget.h
    src/MpvWidget.cpp

    resources/GVPlayer.icns
)

target_link_libraries(
    GVPlayer
    PRIVATE
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        PkgConfig::MPV
)

target_compile_definitions(
    GVPlayer
    PRIVATE
        QT_NO_KEYWORDS
)

set_target_properties(
    GVPlayer
    PROPERTIES

        MACOSX_BUNDLE
            TRUE

        MACOSX_BUNDLE_GUI_IDENTIFIER
            "com.yassersky.gvplayer"

        MACOSX_BUNDLE_BUNDLE_NAME
            "GV Player"

        MACOSX_BUNDLE_BUNDLE_VERSION
            "${PROJECT_VERSION}"

        MACOSX_BUNDLE_SHORT_VERSION_STRING
            "${PROJECT_VERSION}"

        MACOSX_BUNDLE_ICON_FILE
            "GVPlayer.icns"

        OUTPUT_NAME
            "GVPlayer"
)
