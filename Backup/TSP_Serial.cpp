#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <string>
#include <iomanip>
using namespace std;

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
  cost += arr[min_index][node];

  cout << endl
       << "Vertex: " << node << "\tCost: " << cost;
}

int main()
{
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

  srand(time(NULL));

  // initialization start
  clock_t init_start = clock();
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

  // calculate time taken for initialization (weights array, visited array)
  double init_time = double(init_end - init_start) / CLOCKS_PER_SEC;

  clock_t bfs_start = clock();
  // calling BFS function for each vertex
  for (int i = 0; i < n; i++)
  {
    BFS(i, arr, vis, n);
  }
  clock_t bfs_end = clock();

  // calculate time taken for finding Hamiltonian cycle for each node
  double bfs_time = double(bfs_end - bfs_start) / CLOCKS_PER_SEC;

  // print input size
  cout << endl
       << "Number of vertices (input size): " << n;

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