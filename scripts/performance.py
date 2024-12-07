import subprocess
import matplotlib.pyplot as plt
import random
import string
import time
import concurrent.futures
from datetime import datetime
import re
# File to store results
RESULT_FILE = "test_result.txt"

MAX_FILES = int(1e7)
MAX_NAME_LENGTH = 6
MAX_ID = int(1e6)

# Generates a random START_YEAR between 2000 and current year
def generate_random_year():
    current_year = datetime.now().year
    return random.randint(2000, current_year)

# Generates a unique ID not in the generated IDs set
def generate_unique_id(generated_ids):
    while True:
        id = random.randint(1, MAX_ID)
        if id not in generated_ids:
            generated_ids.add(id)
            return id

# Generates a random ISO 8601 timestamp within a random year
def generate_random_timestamp():
    year = generate_random_year()  # Random year now
    month = random.randint(1, 12)
    day = random.randint(1, 28)  # Keeping it simple for month-day validation
    hour = random.randint(0, 23)
    minute = random.randint(0, 59)
    second = random.randint(0, 59)
    return f"{year}-{month:02d}-{day:02d}T{hour:02d}:{minute:02d}:{second:02d}"

# Generates a random filename of length `length`
def generate_random_filename(length):
    charset = string.ascii_letters + string.digits
    return ''.join(random.choice(charset) for _ in range(length))

# Function to generate test case for a chunk of files
def generate_file_chunk(start_index, end_index, generated_ids):
    files_chunk = []

    for _ in range(start_index, end_index):
        file_name_length = random.randint(3, MAX_NAME_LENGTH)
        file_name = generate_random_filename(file_name_length)
        file_id = generate_unique_id(generated_ids)
        timestamp = generate_random_timestamp()
        files_chunk.append(f"{file_name}.txt {file_id} {timestamp}")
    
    return files_chunk

# Main test case generator function using multithreading
def generate_test_case(num_files):
    # Track unique IDs in a set
    generated_ids = set()
    
    # Split the workload into chunks for multithreading
    chunk_size = num_files // 8  # Divide work into 8 parts for multithreading, adjust as needed
    chunks = [(i * chunk_size, (i + 1) * chunk_size) for i in range(8)]
    chunks[-1] = (chunks[-1][0], num_files)  # Make sure the last chunk includes all remaining files
    
    # Choose a random sorting criterion
    sorting_criteria = ["Name", "ID", "Timestamp"]
    criterion = random.choice(sorting_criteria)
    with open("temp.txt", "w") as file:
        file.write(f"{num_files}\n")
        
        # Use ThreadPoolExecutor to process file chunks in parallel
        with concurrent.futures.ThreadPoolExecutor() as executor:
            futures = []
            for start_idx, end_idx in chunks:
                futures.append(executor.submit(generate_file_chunk, start_idx, end_idx, generated_ids))

            # Collect results from each chunk and write to file
            for future in concurrent.futures.as_completed(futures):
                chunk = future.result()
                file.writelines([line + '\n' for line in chunk])  # Write chunk to file
        
        # Write the sorting criterion at the end of the file
        file.write(f"{criterion}\n")

    print("Test case generated in 'test_case1.txt'")

# Function to execute measure.sh and parse runtime and memory usage
def measure_execution():
    result = subprocess.run(["./measure.sh", "./test"], capture_output=True, text=True)
    
    # Parse time and memory from the output
    merge_sort_time_pattern = re.compile(r"Parallel merge sort completed in \x1b\[33m([\d.]+) seconds")
    runtime = 0.0
    merge_sort_time = 0.0
    memory_usage = 0.0
    for line in result.stdout.splitlines():
        if "Total Runtime" in line:
            runtime = float(line.split(":")[-1].strip())
        elif "Max Memory Usage" in line:
            memory_usage = float(line.split(":")[-1].strip()) 
        elif merge_sort_time_pattern.search(line):
            merge_sort_time = float(merge_sort_time_pattern.search(line).group(1))
    print(merge_sort_time)
    print(memory_usage)
    return merge_sort_time, memory_usage

# Main function to run experiments
def run_experiment(max_n, step):
    ns = list(range(step, max_n + 1, step))
    merge_sort_times = []
    merge_sort_memory = []
    # ns =  [10,100, 1000, 10000, 100000, 1000000]
    ns =  [100,1000, 10000, 100000, 1000000, 5000000, 10000000]
        
    for n in ns:
        with open("temp.txt", "w") as file:
            file.write(f"{n}\n")
                    
        # generate_test_case(n)
        
        # Measure Distributed Merge Sort
        exec_time, memory_usage = measure_execution()
        merge_sort_times.append(exec_time)
        merge_sort_memory.append(memory_usage)

        print(f"Completed for n = {n}: Merge Sort Time = {merge_sort_times[-1]}, Memory Usage = {merge_sort_memory[-1]} KB")

    # Save results for reference
    with open(RESULT_FILE, "w") as f:
        f.write("n,Merge Sort Time,Merge Sort Memory\n")
        for i in range(len(ns)):
            f.write(f"{ns[i]},{merge_sort_times[i]},{merge_sort_memory[i]}\n")

    # Plotting
    plot_execution_times(ns, merge_sort_times)
    plot_memory_usage(ns,merge_sort_memory)

def plot_execution_times(ns, merge_sort_times):
    plt.figure(figsize=(12, 8))  # Increased figure size for better visibility
    
    # Improved color scheme and marker style
    plt.plot(ns, merge_sort_times, label="Distributed Merge Sort", marker='o', color='#1f77b4', markersize=8, linewidth=2.0)
    
    # Logarithmic scale for both axes
    plt.yscale('log')
    plt.xscale('log')
    
    # Enhanced labels and title
    plt.xlabel("Input Size (n)", fontsize=14, fontweight='bold', color='#333333')
    plt.ylabel("Execution Time (seconds)", fontsize=14, fontweight='bold', color='#333333')
    plt.title("Execution Time vs Input Size for Distributed Merge Sort", fontsize=16, fontweight='bold', color='#333333')

    # Customizing the legend
    plt.legend(fontsize=12, loc='upper left', frameon=True, shadow=True, borderpad=1)

    # Adding grid for better readability
    plt.grid(True, which="both", linestyle='--', linewidth=0.7, alpha=0.7)
    
    # Annotate specific data points
    for i, (n, time) in enumerate(zip(ns, merge_sort_times)):
        plt.annotate(f"{time:.2e}", 
                     (n, time), 
                     textcoords="offset points", 
                     xytext=(0,10), 
                     ha='center', 
                     fontsize=10, 
                     color='#1f77b4')

    # Beautify the x-axis and y-axis ticks with custom format
    plt.xticks(ticks=ns, labels=[f"{int(n):,}" for n in ns], fontsize=12, rotation=45)
    plt.yticks(fontsize=12)

    # Save the figure with high resolution
    plt.tight_layout()
    plt.savefig("enhanced_execution_time_comparison.png", dpi=300)
    plt.show()


def plot_memory_usage(n_values, memory_usages):
    """Plot memory usage with improved aesthetics."""
    plt.style.use("ggplot")

    fig, ax = plt.subplots(figsize=(12, 7))
    colors = plt.cm.viridis([0.2, 0.4, 0.6, 0.8, 1.0])
    bar_width = 0.5  # Reduced width
    bars = ax.bar([str(n) for n in n_values], memory_usages, color=colors, edgecolor='black', width=bar_width)

    for bar in bars:
        height = bar.get_height()
        ax.annotate(f'{height} KB',
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 5),
                    textcoords="offset points",
                    ha='center', va='bottom', fontsize=12, color='black')

    ax.set_xlabel("Input Size (n)", fontsize=14, fontweight='bold')
    ax.set_ylabel("Maximum Memory Usage (KB)", fontsize=14, fontweight='bold')
    ax.set_title("Enhanced Memory Usage Analysis for Distributed Merge Sort", fontsize=16, fontweight='bold')
    ax.set_xticks(range(len(n_values)))
    ax.set_xticklabels([f"{n:,}" for n in n_values], fontsize=12)
    ax.tick_params(axis='y', labelsize=12)
    ax.grid(visible=True, linestyle='--', linewidth=0.7)
    ax.legend(["Memory Usage"], loc="upper left", fontsize=12)
    plt.tight_layout()
    plt.savefig("Memory Bar Graph.png", dpi=300)
    plt.show()

if __name__ == "__main__":
    max_n = 10000   # Maximum input size
    step = 1000     # Incremental step for input size

    run_experiment(max_n, step)
