#ifndef PARALLEL_COUNT_SORT_H
#define PARALLEL_COUNT_SORT_H

#include "file_info.h"

// Sorts an array of FileInfo structures by the fileId field in parallel.
// Parameters:
//   files     - Array of FileInfo structures to be sorted.
//   num_files - Number of elements in the files array.
void parallel_count_sort_by_id(FileInfo *files, int num_files);
void parallel_count_sort_by_id_sync(FileInfo *files, int num_files);
void parallel_count_sort_by_name(FileInfo *files, int num_files);
void parallel_count_sort_by_name_sync(FileInfo *files, int num_files);
void parallel_count_sort_by_timestamp(FileInfo *files, int num_files);
void parallel_count_sort_by_name(FileInfo *files, int num_files) ;
void parallel_count_sort_by_timestamp_sync(FileInfo *files, int num_files) ;
#endif // PARALLEL_COUNT_SORT_H
