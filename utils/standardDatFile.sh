#!/bin/bash

# Constant precision value
precision=9

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

    # Convert numbers to scientific notation and adjust decimals with specified precision
    awk -i inplace -v prec="$precision" '
        BEGIN {
            FS = "\t";
            OFS = "\t";
        }

        function convert_to_scientific(num) {
            return sprintf("%.*e", prec - 1, num);
        }

        function adjust_to_precision(num, prec) {
            split(num, parts, /[eE]/);
            base = sprintf("%.*f", prec, parts[1]);
            return base "e" parts[2];
        }

        {
            for (i=1; i<=NF; i++) {
                if ($i ~ /^[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)[eE][+-]?[0-9]+$/) {
                    # Convert the scientific notation to fixed-point format with the specified precision
                    $i = adjust_to_precision($i, prec);
                } else if ($i ~ /^[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)$/) {
                    # Convert the regular number to scientific notation
                    $i = convert_to_scientific($i);
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
