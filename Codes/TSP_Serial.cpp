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
void update_metrics(int n, int threads, double time, const string &filename)
{
    ifstream inFile(filename);
    ofstream outFile(filename + "_tmp.txt");

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

    remove(filename.c_str());
    rename((filename + "_tmp.txt").c_str(), filename.c_str());
}

void BFS(int node, int **arr, int **vis, int n)
{
    int start_node = node;
    int min;
    int min_index;
    int visited_count = 0;
    int cost = 0;

    // mark starting node as visited
    vis[node][node] = 1;

    // increment visited nodes count --- all nodes should be visited
    visited_count++;

    // visited_count == n means all nodes have been visited
    while (visited_count < n)
    {
        min = INT_MAX;

        for (int i = 0; i < n; i++)
        {
            if (arr[start_node][i] < min && start_node != i && vis[node][i] == 0)
            {
                min = arr[start_node][i];
                min_index = i;
            }
        }
        start_node = min_index;
        cost += min;
        vis[node][min_index] = 1;
        visited_count++;
    }
    // add edge weight between last node visted to starting node
    cost += arr[min_index][node];

    cout << endl
         << "Vertex: " << node << "\tCost: " << cost;
}

int main(int argc, char **argv)
{
    int thread_count = 1;

    cout << "Enter number of vertices {min 2 vertices}: ";
    int n;
    cin >> n;

    // allocation of array - entries represent edge weights
    int **arr = new int *[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new int[n];
    }

    // allocation of visited array - entries represent visited vertices
    int **vis = new int *[n];
    for (int i = 0; i < n; i++)
    {
        vis[i] = new int[n];
    }

    srand(time(NULL));

    // initialization start
    clock_t init_start = clock();
    for (int i = 0; i <= n - 2; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            int val = rand() % 20 + 5; // to ensure only diagonal entries are zero
            arr[i][j] = val;
            arr[j][i] = val;
        }
    }

    // mark diagonal entries 0 since there are no loops
    for (int i = 0; i < n; i++)
    {
        arr[i][i] = 0;
    }

    // initialize visited array
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            vis[i][j] = 0;
        }
    }
    clock_t init_end = clock();
    // initialization end

    // calculate time taken for initialization (weights array, visited array)
    double init_time = double(init_end - init_start) / CLOCKS_PER_SEC;

    // processing start
    clock_t bfs_start = clock();

    // calling BFS function for each vertex
    for (int i = 0; i < n; i++)
    {
        BFS(i, arr, vis, n);
    }
    clock_t bfs_end = clock();
    // processing end

    // calculate time taken for finding Hamiltonian cycle for each node
    double bfs_time = double(bfs_end - bfs_start) / CLOCKS_PER_SEC;

    // print input size
    cout << endl
         << "Number of vertices (input size): " << n;

    // print number of threads/processes
    // This is serialized implementation so thread/process count will default to 1
    cout << endl
         << "Number of threads: " << thread_count;

    // print initialization time
    cout << endl
         << "Initialization time: " << (int)(init_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(init_time, 60) << " seconds";

    // print processing time
    cout << endl
         << "BFS time: " << (int)(bfs_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(bfs_time, 60) << " seconds" << endl;

    // This is serialized implementation so thread/process count will default to 1
    // This is being done for speedup and efficiency calculation later on
    update_metrics(n, thread_count, bfs_time, "Metrics_omp.txt");
    update_metrics(n, thread_count, bfs_time, "Metrics_mpi.txt");

    // free memory
    for (int i = 0; i < n; i++)
    {
        delete[] arr[i];
        delete[] vis[i];
    }
    delete[] arr;
    delete[] vis;

    return 0;
}
