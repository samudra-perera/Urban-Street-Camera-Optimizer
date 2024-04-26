// g++ -o build/ece650-a4 ece650-a4.cpp -std=c++11
#include <iostream>
#include <vector>
#include <utility>
#include <sstream>
#include <string>
#include <algorithm>
// Defines unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
#include "cover_solver.hpp"
using namespace std;

Graph::Graph(int V) : V(V), adj(vector<vector<int>>(V + 1)), edges(vector<pair<int, int>>()) {}

void Graph::setVertices(int V)
{
  this->V = V;
  adj.clear();       // Clear existing list
  adj.resize(V + 1); // For 1 based indexing since V starts from 1 .... N
  edges.clear();
}

void Graph::addEdge(int src, int dst)
{
  adj[src].push_back(dst);
  adj[dst].push_back(src);

  if (src < dst)
  {
    edges.emplace_back(src, dst);
  }
  else
  {
    edges.emplace_back(src, dst);
  }
}

int Graph::numOfVertex()
{
  return this->V;
}

const vector<pair<int, int>> &Graph::getEdges()
{
  return this->edges;
};

void Graph::clearEdges()
{
  this->edges.clear();
}

vector<vector<int>> Graph::getAdjList()
{
  return this->adj;
}

vector<int> Graph::getVertexCover()
{
  // Storing the indexes of the minimum vertex cover
  vector<int> vertex_cover;

  // Try vertex covers from k=1 up to V to find the smallest k for which a cover exists
  for (int k = 1; k <= V; ++k)
  {
    // Create a SAT solver instance for each k
    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    // Create a 2D Vector of Minisat Literals --> row represents Vertex, and column represents
    // Position on the VC --> for encoding
    vector<vector<Minisat::Lit>> matrix(V, vector<Minisat::Lit>(k));

    ////Creating the SAT variables within the matrix and creating literals from it
    for (int i = 0; i < V; ++i)
    {
      for (int j = 0; j < k; ++j)
      {
        matrix[i][j] = Minisat::mkLit(solver->newVar());
      }
    }

    // 1st clause --> Ensuring that at least one vertex is in each position of the vertex
    //  covers up to size K
    for (int i = 0; i < k; ++i)
    {
      Minisat::vec<Minisat::Lit> clause;
      for (int j = 0; j < V; ++j)
      {
        clause.push(matrix[j][i]);
      }
      solver->addClause(clause);
    }

    // 2nd Clause --> No one vertex can appear twice in a vertex cover
    // Clause added to forbid Vertexes from being true in more than one position
    for (int m = 0; m < V; ++m)
    {
      for (int p = 0; p < k - 1; ++p)
      {
        for (int q = p + 1; q < k; ++q)
        {
          solver->addClause(~matrix[m][p], ~matrix[m][q]);
        }
      }
    }

    // 3rd clause --> No vertex appears more than once in the VC
    for (int m = 0; m < k; ++m)
    {
      for (int p = 0; p < V - 1; ++p)
      {
        for (int q = p + 1; q < V; ++q)
        {
          solver->addClause(~matrix[p][m], ~matrix[q][m]);
        }
      }
    }

    // 4th Clause --> Iteration over all edges and add a clause to ensure that at least one of its
    // endpoints is in the vertex cover
    for (const auto &edge : edges)
    {
      Minisat::vec<Minisat::Lit> clause;
      for (int j = 0; j < k; ++j)
      {
        clause.push(matrix[edge.first - 1][j]); // Adjust for one-based indexing
        clause.push(matrix[edge.second - 1][j]);
      }
      solver->addClause(clause);
    }

    // Check for solution
    bool res = solver->solve();
    if (res)
    {
      for (int i = 0; i < V; ++i)
      {
        for (int j = 0; j < k; ++j)
        {
          if (solver->modelValue(matrix[i][j]) == Minisat::l_True)
          {
            vertex_cover.push_back(i + 1); // Convert to 1-based index
          }
        }
      }
      return vertex_cover;
    }
  }
  cerr << "Error: Unable to find vertex cover" << endl;
  return vertex_cover; // Empty if no cover exists
}

int main(int argc, char **argv)
{
  // Initialize Graph
  Graph graph(0);

  while (!cin.eof())
  {
    // read a line of input until EOL and store in a string
    string line;
    getline(cin, line);

    // create an input stream based on the line
    // we will use the input stream to parse the line
    istringstream input(line);

    // While there are characters if not break
    if (cin.eof())
    {
      break;
    }

    char command = line[0];

    if (command == 'V')
    {
      // Extract the integer after 'V'
      // Using atoi to conver to integer
      // cout << line << endl;
      int V = atoi(line.substr(2).c_str());
      // Validation for V
      if (V < 2)
      {
        cerr << "Error: Please enter a Vertex greater than 1." << endl;
        continue;
      }
      // cout << "Vertex:" << V << endl;
      graph.setVertices(V);

      continue;
    }
    else if (command == 'E')
    {
      // cout << line << endl;
      string edgeString = line.substr(3, line.size() - 4);
      stringstream ss(edgeString);
      int src, dst;
      char skip; // Used to skip characters '<', '>', and ','

      // validation for edges
      while (ss >> skip && skip == '<' && ss >> src >> skip && skip == ',' && ss >> dst >> skip && skip == '>')
      {
        // validation for edges
        // 3 Case -> less than 1, greater than number of vertex or equal to eachother
        // Assumed that the case of edges being outputted is not a possibility
        if (src == dst || src < 1 || src > graph.numOfVertex() || dst < 1 || dst > graph.numOfVertex())
        {
          cerr << "Error: Invalid Set of Edges." << endl;
          continue;
        }
        else
        {
          // If passes add edges to the adjacency list
          // cout << src << "and" << dst << endl;
          graph.addEdge(src, dst);
        };

        // Check if there is a comma following the current edge, indicating more edges to parse
        if (ss.peek() == ',')
        {
          ss.ignore(); // Skip the comma
        }
      }
      // After adding all edges computer VC and is not empty
      if (!graph.getEdges().empty())
      {
        // const auto &edges = graph.getEdges(); // Get a reference to the edges vector
        // for (const auto &edge : edges)
        // {
        //   cout << "<" << edge.first << "," << edge.second << "> ";
        // }
        // cout << endl;
        vector<int> vertexCover;
        vertexCover = graph.getVertexCover();
        if (!vertexCover.empty())
        {
          sort(vertexCover.begin(), vertexCover.end());
          for (int v : vertexCover)
          {
            cout << v << " ";
          }
          cout << endl;
        }
        vertexCover.clear();
        graph.clearEdges();
      }
      else
      {
        cerr << "Error: Unable to compute vertex cover." << endl;
      }
    }
    else
    {
      // Program should handle empty inputs
      continue;
    }
  }
  return 0;
}
