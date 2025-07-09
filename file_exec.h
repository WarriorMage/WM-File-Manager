#pragma once

// Used to denote various permissions the user may have over a file.
typedef enum permission
{
    READ = 4,
    WRITE = 2,
    EXECUTE = 1
} permission;

// Used to denote the type of file being used.
typedef enum filetype
{
    REGFILE,
    DIRECTORY,
    SYMLINK,
    CHRDEV,
    BLKDEV,
    FIFO,
    SOCKET,
    ERRTYPE // Could not be identified
} filetype;

// Returns the permissions the current user over the argument as a 3-bit integer.
int check_permissions(const char *file);

// Checks what type of file the argument is. (directory, symlink, regular etc.)
filetype file_type(const char *path);

/* Once a file is determined as a regular file, use this function to open
it with exec*() and xdg-open accordingly. */
void open_regfile(const char *path, int col_length, const char *highlight);
