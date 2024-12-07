import random
import string
import time
import concurrent.futures
from datetime import datetime

MAX_FILES = int(1e8)
MAX_NAME_LENGTH = int(5)
MAX_ID = int(1e5+1)

# Generates a random START_YEAR between 2000 and current year
def generate_random_year():
    current_year = datetime.now().year
    return random.randint(2022, current_year)

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


# Generates a random filename of length `length` using only alphabetic characters
def generate_random_filename(length):
    charset = string.ascii_letters  # Use only alphabetic characters
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
    criterion = "Name"  # Hardcoded for now
    
    with open("test_case_count_final.txt", "w") as file:
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

    print("Test case generated in 'test_case_count_final.txt'")

import sys
def main():
    random.seed(int(time.time()))  # Seed the random number generator

    # num_files = int(input("Enter the number of files to generate: "))
    # num_files = random.randint(1, MAX_FILES)
    num_files = int(sys.argv[1])
    
    if num_files > MAX_FILES:
        print(f"Max number of files is {MAX_FILES}")
        return

    generate_test_case(num_files)

if __name__ == "__main__":
    main()
