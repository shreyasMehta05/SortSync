#ifndef COUNT_SORT_H
#define COUNT_SORT_H

#include "file_info.h"

// Function to perform counting sort on FileInfo array by fileId
void count_sort_by_id(FileInfo *files, int num_files);
void count_sort_by_name(FileInfo *files, int num_files);
void count_sort_by_timestamp(FileInfo *files, int num_files);

#endif // COUNT_SORT_H
