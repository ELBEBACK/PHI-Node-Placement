#!/bin/bash
set -e


DOT_DIR="output/dot"
PNG_DIR="output/png"

mkdir -p "$PNG_DIR"

for dot_file in "$DOT_DIR"/*.dot; do
    [ -f "$dot_file" ] || continue
    name=$(basename "$dot_file" .dot)
    dot -Tpng "$dot_file" -o "$PNG_DIR/$name.png"
    echo "converted $name.png"
done