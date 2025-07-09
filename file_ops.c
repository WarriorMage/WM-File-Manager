#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <ncurses.h>
#include "ncurses_func.h"
#include "file_exec.h"
#include "common_globals.h"
#include "LinkedListGeneral.h"

#define ITEM_SIZE (NAME_MAX + 1)

typedef struct dirent dirent;

bool paste(const list *cliplist, const char *dest, bool is_top_level, const char *toplevel)
{
    char srcpath[PATH_MAX];
    char destpath[PATH_MAX];
    char buffer[BUFSIZ];
    const char *relative = NULL;

    if (!cliplist)
        return false;
    if (!(check_permissions(dest) & WRITE))
    {
        print_status(false, "You don't have privilege to paste files in current directory!");
        return false;
    }

    for (size_t i = 1; i < length(cliplist); i++)
    {
        char *item = (char *)getnode(cliplist, (long)i);
        snprintf(srcpath, sizeof(srcpath), "%s/%s", (char *)getnode(cliplist, 0L), item);
        snprintf(destpath, sizeof(destpath), "%s/%s", dest, item);
        relative = srcpath + strlen(toplevel) + 1;

        if (is_top_level)
        {
            char *resolved_dest = malloc(PATH_MAX);
            char *resolved_src = realpath(srcpath, NULL);
            realpath(dest, resolved_dest);
            strcat(resolved_dest, "/");
            strncat(resolved_dest, item, strlen(item));

            if (resolved_src && resolved_dest &&
                strncmp(resolved_dest, resolved_src, strlen(resolved_src)) == 0 &&
                resolved_dest[strlen(resolved_src)] == '/')
            {
                relative = resolved_src + strlen(toplevel) + 1;
                print_status(false, "Can't paste %s item within itself!", relative);
                free(resolved_src);
                free(resolved_dest);
                continue;
            }
            free(resolved_src);
            free(resolved_dest);
        }

        int innerperms = check_permissions(srcpath);
        struct stat permcheck;
        if (stat(srcpath, &permcheck) == -1)
        {
            print_status(true, "Failed to gather metadata for %s, can't copy/cut/paste. Press any key to skip: ");
            continue;
        }

        if (file_type(srcpath) == DIRECTORY)
        {
            if ((innerperms & (READ | EXECUTE)) != (READ | EXECUTE))
            {

                print_status(true, "You don't have permissions to copy/cut from %s directory. Press any key to skip.", relative);
                continue;
            }

            DIR *pastedir = opendir(srcpath);

            mkdir(destpath, 0755);
            list *passlist = createlist(sizeof(char) * PATH_MAX);
            insertnode(passlist, 0L, srcpath);

            dirent *info;
            while ((info = readdir(pastedir)) != NULL)
            {
                if (info->d_name[0] != '.' || (info->d_name[1] != '\0' && info->d_name[1] != '.'))
                    insertnode(passlist, -1L, (void *)info->d_name);
            }
            paste(passlist, destpath, false, toplevel);

            freelist(passlist);
            closedir(pastedir);
        }
        else
        {
            if (!(innerperms & READ))
            {
                print_status(true, "Insufficient permissions to copy/cut %s. Press any key to skip.", relative);
                continue;
            }

            FILE *srcfile = fopen(srcpath, "rb");
            if (!srcfile)
            {
                print_status(true, "Failed to copy %s from source. Press any key to proceed: ", item);
                continue;
            }
            snprintf(destpath, sizeof(destpath), "%s/%s", dest, (char *)getnode(cliplist, (long)i));

            FILE *destfile = fopen(destpath, "rb");

            if (destfile || errno != ENOENT)
            {
                int confirm = print_status(true, "File with same name %s already exists in the destination. Do you want to overwrite? (v to confirm, else cancel): ", item);
                if (destfile)
                    fclose(destfile);
                if (confirm != (int)'v')
                    continue;
            }

            destfile = fopen(destpath, "wb");
            if (!destfile)
            {
                // Relative remains same for both source and destination.
                print_status(true, "Failed to paste into %s. Press any key to skip: ", relative);
                continue;
            }

            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFSIZ, srcfile)) > 0)
                fwrite(buffer, 1, bytes_read, destfile);
            fclose(srcfile);
            fclose(destfile);
        }
        chmod(destpath, permcheck.st_mode & 0777);
    }
    return true;
}

void delete_items(list *srclist, bool automated, const char *toplevel)
{
    int dirperms = check_permissions((char *)getnode(srclist, 0L));
    const char *relative = NULL;
    char delpath[PATH_MAX];

    if (!(dirperms & WRITE))
    {
        if (!automated)
            print_status(false, "You don't have privilege to delete files in current directory!");
        else
        {
            relative = (char *)getnode(srclist, 0L) + strlen(toplevel) + 1;
            print_status(true, "You don't have permissions to delete/cut from %s directory. Press any key to skip.", (strlen(relative)) ? relative : (char *)getnode(srclist, 0L));
        }
        return;
    }

    list *deleteboard = createlist(sizeof(char) * PATH_MAX);
    copylist(&deleteboard, srclist);
    int delchoice;
    if (!automated)
    {
        delchoice = print_status(true, "Are you sure you want to delete these %ld items? (d to confirm, else cancel): ", length(deleteboard) - 1);
    }
    else
    {
        delchoice = (int)'d';
    }
    if (delchoice == (int)'d')
    {
        for (size_t i = 1; i < length(deleteboard); i++)
        {
            snprintf(delpath, sizeof(delpath), "%s/%s", (char *)getnode(deleteboard, 0L), (char *)getnode(deleteboard, (long)i));
            relative = delpath + strlen(toplevel) + 1;

            if (file_type(delpath) == DIRECTORY)
            {
                char tempbuf[PATH_MAX];
                strncpy(tempbuf, delpath, sizeof(tempbuf));
                dirent *info;

                int innerperms = check_permissions(delpath);
                if ((innerperms & (READ | EXECUTE)) != (READ | EXECUTE))
                {
                    print_status(true, "You don't have permissions to delete/cut from %s directory. Press any key to skip.", relative);
                    continue;
                }
                DIR *deldir = opendir(delpath);

                list *innerclip = createlist(sizeof(char) * PATH_MAX);
                while ((info = readdir(deldir)) != NULL)
                {
                    if (info->d_name[0] != '.' || (info->d_name[1] != '\0' && info->d_name[1] != '.'))
                        insertnode(innerclip, -1L, (void *)info->d_name);
                }
                insertnode(innerclip, 0L, delpath);

                delete_items(innerclip, true, toplevel);
                freelist(innerclip);
                closedir(deldir);
                rmdir(tempbuf);
            }
            else
                remove(delpath);
        }
    }
    print_status(false, "%ld items deleted!", length(deleteboard) - 1);
    freelist(deleteboard);
}

bool extension_change(const char *oldname, const char *newname)
{
    // Invalid case
    if (!(oldname && newname))
        return false;

    const char *extension_old = strrchr(oldname, (int)'.');
    const char *extension_new = strrchr(newname, (int)'.');
    if (!(extension_old || extension_new))
        return false;
    else if (!extension_old)
        return (newname == extension_new) ? false : true;
    else if (!extension_new)
        return (oldname == extension_old) ? false : true;
    else if ((newname == extension_new) && (oldname == extension_old))
        return false;
    else if (strcmp(extension_old, extension_new) == 0)
        return false;
    else
        return true;
}

bool toggle_hiddenness(const char *oldname, const char *newname)
{
    if (!(oldname && newname))
        return false;
    return ((oldname[0] != '.' && newname[0] == '.') || (oldname[0] == '.' && newname[0] != '.'));
}

bool rename_item(void)
{
    if (length(selected) != 1)
    {
        print_status(false, "Select exactly one item to rename!");
    }
    else
    {
        char newname[ITEM_SIZE];
        memset(newname, 0, sizeof(newname));
        print_status(false, "Enter the new name: ");
        echo();
        attron(A_REVERSE);
        getnstr(newname, sizeof(newname));
        attroff(A_REVERSE);
        noecho();

        if (newname[ITEM_SIZE - 1] != '\0')
        {
            print_status(false, "File name too long!");
            return false;
        }
        if (strlen(newname) == 0 || strcmp(newname, ".") == 0 || strcmp(newname, "..") == 0)
        {
            print_status(false, "Empty name / reserved names(. & ..) not allowed!");
            return false;
        }
        if (strchr(newname, (int)'/'))
        {
            print_status(false, "/ is not allowed in file/directory names!");
            return false;
        }

        const char *oldname = getnode(selected, 0L);
        if (extension_change(oldname, newname))
        {
            int change_ext = print_status(true, "Are you sure to change the extension? It may trouble file associations. Press r to confirm, else cancel: ");
            if (change_ext != (int)'r')
            {
                print_status(false, "Rename aborted.");
                return false;
            }
        }
        if (toggle_hiddenness(oldname, newname))
        {
            bool prev_hidden = (oldname[0] == '.');
            int toggle = print_status(true, "Are you sure you want to %s this file? r to proceed, else cancel: ", prev_hidden ? "unhide" : "hide");
            if (toggle != (int)'r')
            {
                size_t newlen = strlen(newname);
                if (prev_hidden)
                {
                    if (newlen == NAME_MAX)
                        print_status(false, "File name too long to add hiding character '.'");
                    else
                    {
                        memmove(newname + 1, newname, newlen + 1);
                        newname[0] = '.';
                    }
                }
                else
                    memmove(newname, newname + 1, newlen);
            }
        }

        if (rename(oldname, newname) != 0)
        {
            print_status(false, "Error renaming: %s", strerror(errno));
            return false;
        }
        else
        {
            print_status(false, "File renamed!");
            return true;
        }
    }
}