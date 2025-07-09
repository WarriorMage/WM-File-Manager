#pragma once

#include <stdbool.h>

// To capture the terminal size before drawing.
extern int maxy;
extern int maxx;

// Call this function to initialize ncurses with the colours and settings.
void start_prog_ncurses(void);

// Prints messages in the status bar with or without a keypress wait.
int print_status(bool wait, const char *fmt, ...);

// Draw the main file explorer window after modifying the inner items.
void draw_main_ui(int col_length, const char *highlight);
