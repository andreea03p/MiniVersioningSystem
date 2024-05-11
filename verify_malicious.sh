#!/bin/bash

evaluate_file() 
{
    local file_path="$1"

    num_lines=$(wc -l < "$file_path")
    num_words=$(wc -w < "$file_path")
    num_chars=$(wc -m < "$file_path")

    # Check if the file meets suspicious criteria - non-ascii and specific words
    if [ "$num_lines" -lt 3 ] && [ "$num_words" -gt 1000 ] && [ "$num_chars" -gt 2000 ]; then
        if LC_ALL=C grep -q -P '[\x80-\xFF]' "$file_path" || grep -q -Ei 'corrupted|dangerous|risk|attack|malware|malicious' "$file_path"; then
            echo "$(basename "$file_path")"
        else
            echo "SAFE"
            chmod 000 "$file_path"
            if [ $? -ne 0 ]; then
                echo "Error: Failed to change permissions of $file_path."
                exit 1
            fi
        fi
    else
        echo "SAFE"
        chmod 000 "$file_path"
        if [ $? -ne 0 ]; then
            echo "Error: Failed to change permissions of $file_path."
            exit 1
        fi
    fi
}

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file_path>"
    exit 1
fi

file_path="$1"

if [ ! -f "$file_path" ]; then
    echo "Error: File $file_path does not exist."
    exit 1
fi

chmod 777 "$file_path"
if [ $? -ne 0 ]; then
    echo "Error: Failed to change permissions of $file_path."
    exit 1
fi

evaluate_file "$file_path"