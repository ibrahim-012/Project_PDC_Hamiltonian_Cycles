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
void update_metrics(int n, int threads, double time) {
    ifstream inFile("Metrics_mpi.txt");
    ofstream outFile("Metrics_mpi_tmp.txt");

    string line;
    bool updated = false;
    
    while (getline(inFile, line)) {
        stringstream ss(line);
        int file_n, file_threads;
        double file_time;
        
        ss >> file_n >> file_threads >> file_time;

        if (file_n == n && file_threads == threads) {
            if (time < file_time) {
                outFile << n << " " << threads << " " << time << endl;
                updated = true;
            } else {
                outFile << line << endl;
            }
        } else {
            outFile << line << endl;
        }
    }

    if (!updated) {
        outFile << n << " " << threads << " " << time << endl;
    }

    inFile.close();
    outFile.close();

    remove("Metrics_mpi.txt");
    rename("Metrics_mpi_tmp.txt", "Metrics_mpi.txt");
}

void BFS(int node, int **arr, int **vis, int n) {
    int start_node = node;
    int min;
    int min_index;
    int visited_count = 0;
    int cost = 0;

    vis[node][node] = 1;
    visited_count++;

    while (visited_count < n) {
        min = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (arr[start_node][i] < min && start_node != i && vis[node][i] == 0) {
                min = arr[start_node][i];
                min_index = i;
            }
        }
        start_node = min_index;
        cost += min;
        vis[node][min_index] = 1;
        visited_count++;
    }
    cost += arr[min_index][node];

    cout << endl << "Vertex: " << node << "\tCost: " << cost;
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    if (rank == 0) {
        cout << "Enter number of vertices {min 2 vertices}: ";
        cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int **arr = new int *[n];
    for (int i = 0; i < n; i++) {
        arr[i] = new int[n];
    }

    int **vis = new int *[n];
    for (int i = 0; i < n; i++) {
        vis[i] = new int[n];
    }

    srand(time(NULL));

    double init_start = MPI_Wtime();
    if (rank == 0) {
        for (int i = 0; i <= n - 2; i++) {
            for (int j = i + 1; j < n; j++) {
                int val = rand() % 20;
                arr[i][j] = val + 5;
                arr[j][i] = arr[i][j];
            }
        }

        for (int i = 0; i < n; i++) {
            arr[i][i] = 0;
        }
    }

    MPI_Bcast(&arr[0][0], n * n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double init_end = MPI_Wtime();

    double init_time = init_end - init_start;

    double bfs_start = MPI_Wtime();
    int nodes_per_process = n / size;
    int start_idx = rank * nodes_per_process;
    int end_idx = (rank + 1) * nodes_per_process;
    if (rank == size - 1) {
        end_idx = n;
    }

    for (int i = start_idx; i < end_idx; i++) {
        BFS(i, arr, vis, n);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double bfs_end = MPI_Wtime();

    double bfs_time = bfs_end - bfs_start;

    if (rank == 0) {
        cout << endl << "Number of vertices (input size): " << n;

        cout << endl << "Number of processes: " << size;

        cout << endl << "Initialization time: " << (int)(init_time / 60) << " minutes "
             << fixed << setprecision(6) << fmod(init_time, 60) << " seconds";

        cout << endl << "BFS time: " << (int)(bfs_time / 60) << " minutes "
             << fixed << setprecision(6) << fmod(bfs_time, 60) << " seconds" << endl;

        update_metrics(n, size, bfs_time);
    }

    for (int i = 0; i < n; i++) {
        delete[] arr[i];
        delete[] vis[i];
    }
    delete[] arr;
    delete[] vis;

    MPI_Finalize();
    return 0;
}
