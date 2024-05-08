#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file_path> <destination_dir>"
    exit 1
fi

file_path=$(realpath "$1")
destination_dir=$(realpath "$2")

if [ ! -f "$file_path" ]; then
    echo "Error: File $file_path does not exist."
    exit 1
fi

if [ ! -d "$destination_dir" ]; then
    echo "Error: Destination directory $destination_dir does not exist."
    exit 1
fi

chmod 777 "$file_path" || { echo "Error: Failed to change permissions of $file_path."; exit 1; }

# Replace all slashes in the path with underscores for the filename
filename=$(echo "$file_path" | tr '/' '_')

destination_file="$destination_dir/$filename"

if [ -e "$destination_file" ]; then
    echo "Error: File $filename already exists in $destination_dir."
    exit 1
fi

mv "$file_path" "$destination_file" && \
    echo "File moved successfully to $destination_dir as $filename." || \
    echo "Failed to move file to $destination_dir."
