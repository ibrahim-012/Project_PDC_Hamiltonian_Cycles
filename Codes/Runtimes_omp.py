import matplotlib.pyplot as plt

# Read data from the file
data = []

with open("Metrics_omp.txt", "r") as file:
    for line in file:
        dataset_size, num_threads, execution_time = map(float, line.split())
        data.append((int(dataset_size), int(num_threads), execution_time))

# Extract dataset sizes, threads, and execution times
dataset_sizes = sorted(set(x[0] for x in data))
threads = sorted(set(x[1] for x in data))
execution_times = {thread: [] for thread in threads}

# Organize the data by threads
for dataset_size in dataset_sizes:
    for thread in threads:
        for entry in data:
            if entry[0] == dataset_size and entry[1] == thread:
                execution_times[thread].append(entry[2])

# Plot the data
plt.figure(figsize=(10, 6))

for thread in threads:
    plt.plot(dataset_sizes, execution_times[thread], label=f'{thread} threads', marker='o')

# Add labels and title
plt.title('OpenMP Execution Times')
plt.xlabel('Dataset Size')
plt.ylabel('Execution Time (seconds)')
plt.legend()

# Display the plot
plt.grid(True)
plt.show()
