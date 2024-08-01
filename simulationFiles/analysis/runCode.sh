#!/bin/bash

# Check if at least one argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <parameter>"
    exit 1
fi

# Accessing the first argument passed to the script
rootFile_sig="$1"
rootFile_sigBIB="$2"
rootFile_digi="$3"
rootFile_digiBIB="$4"
layer="$5"

root -l "../analysis_code/trackerHit_analysis.C(\"$rootFile_sig\", \"$rootFile_sigBIB\",\"$rootFile_digi\", \"$rootFile_digiBIB\", \"$layer\")"
