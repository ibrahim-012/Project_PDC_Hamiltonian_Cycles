#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <string>
#include <iomanip>
#include <omp.h>
using namespace std;

void BFS(int node, int **arr, int **vis, int n, string *v_path, string *w_path)
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

    // append starting vertex to v_path for printing later
    v_path[node].append(to_string(node));

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

        // append next visited vertex to v_path
        v_path[node].append("->");
        v_path[node].append(to_string(min_index));

        // append edge weight between last node and visited node to w_path
        w_path[node].append("->");
        w_path[node].append(to_string(min));
    }
    cost += arr[min_index][node];

    // append starting vertex to v_path to complete Hamiltonian cycle
    v_path[node].append("->");
    v_path[node].append(to_string(node));

    // append edge weight between last visited node and start node to w_path
    w_path[node].append("->");
    w_path[node].append(to_string(arr[min_index][node]));

#pragma omp critical
    {
        cout << endl
             << "Vertex: " << node;
        cout << endl
             << "Path:     " << v_path[node];
        cout << endl
             << "Weights: " << w_path[node];
        cout << endl
             << "Cost: " << cost << endl;
    }
}

int main(int argc, char **argv)
{
    int thread_count = atoi(argv[1]);

    cout << "Enter number of vertices {min 2 vertices}: ";
    int n;
    cin >> n;

    // allocation of array - entries represent edge weights
    int **arr = new int *[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = new int[n];
    }

    // allocation of visited array - entries represent visited vertex
    int **vis = new int *[n];
    for (int i = 0; i < n; i++)
    {
        vis[i] = new int[n];
    }

    // allocation of arrays to hold vertices and edge weights for printing paths
    string v_path[n];
    string w_path[n];

    srand(time(NULL));

    // initialization start
    double init_start = omp_get_wtime();
    for (int i = 0; i <= n - 2; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            int val = rand() % 20;
            arr[i][j] = val + 5;
            arr[j][i] = arr[i][j];
        }
    }

// mark diagonal entries 0 since there are no loops
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < n; i++)
    {
        arr[i][i] = 0;
    }

// initialize visited array
#pragma omp parallel for collapse(2) num_threads(thread_count)
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            vis[i][j] = 0;
        }
    }
    double init_end = omp_get_wtime();
    // initialization end

    // calculate time taken for initialization (weights array, visited array)
    double init_time = init_end - init_start;

    // printing array for verifying correctness of program
    cout << endl
         << "Adjacency Matrix (entries represent edge weights b/w two vertices)" << endl;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cout << arr[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;

    double bfs_start = omp_get_wtime();

// calling BFS function for each vertex
#pragma omp parallel for num_threads(thread_count)
    for (int i = 0; i < n; i++)
    {
        BFS(i, arr, vis, n, v_path, w_path);
    }
    double bfs_end = omp_get_wtime();

    // calculate time taken for finding Hamiltonian cycle for each node
    double bfs_time = bfs_end - bfs_start;

    // print input size
    cout << endl
         << "Number of vertices (input size): " << n;

    // print number of threads assigned
    cout << endl
         << "Number of threads: " << omp_get_num_threads;

    // print initialization time
    cout << endl
         << "Initialization time: " << (int)(init_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(init_time, 60) << " seconds";

    // print processing time
    cout << endl
         << "BFS time: " << (int)(bfs_time / 60) << " minutes "
         << fixed << setprecision(6) << fmod(bfs_time, 60) << " seconds" << endl;

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