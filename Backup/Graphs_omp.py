import matplotlib.pyplot as plt
import pandas as pd
from collections import defaultdict

# Read and parse the file
def parse_file(file_path):
    data = defaultdict(lambda: defaultdict(dict))  # {n: {threads: BFS time}}
    with open(file_path, 'r') as f:
        for line in f:
            n, threads, bfs_time = line.split()
            n = int(n)
            threads = int(threads)
            bfs_time = float(bfs_time)
            data[n][threads] = bfs_time
    return data

# Calculate Speedup and Efficiency
def calculate_speedup_efficiency(data):
    speedup = defaultdict(list)  # {n: [(threads, speedup)]}
    efficiency = defaultdict(list)  # {n: [(threads, efficiency)]}
    for n, thread_data in data.items():
        serial_time = thread_data[1]  # Serial time is when threads = 1
        for threads, bfs_time in thread_data.items():
            spd = serial_time / bfs_time
            eff = spd / threads
            if threads != 1:  # No need to include serial run in graphs
                speedup[n].append((threads, spd))
                efficiency[n].append((threads, eff))
    return speedup, efficiency

# Plot graphs in the same window
def plot_graphs_together(speedup, efficiency):
    # Create a figure with two subplots (1 row, 2 columns)
    fig, axes = plt.subplots(1, 2, figsize=(15, 6))  # 1 row, 2 columns

    # Speedup Graph
    for n, values in speedup.items():
        threads, spds = zip(*values)
        axes[0].plot(threads, spds, label=f'Dataset Size: {n}', marker='o')
    axes[0].set_title('OpenMP Speedup Analysis')
    axes[0].set_xlabel('Number of Threads')
    axes[0].set_ylabel('Speedup')
    axes[0].legend()
    axes[0].grid(True)

    # Efficiency Graph
    for n, values in efficiency.items():
        threads, effs = zip(*values)
        axes[1].plot(threads, effs, label=f'Dataset Size: {n}', marker='o')
    axes[1].set_title('OpenMP Efficiency Analysis')
    axes[1].set_xlabel('Number of Threads')
    axes[1].set_ylabel('Efficiency')
    axes[1].legend()
    axes[1].grid(True)

    # Adjust layout to prevent overlapping
    plt.tight_layout()
    plt.show()

# Main function to run the workflow
def main():
    file_path = 'Metrics_omp.txt'  # Update the file path to Metrics_omp.txt
    data = parse_file(file_path)
    speedup, efficiency = calculate_speedup_efficiency(data)
    plot_graphs_together(speedup, efficiency)  # Plot both graphs in the same window

if __name__ == '__main__':
    main()
