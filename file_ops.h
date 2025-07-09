#pragma once

#include "LinkedListGeneral.h"

/* Pastes all the items present in cliplist to dest, ensuring exceptions are handled and no item is pasted in itself. */
bool paste(const list *cliplist, const char *dest, bool is_top_level, const char *toplevel);

/* Deletes all the items in srclist, with or without a prompt depending on the value of automated. */
void delete_items(list *srclist, bool automated, const char *toplevel);

// Asks for a new name, validates it and renames the selected file.
bool rename_item(void);