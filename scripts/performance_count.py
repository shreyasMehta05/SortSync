import re
import matplotlib.pyplot as plt

# Load data from count_test_output.txt
with open("count_test_output.txt", "r") as file:
    log_data = file.read()

# Define lists to hold parsed values
input_sizes = []
execution_times = []
memory_usages = []
ranges = []

# Parse log data
for line in log_data.splitlines():
    # Match input size (number of files)
    if "Number of files:" in line:
        input_sizes.append(int(line.split(":")[-1].strip()))
    
    # Match execution time for Parallel Count Sort
    elif "Parallel count sort completed in" in line:
        execution_time = float(re.search(r"(\d+\.\d+) seconds", line).group(1))
        execution_times.append(execution_time)
    
    # Match maximum memory usage
    elif "Max Memory Usage (KB):" in line:
        memory_usage = float(line.split(":")[-1].strip())
        memory_usages.append(memory_usage)
        
    # Match range
    elif "Range:" in line:
        range_value = int(line.split(":")[-1].strip())
        ranges.append(range_value)

# Print lengths of lists to check for mismatches
print("Lengths of parsed data lists:")
print("Input Sizes:", len(input_sizes))
print("Execution Times:", len(execution_times))
print("Memory Usages:", len(memory_usages))
print("Ranges:", len(ranges))

# Plot data only if all lists have the same length
if len(input_sizes) == len(execution_times) == len(memory_usages) == len(ranges):
    # Plot data
    plt.figure(figsize=(12, 12))

    # Plot Execution Time vs Input Size
    plt.subplot(3, 1, 1)
    plt.plot(input_sizes, execution_times, marker='o', color='b', label='Execution Time (s)')
    plt.yscale('log')
    plt.xscale('log')
    plt.xlabel('Input Size (Number of Files)', fontsize=12)
    plt.ylabel('Execution Time (s)', fontsize=12)
    plt.title('Execution Time vs Input Size for Count Sort (Timestamp)', fontsize=14)
    plt.legend()
    plt.grid(True)

    # Plot Range vs Input Size
    plt.subplot(3, 1, 2)
    plt.plot(input_sizes, ranges, marker='x', color='g', linestyle='--', label='Range')
    plt.yscale('log')
    plt.xscale('log')
    plt.xlabel('Input Size (Number of Files)', fontsize=12)
    plt.ylabel('Range', fontsize=12)
    plt.title('Range vs Input Size for Count Sort (Timestamp)', fontsize=14)
    plt.legend()
    plt.grid(True)

    # Plot Memory Usage vs Input Size
    plt.subplot(3, 1, 3)
    plt.bar([str(size) for size in input_sizes], memory_usages, color='purple', edgecolor='black')
    plt.xlabel('Input Size (Number of Files)', fontsize=12)
    plt.ylabel('Max Memory Usage (KB)', fontsize=12)
    plt.title('Memory Usage for Count Sort', fontsize=14)
    plt.grid(True, axis='y', linestyle='--')

    # Adjust layout for better spacing
    plt.tight_layout()

    # Save and show all plots
    plt.savefig("Performance_for_count_sort_check.png")
    plt.show()
else:
    print("Error: Parsed data lists have mismatched lengths. Please check the input data.")
