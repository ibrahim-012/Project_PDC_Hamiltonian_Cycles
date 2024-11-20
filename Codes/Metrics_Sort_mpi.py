# Define the file path
file_path = "Metrics_mpi.txt"

# Read the data from the file
with open(file_path, "r") as file:
    lines = file.readlines()

# Parse the data into a list of tuples (input_size, num_threads, time)
data = []
for line in lines:
    input_size, num_threads, time = line.split()
    data.append((int(input_size), int(num_threads), float(time)))

# Sort the data based on input_size, then num_threads
sorted_data = sorted(data, key=lambda x: (x[0], x[1]))

# Write the sorted data back to the same file
with open(file_path, "w") as file:
    for entry in sorted_data:
        file.write(f"{entry[0]} {entry[1]} {entry[2]:.5f}\n")
