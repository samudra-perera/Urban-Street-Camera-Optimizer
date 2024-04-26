# Write a py script to run and automatically grab outputs from graphGen
import subprocess

# Define paths and base commands to avoid repetition
graph_gen_command_base = "/home/agurfink/ece650/graphGen/graphGen"
result_dir_base = "/home/spperera/spperera-z45guo/project/result"
ece650_prj_executable = "./build/ece650-prj"

# iterate from 20 to 50 in steps of 5
for i in range(20, 50, 5):
    print(i)
    # Define filenames
    input_file = f"{result_dir_base}/graph_{i}.txt"
    output_file = f"{result_dir_base}/result_{i}.txt"

    # Add loop to run write into txt 10times per  graphgeneration

    # Step 1: Generate the graph data using graphGen and write it to graph_{i}.txt
    with open(input_file, "w") as graph_out:
        subprocess.run([graph_gen_command_base, str(i)], stdout=graph_out, check=True)

    # Step 2: Process the generated graph with ece650-prj and append the output to result_{i}.txt
    with open(input_file, "r") as infile, open(output_file, "a") as outfile:
        subprocess.run(ece650_prj_executable, stdin=infile, stdout=outfile, check=True)
