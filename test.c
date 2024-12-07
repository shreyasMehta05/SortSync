#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parallel_merge_sort.h"
#include "file_info.h"
#include <time.h>
#include "parallel_count_sort.h"
#include "count_sort.h"
#include <stdbool.h>
// #define TEST
#define __DYNAMIC_THRESHOLD__ (int)1e9
#define TEST
// ANSI escape codes for colors
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define MAGENTA "\x1b[35m"
#define RESET   "\x1b[0m"
#define PINK    "\x1b[35m"
#define BLUE    "\x1b[34m"

#ifdef TEST
#define NUM_THREADS_COUNT 4

double get_elapsed_time(struct timespec start, struct timespec end) {
    double start_sec = start.tv_sec + (double)start.tv_nsec / 1e9;
    double end_sec = end.tv_sec + (double)end.tv_nsec / 1e9;
    return end_sec - start_sec;
}

int is_sorted(FileInfo *arr, int size, int (*cmp)(const FileInfo *, const FileInfo *)) {
    for (int i = 1; i < size; i++) {
        if (cmp(&arr[i - 1], &arr[i]) > 0) return 0;
    }
    return 1;
}
// Assuming FileInfo struct has fields: char name[], int fileId, char timestamp[]

void verify_sort_stability(FileInfo original[], FileInfo sorted[], int size, int sort_by) {
    int errors = 0;
    printf("\nVerifying sort and stability for %s:\n",
           sort_by == 2 ? "Name" :
           sort_by == 1 ? "ID" : "Timestamp");

    for (int i = 1; i < size; i++) {
        bool is_error = false;
        int cmp = 0;

        switch (sort_by) {
            case 2: // Name
                cmp = comparebyNameCount(&sorted[i - 1], &sorted[i]);
                break;
            case 1: // ID
                cmp =  compareById(&sorted[i - 1], &sorted[i]);
                break;
            case 3: // Timestamp
                cmp = compareByTimestamp(&sorted[i - 1], &sorted[i]);
                break;
        }

        // Check if the array is sorted
        if (cmp > 0) {
            printf("Sort error at %d: '%s', %d, '%s' > '%s', %d, '%s'\n",
                   i,
                   sorted[i - 1].name, sorted[i - 1].fileId, sorted[i - 1].timestamp,
                   sorted[i].name, sorted[i].fileId, sorted[i].timestamp);
            is_error = true;
        }

        // Check stability if sorting key is equal, check relative order
        if (cmp == 0) {
            // Search for these elements in the original array and compare their indices
            int index1 = -1, index2 = -1;
            for (int j = 0; j < size; j++) {
                if (index1 == -1 &&
                    ((sort_by == 1 && strcmp(original[j].name, sorted[i - 1].name) == 0) ||
                     (sort_by == 2 && original[j].fileId == sorted[i - 1].fileId) ||
                     (sort_by == 3 && strcmp(original[j].timestamp, sorted[i - 1].timestamp) == 0))) {
                    index1 = j;
                }

                if (index2 == -1 &&
                    ((sort_by == 1 && strcmp(original[j].name, sorted[i].name) == 0) ||
                     (sort_by == 2 && original[j].fileId == sorted[i].fileId) ||
                     (sort_by == 3 && strcmp(original[j].timestamp, sorted[i].timestamp) == 0))) {
                    index2 = j;
                }

                if (index1 != -1 && index2 != -1) {
                    break;
                }
            }

            // If the original index of the previous element is greater, stability is violated
            if (index1 > index2) {
                printf("Stability error at %d: '%s', %d, '%s' and '%s', %d, '%s'\n",
                       i,
                       sorted[i - 1].name, sorted[i - 1].fileId, sorted[i - 1].timestamp,
                       sorted[i].name, sorted[i].fileId, sorted[i].timestamp);
                is_error = true;
            }
        }

        if (is_error) errors++;
    }

    if (errors == 0) {
        printf("Sort and stability verification passed!\n");
    } else {
        printf("Found %d errors in sort or stability\n", errors);
    }
}
#endif





int main(){
    int numOfFiles;
    FILE *inputFile = NULL;

    // Open the file for input when the test flag is enabled
#ifdef TEST
    // inputFile = fopen("check.txt", "r");
    inputFile = fopen("test_case_count_10.txt", "r");
    if (!inputFile) {
        fprintf(stderr, RED "Unable to open input file\n" RESET);
        return EXIT_FAILURE;
    }
#endif

    // Read number of files
    if (inputFile) {
        fscanf(inputFile, "%d", &numOfFiles);  // Reading from file
    } else {
        scanf("%d", &numOfFiles);  // Reading from standard input
    }

    

    // Allocate memory for file info
    FileInfo *files = mallocWrapperForFile(numOfFiles);

    // Read file data
    for (int i = 0; i < numOfFiles; i++) {
        if (inputFile) {
            fscanf(inputFile, "%s %d %s", files[i].name, &files[i].fileId, files[i].timestamp);
        } else {
            scanf("%s %d %s", files[i].name, &files[i].fileId, files[i].timestamp);
        }
    }

    char sortingCriteria[50];
    if (inputFile) {
        fscanf(inputFile, "%s", sortingCriteria);
    } else {
        scanf("%s", sortingCriteria);
    }

    if(numOfFiles<=__DYNAMIC_THRESHOLD__){
        #ifdef TEST
        printf(PINK "Running in test mode\n"RESET YELLOW);
        printf("Number of files: %d\n", numOfFiles);
        printf("Sorting by %s\n", sortingCriteria);
        printf("Number of threads: %d\n", NUM_THREADS_COUNT);
        // FileInfo *files_copy_test = mallocWrapperForFile(numOfFiles);
        // memcpy(files_copy_test, files, numOfFiles * sizeof(FileInfo));

        struct timespec start_time, end_time;
        int option;
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if(strcmp(sortingCriteria,"ID")==0){
            count_sort_by_id(files, numOfFiles);
            option = 1;
        }
        else if(strcmp(sortingCriteria,"Name")==0){
            count_sort_by_name(files, numOfFiles);
            option = 2;
        }
        else if(strcmp(sortingCriteria,"Timestamp")==0){
            count_sort_by_timestamp(files, numOfFiles);
            option = 3;
        }
    

        #endif
        #ifdef TEST
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        #endif 

        if(strcmp(sortingCriteria,"ID")==0){
            parallel_count_sort_by_id_sync(files, numOfFiles);
        }
        else if(strcmp(sortingCriteria,"Name")==0){
            parallel_count_sort_by_name_sync(files, numOfFiles);
        }
        else if(strcmp(sortingCriteria,"Timestamp")==0){
            parallel_count_sort_by_timestamp_sync(files, numOfFiles);
        }
        #ifdef TEST
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double parallel_sort_time = get_elapsed_time(start_time, end_time);
        printf(GREEN "Parallel count sort completed in "YELLOW"%.6f seconds"GREEN" for %d elements\n" RESET, parallel_sort_time, numOfFiles);
        
        

        #endif
        if(numOfFiles<=0){
            printf("\nSorted files:\n");
            printf("%s\n",sortingCriteria);
            printFiles(files, numOfFiles);
        }
        else{
            printf(MAGENTA "Too many files to print\n" RESET);
        }
        return EXIT_SUCCESS;
    }
    return 0;
}
