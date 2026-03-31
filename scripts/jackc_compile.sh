#!/bin/sh
if [ $# -lt 1 ]; then
    echo "Usage: $0 <path_to_rars> <jack_sources_dir> <output_dir>"
    exit 1
fi

rars_path="$1"
jack_sources_dir="$2"
output_dir="$3"

jack_sources_file="jack-sources.txt"
jack_vm_sources_file="jack-vm-sources.txt"

find $jack_sources_dir -name "*.jack" -exec realpath {} \; > $jack_sources_file
java -jar $rars_path "jackc_frontend.s" pa $jack_sources_file $output_dir

find $base_dir -name "*.vm" -exec realpath {} \; > $jack_vm_sources_file
java -jar $rars_path "jackc_backend.s" pa $jack_vm_sources_file $output_dir
