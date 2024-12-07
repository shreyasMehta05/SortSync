import subprocess

# File to store output
output_filename = "count_test_output.txt"
n1 = [
    1000, 2000, 3000, 4000, 5000, 
    6000, 7000, 8000, 9000, 10000, 
    15000, 20000, 25000, 30000, 40000, 
    50000, 75000, 100000
]
def run_command_n_times(n):
    with open(output_filename, "w") as output_file:
        for i in range(n):
            try:
                # Run the testcase generator
                x=7**i
                subprocess.run(["python3", "testcasegenerator.py",f"{n1[i]}"], check=True, text=True, capture_output=True)

                # Run make clean
                make_clean = subprocess.run(["make", "clean"], check=True, text=True, capture_output=True)
                output_file.write(f"Run {i + 1} - make clean:\n{make_clean.stdout}\n{make_clean.stderr}\n")

                # # Run the make command
                make_command = subprocess.run(
                    ["make", "THRESHOLD=156250", "TEST=true", "NUM_THREADS_COUNT=4"],
                    check=True, text=True, capture_output=True
                )
                output_file.write(f"Run {i + 1} - make command:\n{make_command.stdout}\n{make_command.stderr}\n\n")

                # Run measure.sh with ./main
                measure_command = subprocess.run(["./measure.sh", "./main"], check=True, text=True, capture_output=True)
                output_file.write(f"Run {i + 1} - ./measure.sh ./main output:\n{measure_command.stdout}\n{measure_command.stderr}\n\n")

            except subprocess.CalledProcessError as e:
                output_file.write(f"Error in run {i + 1}:\n{e.stdout}\n{e.stderr}\n\n")
                print(f"Run {i + 1} encountered an error. Check {output_filename} for details.")
            print(f"Run {i + 1} completed.")
# Run the command 5 times (replace 5 with the desired number of times)

run_command_n_times(n1.__len__())