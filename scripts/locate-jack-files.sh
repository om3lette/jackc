#!/bin/sh
if [ $# -lt 2 ]; then
    echo "Usage: $0 <base_directory> <std_directory>"
    exit 1
fi

base_dir="$1"
std_dir="$2"
find "$base_dir" -name "*.jack" -exec realpath {} \; > jack-sources.txt
find "$std_dir" -name "*.jack" -exec realpath {} \; >> jack-sources.txt
