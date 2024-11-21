#!/bin/bash

# Define the range of input sizes and number of threads
start_size=1000
end_size=8000
step_size=1000
thread_counts=(2 4 8)

# Compile the parallel OpenMP program once
make parallel_omp

# Loop through the input sizes and thread counts
for size in $(seq $start_size $step_size $end_size); do
    for threads in "${thread_counts[@]}"; do
        echo "$size" | ./TSP_P_OMP "$threads"
    done
done
