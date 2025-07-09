#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ncurses.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>
#include "LinkedListGeneral.h"

#define ITEM_SIZE (NAME_MAX + 1)

#include "ncurses_func.h"
#include "file_exec.h"
#include "file_ops.h"
#include "generic_utils.h"
#include "common_globals.h"

bool exit_flag = false;
bool cut_signal = false;
bool rename_success = false;

int selectentry(int col_length, const char *highlight, size_t *highlighted)
{
    int inputl = wgetch(stdscr);
    attron(A_REVERSE);
    mvhline(maxy - 1, 0, ' ', maxx);
    attroff(A_REVERSE);
    void *selectitem;
    short buf_index = 0;
    char number_select_buf[12] = {0};

    if (inputl >= '0' && inputl <= '9')
    {
        number_select_buf[buf_index++] = inputl;
        number_select_buf[buf_index] = '\0';
        size_t index = atoi(number_select_buf);
        if (index == 0)
            return -1;

        (*highlighted) = ((index <= length(contents)) ? index : length(contents)) - 1;
        scroll_offset = (long)(*highlighted) > (maxy - 3) ? (*highlighted) - (maxy - 3) : 0;
    }
    else if (buf_index > 0)
    {
        buf_index = 0;
        number_select_buf[0] = '\0';
    }
    switch (inputl)
    {
    case KEY_UP:
        if ((*highlighted) > 0)
        {
            (*highlighted)--;
            if ((*highlighted < scroll_offset))
                scroll_offset--;
        }
        break;
    case KEY_DOWN:
        if ((*highlighted) < length(contents) - 1)
        {
            (*highlighted)++;
            if ((*highlighted) >= maxy - 2 + scroll_offset)
                scroll_offset++;
        }
        break;
    case 's':
        selectitem = getnode(contents, *highlighted);
        long pos = member(selected, selectitem, strcomparearr);
        if (pos == -1 && (strcmp((char *)selectitem, "..") != 0))
            insertnode(selected, -1, selectitem);
        else
            deleteNodeByIndex(selected, pos);
        break;
    case 'a':
        for (size_t i = 0; i < length(contents); i++)
        {
            selectitem = getnode(contents, (long)i);
            long pos = member(selected, selectitem, strcomparearr);
            if (pos == -1 && (strcmp((char *)selectitem, "..") != 0))
                insertnode(selected, -1, selectitem);
        }
        break;
    case '\n':
    case KEY_ENTER:
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", cwd, (char *)getnode(contents, *highlighted));
            char *target = realpath(fullpath, NULL);
            if (!target)
            {
                if (file_type(fullpath) == SYMLINK)
                    print_status(false, "Symlink invalid / unable to resolve.");
                else
                    print_status(false, "Unable to resolve target.");
                break;
            }

            switch (file_type(target))
            {
            case DIRECTORY:
                if ((check_permissions(target) & (READ | EXECUTE)) == (READ | EXECUTE))
                {
                    chdir(target);
                    (*highlighted) = 0;
                    scroll_offset = 0;
                }
                else
                    print_status(false, "Permission denied to open/browse this directory.");
                break;

            case REGFILE:
                open_regfile(target, col_length, highlight);
                break;

            case CHRDEV:
            case BLKDEV:
                print_status(false, "Character / block device files are not meant to be opened manually.");
                break;

            case FIFO:
            case SOCKET:
                print_status(false, "FIFO / Sockets cannot be opened manually.");
                break;

            default:
                print_status(false, "Unable to determine the file type.");
            }
            free(target);
        }
        break;

    case 'Q':
    case 'q':
        exit_flag = true;
        break;

    case 'x':
        cut_signal = true;
    case 'c':
        if (clipboard)
            freelist(clipboard);
        clipboard = createlist(sizeof(char) * PATH_MAX);
        copylist(&clipboard, selected);
        insertnode(clipboard, 0, getcwd(cwd, sizeof(cwd)));
        print_status(false, "Added %ld items to clipboard.", length(clipboard) - 1);
        break;

    case 'v':
        if (paste(clipboard, ".", true, (char *)getnode(clipboard, 0L)))
        {
            if (cut_signal)
            {
                delete_items(clipboard, true, (char *)getnode(clipboard, 0L));
                cut_signal = false;
            }
        }
        freelist(clipboard);
        clipboard = createlist(sizeof(char) * ITEM_SIZE);
        break;

    case 'd':
        insertnode(selected, 0, getcwd(cwd, sizeof(cwd)));
        delete_items(selected, false, cwd);
        break;

    case 'r':
        rename_success = false;
        if (!(check_permissions(getcwd(cwd, sizeof(cwd))) & WRITE))
        {
            print_status(false, "Insufficient permissions to rename files in this directory.");
            break;
        }
        rename_success = rename_item();
    }
    return inputl;
}

int main(void)
{
    start_prog_ncurses();
    signal(SIGTTOU, SIG_IGN);

    chdir(getenv("HOME"));
    DIR *curdir = opendir(".");
    struct dirent *info;

    attron(A_REVERSE);
    mvhline(maxy - 1, 0, ' ', maxx);
    attroff(A_REVERSE);

    size_t highlighted = 0;
    char *highlight = NULL;
    int input = 0;
    int col_length;

    while (!exit_flag)
    {
        if (input == KEY_ENTER || input == (int)'\n' || input == 0 || input == (int)'v' || input == (int)'d' || input == (int)'r')
        {
            if (input != (int)'v')
            {
                freelist(selected);
                selected = createlist(sizeof(char) * ITEM_SIZE);
            }
            if (input == (int)'v' || input == (int)'d' || (input == (int)'r' && rename_success) || file_type(fullpath) == DIRECTORY || file_type(fullpath) == SYMLINK)
            {
                freelist(contents);
                closedir(curdir);
                curdir = opendir(".");
            }

            if (input == 0 || input == (int)'v' || input == (int)'d' || (input == (int)'r' && rename_success) || file_type(fullpath) == DIRECTORY || file_type(fullpath) == SYMLINK)
            {
                contents = createlist(sizeof(char) * ITEM_SIZE);
                while ((info = readdir(curdir)) != NULL)
                {
                    if (info->d_name[0] != '.' || (info->d_name[0] == '.' && info->d_name[1] == '.'))
                        insertnode(contents, length(contents), (void *)info->d_name);
                }
                sortlist(contents);

                if (strcmp(getcwd(cwd, sizeof(cwd)), "/") == 0)
                {
                    long index;
                    if ((index = member(contents, (void *)"..", strcomparearr)) >= 0)
                        deleteNodeByIndex(contents, index);
                }

                // Dealt with in the case statement itself
                if (input != KEY_ENTER && input != (int)'\n')
                {
                    highlighted = 0;
                    scroll_offset = 0;
                }
                highlight = getnode(contents, highlighted);
                col_length = min(maxy - 2, (long)length(contents));
            }
        }
        refresh();
        draw_main_ui(col_length, highlight);
        input = selectentry(col_length, highlight, &highlighted);
        highlight = getnode(contents, highlighted);
    }

    endwin();
    closedir(curdir);
    freelist(selected);
    freelist(contents);
    return 0;
}
