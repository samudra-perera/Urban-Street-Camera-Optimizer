#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <queue>

using namespace std;

// Class declaration for the Graph class
class Graph
{
  // Number of Vertices
  int V;

  // Pointer to an adjacency list
  vector<vector<int>> adj;

public:
  // Constructor declaration
  Graph(int V);

  // Function to add an edge to the graph
  void addEdge(int src, int dst);

  // Function to perform Breadth First Search (BFS) from a given source vertex
  bool BFS(int src, int dst, vector<int> &pred);

  // Function to return the number of vertices in the graph
  int numOfVertex();

  // Function to set the # of vertices in the graph
  void setVertices(int V);

  // Function to print the shortest path
  void printShortestPath(vector<vector<int>> adj, int src, int dst, int V);

  // Function to get the adjacency list
  vector<vector<int>> getAdjList();
};
