#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <string>
#include <iomanip>
#include <omp.h>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// Helper function to check if an entry exists in the file and update it
void update_metrics(int n, int threads, double time) {
    // Open the file for reading and writing
    ifstream inFile("Metrics_omp.txt");
    ofstream outFile("Metrics_omp_tmp.txt");

    string line;
    bool updated = false;
    
    while (getline(inFile, line)) {
        stringstream ss(line);
        int file_n, file_threads;
        double file_time;
        
        ss >> file_n >> file_threads >> file_time;

        if (file_n == n && file_threads == threads) {
            // If the combination of n and threads exists and new time is smaller, update it
            if (time < file_time) {
                outFile << n << " " << threads << " " << time << endl;
                updated = true;
            } else {
                outFile << line << endl;  // Retain the old entry if new time is not smaller
            }
        } else {
            outFile << line << endl;  // Copy the existing entry to the new file
        }
    }

    // If the entry was not updated, append the new entry
    if (!updated) {
        outFile << n << " " << threads << " " << time << endl;
    }

    inFile.close();
    outFile.close();

    // Replace old file with the new file
    remove("Metrics_omp.txt");
    rename("Metrics_omp_tmp.txt", "Metrics_omp.txt");
}

void BFS(int node, int **arr, int **vis, int n) {
    int start_node = node;
    int min;
    int min_index;
    int visited_count = 0;
    int cost = 0;

    // mark starting node as visited
    vis[node][node] = 1;

    // increment visited nodes count --- all nodes should be visited
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

    cout << endl
         << "Vertex: " << node << "\tCost: " << cost;
}

int main(int argc, char **argv) {
    int thread_count = atoi(argv[1]);
    omp_set_num_threads(thread_count);

    cout << "Enter number of vertices {min 2 vertices}: ";
    int n;
    cin >> n;

    // allocation of array - entries represent edge weights
    int **arr = new int *[n];
    for (int i = 0; i < n; i++) {
        arr[i] = new int[n];
    }

    // allocation of visited array - entries represent visited vertex
    int **vis = new int *[n];
    for (int i = 0; i < n; i++) {
        vis[i] = new int[n];
    }

    srand(time(NULL));

    // initialization start
    double init_start = omp_get_wtime();
    for (int i = 0; i <= n - 2; i++) {
        for (int j = i + 1; j < n; j++) {
            int val = rand() % 20;
            arr[i][j] = val + 5;
            arr[j][i] = arr[i][j];
        }
    }

    // mark diagonal entries 0 since there are no loops
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < n; i++) {
        arr[i][i] = 0;
    }

    // initialize visited array
#pragma omp parallel for collapse(2) num_threads(thread_count)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            vis[i][j] = 0;
        }
    }
    double init_end = omp_get_wtime();
    // initialization end

    // calculate time taken for initialization (weights array, visited array)
    double init_time = init_end - init_start;

    // processing start
    double bfs_start = omp_get_wtime();

    // calling BFS function for each vertex
#pragma omp parallel for num_threads(thread_count) schedule(guided)
    for (int i = 0; i < n; i++) {
        BFS(i, arr, vis, n);
    }
    double bfs_end = omp_get_wtime();

    // calculate time taken for finding Hamiltonian cycle for each node
    double bfs_time = bfs_end - bfs_start;

    // print input size
    cout << endl
         << "Number of vertices (input size): " << n;

    // print number of threads assigned
#pragma omp parallel
    {
#pragma omp single
        {
            cout << endl
                 << "Number of threads: " << omp_get_num_threads();
        }
    }

    // print initialization time
    cout << endl
         << "Initialization time: " << (int)(init_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(init_time, 60) << " seconds";

    // print processing time
    cout << endl
         << "BFS time: " << (int)(bfs_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(bfs_time, 60) << " seconds" << endl;

    // Update the metrics file with the new data
    update_metrics(n, thread_count, bfs_time);

    // free memory
    for (int i = 0; i < n; i++) {
        delete[] arr[i];
        delete[] vis[i];
    }
    delete[] arr;
    delete[] vis;

    return 0;
}
