#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file_path> <destination_dir>"
    exit 1
fi

file_path="$1"
destination_dir="$2"

if [ ! -f "$file_path" ]; then
    echo "Error: File $file_path does not exist."
    exit 1
fi

if [ ! -d "$destination_dir" ]; then
    echo "Error: Destination directory $destination_dir does not exist."
    exit 1
fi

chmod 777 "$file_path" || { echo "Error: Failed to change permissions of $file_path."; exit 1; }

filename=$(basename -- "$file_path")
destination_file="$destination_dir/$filename"

if [ -e "$destination_file" ]; then
    echo "Error: File $filename already exists in $destination_dir."
    exit 1
fi

mv "$file_path" "$destination_file" && \
    echo "File $filename moved successfully to $destination_dir." || \
    echo "Failed to move file $filename to $destination_dir."
