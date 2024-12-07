#include "parallel_merge_sort.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// #define THRESHOLD 10000
int _THRESHOLD_;
int max_depth = 0;
pthread_mutex_t depth_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex to synchronize access to max_depth

void merge(FileInfo *files, int left, int mid, int right, int (*cmp)(const FileInfo *, const FileInfo *)) {
    int i = left, j = mid + 1, k = 0;
    FileInfo *result = (FileInfo *)malloc((right - left + 1) * sizeof(FileInfo));
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Merging the two halves
    while (i <= mid && j <= right) {
        if (cmp(&files[i], &files[j]) <= 0) {
            result[k++] = files[i++];
        } else {
            result[k++] = files[j++];
        }
    }
    while (i <= mid) result[k++] = files[i++];
    while (j <= right) result[k++] = files[j++];

    // Copy the result back to the original array
    for (i = left, k = 0; i <= right; i++, k++) {
        files[i] = result[k];
    }
    free(result);

}

void *recursive_sort(void *args) {
    SortArgs *sortArgs = (SortArgs *)args;
    int left = sortArgs->left;
    int right = sortArgs->right;
    int (*cmp)(const FileInfo *, const FileInfo *) = sortArgs->cmp;
    FileInfo *files = sortArgs->files;
    int depth = sortArgs->depth;
    // printf("Depth: %d, Left: %d, Right: %d\n", depth, left, right);
    // Update max_depth if this depth is greater
    pthread_mutex_lock(&depth_mutex);
    if (depth > max_depth) {
        max_depth = depth;
    }
    pthread_mutex_unlock(&depth_mutex);

    // if(right - left < THRESHOLD || right - left < _THRESHOLD_) {
    if(right - left < _THRESHOLD_) {
        // qsort(sortArgs->files + left, right - left + 1, sizeof(FileInfo), cmp);
        merge_sort(files, left, right, cmp);

        return NULL;
    }
    if(left >= right) return NULL;
    int mid = (left + (right - left) / 2);
    SortArgs leftArgs = {files, left, mid, cmp,depth+1};
    SortArgs rightArgs = {files, mid + 1, right, cmp,depth+1};
    // printf("Thread created\n");
    pthread_t leftThread;
    pthread_t rightThread;
    pthread_create(&leftThread, NULL, recursive_sort, &leftArgs);
    pthread_create(&rightThread, NULL, recursive_sort, &rightArgs);
    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

    merge(sortArgs->files, left, mid, right, cmp);
    return NULL;
}
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void parallel_merge_sort(FileInfo *files, int size, int (*cmp)(const FileInfo *, const FileInfo *)) {
    if (size <= 0) {
        fprintf(stderr, "Array size should be greater than 0\n");
        exit(EXIT_FAILURE);
    }

    // 3 --> (2,3)
    _THRESHOLD_ = 3*size/16; // 1/8 5/32
    // _THRESHOLD_ = 10000;
    #ifdef TEST
    printf(YELLOW"Threshold: %d\n"RESET, _THRESHOLD_);
    #endif
    SortArgs args = {files, 0, size - 1, cmp,0};
    recursive_sort(&args);
    #ifdef TEST
    printf("Total Depth of Recursion Tree: %d\n", max_depth);
    #endif
}
