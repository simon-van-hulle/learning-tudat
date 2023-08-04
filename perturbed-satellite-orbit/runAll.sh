#!/bin/bash

# Tell what the script will do and ask if the user wants to continue

echo "Do you want to run the program? (y/n)"
read answer
if [ "$answer" != "${answer#[Yy]}" ]; then
    make
    echo
    echo
    echo Runing C++ version
    echo
    echo
    ./build/perturbed-sat

    echo
    echo
    echo Runing Python version
    echo
    echo
    ../utils/runPythonCustomTudatKernel.sh perturbed_satellite_orbit.py
fi
