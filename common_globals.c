#define _GNU_SOURCE
#include <limits.h>
#include "LinkedListGeneral.h"

list *contents;
list *selected;
list *clipboard;
size_t scroll_offset = 0;

char fullpath[PATH_MAX];
char cwd[PATH_MAX];
