cmake_minimum_required(VERSION 3.16)

project(mdeditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

# Without a choice of the configuration the program is built the way it is
# run: with optimizations. The preview has to follow the typing without delay.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "The configuration of the build." FORCE)
endif()

include(CTest)
include(GNUInstallDirs)

# Qt 5.15 is the oldest version whose rich text engine knows Markdown
# structures that the preview is made of.
find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets)

# Applies the compiler settings that are common for all the targets.
function(mdeditor_configure_target target)
    if(MSVC)
        # /utf-8: the sources and the executable character set are UTF-8.
        target_compile_options(${target} PRIVATE /W4 /permissive- /utf-8 /Zc:__cplusplus)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Everything that does not need widgets: the Markdown converter, the
# documents that are made of the Markdown text, and the files.
add_library(mdeditor_core STATIC
    src/core/document_io.cpp
    src/core/document_io.h
    src/core/html_entities.inc
    src/core/html_page.cpp
    src/core/html_page.h
    src/core/markdown_block.cpp
    src/core/markdown_inline.h
    src/core/markdown_inline_emphasis.cpp
    src/core/markdown_inline_links.cpp
    src/core/markdown_inline_parse.cpp
    src/core/markdown_inline_parser.h
    src/core/markdown_inline_render.cpp
    src/core/markdown_inline_scan.cpp
    src/core/markdown_link.cpp
    src/core/markdown_parser.h
    src/core/markdown_renderer.cpp
    src/core/markdown_renderer.h
    src/core/markdown_structure.cpp
    src/core/markdown_structure.h
    src/core/markdown_table.cpp
    src/core/rich_text_document.cpp
    src/core/rich_text_document.h
)
target_include_directories(mdeditor_core PUBLIC src)
target_link_libraries(mdeditor_core PUBLIC Qt5::Gui)
mdeditor_configure_target(mdeditor_core)

# The window of the program and its panels.
add_library(mdeditor_gui STATIC
    src/gui/main_window.cpp
    src/gui/main_window.h
    src/gui/main_window_files.cpp
    src/gui/markdown_editor.cpp
    src/gui/markdown_editor.h
    src/gui/markdown_preview.cpp
    src/gui/markdown_preview.h
)
target_link_libraries(mdeditor_gui PUBLIC mdeditor_core Qt5::Widgets)
target_compile_definitions(mdeditor_gui PUBLIC MDEDITOR_VERSION="${PROJECT_VERSION}")
mdeditor_configure_target(mdeditor_gui)

add_executable(mdeditor src/main.cpp)
target_link_libraries(mdeditor PRIVATE mdeditor_gui)
mdeditor_configure_target(mdeditor)
set_target_properties(mdeditor PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)
install(TARGETS mdeditor RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
