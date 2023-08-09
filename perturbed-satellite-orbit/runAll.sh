#!/bin/bash

empty_lines() {
    local num_lines=$1
    for (( i = 1; i <= num_lines; i++ )); do
        echo
    done
}

# Function to draw a line of 80 ~ characters
line() {
    local num_chars=80
    for (( i = 1; i <= num_chars; i++ )); do
        echo -n "-"
    done
    echo
}

check_first_lines_equal() {
    first_line_file1=$(head -n 1 "$1")
    first_line_file2=$(head -n 1 "$2")

    if [ "$first_line_file1" == "$first_line_file2" ]; then
        echo "The first lines are equal."
    else
        echo "The first lines are different."
    fi
}



# Tell what the script will do and ask if the user wants to continue

echo This script will do the following:
echo 1. Build the C++ code
echo 2. Run the C++ code
echo 3. Run the Python code
echo 4. Compare the output of the C++ and Python code
echo 5. Postprocess the output of the C++ and Python code
echo
echo Press enter to continue
read continue

make

empty_lines 2
echo Runing C++ version
empty_lines 2
./build/perturbed-sat

empty_lines 2
echo Runing Python version
empty_lines 2
../utils/runPythonCustomTudatKernel.sh perturbed_satellite_orbit.py

empty_lines 2   
line
echo Standardising output .dat files for comparison
../utils/standardDatFile.sh output/*/*.dat
empty_lines 2


line
echo Comparing state history

state_diff=$(diff output/**/state.dat | tee output/state_diff.dat | wc -l) && echo Diff written to output/state_diff.dat
echo Length of state.dat diff: $state_diff
check_first_lines_equal output/python/state.dat output/cpp/state.dat

empty_lines 2

depvar_diff=$(diff output/**/depvar.dat | tee output/depvar_diff.dat | wc -l) && echo Diff written to output/depvar_diff.dat
echo Length of depvar.dat diff: $depvar_diff
check_first_lines_equal output/python/depvar.dat output/cpp/depvar.dat

empty_lines 5

python postprocess.py
echo Postprocessing is done for both the C++ and Python results and written to respective subdirectories
line
