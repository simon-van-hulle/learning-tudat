#!/bin/bash

# Constant precision value
precision=1

# Function to process a single file in-place
process_file() {
    local filename="$1"

    # Check if the file exists
    if [ ! -f "$filename" ]; then
        echo "File '$filename' not found."
        return
    fi

    # Process the file in-place using sed
    sed -E -i 's/[[:space:]]+/\t/g' "$filename"
    
    # Format regular numbers with specified precision and add trailing zeroes
    awk -i inplace -v prec="$precision" '
        BEGIN {
            FS = "\t";
            OFS = "\t";
        }
        {
            for (i=1; i<=NF; i++) {
                if ($i ~ /^[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)$/) {
                    # Convert the number to fixed-point format with the specified precision
                    $i = sprintf("%.*f", prec, $i);
                }
            }
            print;
        }
    ' "$filename"
    echo "Standardised file '$filename'."
}

# Check if the number of arguments is correct
if [ $# -lt 1 ]; then
    echo "Usage: $0 <file1> [file2] [file3] ..."
    exit 1
fi

# Process each file in-place
for filename in "$@"; do
    process_file "$filename"
done







# echo This script removes trailing zeros from all numbers in the given files. It will affect the following files:
# echo "$@"
# read answer



# for f in "$@"; do
#     echo "Processing $f"
#     # Remove trailing zeroes from decimal numbers
#     sed -i -E 's/\b([0-9]+)\.0+\b/\1/g; s/\b([0-9]+\.[0-9]*[1-9])0+\b/\1/g' $f

#     # Replace spaces between numbers with a single tab
#     sed -i -E 's/([0-9]) +([0-9])/\1\t\2/g' $f
# done
