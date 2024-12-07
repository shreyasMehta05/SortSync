#define _XOPEN_SOURCE 700  // Enable POSIX functions like strptime
#include "file_info.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<strings.h>
#include<stdbool.h>
int compareByName(const FileInfo *a, const FileInfo *b) {
    return strcmp(a->name, b->name);
}

int compareById(const FileInfo *a, const FileInfo *b) {
    return (a->fileId - b->fileId);
}


int compareByTimestamp(const FileInfo *a, const FileInfo *b) {
    struct tm tm1 = {0}, tm2 = {0}; // Initialize to zero

    // Parse timestamps from FileInfo structures
    if (strptime(a->timestamp, "%Y-%m-%dT%H:%M:%S", &tm1) == NULL ||
        strptime(b->timestamp, "%Y-%m-%dT%H:%M:%S", &tm2) == NULL) {
        fprintf(stderr, "Error parsing timestamps.\n");
        return 0;
    }

    // Convert parsed tm structures to time_t values
    time_t time1 = mktime(&tm1);
    time_t time2 = mktime(&tm2);

    // Compare time1 and time2 and return the result
    return (time1 > time2) - (time1 < time2);
}

void printFileInfo(const FileInfo *file) {
    printf("%s %d %s\n", file->name, file->fileId, file->timestamp);
    // printf("%s %d %s %d\n", file->name, file->fileId, file->timestamp, file->fileHash);
}

FileInfo* mallocWrapperForFile(int numOfFiles) {
    FileInfo *files = (FileInfo *)malloc(numOfFiles * sizeof(FileInfo));
    if(files == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    return files;
}

void printFiles(FileInfo *files, int numOfFiles) {
    for (int i = 0; i < numOfFiles; i++) {
        printFileInfo(&files[i]);
    }
}

int comparebyNameCount(const FileInfo *a, const FileInfo *b){
    // Case-insensitive comparison of names
    int cmp = strcasecmp(a->name, b->name);
    
    return cmp;
}

