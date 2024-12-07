#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include "file_info.h"
void merge_sort(FileInfo *arr, int left, int right, int (*cmp)(const FileInfo *, const FileInfo *));

#endif
