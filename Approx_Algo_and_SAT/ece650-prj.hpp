#pragma once

#include <vector>
#include <utility>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
using namespace std;

class Graph
{
  int V;                   // Number of Vertices
  vector<vector<int>> adj; // Adjacency List

  vector<pair<int, int>> edges;

public:
  Graph(int V); // Constructor Decleration

  // Add Edges
  void addEdge(int src, int dst);

  // Setting Vertices
  void setVertices(int V);

  // Getting Vertices
  int numOfVertex();

  // Getting adjacncy list
  vector<vector<int>> getAdjList();

  // Getting the edges
  const vector<pair<int, int>> &getEdges();

  void clearEdges();

  // Getting the min vertex cover
  vector<int> getVertexCover();

  // Optimized version of CNF-SAT to 3SAT
  vector<int> getVertexCover3SAT();

  // Getting the Approx - VC - 1
  vector<int> getApproxVC1();

  // Getting the Approx - VC -2
  vector<int> getApproxVC2();
};