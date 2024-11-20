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
using namespace std;

// Helper function to check if an entry exists in the file and update it for OpenMP
void update_metrics(int n, int threads, double time, const string& filename) {
    ifstream inFile(filename);
    ofstream outFile(filename + "_tmp.txt");

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

    remove(filename.c_str());
    rename((filename + "_tmp.txt").c_str(), filename.c_str());
}

// Helper function to check if an entry exists in the file and update it for MPI
void update_metrics_mpi(int n, double time) {
    update_metrics(n, 1, time, "Metrics_mpi.txt");  // MPI is serialized, so use thread count 1
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

    cout << endl
         << "Vertex: " << node << "\tCost: " << cost;
}

int main(int argc, char **argv) {
    int thread_count = 1;

    cout << "Enter number of vertices {min 2 vertices}: ";
    int n;
    cin >> n;

    int **arr = new int *[n];
    for (int i = 0; i < n; i++) {
        arr[i] = new int[n];
    }

    int **vis = new int *[n];
    for (int i = 0; i < n; i++) {
        vis[i] = new int[n];
    }

    srand(time(NULL));

    clock_t init_start = clock();
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

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            vis[i][j] = 0;
        }
    }
    clock_t init_end = clock();

    double init_time = double(init_end - init_start) / CLOCKS_PER_SEC;

    clock_t bfs_start = clock();

    for (int i = 0; i < n; i++) {
        BFS(i, arr, vis, n);
    }
    clock_t bfs_end = clock();

    double bfs_time = double(bfs_end - bfs_start) / CLOCKS_PER_SEC;

    cout << endl
         << "Number of vertices (input size): " << n;

    cout << endl
         << "Number of threads: " << thread_count;

    cout << endl
         << "Initialization time: " << (int)(init_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(init_time, 60) << " seconds";

    cout << endl
         << "BFS time: " << (int)(bfs_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(bfs_time, 60) << " seconds" << endl;

    update_metrics(n, thread_count, bfs_time, "Metrics_omp.txt");
    update_metrics_mpi(n, bfs_time);  // Update for MPI file

    for (int i = 0; i < n; i++) {
        delete[] arr[i];
        delete[] vis[i];
    }
    delete[] arr;
    delete[] vis;

    return 0;
}
