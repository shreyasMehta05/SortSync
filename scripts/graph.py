import re
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import FormatStrFormatter  # Import for precise formatting

# File to parse
output_file = "output_results_4_scaled.txt"
summary_file = "performance_summary_4_scaled.txt"  # File to store performance summary

# Data storage
thresholds = []
regular_times = []
parallel_times = []
speedups = []
time_differences = []

# Regular expressions to extract data
threshold_pattern = re.compile(r"--- Threshold: (\d+) ---")
regular_time_pattern = re.compile(r"Regular merge sort completed in \x1b\[33m([\d.]+) seconds")
parallel_time_pattern = re.compile(r"Parallel merge sort completed in \x1b\[33m([\d.]+) seconds")
speedup_pattern = re.compile(r"Speedup: ([\d.]+)")


# Parse the output file
with open(output_file, "r") as f:
    for line in f:
        threshold_match = threshold_pattern.search(line)
        if threshold_match:
            thresholds.append(int(threshold_match.group(1)))
            
        
        regular_time_match = regular_time_pattern.search(line)
        if regular_time_match:
            regular_times.append(float(regular_time_match.group(1)))
        
        parallel_time_match = parallel_time_pattern.search(line)
        if parallel_time_match:
            parallel_times.append(float(parallel_time_match.group(1)))
        
        speedup_match = speedup_pattern.search(line)
        if speedup_match:
            speedups.append(float(speedup_match.group(1)))


# Calculate time differences
for i in range(len(regular_times)):
    time_differences.append(regular_times[i] - parallel_times[i])

if len(time_differences) == 0:
    print("Warning: No time differences calculated. Check input data.")

print(time_differences)
print(speedups)
print(regular_times)
print(parallel_times)
# Define a modern color palette
colors = {
    'regular': '#2E86AB',    # Deep blue
    'parallel': '#48A9A6',   # Teal
    'difference': '#E4959E', # Coral pink
    'speedup': '#6B4E71',    # Deep purple
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
    'grid.color': colors['grid'],
    'figure.subplot.left': 0.1,
    'figure.subplot.right': 0.9,
    'figure.subplot.bottom': 0.15,
    'figure.subplot.top': 0.85
})

# First plot: Times comparison
fig, ax = plt.subplots(figsize=(10, 6))

# Add a subtle background color
ax.set_facecolor(colors['background'])
fig.patch.set_facecolor('white')

# Plot with enhanced styling
ax.plot(thresholds, regular_times, 
        label="Regular Merge Sort Time", 
        marker='o', 
        markersize=8, 
        linewidth=2.5, 
        color=colors['regular'],
        alpha=0.9)

ax.plot(thresholds, parallel_times, 
        label="Parallel Merge Sort Time", 
        marker='o', 
        markersize=8, 
        linewidth=2.5, 
        color=colors['parallel'],
        alpha=0.9)

if time_differences:
    ax.plot(thresholds, time_differences, 
            label="Time Difference", 
            marker='o', 
            markersize=8, 
            linestyle='--', 
            linewidth=2.5, 
            color=colors['difference'],
            alpha=0.8)

# Enhance the appearance
ax.set_xscale('log')
ax.set_xlabel("Threshold", fontweight='bold', fontsize=11)
ax.set_ylabel("Time (seconds)", fontweight='bold', fontsize=11)
ax.set_title("Merge Sort Performance Analysis", fontsize=14, pad=20, fontweight='bold')
ax.legend(loc='best', framealpha=0.95, edgecolor='none')
ax.grid(True, color=colors['grid'], linestyle='-', linewidth=0.5, alpha=0.5)

# Adjust y-axis precision
ax.yaxis.set_major_formatter(FormatStrFormatter('%.6f'))
# plt.ylim(0, max(regular_times) * 1.2)

# Save the first plot
plt.savefig("merge_sort_times_comparison_4_scaled.png", dpi=300)
# plt.show()

# Second plot: Speedup
fig, ax = plt.subplots(figsize=(10, 6))

# Add background color
ax.set_facecolor(colors['background'])
fig.patch.set_facecolor('white')

# Plot speedup with enhanced styling
ax.plot(thresholds, speedups, 
        label="Speedup", 
        marker='o', 
        markersize=8, 
        linewidth=2.5, 
        color=colors['speedup'],
        alpha=0.9)

# Enhance the appearance
ax.set_xscale('log')
ax.set_xlabel("Threshold", fontweight='bold', fontsize=11)
ax.set_ylabel("Speedup", fontweight='bold', fontsize=11)
ax.set_title("Merge Sort Speedup Analysis", fontsize=14, pad=20, fontweight='bold')
ax.legend(loc='best', framealpha=0.95, edgecolor='none')
ax.grid(True, color=colors['grid'], linestyle='-', linewidth=0.5, alpha=0.5)

# Adjust y-axis precision
ax.yaxis.set_major_formatter(FormatStrFormatter('%.4f'))

# Save the second plot
plt.savefig("merge_sort_speedup_analysis_4_scaled.png", dpi=300)
# plt.show()

# Print performance summary and save it to a file
best_parallel_idx = np.argmin(parallel_times)
max_speedup_idx = np.argmax(speedups)

performance_summary = f"""
Performance Analysis Summary:
Optimal threshold for parallel performance: {thresholds[best_parallel_idx]}
Best parallel execution time: {parallel_times[best_parallel_idx]:.6f} seconds
Maximum speedup: {speedups[max_speedup_idx]:.4f}x at threshold {thresholds[max_speedup_idx]}

Average speedup: {np.mean(speedups):.4f}x
Median speedup: {np.median(speedups):.4f}x
Speedup standard deviation: {np.std(speedups):.4f}
"""

# Save the summary to a file
with open(summary_file, 'w') as f:
    f.write(performance_summary)

print(performance_summary)
