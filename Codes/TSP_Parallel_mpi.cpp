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

    vis[node * n + node] = 1;
    visited_count++;

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

    cost += arr[min_index * n + node];

    cout << "Vertex: " << node << "\tCost: " << cost << endl;
}

int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    if (rank == 0)
    {
        cout << "Enter number of vertices {min 2 vertices}: ";
        cin >> n;
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int *arr = new int[n * n];
    int *vis = new int[n * n];
    MPI_Barrier(MPI_COMM_WORLD);

    double init_start = MPI_Wtime();
    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = i + 1; j < n; j++)
            {
                int val = rand() % 20 + 5;
                arr[i * n + j] = val;
                arr[j * n + i] = val;
            }
            arr[i * n + i] = 0;
        }

        fill(vis, vis + n * n, 0);
    }

    double init_end = MPI_Wtime();
    double init_time = init_end - init_start;

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(arr, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(vis, n * n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    int nodes_per_process = n / size;
    int remainder = n % size;
    int start = rank * nodes_per_process + min(rank, remainder);
    int end = start + nodes_per_process + (rank < remainder ? 1 : 0);

    double bfs_start = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    for (int i = start; i < end; i++)
    {
        BFS(i, arr, vis, n);
    }

    double bfs_end = MPI_Wtime();
    double bfs_time = bfs_end - bfs_start;
    MPI_Barrier(MPI_COMM_WORLD);

    double global_bfs_time;
    MPI_Reduce(&bfs_time, &global_bfs_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

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

    delete[] arr;
    delete[] vis;

    MPI_Finalize();

    return 0;
}
