#include "common_globals.h"
#include <ncurses.h>
#include <stdarg.h>
#include <unistd.h>
#include "file_exec.h"
#include "generic_utils.h"
#include "LinkedListGeneral.h"

int maxy, maxx;

void start_prog_ncurses(void)
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, true);
    start_color();
    init_pair(10, COLOR_GREEN, COLOR_BLACK);
    init_pair(DIRECTORY, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SYMLINK, COLOR_BLUE, COLOR_BLACK);
    init_pair(FIFO, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(SOCKET, COLOR_CYAN, COLOR_BLACK);
    init_pair(CHRDEV, COLOR_RED, COLOR_BLACK);
    init_pair(BLKDEV, COLOR_RED, COLOR_WHITE);
    getmaxyx(stdscr, maxy, maxx);
    refresh();
}

int print_status(bool wait, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    attron(A_REVERSE);
    move(maxy - 1, 0);
    hline(' ', maxx);
    vw_printw(stdscr, fmt, args);
    attroff(A_REVERSE);

    va_end(args);

    if (wait)
        return (getch());
    return -1;
}

void draw_main_ui(int col_length, const char *highlight)
{
    attron(A_REVERSE);
    mvhline(0, 0, ' ', maxx);
    mvprintw(0, 0, "Current directory : %s", getcwd(cwd, sizeof(cwd)));
    attroff(A_REVERSE);

    for (int i = 0; i < col_length; i++)
    {
        char *item = (char *)getnode(contents, i + scroll_offset);
        snprintf(fullpath, sizeof(fullpath), "%s/%s", cwd, item);
        mvhline(i + 1, 0, ' ', maxx);
        mvprintw(i + 1, 0, "%4ld\t", i + 1 + scroll_offset);

        filetype x = file_type(fullpath);
        attron(COLOR_PAIR(x));
        if (item == highlight)
            attron(A_REVERSE);
        if (member(selected, (const void *)item, strcomparearr) != -1)
            attron(COLOR_PAIR(10));
        printw("%s", item);
        if (member(selected, (const void *)item, strcomparearr) != -1)
            attroff(COLOR_PAIR(10));
        if (item == highlight)
            attroff(A_REVERSE);
        attroff(COLOR_PAIR(x));
    }

    if ((long)length(contents) < maxy - 2)
    {
        for (long i = 0; i < ((maxy - 2) - (long)length(contents)); i++)
        {
            mvhline(col_length + i + 1, 0, ' ', maxx);
        }
    }
}