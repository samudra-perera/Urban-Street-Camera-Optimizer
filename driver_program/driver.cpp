#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <vector>
using namespace std;

int main(int argc, char **argv)
{

    vector<pid_t> children;
    pid_t child_rgen;
    pid_t child_a1;
    pid_t child_a2;

    // Create pipes
    int pipe_rgen_a1[2];
    int pipe_rgen_a1_status = pipe(pipe_rgen_a1);

    if (pipe_rgen_a1_status == -1)
    {
        cerr << "Error: Failed to create pipe from rgen to a1" << endl;
    }

    // Run Rgen
    child_rgen = fork();

    if (child_rgen == -1)
    {
        cerr << "Error: Count not fork rgen" << endl;
        return 1;
    }
    else if (child_rgen == 0)
    {
        dup2(pipe_rgen_a1[1], STDOUT_FILENO);
        close(pipe_rgen_a1[0]);
        close(pipe_rgen_a1[1]);
        execvp("./rgen", argv);
        cerr << "Error: rgen could not be executed" << endl;
        exit(1);
    }

    // Pipe Randomly generated streets into A1
    children.push_back(child_rgen);

    // Pipe A1 to A2
    int pipe_a1_a2[2];
    int pipe_a1_a2_status = pipe(pipe_a1_a2);

    if (pipe_a1_a2_status == -1)
    {
        cerr << "Error: Failed to make pipe from A1 to A2" << endl;
        exit(1);
    }
    // Pipe from A1 to A2
    child_a1 = fork();
    if (child_a1 == -1)
    {
        cerr << "Error: Could not fork A1" << endl;
    }
    else if (child_a1 == 0)
    {
        dup2(pipe_rgen_a1[0], STDIN_FILENO);
        dup2(pipe_a1_a2[1], STDOUT_FILENO);

        close(pipe_rgen_a1[0]);
        close(pipe_rgen_a1[1]);
        close(pipe_a1_a2[0]);
        close(pipe_a1_a2[1]);

        char *py_file[3];
        py_file[0] = (char *)"usr/bin/python3";
        py_file[1] = "street_intersection.py";
        py_file[2] = nullptr;

        execv("/usr/bin/python3", py_file);
        cerr << "Error: Could not execute A1" << endl;
        return 1;
    }
    children.push_back(child_a1);

    // Piping for A2 for A3
    child_a2 = fork();
    if (child_a2 == -1)
    {
        cerr << "Error: Count not fork A2" << endl;
        return 1;
    }
    else if (child_a2 == 0)
    {
        dup2(pipe_a1_a2[0], STDIN_FILENO);
        close(pipe_a1_a2[1]);
        close(pipe_a1_a2[0]);
        execvp("./shortest_path", argv);
        cerr << "Error: A2 could not be executed" << endl;
        return 1;
    }

    dup2(pipe_a1_a2[1], STDOUT_FILENO);
    close(pipe_a1_a2[0]);
    close(pipe_a1_a2[1]);

    children.push_back(child_a2);

    while (!cin.eof())
    {
        // Read input
        string input;
        getline(cin, input);
        if (input.size() > 0)
        {
            cout << input << endl;
        }
    }
    int status;
    for (int ptr = 0; ptr < children.size(); ptr++)
    {
        kill(children[ptr], SIGTERM);
        waitpid(ptr, &status, 0);
    }
    return 0;
}
