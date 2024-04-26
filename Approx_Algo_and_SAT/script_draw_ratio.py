import os
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

folder_path = 'result'
file_pattern = 'result_{}.txt'
i_values = list(range(2, 15, 2))  


averages = {'APPROX-VC-1': [], 'APPROX-VC-2': []}
std_devs = {'APPROX-VC-1': [], 'APPROX-VC-2': []}


for i in i_values:
    file_path = os.path.join(folder_path, file_pattern.format(i))
    if not os.path.exists(file_path):
        print(f'File {file_path} does not exist.')
        continue
    

    ratios = {'APPROX-VC-1': [], 'APPROX-VC-2': []}
    with open(file_path, 'r') as file:
        for line in file:
            if 'APPROX-VC-1:' in line or 'APPROX-VC-2:' in line:
                parts = line.strip().split()
                ratio_index = parts.index('Ratio:') + 1
                ratio = float(parts[ratio_index])
                if 'APPROX-VC-1:' in line:
                    ratios['APPROX-VC-1'].append(ratio)
                else:
                    ratios['APPROX-VC-2'].append(ratio)
    

    for key in ratios:
        if ratios[key]:
            average_ratio = np.mean(ratios[key])
            std_dev = np.std(ratios[key])
            averages[key].append(average_ratio)
            std_devs[key].append(std_dev)
        else:
            averages[key].append(0)  
            std_devs[key].append(0)

plt.figure(figsize=(10, 6))
for key, values in averages.items():
    plt.errorbar(i_values, values, yerr=std_devs[key], label=key, marker='o', capsize=5)

plt.xlabel('i')
plt.ylabel('Average Ratio')
plt.title('Average Ratio and Standard Deviation of APPROX-VC-1 and APPROX-VC-2 across Files')
plt.legend()
plt.grid(True)
plt.savefig('average_ratios_std.png')


print("The plot with standard deviations has been saved as 'average_ratios_std.png'.")
