#define _XOPEN_SOURCE  // Ensures POSIX compatibility for functions like strptime()

#include "count_sort.h" 
#include "file_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <ctype.h>  // For functions like tolower()
#include <stdint.h> // For fixed-width integer types
#include <math.h>   // For mathematical functions



// Function to perform counting sort on FileInfo array by fileId
void count_sort_by_id(FileInfo *files, int num_files) {
    
    
    // Find the minimum and maximum fileId


    int min_id = files[0].fileId;
    int max_id = files[0].fileId;

    for(int i = 1; i < num_files; i++) {
        if(files[i].fileId < min_id) {
            min_id = files[i].fileId;
        }
        if(files[i].fileId > max_id) {
            max_id = files[i].fileId;
        }
    }

    int range = max_id - min_id + 1;

   
    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }

    // Step 1: Count occurrences of each fileId
    for (int i = 0; i < num_files; i++) {
        global_count[files[i].fileId - min_id]++;
    }

 
    // Step 2: Calculate start positions for each fileId
    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }

    // Step 3: Build the sorted output array
    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    if (!sorted_files) {
        perror("Memory allocation failed");
        free(global_count);
        return;
    }

    for (int i = num_files - 1; i >= 0; i--) {
        int file_id_index = files[i].fileId - min_id;
        int sorted_index = global_count[file_id_index] - 1;
        sorted_files[sorted_index] = files[i];
        global_count[file_id_index]--;
    }

    // Step 4: Copy the sorted elements back to the original array
    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }

    // Free allocated memory
    free(global_count);
    free(sorted_files);


    

   
}
int hash_name_regular(char* key)
{
    // printf("%s  ",key);
    // const char *dot_txt = strstr(key, ".txt");
    long long int val = 0;
    int l = strlen(key)-4;
    // printf("l: %d   ",l);
    int ans = 5 ;

    for (int i = 0; i < l; i++)
    {
        char c = tolower(key[i]);
        val += pow(29,ans) * (c-'a'+1);
        ans--;
    }
    // printf("hash: %lld\n",val);
    return val;
}


void count_sort_by_name(FileInfo *files, int num_files) {
    for(int i = 0; i < num_files; i++) {
        files[i].fileHash = hash_name_regular(files[i].name);
    }

    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;

    for(int i = 1; i < num_files; i++) {
        if(files[i].fileHash < min_hash) {
            min_hash = files[i].fileHash;
        }
        if(files[i].fileHash > max_hash) {
            max_hash = files[i].fileHash;
        }
    }

    int range = max_hash - min_hash + 1;

    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }
    for (int i = 0; i < num_files; i++) {
        global_count[files[i].fileHash - min_hash]++;
    }

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }

    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    if (!sorted_files) {
        perror("Memory allocation failed");
        free(global_count);
        return;
    }

    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = global_count[file_hash_index] - 1;
        sorted_files[sorted_index] = files[i];
        global_count[file_hash_index]--;
    }

    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }

    free(global_count);
    free(sorted_files);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(): %.6f seconds\n", elapsed_time);
    
}

// Helper function to convert timestamp (e.g. "2024-11-10 14:30:00") to epoch time (seconds since 1970)
time_t convert_timestamp_to_epoch_regular(const char *timestamp) {
    struct tm tm_time = {0};  // Initialize all fields to 0
    char *parsing_result;

    // Print input for debugging
    // printf("Parsing timestamp: '%s'\n", timestamp);

    // Use strptime for more robust parsing
    parsing_result = strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm_time);
    
    if (parsing_result == NULL) {
        fprintf(stderr, "Failed to parse timestamp: %s\n", timestamp);
        return -1;
    }

    // Set DST to -1 to let mktime determine if DST is in effect
    tm_time.tm_isdst = -1;

    // Convert to epoch time
    time_t epoch_time = mktime(&tm_time);
    if (epoch_time == -1) {
        perror("mktime failed");
        return -1;
    }
    // printf("Epoch time: %ld\n", epoch_time);
    return epoch_time;
}

void count_sort_by_timestamp(FileInfo *files, int num_files) {
    for(int i = 0; i < num_files; i++) {
        files[i].fileHash = convert_timestamp_to_epoch_regular(files[i].timestamp);
    }

    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;


    for(int i = 1; i < num_files; i++) {
        if(files[i].fileHash < min_hash) {
            min_hash = files[i].fileHash;
        }
        if(files[i].fileHash > max_hash) {
            max_hash = files[i].fileHash;
        }
    }


    int range = max_hash - min_hash + 1;

    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }

    for (int i = 0; i < num_files; i++) {
        global_count[files[i].fileHash - min_hash]++;
    }

    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }

    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    if (!sorted_files) {
        perror("Memory allocation failed");
        free(global_count);
        return;
    }

    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = global_count[file_hash_index] - 1;
        sorted_files[sorted_index] = files[i];
        global_count[file_hash_index]--;
    }

    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }

    free(global_count);
    free(sorted_files);
    
}
    