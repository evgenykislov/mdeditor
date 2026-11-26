#!/bin/sh
# Builds the development driver of the Markdown converter.
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
SRC=${1:-$HOME/programm/mdeditor/src}
g++ -std=c++17 -fPIC -Wall -Wextra -Wpedantic -I "$SRC" "$HERE/mdtest.cpp" \
    $(ls "$SRC"/core/markdown_*.cpp) \
    -o "$HERE/mdtest" $(pkg-config --cflags Qt5Core) -l:libQt5Core.so.5
