import matplotlib.pyplot as plt

# Initialize empty lists to store the data
dataset_sizes = set()
processes = set()
execution_data = {}

# Read data from the file
with open("Metrics_mpi.txt", "r") as file:
    for line in file:
        dataset_size, num_processes, execution_time = line.split()
        dataset_size = int(dataset_size)
        num_processes = int(num_processes)
        execution_time = float(execution_time)
        
        # Store the dataset sizes and processes
        dataset_sizes.add(dataset_size)
        processes.add(num_processes)

        # Organize data in a dictionary with dataset size and processes
        if dataset_size not in execution_data:
            execution_data[dataset_size] = {}
        execution_data[dataset_size][num_processes] = execution_time

# Sort the dataset sizes and processes
dataset_sizes = sorted(dataset_sizes)
processes = sorted(processes)

# Plot the data
plt.figure(figsize=(10, 6))

# Plot execution times for each number of processes
for process in processes:
    times = [execution_data[dataset_size].get(process, None) for dataset_size in dataset_sizes]
    plt.plot(dataset_sizes, times, label=f'{process} processes', marker='o')

# Add labels and title
plt.title('MPI Execution Times')
plt.xlabel('Dataset Size')
plt.ylabel('Execution Time (seconds)')
plt.legend()

# Display the plot
plt.grid(True)
plt.show()
