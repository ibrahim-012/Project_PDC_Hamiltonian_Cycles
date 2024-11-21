import matplotlib.pyplot as plt
import pandas as pd
from collections import defaultdict

# Read and parse the file for MPI
def parse_file(file_path):
    data = defaultdict(lambda: defaultdict(dict))  # {n: {processes: MPI time}}
    with open(file_path, 'r') as f:
        for line in f:
            n, processes, mpi_time = line.split()
            n = int(n)
            processes = int(processes)
            mpi_time = float(mpi_time)
            data[n][processes] = mpi_time
    return data

# Calculate Speedup and Efficiency for MPI
def calculate_speedup_efficiency(data):
    speedup = defaultdict(list)  # {n: [(processes, speedup)]}
    efficiency = defaultdict(list)  # {n: [(processes, efficiency)]}
    for n, process_data in data.items():
        serial_time = process_data[1]  # Serial time is when processes = 1
        for processes, mpi_time in process_data.items():
            spd = serial_time / mpi_time
            eff = spd / processes
            if processes != 1:  # No need to include serial run in graphs
                speedup[n].append((processes, spd))
                efficiency[n].append((processes, eff))
    return speedup, efficiency

# Plot graphs in the same window for MPI
def plot_graphs_together(speedup, efficiency):
    # Create a figure with two subplots (1 row, 2 columns)
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))  # 1 row, 2 columns

    # Speedup Graph
    for n, values in speedup.items():
        processes, spds = zip(*values)
        axes[0].plot(processes, spds, label=f'Dataset Size: {n}', marker='o')
    axes[0].set_title('MPI Speedup Analysis')
    axes[0].set_xlabel('Number of Processes')
    axes[0].set_ylabel('Speedup')
    axes[0].legend()
    axes[0].grid(True)
    axes[0].set_xticks(sorted(set(x for x, _ in values)))  # Set integer tick marks

    # Efficiency Graph
    for n, values in efficiency.items():
        processes, effs = zip(*values)
        axes[1].plot(processes, effs, label=f'Dataset Size: {n}', marker='o')
    axes[1].set_title('MPI Efficiency Analysis')
    axes[1].set_xlabel('Number of Processes')
    axes[1].set_ylabel('Efficiency')
    axes[1].legend()
    axes[1].grid(True)
    axes[1].set_xticks(sorted(set(x for x, _ in values)))  # Set integer tick marks

    # Adjust layout to prevent overlapping
    plt.tight_layout()
    plt.show()


# Main function to run the workflow
def main():
    file_path = 'Metrics_mpi.txt'  # Update the file path to Metrics_mpi.txt
    data = parse_file(file_path)
    speedup, efficiency = calculate_speedup_efficiency(data)
    plot_graphs_together(speedup, efficiency)  # Plot both graphs in the same window

if __name__ == '__main__':
    main()
