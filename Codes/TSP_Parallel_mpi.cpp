#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <mpi.h>

using namespace std;

// Helper function to check if an entry exists in the file and update it
void update_metrics(int n, int threads, double time)
{
    ifstream inFile("Metrics_mpi.txt");
    ofstream outFile("Metrics_mpi_tmp.txt");

    string line;
    bool updated = false;

    while (getline(inFile, line))
    {
        stringstream ss(line);
        int file_n, file_threads;
        double file_time;

        ss >> file_n >> file_threads >> file_time;

        if (file_n == n && file_threads == threads)
        {
            if (time < file_time)
            {
                outFile << n << " " << threads << " " << time << endl;
                updated = true;
            }
            else
            {
                outFile << line << endl;
            }
        }
        else
        {
            outFile << line << endl;
        }
    }

    if (!updated)
    {
        outFile << n << " " << threads << " " << time << endl;
    }

    inFile.close();
    outFile.close();
    remove("Metrics_mpi.txt");
    rename("Metrics_mpi_tmp.txt", "Metrics_mpi.txt");
}

void BFS(int node, int *arr, int *vis, int n)
{
    int start_node = node;
    int min;
    int min_index;
    int visited_count = 0;
    int cost = 0;

    // mark starting node as visited
    // e.g. n = 5
    // e.g. 2nd vertex is being visited of total 5 i.e. entry arr[1][0]
    // it will be the 5th entry if the array was one-dimensional
    // i.e. 1*4+1 = 5
    vis[node * n + node] = 1;

    // increment visited nodes count --- all nodes should be visited
    visited_count++;

    // visited_count == n means all nodes have been visited
    while (visited_count < n)
    {
        min = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (arr[start_node * n + i] < min && start_node != i && vis[node * n + i] == 0)
            {
                min = arr[start_node * n + i];
                min_index = i;
            }
        }

        start_node = min_index;
        cost += min;
        vis[node * n + min_index] = 1;
        visited_count++;
    }
    // add edge weight between last node visted to starting node
    cost += arr[min_index * n + node];

    cout << "Vertex: " << node << "\tCost: " << cost << endl;
}

int main(int argc, char **argv)
{
    // rank holds process number of calling process
    // size holds number of processes in the communicator
    int rank, size;

    // start of parallel section
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;

    // only one process will take input
    if (rank == 0)
    {
        cout << "Enter number of vertices {min 2 vertices}: ";
        cin >> n;
    }
    // barrier to make sure no process starts allocating memory before n has been initialized
    MPI_Barrier(MPI_COMM_WORLD);

    int *arr = new int[n * n];
    int *vis = new int[n * n];

    // barrier to make sure all processes have allocated memory before initializion can start
    MPI_Barrier(MPI_COMM_WORLD);

    // initialization start
    double init_start = MPI_Wtime();

    // only one process can initialize arr because of dependency
    // entry arr[i][j] should match entry arr[j][i] in simple graph
    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                int val = rand() % 20 + 5; // to ensure only diagonal entries are zero
                arr[i * n + j] = val;
                arr[j * n + i] = val;
            }
            // mark diagonal entries 0 since there are no loops
            arr[i * n + i] = 0;
        }

        // initialize visited array
        // fill function marks all entries b/w starting address and ending address a particular value
        // 0 in this case
        fill(vis, vis + n * n, 0);
    }

    // initialization end
    double init_end = MPI_Wtime();

    // calculate time taken for initialization (weights array, visited array)
    double init_time = init_end - init_start;

    // broadcast number of vertices to all other processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // barrier to make sure all processes have received value of n before receiving other values
    MPI_Barrier(MPI_COMM_WORLD);

    // broadcast edge weights array to all other processes
    MPI_Bcast(arr, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    // barrier to make sure all processes have received value of n before receiving other values
    MPI_Barrier(MPI_COMM_WORLD);

    // broadcast visited array to all other processes
    MPI_Bcast(vis, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    // barrier to make sure all processes have received value of n before receiving other values
    MPI_Barrier(MPI_COMM_WORLD);

    // calculate start and end indices for each process to divide computations
    int nodes_per_process = n / size;
    int remainder = n % size;
    int start = rank * nodes_per_process + min(rank, remainder);
    int end = start + nodes_per_process + (rank < remainder ? 1 : 0);

    // processing start
    double bfs_start = MPI_Wtime();

    // barrier to make sure all processes have computed indices and initialized bfs_start value
    MPI_Barrier(MPI_COMM_WORLD);

    // calling BFS function for each vertex
    for (int i = start; i < end; i++)
    {
        BFS(i, arr, vis, n);
    }
    double bfs_end = MPI_Wtime();
    // processing end

    // calculate time taken for finding Hamiltonian cycle for each node FOR ONE PROCESS
    double bfs_time = bfs_end - bfs_start;

    // barrier to make sure all processes have calculated their partial values for processing time
    MPI_Barrier(MPI_COMM_WORLD);

    // declare global variable to aggregate partial values
    double global_bfs_time;

    // reduce all partial values into global value to compute total processing time
    MPI_Reduce(&bfs_time, &global_bfs_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);

    // barrier to make sure all processes have sent their partial values to master process for reducing
    // necessary to ensure no process disrupts process 0 when writing to terminal
    MPI_Barrier(MPI_COMM_WORLD);

    // only one process will print the input size, number of processes, initialization time, processing time
    if (rank == 0)
    {
        cout << "\nNumber of vertices (input size): " << n << endl;
        cout << "Number of processes: " << size << endl;
        cout << "Initialization time: " << (int)(init_time / 60) << " minutes "
             << fixed << setprecision(6) << fmod(init_time, 60) << " seconds" << endl;
        cout << "BFS time: " << (int)(global_bfs_time / 60) << " minutes "
             << fixed << setprecision(6) << fmod(global_bfs_time, 60) << " seconds" << endl;

        update_metrics(n, size, global_bfs_time);
    }

    // free memory
    delete[] arr;
    delete[] vis;

    // end of parallel section
    MPI_Finalize();

    return 0;
}
