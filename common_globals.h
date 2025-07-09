#pragma once

#define _GNU_SOURCE
#include <limits.h>
#include "LinkedListGeneral.h"

extern list *contents;
extern list *selected;
extern list *clipboard;
extern size_t scroll_offset;

extern char fullpath[PATH_MAX];
extern char cwd[PATH_MAX];
