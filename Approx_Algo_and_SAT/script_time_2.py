import os
import numpy as np
import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

folder_path = "result"
file_names = [f"result_{i}.txt" for i in range(2, 51, 2)]

# Convert to microseconds by multiplying by 1 million
conversion_factor = 1e6
average_times = {"APPROX-VC-1:": [], "APPROX-VC-2:": []}
std_dev_times = {
    "APPROX-VC-1:": [],
    "APPROX-VC-2:": [],
}  # Dictionary to store standard deviations

for file_name in file_names:
    full_path = os.path.join(folder_path, file_name)

    if not os.path.exists(full_path):
        print(f"{full_path} does not exist.")
        continue

    times = {"APPROX-VC-1:": [], "APPROX-VC-2:": []}

    with open(full_path, "r") as file:
        for line in file:
            parts = line.strip().split()
            if parts and parts[0] in times:
                time = (
                    float(parts[-1][:-1]) * conversion_factor
                )  # Convert to microseconds
                times[parts[0]].append(time)

    for key in times:
        if times[key]:
            average_times[key].append(np.mean(times[key]))
            std_dev_times[key].append(np.std(times[key]))
        else:
            average_times[key].append(0)
            std_dev_times[key].append(0)

plt.figure(figsize=(10, 6))
i_values = list(range(2, 51, 2))

# plt.yscale("log")

for key, values in average_times.items():
    plt.errorbar(
        i_values, values, yerr=std_dev_times[key], label=key[:-1], fmt="-o", capsize=5
    )

plt.xlabel("V")
plt.ylabel("Average Time (μs)")
plt.title(
    "Average Time of Algorithms for Each Vertex in Microseconds with Standard Deviation"
)
plt.legend()
plt.savefig("VC1andVC2Runtimes_std.png")
print(
    "The plot with standard deviations has been saved as 'VC1andVC2Runtimes_std.png'."
)
