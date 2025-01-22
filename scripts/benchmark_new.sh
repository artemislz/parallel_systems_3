#!/bin/bash

# Array of matrix sizes
matrix_sizes=(8192 16384 32768)

# Loop through each size
for size in "${matrix_sizes[@]}"
do
    echo "Running benchmark for matrix size: $size"
    mpiexec -f machines -n $1 ./matrix_mul $size
    echo "----------------------------------------"
done