import re
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FormatStrFormatter

# File to parse
output_file = "count_test_output.txt"
summary_file = "performance_summary_merge_sort.txt"  

# Data storage
regular_times = []
parallel_times = []
speedups = []
time_differences = []
memory_usages = []

# Regular expressions to extract data
regular_time_pattern = re.compile(r"Regular merge sort completed in \x1b\[33m([\d.]+) seconds")
parallel_time_pattern = re.compile(r"Parallel merge sort completed in \x1b\[33m([\d.]+) seconds")
speedup_pattern = re.compile(r"Speedup: ([\d.]+)")
memory_pattern = re.compile(r"Max Memory Usage \(KB\): (\d+)")

# Parse the output file
with open(output_file, "r") as f:
    content = f.read()
    # Split content by runs if there are multiple runs
    runs = content.split("Run")
    
    for run in runs:
        if not run.strip():
            continue
            
        regular_time_match = regular_time_pattern.search(run)
        if regular_time_match:
            regular_times.append(float(regular_time_match.group(1)))
        
        parallel_time_match = parallel_time_pattern.search(run)
        if parallel_time_match:
            parallel_times.append(float(parallel_time_match.group(1)))
        
        speedup_match = speedup_pattern.search(run)
        if speedup_match:
            speedups.append(float(speedup_match.group(1)))
            
        memory_match = memory_pattern.search(run)
        if memory_match:
            memory_usages.append(int(memory_match.group(1)))

# Calculate time differences
for i in range(len(regular_times)):
    time_differences.append(regular_times[i] - parallel_times[i])

# Define a modern color palette
colors = {
    'regular': '#FF7E67',    # Coral orange
    'parallel': '#2EC4B6',   # Teal
    'difference': '#3D84A8', # Steel blue
    'speedup': '#78C091',    # Forest green
    'memory': '#FFA62B',     # Golden orange
    'background': '#F7F7F7', # Light gray
    'grid': '#E0E0E0'        # Medium gray
}

# Set style parameters
plt.style.use('seaborn-v0_8-whitegrid')
plt.rcParams.update({
    'figure.dpi': 300,
    'font.size': 10,
    'font.family': 'sans-serif',
    'axes.titlesize': 12,
    'axes.labelsize': 11,
    'figure.figsize': (15, 10),
    'axes.facecolor': colors['background'],
    'figure.facecolor': 'white',
    'grid.color': colors['grid']
})

# Create subplots for all metrics
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
fig.suptitle('Merge Sort Performance Analysis', fontsize=16, fontweight='bold', y=0.95)

# 1. Execution Times Comparison
bar_width = 0.35
x = np.arange(len(regular_times))
ax1.bar(x - bar_width/2, regular_times, bar_width, label='Regular Sort', color=colors['regular'])
ax1.bar(x + bar_width/2, parallel_times, bar_width, label='Parallel Sort', color=colors['parallel'])
ax1.set_xlabel('Run Number')
ax1.set_ylabel('Time (seconds)')
ax1.set_title('Execution Times Comparison')
ax1.legend()
ax1.grid(True, alpha=0.3)

# 2. Speedup Plot
ax2.plot(x, speedups, marker='o', color=colors['speedup'], linewidth=2)
ax2.set_xlabel('Run Number')
ax2.set_ylabel('Speedup Factor')
ax2.set_title('Speedup Analysis')
ax2.grid(True, alpha=0.3)
ax2.axhline(y=1, color='red', linestyle='--', alpha=0.5)

# 3. Time Differences
ax3.bar(x, time_differences, color=colors['difference'])
ax3.set_xlabel('Run Number')
ax3.set_ylabel('Time Difference (seconds)')
ax3.set_title('Time Savings (Regular - Parallel)')
ax3.grid(True, alpha=0.3)

# 4. Memory Usage
if memory_usages:
    ax4.plot(x, memory_usages, marker='s', color=colors['memory'], linewidth=2)
    ax4.set_xlabel('Run Number')
    ax4.set_ylabel('Memory Usage (KB)')
    ax4.set_title('Memory Usage Analysis')
    ax4.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig("merge_sort_analysis.png", dpi=300, bbox_inches='tight')

# Generate performance summary
performance_summary = f"""
Merge Sort Performance Analysis Summary
====================================

Execution Statistics:
-------------------
Number of Runs Analyzed: {len(regular_times)}

Time Performance:
---------------
Regular Sort:
  - Average: {np.mean(regular_times):.6f} seconds
  - Min: {min(regular_times):.6f} seconds
  - Max: {max(regular_times):.6f} seconds

Parallel Sort:
  - Average: {np.mean(parallel_times):.6f} seconds
  - Min: {min(parallel_times):.6f} seconds
  - Max: {max(parallel_times):.6f} seconds

Speedup Analysis:
---------------
- Average Speedup: {np.mean(speedups):.4f}x
- Maximum Speedup: {max(speedups):.4f}x
- Minimum Speedup: {min(speedups):.4f}x
- Speedup Std Dev: {np.std(speedups):.4f}

Memory Usage:
-----------
- Average: {np.mean(memory_usages):.2f} KB
- Maximum: {max(memory_usages)} KB
- Minimum: {min(memory_usages)} KB
"""

# Save the summary to a file
with open(summary_file, 'w') as f:
    f.write(performance_summary)

print(performance_summary)