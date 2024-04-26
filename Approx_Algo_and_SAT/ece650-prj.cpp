// g++ -o build/ece650-a4 ece650-a4.cpp -std=c++11
#include <iostream>
#include <vector>
#include <utility>
#include <sstream>
#include <chrono>
#include <string>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <pthread.h>
#include <time.h>
#include <iomanip>
// Defines unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
#include "ece650-prj.hpp"
#include "solverFunctions.cpp"

using namespace std;

int pthread_getcpuclockid(pthread_t thread, clockid_t *clockid);

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

// Helper function to ensure clauses are always going to be made of exactly 3 literals
void ensure3SAT(unique_ptr<Minisat::Solver> &solver, Minisat::vec<Minisat::Lit> &input_clause)
{
    if (input_clause.size() == 2)
    {
        // Add a dummy literal to make it a 3-literal clause
        input_clause.push(Minisat::mkLit(solver->newVar(), true));
    }
    else
        while (input_clause.size() > 3)
        {
            Minisat::Lit newVar = Minisat::mkLit(solver->newVar());
            // Create a new clause from the first two literals and a new variable
            Minisat::vec<Minisat::Lit> newClause;
            newClause.push(input_clause[0]);
            newClause.push(input_clause[1]);
            newClause.push(newVar);
            solver->addClause(newClause);

            // Remove the first two literals and modify the third
            input_clause.shrink(2);    // Remove the first two elements efficiently
            input_clause[0] = ~newVar; // Invert the new variable for the next clause
        }
    // Add the final clause
    if (input_clause.size() == 3)
    {
        solver->addClause(input_clause);
    }
}

vector<int> Graph::getVertexCover3SAT()
{
    vector<int> cnf_3sat_cover;

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
            ensure3SAT(solver, clause);
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
        for (auto &edge : edges)
        {
            Minisat::vec<Minisat::Lit> clause;
            for (int j = 0; j < k; ++j)
            {
                clause.push(matrix[edge.first - 1][j]); // Adjust for one-based indexing
                clause.push(matrix[edge.second - 1][j]);
            }
            ensure3SAT(solver, clause);
        }

        // Solving the 3SAT
        bool res = solver->solve();
        if (res)
        {
            for (int i = 0; i <= V; ++i)
            {
                for (int j = 0; j < k; ++j)
                {
                    if (Minisat::toInt(solver->modelValue(matrix[i][j])) == 0)
                    {
                        cnf_3sat_cover.push_back(i);
                        break;
                    }
                }
            }
            return cnf_3sat_cover;
        }
    }
    cerr << "Error: Unable to find Vertex Cover" << endl;
    return cnf_3sat_cover;
}

vector<int> Graph::getApproxVC1()
{
    vector<int> vc1_cover;
    // Copy of edges
    auto edges = this->edges;

    while (!edges.empty())
    {
        unordered_map<int, int> degrees;
        // Calculate Degrees
        // Calculate degrees --> Calculating the number of vertexs with the most edges
        for (const auto &edge : edges)
        {
            degrees[edge.first]++;
            degrees[edge.second]++;
        }

        // Find the vertex of the highest Degree
        int maxVertex = -1;
        int maxDegree = 0;
        for (const auto &degree : degrees)
        {
            if (degree.second > maxDegree)
            {
                maxDegree = degree.second;
                maxVertex = degree.first;
            }
        }

        // Add MaxVertex to the cover
        vc1_cover.push_back(maxVertex);

        // remove all edges incident to the maxVertex
        edges.erase(remove_if(edges.begin(), edges.end(),
                              [maxVertex](const pair<int, int> &edge)
                              {
                                  return edge.first == maxVertex || edge.second == maxVertex;
                              }),
                    edges.end());
    }
    sort(vc1_cover.begin(), vc1_cover.end());
    return vc1_cover;
}

// Helper function for the VC2 approximation
bool isConnectedToUV(const pair<int, int> &edge, int u, int v)
{
    return edge.first == u || edge.second == u || edge.first == v || edge.second == v;
}

vector<int> Graph::getApproxVC2()
{
    vector<int> vc2_cover;
    // Copy of Edges
    auto edges = this->edges;

    while (!edges.empty())
    {
        auto edge = edges.front(); // Pick an arbitrary edge (u,v)
        int u = edge.first;
        int v = edge.second;
        // Add both u and v to vc2
        vc2_cover.push_back(u);
        vc2_cover.push_back(v);

        // Remove all edges connected to u or v
        edges.erase(std::remove_if(edges.begin(), edges.end(),
                                   [u, v](const std::pair<int, int> &edge)
                                   {
                                       return isConnectedToUV(edge, u, v);
                                   }),
                    edges.end());
    }
    // Sort and remove duplicates from indiscriminately adding u and v
    sort(vc2_cover.begin(), vc2_cover.end());
    vc2_cover.erase(unique(vc2_cover.begin(), vc2_cover.end()), vc2_cover.end());
    return vc2_cover;
}

void *inputThread(void *args)
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

            // Start making threads after computing the edges, the all the VC functions will use these edges to output
            //  // After adding all edges computer VC and is not empty
            pthread_t cnf_sat_t, approx_vc1_t, approx_vc2_t;

            // Storing results in pointer
            void *vcResult, *approx1Result, *approx2Result;

            auto startTime = chrono::system_clock::now();

            pthread_create(&cnf_sat_t, NULL, vertexCoverWrapper, static_cast<void *>(&graph));
            pthread_create(&approx_vc1_t, NULL, approxVC1Wrapper, static_cast<void *>(&graph));
            pthread_create(&approx_vc2_t, NULL, approxVC2Wrapper, static_cast<void *>(&graph));
            // pthread_create(&cnf_3SAT_t, NULL, vertexCoverWrapper3SAT, static_cast<void *>(&graph));

            bool cnf_timeout = false;
            while (true)
            {
                auto currentTime = chrono::system_clock::now();
                chrono::duration<double> elapsed = currentTime - startTime;

                if (elapsed.count() >= 120.0)
                { // 2 minutes timeout
                    pthread_cancel(cnf_sat_t);
                    cnf_timeout = true;
                    break;
                }

                // Check if the thread has finished
                if (pthread_tryjoin_np(cnf_sat_t, &vcResult) == 0)
                {
                    break; // cnf_sat_t finished before timeout
                }
            }

            // Wait for computation to finish and retrieving results
            // pthread_join(cnf_sat_t, &vcResult);
            // pthread_join(cnf_3SAT_t, &vc3SATResult);
            pthread_join(approx_vc1_t, &approx1Result);
            pthread_join(approx_vc2_t, &approx2Result);

            size_t num_of_vc_cnf;

            if (!cnf_timeout)
            {
                // Casting the void* to a int and printing results as before
                VertexCoverResult *vcStruct = static_cast<VertexCoverResult *>(vcResult);
                double elapsed_cnf = vcStruct->elapsedTime;
                vector<int> *vc_cnf = vcStruct->result;
                num_of_vc_cnf = vc_cnf->size();
                cout << "CNF-SAT-VC: ";
                for (int v : *vc_cnf)
                    cout << v << " ";
                // cout << " Time: " << std::fixed << std::setprecision(8) << elapsed_cnf << "s\n";
                cout << "\n";
                delete vc_cnf;
                delete vcStruct;
            }
            else
            {
                cout << "CNF-SAT-VC: timeout\n";
            }

            // Casting the void* to a int
            // VertexCoverResult *vcStruct = static_cast<VertexCoverResult *>(vcResult);
            // VertexCoverResult *vc3SATStruct = static_cast<VertexCoverResult *>(vc3SATResult);
            VertexCoverResult *approx1Struct = static_cast<VertexCoverResult *>(approx1Result);
            VertexCoverResult *approx2Struct = static_cast<VertexCoverResult *>(approx2Result);

            // vector<int> *vc = vcStruct->result;
            // vector<int> *cnf3SAT = vc3SATStruct->result;
            vector<int> *approx1 = approx1Struct->result;
            vector<int> *approx2 = approx2Struct->result;

            // double elapsed_cnf = vcStruct->elapsedTime;
            // double elapsed_cnf_3SAT = vc3SATStruct->elapsedTime;
            double elapsed_approx1 = approx1Struct->elapsedTime;
            double elapsed_approx2 = approx2Struct->elapsedTime;

            // Size of the Covers produced by A1 and A2
            size_t num_of_vc_approx1 = approx1->size();
            size_t num_of_vc_approx2 = approx2->size();

            double ratio_vc1, ratio_vc2;

            if (num_of_vc_cnf != 0)
            {

                ratio_vc1 = static_cast<double>(num_of_vc_approx1) / static_cast<double>(num_of_vc_cnf);
                ratio_vc2 = static_cast<double>(num_of_vc_approx2) / static_cast<double>(num_of_vc_cnf);
            }
            else
            {
                // Not sure if this is right
                ratio_vc1 = 0;
                ratio_vc2 = 0;
            }
            // Printing Results
            // cout << "CNF-3SAT-VC-OP: ";
            // for (int v : *cnf3SAT)
            //     cout << v << " ";
            // cout << " Time: " << elapsed_cnf_3SAT << "s\n";
            cout
                << "APPROX-VC-1: ";
            for (int v : *approx1)
                cout << v << " ";
            // cout << "Ratio: " << ratio_vc1 << " Time: " << std::fixed << std::setprecision(8) << elapsed_approx1
            cout << "\n";
            cout << "APPROX-VC-2: ";
            for (int v : *approx2)
                cout << v << " ";
            // cout << "Ratio: " << ratio_vc2 << " Time: " << std::fixed << std::setprecision(8) << elapsed_approx2
            cout << "\n";

            // Deleting the allocated memory
            // delete cnf3SAT;
            delete approx1;
            delete approx2;
            // delete vc3SATStruct;
            delete approx1Struct;
            delete approx2Struct;
        }
        else
        {
            // Program should handle empty inputs
            continue;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t ip_thread;

    int ret = pthread_create(&ip_thread, NULL, &inputThread, NULL);

    pthread_join(ip_thread, NULL);
    if (ret != 0)
    {
        cerr << "Creating Input Thread Failed" << endl;
    }

    return 0;
}
