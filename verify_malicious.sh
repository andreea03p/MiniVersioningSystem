#!/bin/bash

# Function to evaluate file based on criteria
evaluate_file() {
    local file_path=$1
    local isolated_space_dir=$2

    num_lines=$(wc -l < "$file_path")
    num_words=$(wc -w < "$file_path")
    num_chars=$(wc -m < "$file_path")

    # Check if the file meets suspicious criteria
    if [ "$num_lines" -lt 3 ] && [ "$num_words" -gt 1000 ] && [ "$num_chars" -gt 2000 ]; then
    	echo "$(basename "$file_path") is SUSPICIOUS"
        if LC_ALL=C grep -q '[^\x00-\x7F]' "$file_path" || grep -q -E 'corrupted|dangerous|risk|attack|malware|malicious' "$file_path"; then
            echo "$(basename "$file_path") is DANGEROUS"
            mv "$file_path" "$isolated_space_dir"
	    if [ $? -eq 0 ]; then
	    	echo "$(basename "$file_path") moved to ISOLATED DIR successfully"
	    else
 	    	echo "Failed to move $(basename "$file_path") to ISOLATED DIR"
	    fi
        else
            echo "$(basename "$file_path") is SAFE"
            chmod 000 "$file_path"
	    if [ $? -ne 0 ]; then
	    	echo "Error: Failed to change permissions of $file_path."
	    	exit 1
	    fi
        fi
    else
        echo "$(basename "$file_path") is SAFE"
        chmod 000 "$file_path"
	if [ $? -ne 0 ]; then
	    echo "Error: Failed to change permissions of $file_path."
	    exit 1
	fi
    fi
}

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file_path> <isolated_space_dir>"
    exit 1
fi

file_path=$1
isolated_space_dir=$2

# Check if the file exists
if [ ! -f "$file_path" ]; then
    echo "Error: File $file_path does not exist."
    exit 1
fi


# Change permissions of the file to make it writable
chmod 777 "$file_path"
if [ $? -ne 0 ]; then
    echo "Error: Failed to change permissions of $file_path."
    exit 1
fi

# Check if the file is readable
if [ ! -r "$file_path" ]; then
    echo "Error: File $file_path is not readable."
    exit 1
fi


# Evaluate the file
evaluate_file "$file_path" "$isolated_space_dir"

