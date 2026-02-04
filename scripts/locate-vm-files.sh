#!/bin/sh
if [ $# -lt 1 ]; then
    echo "Usage: $0 <base_directory>"
    exit 1
fi

base_dir="$1"
find "$base_dir" -name "*.vm" -exec realpath {} \; > jack-vm-sources.txt
