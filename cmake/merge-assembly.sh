#!/bin/sh
set -e

BIN_DIR="$1"
ASM_DIR="$2"

mkdir -p "$ASM_DIR"
rm -rf "$ASM_DIR"/*.s

find . -maxdepth 1 -name "*.s" -exec mv {} "$ASM_DIR" \;

for f in "$ASM_DIR"/*.s; do
    [ -f "$f" ] || continue

    base="$(basename "$f" .s)"
    l_tmp="${ASM_DIR}/${base}.label.tmp"
    f_tmp="${ASM_DIR}/${base}.func.tmp"

    # Namespace local labels per file (.L* labels)
    sed -E "s/\.L([A-Za-z0-9_]+)/.L_${base}_\1/g" "$f" > "$l_tmp"
    # Namespace __func__ symbols
    sed -E "s/__func__\.([0-9]+)/__func__${base}_\1/g" "$l_tmp" > "$f_tmp"

    mv "$f_tmp" "$f"
done

cat "${ASM_DIR}"/*.s > "$BIN_DIR"/jackc.s
sed -i.bak '/^[[:space:]]*\.string[[:space:]]*"main"/d' "$BIN_DIR/jackc.s"
