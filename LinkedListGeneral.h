#pragma once

#include <stddef.h>

// To indicate success or error from a function
typedef enum retcode
{
    SUCCESS,
    INVIND,  // Invalid index entered (like negative)
    DELOBD,  // Deletion requested out of bounds
    NOALLOC, // Memory allocation failed
    INVINP   // Invalid input - like NULL pointer for data
} retcode;

// Contains the definitions for the linked list
typedef struct list list;

// Creates an linked list with each element of specified element_size
list *createlist(long element_size);

// Returns length of a linked list given in parameter
size_t length(const list *sizelist);

/*Frees all the memory allocated to a linked list. Must always be coupled
with createlist() */
void freelist(list *dellist);

/* Returns the element at pos position of the given list. Returns NULL for all
exceptions. Typecast and use the void* as required. */
void *getnode(const list *travlist, long pos);

// Modifies the value of an existing node with data
retcode setnode(list *inslist, long pos, const void *data);

/* Insert an element at the specified position or returns error as retcode.
Inserts at end if -1 is specified as pos. */
retcode insertnode(list *inslist, long pos, const void *data);

// Delete the element at the specified index or returns error as retcode.
retcode deleteNodeByIndex(list *dellist, long pos);

// Creates a deepcopy of src at *dest
void copylist(list **dest, const list *src);

/* Checks if a given object exists in a linked list. Returns index if yes,
-1 otherwise. */
long member(const list *looplist, const void *check, int (*compare)(const void *, const void *));
