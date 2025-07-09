#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include "LinkedListGeneral.h"

#define ITEM_SIZE (NAME_MAX + 1)

int min(int a, int b)
{
    return (a < b ? a : b);
}

int strcomparearr(const void *pa, const void *pb)
{
    return (strcasecmp((const char *)pa, (const char *)pb));
}

int strcompareptr(const void *pa, const void *pb)
{
    return (strcasecmp(*(const char **)pa, *(const char **)pb));
}

void MergeSort(void *arr, size_t n, size_t ele_size, int (*compare)(const void *, const void *))
{
    size_t mid = 0;
    void *arr2 = malloc(n * ele_size);
    char *chararr = (char *)arr;
    char *chararr2 = (char *)arr2;
    if (n > 1)
    {
        mid = (n - 1) / 2;
        MergeSort(arr, mid + 1, ele_size, compare);
        MergeSort((void *)(chararr + ((mid + 1) * ele_size)), n - mid - 1, ele_size, compare);
        size_t i = 0, j = mid + 1, k = 0;
        while (i < mid + 1 && j < n)
        {
            if (compare(chararr + (i * ele_size), chararr + (j * ele_size)) < 0)
            {
                memcpy(chararr2 + (k * ele_size), chararr + (i * ele_size), ele_size);
                i++;
            }
            else
            {
                memcpy(chararr2 + (k * ele_size), chararr + (j * ele_size), ele_size);
                j++;
            }
            k++;
        }
        while (i < mid + 1)
        {
            memcpy(chararr2 + (k * ele_size), chararr + (i * ele_size), ele_size);
            i++;
            k++;
        }
        while (j < n)
        {
            memcpy(chararr2 + (k * ele_size), chararr + (j * ele_size), ele_size);
            j++;
            k++;
        }
        memcpy(chararr, chararr2, n * ele_size);
    }
    free(arr2);
}

void sortlist(list *srclist)
{
    size_t len = length(srclist);
    char **temparr = (char **)malloc(sizeof(char *) * len);
    for (size_t i = 0; i < len; i++)
        temparr[i] = (char *)malloc(sizeof(char) * ITEM_SIZE);

    for (size_t i = 0; i < len; i++)
        strcpy(temparr[i], (char *)getnode(srclist, i));

    MergeSort(temparr, len, sizeof(char *), strcompareptr);

    for (size_t i = 0; i < len; i++)
        setnode(srclist, i, temparr[i]);
    for (size_t i = 0; i < len; i++)
        free(temparr[i]);
    free(temparr);
}