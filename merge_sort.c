#include "merge_sort.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_info.h"

void mergeArrays(FileInfo *arr,int left,int mid,int right,int (*cmp)(const FileInfo *, const FileInfo *)){
    int left_size = mid - left + 1;
    int right_size = right - mid;
    FileInfo *left_half = (FileInfo *)malloc(left_size * sizeof(FileInfo));
    FileInfo *right_half = (FileInfo *)malloc(right_size * sizeof(FileInfo));
    if(left_half == NULL || right_half == NULL){
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < left_size; i++){
        left_half[i] = arr[left + i];
    }
    for(int i = 0; i < right_size; i++){
        right_half[i] = arr[mid + 1 + i];
    }
    int i = 0, j = 0, k = left;
    while(i < left_size && j < right_size){
        if(cmp(&left_half[i], &right_half[j]) <= 0){
            arr[k] = left_half[i];
            i++;
        }else{
            arr[k] = right_half[j];
            j++;
        }
        k++;
    }
    while(i < left_size){
        arr[k] = left_half[i];
        i++;
        k++;
    }
    while(j < right_size){
        arr[k] = right_half[j];
        j++;
        k++;
    }
    free(left_half);
    free(right_half);
    
}




void merge_sort(FileInfo *arr, int left, int right, int (*cmp)(const FileInfo *, const FileInfo *)){
    if(left >= right){
        return;
    }
    int mid = left + (right - left) / 2;
    merge_sort(arr, left, mid, cmp);
    merge_sort(arr, mid + 1, right, cmp);
    mergeArrays(arr, left, mid, right, cmp);
}