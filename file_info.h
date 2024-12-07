#ifndef FILE_INFO_H
#define FILE_INFO_H

typedef struct {
    char name[50];
    int fileId;
    char timestamp[20];
    int fileHash;
    // int count;
} FileInfo;

int compareByName(const FileInfo *a, const FileInfo *b);
int compareById(const FileInfo *a, const FileInfo *b);
int compareByTimestamp(const FileInfo *a, const FileInfo *b);
int comparebyNameCount(const FileInfo *a, const FileInfo *b);
void printFileInfo(const FileInfo *file);
FileInfo* mallocWrapperForFile(int numOfFiles);
void printFiles(FileInfo *files, int numOfFiles);
// void verify_sort_stability(FileInfo originalArray[], FileInfo sortedArray[], int numFiles, int sortKey);


#endif
