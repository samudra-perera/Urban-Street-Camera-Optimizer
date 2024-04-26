import subprocess

# Define paths and base commands to avoid repetition
graph_gen_command_base = "/home/agurfink/ece650/graphGen/graphGen"
result_dir_base = "/home/spperera/spperera-z45guo/project/result"
ece650_prj_executable = "./build/ece650-prj"

# Iterate from 2 to 18 in steps of 2
for i in range(2, 16, 2):
    print(f"Processing graphs for i={i}")

    # Generate 10 unique graphs for the current i
    for graph_index in range(10):
        input_file = f"{result_dir_base}/graph_{i}.txt"
        output_file = f"{result_dir_base}/result_{i}.txt"

        # Step 1: Generate the graph data using graphGen and write it to graph_{i}_{graph_index}.txt
        with open(input_file, "w") as graph_out:
            subprocess.run(
                [graph_gen_command_base, str(i)], stdout=graph_out, check=True
            )

        # Step 2: Process the generated graph 10 times with ece650-prj and append the output to result_{i}_{graph_index}.txt
        for test_run in range(10):
            with open(input_file, "r") as infile, open(output_file, "a") as outfile:
                subprocess.run(
                    [ece650_prj_executable], stdin=infile, stdout=outfile, check=True
                )
