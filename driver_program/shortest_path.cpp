#include <iostream>
#include <sstream>
#include <vector>
#include "shortest_path.hpp"
using namespace std;

Graph::Graph(int V) : V(V), adj(V)
{
}

void Graph::setVertices(int V)
{
    this->V = V;
    adj.clear();       // Clear existing list
    adj.resize(V + 1); // For 1 based indexing since all vertices start from 1 .... N
}

void Graph::addEdge(int src, int dst)
{
    adj[src].push_back(dst);
    adj[dst].push_back(src);
}

int Graph::numOfVertex()
{
    return V;
}

vector<vector<int>> Graph::getAdjList()
{
    return adj;
}

bool Graph::BFS(int src, int dst, vector<int> &pred)
{
    // Placeholder for the BFS implementation
    // Taken from the https://www.geeksforgeeks.org/shortest-path-unweighted-graph/
    // Queue of verticies
    list<int> queue;

    // Boolean array which stores the information whether the ith vertex is reached at least once in the BFS
    vector<bool> visited(V + 1, false);

    // Initially all vertices are unvisited to V[i] for all i is false
    // So no path is constructed yet
    for (int i = 1; i < V + 1; i++)
    {
        visited[i] = false;
        pred[i] = -1;
    }

    // Now Source is first to be visited
    visited[src] = true;
    queue.push_back(src);

    // BFS Algorithm
    while (!queue.empty())
    {
        int u = queue.front();
        queue.pop_front();
        for (int i = 0; i < adj[u].size(); i++)
        {
            if (visited[adj[u][i]] == false)
            {
                visited[adj[u][i]] = true;
                pred[adj[u][i]] = u;
                queue.push_back(adj[u][i]);

                if (adj[u][i] == dst)
                    return true;
            }
        }
    }

    return false;
}

void Graph::printShortestPath(vector<vector<int>> adj, int src, int dst, int V)
{
    vector<int> pred(V + 1, -1);

    if (BFS(src, dst, pred) == false)
    {
        cerr << "Error: Destination does not exist " << endl;
        return;
    }

    // Vector path to store shortest path
    vector<int> path;
    int crawl = dst;
    path.push_back(crawl);
    while (pred[crawl] != -1)
    {
        path.push_back(pred[crawl]);
        crawl = pred[crawl];
    }

    // Printing path from source to destination
    for (int i = path.size() - 1; i >= 0; i--)
    {
        cout << path[i];
        if (i > 0)
        {
            cout << "-";
        }
    }
    cout << ""
         << endl;
}

int main(int argc, char **argv)
{
    // Initialize Graph
    Graph graph(0);
    // read from stdin until EOF
    while (!cin.eof())
    {
        // read a line of input until EOL and store in a string
        string line;
        getline(cin, line);

        // create an input stream based on the line
        // we will use the input stream to parse the line
        istringstream input(line);

        // we expect each line to contain a list of numbers
        // this vector will store the numbers.
        // they are assumed to be unsigned (i.e., positive)
        vector<unsigned> nums;

        // while there are characters in the input line
        // Read from stdin until EOF
        if (cin.eof())
        {
            break;
        }

        char command = line[0];
        // cout << input << endl;
        // string values = input[1];
        // Check if the input starts with 'V'
        // We can assume that the input will be valid for Vertex
        if (command == 'V')
        {
            // Extract the integer after 'V'
            // Using atoi to conver to integer
            cout << line << endl;
            int V = atoi(line.substr(2).c_str());
            // Validation for V
            if (V < 2)
            {
                cerr << "Error: Please enter a Vertex greater than 1." << endl;
                continue;
            }

            ////////Important to clear previous graph everytime a new V command is inputted into the commandline
            // graph.clearList();
            graph.setVertices(V);
            continue;
        }
        else if (command == 'E')
        {
            cout << line << endl;
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
                    graph.addEdge(src, dst);
                };

                // Check if there is a comma following the current edge, indicating more edges to parse
                if (ss.peek() == ',')
                {
                    ss.ignore(); // Skip the comma
                }
            }
            continue;
        }
        // Ensure that a valid graph is possible
        else if (command == 's')
        {
            // Error handling for the S
            int source, destination;
            string shortPath = line.substr(2);
            istringstream ss(shortPath);
            ss >> source >> destination;

            if (source == destination || source > graph.numOfVertex() || destination > graph.numOfVertex() || source < 1 || destination < 1)
            {
                cerr << "Error: source or destination does not exist" << endl;
                continue;
            }

            graph.printShortestPath(graph.getAdjList(), source, destination, graph.numOfVertex());
            continue;
        }
        else
        {
            // Program should handle empty inputs
            continue;
        }
    }
    return 0;
}
