#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILES (int)1e6
#define MAX_NAME_LENGTH 6
#define START_YEAR 2023
#define MAX_ID (int)1e6

// Generates a unique ID not in the generated_ids array
int generate_unique_id(int *generated_ids, int count) {
    int id;
    int unique;
    do {
        unique = 1;
        id = rand() % MAX_ID + 1;
        for (int i = 0; i < count; i++) {
            if (generated_ids[i] == id) {
                unique = 0;
                break;
            }
        }
    } while (!unique);
    return id;
}

// Generates a random ISO 8601 timestamp within the year START_YEAR
void generate_random_timestamp(char *timestamp) {
    struct tm time;
    time.tm_year = START_YEAR - 1900;
    time.tm_mon = rand() % 12;
    time.tm_mday = rand() % 28 + 1;
    time.tm_hour = rand() % 24;
    time.tm_min = rand() % 60;
    time.tm_sec = rand() % 60;

    strftime(timestamp, 20, "%Y-%m-%dT%H:%M:%S", &time);
}

// Generates a random filename of length len
void generate_random_filename(char *filename, int len) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < len - 1; i++) {
        filename[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    filename[len - 1] = '\0';  // Null-terminate the string
}
// Main test case generator function
void generate_test_case(int num_files) {
    const char *sorting_criteria[] = {"Name", "ID", "Timestamp"};
    int ids[MAX_FILES] = {0};  // Array to track unique IDs
    FILE *file = fopen("test_case.txt", "w");

    if (file == NULL) {
        perror("Unable to create test case file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", num_files);
    if(num_files>1e6){
        
    }
    for (int i = 0; i < num_files; i++) {
        char file_name[MAX_NAME_LENGTH];
        int file_id = generate_unique_id(ids, i);
        ids[i] = file_id;
        char timestamp[20];

        
        int len = rand() % (MAX_NAME_LENGTH) + 3;
        generate_random_filename(file_name, len);
        generate_random_timestamp(timestamp);

        fprintf(file, "%s.txt %d %s\n", file_name, file_id, timestamp);
    }

    // Choose a random sorting criterion
    const char *criterion = sorting_criteria[rand() % 3];
    fprintf(file, "%s\n", criterion);

    fclose(file);
    printf("Test case generated in 'test_case.txt'\n");
}

int main() {
    srand((unsigned int)time(NULL));  // Seed random number generator

    int num_files;
    printf("Enter the number of files to generate: ");
    scanf("%d", &num_files);
    if (num_files > MAX_FILES) {
        fprintf(stderr, "Max number of files is %d\n", MAX_FILES);
        return EXIT_FAILURE;
    }

    generate_test_case(num_files);
    return EXIT_SUCCESS;
}
