#define _XOPEN_SOURCE  // Ensures POSIX compatibility for functions like strptime()

#include "parallel_count_sort.h"
#include "file_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <ctype.h>  // For functions like tolower()
#include <stdint.h> // For fixed-width integer types
#include <math.h>   // For mathematical functions

// #define TEST
#define NUM_THREADS 4
typedef struct {
    FileInfo *files;
    int **local_counts;
    int start;
    int end;
    int max_value;
    int thread_index;
    int min_value;
    int* count;
} ThreadData;

#define MAXY (int)1e8
int count[MAXY] = {0};
void *count_frequency_by_id(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Local frequency count for each thread
    int *local_count = (int *)calloc(data->max_value - data->min_value + 1, sizeof(int));
    if (!local_count) {
        perror("Memory allocation failed");
        pthread_exit(NULL);
    }

    // Count occurrences within the assigned range
    for (int i = data->start; i < data->end; i++) {
        local_count[data->files[i].fileId - data->min_value]++;
    }

    // Store the local count into the array of local counts
    data->local_counts[data->thread_index] = local_count;

    pthread_exit(NULL);
}

void parallel_count_sort_by_id(FileInfo *files, int num_files) {
   
    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the minimum and maximum fileId
    int min_id = files[0].fileId;
    int max_id = files[0].fileId;
    for (int i = 1; i < num_files; i++) {
        if (files[i].fileId < min_id) min_id = files[i].fileId;
        if (files[i].fileId > max_id) max_id = files[i].fileId;

    }
    int range = max_id - min_id + 1;

    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_id, max_id, range);

    #endif


    // Allocate space for each thread's local count array
    int **local_counts = (int **)malloc(num_threads * sizeof(int *));
    if (!local_counts) {
        perror("Memory allocation failed");
        return;
    }
    
    #ifdef TEST
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    #endif

    // Divide the array into chunks and start counting threads
    int chunk_size = num_files / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].local_counts = local_counts;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_id;
        thread_data[i].min_value = min_id;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_by_id, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %f seconds\n", time_taken);
    #endif

    // Aggregate counts into a global count array
    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }

    for (int i = 0; i < num_threads; i++) {
        for (int j = 0; j < range; j++) {
            global_count[j] += local_counts[i][j];
        }
        free(local_counts[i]);  // Free each thread's local count array after aggregation
    }
    free(local_counts);
    




    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }
    // Build the sorted output array based on fileId
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
    // Copy the sorted elements back to the original array
    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }
     
    // Free allocated memory
    free(global_count);
    free(sorted_files);


}






void *count_frequency_id_sync(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // increase the int count array for each fileHash for the given_range for this thread
    for (int i = data->start; i < data->end; i++) {
        // __sync_fetch_and_add(&count[data->files[i].fileHash - data->min_value], 1);
        __sync_fetch_and_add(&count[data->files[i].fileId - data->min_value], 1);
    }

    return NULL;
}



void parallel_count_sort_by_id_sync(FileInfo *files, int num_files) {
    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the min and max fileHash (which is the id now)
    int min_id = files[0].fileId;
    int max_id = files[0].fileId;

    for (int i = 1; i < num_files; i++) {
        if (files[i].fileId < min_id) min_id = files[i].fileId;
        if (files[i].fileId > max_id) max_id = files[i].fileId;
    }
    int range = max_id - min_id + 1;
    
    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_id, max_id, range);
    #endif
    
    #ifdef TEST
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif

    // Divide the array into chunks and start counting threads

    int chunk_size = num_files / num_threads;

    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_id;
        thread_data[i].min_value = min_id;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_id_sync, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %.6f seconds\n", elapsed_time);
    #endif

    
    #ifdef TEST
    elapsed_time = 0;
    #endif

    ////////////////////////////////////////////
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    ////////////////////////////////////////////

    // Prefix sum to calculate the cumulative count
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileId - min_id;
        int sorted_index = count[file_hash_index] - 1;
        count[file_hash_index]--;
        sorted_files[sorted_index] = files[i];
    }
    memmove(files, sorted_files, num_files * sizeof(FileInfo));

    free(sorted_files);

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    elapsed_time += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Counting): %.6f seconds\n", elapsed_time);
    #endif

   
}




// Helper function to convert timestamp (e.g. "2024-11-10 14:30:00") to epoch time (seconds since 1970)
time_t convert_timestamp_to_epoch(const char *timestamp) {
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


void *count_frequency_by_timestamp(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Local frequency count for each thread
    int *local_count = (int *)calloc(data->max_value - data->min_value + 1, sizeof(int));
    if (!local_count) {
        perror("Memory allocation failed");
        pthread_exit(NULL);
    }

    // Count occurrences within the assigned range based on fileHash (epoch time)
    for (int i = data->start; i < data->end; i++) {
        local_count[data->files[i].fileHash - data->min_value]++;
    }

    // Store the local count into the array of local counts
    data->local_counts[data->thread_index] = local_count;

    pthread_exit(NULL);
}

void parallel_count_sort_by_timestamp(FileInfo *files, int num_files) {

    // Calculate the min and max timestamp so we can use it as the range for sorting
    for (int i = 0; i < num_files; i++) {
        time_t epoch_time = convert_timestamp_to_epoch(files[i].timestamp);
        if (epoch_time == -1) {
            fprintf(stderr, "Failed to convert timestamp to epoch time\n");
            return;
        }
        files[i].fileHash = epoch_time; // Store the epoch time in fileHash
    }

    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the min and max fileHash (which is the epoch time now)
    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;
    for (int i = 1; i < num_files; i++) {
        if (files[i].fileHash < min_hash) min_hash = files[i].fileHash;
        if (files[i].fileHash > max_hash) max_hash = files[i].fileHash;
    }
    int range = max_hash - min_hash + 1;

    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_hash, max_hash, range);
    #endif

    // Allocate space for each thread's local count array
    int **local_counts = (int **)malloc(num_threads * sizeof(int *));
    if (!local_counts) {
        perror("Memory allocation failed");
        return;
    }

    #ifdef TEST
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    #endif

    // Divide the array into chunks and start counting threads
    int chunk_size = num_files / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].local_counts = local_counts;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_hash;
        thread_data[i].min_value = min_hash;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_by_timestamp, &thread_data[i]);  // Use the correct function here
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %f seconds\n", time_taken);
    #endif


    // Aggregate counts into a global count array
    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }

    for (int i = 0; i < num_threads; i++) {
        for (int j = 0; j < range; j++) {
            global_count[j] += local_counts[i][j];
        }
        free(local_counts[i]);  // Free each thread's local count array after aggregation
    }
    free(local_counts);



    // Prefix sum to calculate the cumulative count
    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }

    // Build the sorted output array based on fileHash (epoch time)
    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    if (!sorted_files) {
        perror("Memory allocation failed");
        free(global_count);
        return;
    }

    // Sort based on fileHash (epoch time)
    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = global_count[file_hash_index] - 1;
        sorted_files[sorted_index] = files[i];
        global_count[file_hash_index]--;
    }

    // Copy the sorted elements back to the original array
    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }

    // Free allocated memory
    free(global_count);
    free(sorted_files);


}







int hash_name(char* key)
{
    // printf("%s  ",key);
    // const char *dot_txt = strstr(key, ".txt");
    long long int val = 0;
    int l = strlen(key);
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


typedef struct {
    int **local_counts;
    int *global_count;
    int start;
    int end;
    int num_threads;
} SumThreadData;

void *sum_local_counts(void *arg) {
    SumThreadData *data = (SumThreadData *)arg;
    for (int j = data->start; j < data->end; j++) {
        for (int i = 0; i < data->num_threads; i++) {
            data->global_count[j] += data->local_counts[i][j];
        }
    }
    return NULL;
}
// Function to parallelize the summation process
void parallel_aggregate_local_counts(int **local_counts, int *global_count, int range, int num_threads) {
    SumThreadData sum_data[num_threads];
    pthread_t sum_threads[num_threads];
    int chunk_size = range / num_threads;

    // Create threads to handle portions of the global_count array
    for (int i = 0; i < num_threads; i++) {
        sum_data[i].local_counts = local_counts;
        sum_data[i].global_count = global_count;
        sum_data[i].start = i * chunk_size;
        sum_data[i].end = (i == num_threads - 1) ? range : (i + 1) * chunk_size;
        sum_data[i].num_threads = num_threads;
        pthread_create(&sum_threads[i], NULL, sum_local_counts, &sum_data[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(sum_threads[i], NULL);
    }
    

}

void *count_frequency_by_name(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // Local frequency count for each thread
    int *local_count = (int *)calloc(data->max_value - data->min_value + 1, sizeof(int));
    if (!local_count) {
        perror("Memory allocation failed");
        pthread_exit(NULL);
    }

    // Count occurrences within the assigned range based on fileHash (name hash)
    for (int i = data->start; i < data->end; i++) {
        local_count[data->files[i].fileHash - data->min_value]++;
    }

    // Store the local count into the array of local counts
    data->local_counts[data->thread_index] = local_count;

    pthread_exit(NULL);
}

void parallel_count_sort_by_name(FileInfo *files, int num_files) {

    // Calculate the min and max name hash so we can use it as the range for sorting
    for (int i = 0; i < num_files; i++) {
        files[i].fileHash = hash_name(files[i].name); // Store the hash of the file name in fileHash
    }

    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the min and max fileHash (which is the hash of the file name now)
    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;
    for (int i = 1; i < num_files; i++) {
        if (files[i].fileHash < min_hash) min_hash = files[i].fileHash;
        if (files[i].fileHash > max_hash) max_hash = files[i].fileHash;
    }
    int range = max_hash - min_hash + 1;

    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_hash, max_hash, range);
    #endif


    // Allocate space for each thread's local count array
    int **local_counts = (int **)malloc(num_threads * sizeof(int *));
    if (!local_counts) {
        perror("Memory allocation failed");
        return;
    }
    
    #ifdef TEST
    struct timespec start_time, end_time;  
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    // Divide the array into chunks and start counting threads
    int chunk_size = num_files / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].local_counts = local_counts;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_hash;
        thread_data[i].min_value = min_hash;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_by_name, &thread_data[i]);  // Use the name-based counting function
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %.6f seconds\n", elapsed_time);
    #endif

    // Aggregate counts into a global count array
    int *global_count = (int *)calloc(range, sizeof(int));
    if (!global_count) {
        perror("Memory allocation failed");
        return;
    }
    #ifdef TEST
    elapsed_time = 0;
    #endif

    ////////////////////////////////////////////
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    ////////////////////////////////////////////
    // Parallelize the aggregation process here
    parallel_aggregate_local_counts(local_counts, global_count, range, num_threads);
    for(int i = 0; i < num_threads; i++){
        free(local_counts[i]);
    }
    free(local_counts);
    // for (int i = 0; i < num_threads; i++) {
    //     for (int j = 0; j < range; j++) {
    //         global_count[j] += local_counts[i][j];
    //     }
    //     free(local_counts[i]);  // Free each thread's local count array after aggregation
    // }
    // free(local_counts);
    // Prefix sum to calculate the cumulative count
    for (int i = 1; i < range; i++) {
        global_count[i] += global_count[i - 1];
    }

    // Build the sorted output array based on fileHash (name hash)
    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    if (!sorted_files) {
        perror("Memory allocation failed");
        free(global_count);
        return;
    }

    // Sort based on fileHash (name hash)
    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = global_count[file_hash_index] - 1;
        sorted_files[sorted_index] = files[i];
        global_count[file_hash_index]--;
    }

    // Copy the sorted elements back to the original array
    for (int i = 0; i < num_files; i++) {
        files[i] = sorted_files[i];
    }

    // Free allocated memory
    free(global_count);
    free(sorted_files);
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
     elapsed_time += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Counting): %.6f seconds\n", elapsed_time);
    #endif
}



void *count_frequency_by_name_sync(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // increase the int count array for each fileHash for the given_range for this thread
    for (int i = data->start; i < data->end; i++) {
        __sync_fetch_and_add(&count[data->files[i].fileHash - data->min_value], 1);
    }

    return NULL;
}


void parallel_count_sort_by_name_sync(FileInfo *files, int num_files) {

    // Calculate the min and max name hash so we can use it as the range for sorting
    for (int i = 0; i < num_files; i++) {
        files[i].fileHash = hash_name(files[i].name); // Store the hash of the file name in fileHash
    }

    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the min and max fileHash (which is the hash of the file name now)
    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;
    for (int i = 1; i < num_files; i++) {
        if (files[i].fileHash < min_hash) min_hash = files[i].fileHash;
        if (files[i].fileHash > max_hash) max_hash = files[i].fileHash;
    }
    int range = max_hash - min_hash + 1;

    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_hash, max_hash, range);
    #endif


    // Allocate space for each thread's local count array
    // int **local_counts = (int **)malloc(num_threads * sizeof(int *));
    // if (!local_counts) {
    //     perror("Memory allocation failed");
    //     return;
    // }
    
    #ifdef TEST
    struct timespec start_time, end_time;  
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    // Divide the array into chunks and start counting threads
    int chunk_size = num_files / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        // thread_data[i].local_counts = local_counts;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_hash;
        thread_data[i].min_value = min_hash;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_by_name_sync, &thread_data[i]);  // Use the name-based counting function
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %.6f seconds\n", elapsed_time);
    #endif

    
    #ifdef TEST
    elapsed_time = 0;
    #endif

    ////////////////////////////////////////////
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    ////////////////////////////////////////////
    
    // Prefix sum to calculate the cumulative count
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = count[file_hash_index] - 1;
        count[file_hash_index]--;
        sorted_files[sorted_index] = files[i];
    }
    memmove(files, sorted_files, num_files * sizeof(FileInfo));

    free(sorted_files);
    // free(local_counts);

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
     elapsed_time += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Counting): %.6f seconds\n", elapsed_time);
    #endif
}



void *count_frequency_by_timestamp_sync(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    // increase the int count array for each fileHash for the given_range for this thread
    for (int i = data->start; i < data->end; i++) {
        __sync_fetch_and_add(&count[data->files[i].fileHash - data->min_value], 1);
    }

    return NULL;
}

void parallel_count_sort_by_timestamp_sync(FileInfo *files, int num_files) {

    // Calculate the min and max timestamp so we can use it as the range for sorting
    for (int i = 0; i < num_files; i++) {
        time_t epoch_time = convert_timestamp_to_epoch(files[i].timestamp);
        if (epoch_time == -1) {
            fprintf(stderr, "Failed to convert timestamp to epoch time\n");
            return;
        }
        files[i].fileHash = epoch_time; // Store the epoch time in fileHash
    }

    int num_threads = NUM_THREADS;  // You can adjust the number of threads
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    // Find the min and max fileHash (which is the epoch time now)
    int min_hash = files[0].fileHash;
    int max_hash = files[0].fileHash;
    for (int i = 1; i < num_files; i++) {
        if (files[i].fileHash < min_hash) min_hash = files[i].fileHash;
        if (files[i].fileHash > max_hash) max_hash = files[i].fileHash;
    }
    int range = max_hash - min_hash + 1;

    #ifdef TEST
    printf("Min: %d, Max: %d, Range: %d\n", min_hash, max_hash, range);
    #endif

    #ifdef TEST
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif

    // Divide the array into chunks and start counting threads
    int chunk_size = num_files / num_threads;
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].files = files;
        thread_data[i].start = i * chunk_size;
        thread_data[i].end = (i == num_threads - 1) ? num_files : (i + 1) * chunk_size;
        thread_data[i].max_value = max_hash;
        thread_data[i].min_value = min_hash;
        thread_data[i].thread_index = i;
        pthread_create(&threads[i], NULL, count_frequency_by_timestamp_sync, &thread_data[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Threading): %.6f seconds\n", elapsed_time);
    #endif

    #ifdef TEST
    elapsed_time = 0;
    #endif

    ////////////////////////////////////////////
    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    #endif
    ////////////////////////////////////////////

    // Prefix sum to calculate the cumulative count
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    FileInfo *sorted_files = (FileInfo *)malloc(num_files * sizeof(FileInfo));
    for (int i = num_files - 1; i >= 0; i--) {
        int file_hash_index = files[i].fileHash - min_hash;
        int sorted_index = count[file_hash_index] - 1;
        count[file_hash_index]--;
        sorted_files[sorted_index] = files[i];
    }
    memmove(files, sorted_files, num_files * sizeof(FileInfo));

    free(sorted_files);

    #ifdef TEST
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    elapsed_time += (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("Time taken for counting(Counting): %.6f seconds\n", elapsed_time);
    #endif

}

