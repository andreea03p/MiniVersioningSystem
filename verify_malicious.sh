#!/bin/bash


# Check if a single argument (file path) is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file_path> <isolated_space_dir>"
    exit 1
fi

file_path=$1
isolated_space_dir=$2
echo "File path: $file_path"
echo "Isolated space directory: $isolated_space_dir"

# Check if the file exists
if [ ! -f "$file_path" ]; then
    echo "File $file_path does not exist."
    exit 1
fi

# Change permissions of the file
chmod 777 "$file_path"
if [ $? -ne 0 ]; then
    echo "Failed to change permissions of file $file_path."
    exit 1
fi

# Check if the file contains non-ASCII characters
if LC_ALL=C grep -q '[^ -~]' "$file_path"; then
    echo "File $file_path contains non-ASCII characters."
    exit 1
fi

# Count the number of lines, words, and characters in the file
num_lines=$(wc -l < "$file_path")
num_words=$(wc -w < "$file_path")
num_chars=$(wc -c < "$file_path")

echo "Number of lines in file: $num_lines"
echo "Number of words in file: $num_words"
echo "Number of characters in file: $num_chars"

# Check for keywords associated with corrupted or malicious files
if grep -q -E 'corrupted|dangerous|risk|attack|malware|malicious' "$file_path"; then
    echo "File $file_path is considered dangerous. Moving to isolated space..."
    mv "$file_path" "$isolated_space_dir"
    if [ $? -eq 0 ]; then
        echo "File moved successfully."
    else
        echo "Failed to move file."
        exit 1
    fi
fi

new_file_path="$isolated_space_dir/$(basename "$file_path")"

chmod 000 "$file_path"
if [ $? -ne 0 ]; then
    echo "Failed to change permissions of file $new_file_path."
    exit 1
fi

