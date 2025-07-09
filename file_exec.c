#define _GNU_SOURCE

#include "file_exec.h"
#include "ncurses_func.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ncurses.h>
#include <string.h>

int check_permissions(const char *file)
{
    uid_t uid = geteuid();
    gid_t gid = getegid();

    struct stat info;

    if (stat(file, &info) != 0)
        return -1;

    if (uid == info.st_uid)
        return ((info.st_mode & S_IRWXU) >> 6);
    else if (gid == info.st_gid)
        return ((info.st_mode & S_IRWXG) >> 3);
    else
        return (info.st_mode & S_IRWXO);
}

filetype file_type(const char *path)
{
    struct stat path_stat;
    if (lstat(path, &path_stat) != 0)
        return ERRTYPE;
    if (S_ISDIR(path_stat.st_mode))
        return DIRECTORY;
    else if (S_ISREG(path_stat.st_mode))
        return REGFILE;
    else if (S_ISLNK(path_stat.st_mode))
        return SYMLINK;
    else if (S_ISCHR(path_stat.st_mode))
        return CHRDEV;
    else if (S_ISBLK(path_stat.st_mode))
        return BLKDEV;
    else if (S_ISFIFO(path_stat.st_mode))
        return FIFO;
    else if (S_ISSOCK(path_stat.st_mode))
        return SOCKET;
    return ERRTYPE;
}

void open_regfile(const char *path, int col_length, const char *highlight)
{
    int status_code;
    FILE *magicread = fopen(path, "rb");
    unsigned char magicbytes[4];
    bool should_exec = false;
    bool exec_allowed = (access(path, X_OK) == 0);

    if (!exec_allowed && !magicread)
    {
        print_status(false, "Insufficient permissions to open / execute this file.");
        return;
    }
    if (exec_allowed && magicread)
    {
        fread(magicbytes, 1, 4, magicread);
        fclose(magicread);
        if ((magicbytes[0] == 0x7f && magicbytes[1] == 'E' && magicbytes[2] == 'L' && magicbytes[3] == 'F') || (magicbytes[0] == '#' && magicbytes[1] == '!'))
            should_exec = true;
    }

    int runchoice;
    if (exec_allowed && !magicread)
    {
        runchoice = print_status(true, "Unable to verify integrity of the executable. Still run? (Enter to continue, else cancel): ");
        if (runchoice == (int)'\n' || runchoice == (int)KEY_ENTER)
            should_exec = true;
        else
            return;
    }

    int new_p = fork();
    if (new_p == -1)
        print_status(false, "Failed to open the file. Try again later.");

    else if (new_p == 0)
    {
        if (should_exec)
        {
            setpgid(0, 0);
            endwin();
            execl(path, path, NULL);
        }
        else
        {
            int devnull = open("/dev/null", O_WRONLY);
            if (devnull != -1)
            {
                dup2(devnull, STDOUT_FILENO);
                dup2(devnull, STDERR_FILENO);
                close(devnull);
            }
            execlp("xdg-open", "xdg-open", path, NULL);
        }

        print_status(false, "Failed to %s the file. Try again later.", should_exec ? "execute" : "open (xdg-open required)");
        _exit(127);
    }
    else
    {
        if (should_exec)
        {
            setpgid(new_p, new_p);
            tcsetpgrp(STDIN_FILENO, new_p);
            endwin();
        }
        wait(&status_code);

        if (should_exec)
            tcsetpgrp(STDIN_FILENO, getpgrp());

        start_prog_ncurses();
        draw_main_ui(col_length, highlight);
        refresh();

        if (WIFEXITED(status_code) && (WEXITSTATUS(status_code) != 0))
            print_status(false, "Execution failed with status: %d", WEXITSTATUS(status_code));
        else if (WIFSIGNALED(status_code))
            print_status(false, "Program terminated by signal: %d (%s)", WTERMSIG(status_code), strsignal(WTERMSIG(status_code)));
    }
}