#!/bin/bash

# Define the range of input sizes and number of processes
start_size=1000
end_size=8000
step_size=1000
process_counts=(2 3 4)

# Compile the parallel MPI program once
make parallel_mpi

# Loop through the input sizes and process counts
for size in $(seq $start_size $step_size $end_size); do
    for proc in "${process_counts[@]}"; do
        echo "Running with input size $size and $proc processes..."

        # Run the MPI program with the specified input size and processes
        echo "$size" | make run_parallel_mpi ARG=$proc
    done
done

echo "All executions completed."
