#pragma once

#include <stddef.h>
#include "LinkedListGeneral.h"

// Compares two integers and returns the minimum.
int min(int a, int b);

// Compare function for two stack arrays. (Use with MergeSort())
int strcomparearr(const void *pa, const void *pb);

// Compare function for two pointers to arrays. (Use with MergeSort())
int strcompareptr(const void *pa, const void *pb);

// Generic Merge Sort function for an array.
void MergeSort(void *arr, size_t n, size_t ele_size, int (*compare)(const void *, const void *));

// Sorts the linked list passed in its argument.
void sortlist(list *srclist);
