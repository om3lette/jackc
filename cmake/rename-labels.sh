#!/bin/sh
set -eu

BIN_DIR="$1"

for f in "$BIN_DIR"/*.s; do
    [ -f "$f" ] || continue

    base="$(basename "$f" .s)"
    l_tmp="${base}.label.tmp"
    f_tmp="${base}.func.tmp"

    # Namespace local labels per file (.L* labels)
    sed -E "s/\.L([A-Za-z0-9_]+)/.L_${base}_\1/g" "$f" > "$l_tmp"

    # Namespace __func__ symbols
    sed -E "s/__func__\.([0-9]+)/__func__${base}_\1/g" "$l_tmp" > "$f_tmp"

    mv "$f_tmp" "$f"
done
