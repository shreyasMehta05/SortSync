import subprocess
import numpy as np

# Define the thresholds you want to test
# thresholds = [1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 5000000, 10000000]
thresholds = [
    1000, 2000, 3000, 4000, 5000, 
    6000, 7000, 8000, 9000, 10000, 
    15000, 20000, 25000, 30000, 40000, 
    50000, 75000, 100000, 150000, 200000, 
    250000, 500000, 750000, 1000000, 
    2000000, 3000000, 5000000, 7000000, 
    10000000, 20000000, 30000000, 50000000
    , 70000000, 100000000
]
# make 
thresholds = [
    # 1, 2, 3, 4, 5, 
    # 6, 7, 8, 9, 10, 
    # 15, 20, 25, 30, 40, 
    50, 75, 100, 150, 200, 
    250, 500, 750, 1000, 
    2000, 3000, 5000, 7000, 
    10000, 20000, 30000, 50000
    , 70000, 100000
]


print(thresholds)
# print(x)

output_file = "output_results_4_scaled.txt"

# Open the output file for writing
with open(output_file, "w") as f:
    for threshold in thresholds:
        # Compile with the specified threshold
        make_command = f"make clean; make THRESHOLD={threshold} TEST=true"
        try:
            # Run the make command and capture its output
            make_result = subprocess.run(make_command, shell=True, text=True, capture_output=True)
            
            # Write the make output to the file
            f.write(f"--- Threshold: {threshold} ---\n")
            f.write("Compilation Output:\n")
            f.write(make_result.stdout)
            f.write(make_result.stderr)
            f.write("\n")

            # Run the main program and capture its output
            main_command = "./main"
            main_result = subprocess.run(main_command, shell=True, text=True, capture_output=True)
            
            # Write the main output to the file
            f.write("Execution Output:\n")
            f.write(main_result.stdout)
            f.write(main_result.stderr)
            f.write("\n\n")
            
            print(f"Completed threshold {threshold}")
        
        except subprocess.CalledProcessError as e:
            print(f"An error occurred with threshold {threshold}: {e}")
            f.write(f"Error with threshold {threshold}:\n{e}\n\n")

print(f"Results have been written to {output_file}")
