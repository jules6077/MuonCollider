#!/bin/bash

# Check if at least one argument is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <parameter>"
    exit 1
fi

# Accessing the first argument passed to the script
rootFile1="$1"
rootFile2="$2"
layer="$3"
key="$4"

root -l "../analysis_code/trackerHit_TruthDigiReco.C(\"$rootFile1\", \"$rootFile2\", \"$layer\", \"$key\")"
