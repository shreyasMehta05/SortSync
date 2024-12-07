#ifndef PARALLEL_MERGE_SORT_H
#define PARALLEL_MERGE_SORT_H

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "file_info.h"
#include"merge_sort.h"


typedef struct {
    FileInfo *files;
    int left;
    int right;
    int (*cmp)(const FileInfo *, const FileInfo *);
    int depth;
} SortArgs;

void parallel_merge_sort(FileInfo *files, int size, int (*cmp)(const FileInfo *, const FileInfo *));
void *recursive_sort(void *args);

#endif
