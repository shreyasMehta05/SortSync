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
#define __DYNAMIC_THRESHOLD__ (int)42

// ANSI escape codes for colors
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define MAGENTA "\x1b[35m"
#define RESET   "\x1b[0m"
#define PINK    "\x1b[35m"
#define BLUE    "\x1b[34m"

#ifdef TEST

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

#endif





int main(){
    int numOfFiles;
    FILE *inputFile = NULL;

    // Open the file for input when the test flag is enabled
#ifdef TEST
    // inputFile = fopen("check.txt", "r");
    inputFile = fopen("test_case_count_final.txt", "r");
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

    int (*cmp)(const FileInfo *, const FileInfo *);
    if (strcmp(sortingCriteria, "Name") == 0) {
        cmp = compareByName;
    } else if (strcmp(sortingCriteria, "ID") == 0) {
        cmp = compareById;
    } else if (strcmp(sortingCriteria, "Timestamp") == 0) {
        cmp = compareByTimestamp;
    } else {
        fprintf(stderr, RED "Invalid sorting criteria\n" RESET);
        free(files);
        exit(EXIT_FAILURE);
    }
    if(numOfFiles<=__DYNAMIC_THRESHOLD__){
        #ifdef TEST
        printf(PINK "Running in test mode\n"RESET YELLOW);
        printf("Number of files: %d\n", numOfFiles);
        printf("Sorting by %s\n", sortingCriteria);
        printf("Number of threads: %d\n", NUM_THREADS_COUNT);
        FileInfo *files_copy_test = mallocWrapperForFile(numOfFiles);
        memcpy(files_copy_test, files, numOfFiles * sizeof(FileInfo));

        FileInfo *files_copy = mallocWrapperForFile(numOfFiles);
        memcpy(files_copy, files, numOfFiles * sizeof(FileInfo));
        printf(BLUE"Regular Count Sort Started\n"RESET);
        struct timespec start_time, end_time;
        int option;
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        if(strcmp(sortingCriteria,"ID")==0){
            count_sort_by_id(files_copy, numOfFiles);
            option = 1;
        }
        else if(strcmp(sortingCriteria,"Name")==0){
            count_sort_by_name(files_copy, numOfFiles);
            option = 2;
        }
        else if(strcmp(sortingCriteria,"Timestamp")==0){
            count_sort_by_timestamp(files_copy, numOfFiles);
            option = 3;
        }
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        double regular_sort_time = get_elapsed_time(start_time, end_time);
    

        // verify_sort_stability(files_copy_test, files_copy, numOfFiles,  option);
        free(files_copy);
        printf(GREEN "Regular count sort completed and sorted Successfully\n" RESET);
        #endif
        
        #ifdef TEST
        printf(BLUE"Parallel Count Sort Started\n"RESET);
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
        // verify_sort_stability(files_copy_test, files, numOfFiles,  option);
        printf(GREEN "Parallel count sort completed and sorted Successfully\n" RESET);
        printf(GREEN "Regular count sort completed in "YELLOW"%.6f seconds"GREEN" for %d elements\n" RESET, regular_sort_time, numOfFiles);
        printf(GREEN "Parallel count sort completed in "YELLOW"%.6f seconds"GREEN" for %d elements\n" RESET, parallel_sort_time, numOfFiles);
        printf(YELLOW "Speedup: %.6f\n" RESET, regular_sort_time / parallel_sort_time);
        
        if (parallel_sort_time < regular_sort_time) {
            printf(GREEN "Parallel count sort was faster by"YELLOW" %.6f seconds.\n" RESET, regular_sort_time - parallel_sort_time);
        } else {
            printf(RED "Regular count sort was faster by " YELLOW"%.6f seconds.\n" RESET, parallel_sort_time - regular_sort_time);
        }

        #endif
        // if(numOfFiles<=0){
            // printf("\nSorted files:\n");
            printf("%s\n",sortingCriteria);
            printFiles(files, numOfFiles);
        // }
        // else{
        //     printf(MAGENTA "Too many files to print\n" RESET);
        // }
        return EXIT_SUCCESS;
    }
    
#ifdef TEST
    printf(PINK "Running in test mode\n"RESET YELLOW);
    printf("Number of files: %d\n", numOfFiles);
    printf("Sorting by %s\n", sortingCriteria);
    // printf("Threshold: %d\n", THRESHOLD);
    printf(RESET);

    FileInfo *files_copy = mallocWrapperForFile(numOfFiles);
    memcpy(files_copy, files, numOfFiles * sizeof(FileInfo));
    printf(BLUE"Regular Merge Sort Started\n"RESET);
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Regular merge sort
    merge_sort(files_copy, 0, numOfFiles - 1, cmp);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double regular_sort_time = get_elapsed_time(start_time, end_time);

    if (!is_sorted(files_copy, numOfFiles, cmp)) {
        fprintf(stderr, RED "Array is not sorted\n" RESET);
        free(files);
        exit(EXIT_FAILURE);
    }
    free(files_copy);


    // Success message for regular merge sort
    printf(GREEN "Regular merge sort completed and sorted Successfully\n" RESET);
    printf(BLUE"Parallel Merge Sort Started\n"RESET);
    clock_gettime(CLOCK_MONOTONIC, &start_time);
#endif

    parallel_merge_sort(files, numOfFiles, cmp);

#ifdef TEST

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double parallel_sort_time = get_elapsed_time(start_time, end_time);

    if (!is_sorted(files, numOfFiles, cmp)) {
        fprintf(stderr, RED "Array is not sorted\n" RESET);
        free(files);
        exit(EXIT_FAILURE);
    }
    printf(GREEN "Parallel merge sort completed and sorted Successfully\n" RESET);

    // Print Performance Metrics
    printf(GREEN "Regular merge sort completed in "YELLOW"%.6f seconds"GREEN" for %d elements\n" RESET, regular_sort_time, numOfFiles);
    printf(GREEN "Parallel merge sort completed in "YELLOW"%.6f seconds"GREEN" for %d elements\n" RESET, parallel_sort_time, numOfFiles);

    if (parallel_sort_time == 0) {
        printf(YELLOW "Speedup: N/A\n" RESET);
    } else {
        printf(YELLOW "Speedup: %.6f\n" RESET, regular_sort_time / parallel_sort_time);
    }

    if (parallel_sort_time < regular_sort_time) {
        printf(GREEN "Parallel merge sort was faster by"YELLOW" %.6f seconds.\n" RESET, regular_sort_time - parallel_sort_time);
    } else {
        printf(RED "Regular merge sort was faster by " YELLOW"%.6f seconds.\n" RESET, parallel_sort_time - regular_sort_time);
    }

#endif

    // Check if number of files exceeds the threshold
    #ifdef TEST
    if (numOfFiles > 100) {
        printf(MAGENTA "Too many files to print\n" RESET);
    } else {
        printf("\nSorted files:\n");
        printf("%s\n",sortingCriteria);
        printFiles(files, numOfFiles);
    }
    #else
    // printf("\nSorted files:\n");
    printf("%s\n",sortingCriteria);
    printFiles(files, numOfFiles);
    #endif

    free(files);

    // Close the input file if it was opened
    if (inputFile) {
        fclose(inputFile);
    }

    return 0;
}
