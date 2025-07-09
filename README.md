# WM File Manager

WM File Manager is an interactive, easy to use file manager designed for Unix-like / Linux operating systems. Written in C using libc, glibc and ncurses. Supports basic operations like navigation, rename, cut, copy, paste, delete, and execution of files or scripts.

## Features

- Navigate directories with arrow keys or number input
- Select, copy, cut, and paste files
- Rename or delete items
- Open or execute regular files (ELF, script, or GUI)
- Handle symlinks, FIFOs, and sockets
- Colored interface using `ncurses`
- Minimal dependencies

## Build Instructions

You can build the application yourself with the instructions detailed below or rather just download the executable in the releases section. But note that, the release version may not be always up to date. Plus building manually gives fun.

**Procedure to build:**

1. Make sure you have Git installed, if not install it using the appropriate package manager for your distribution. (apt for Debian / Ubuntu, pacman for Arch etc). You also need to have a C compiler and ncurses installed and setup. If you don't, follow the appropriate tutorials.

2. Once you have git installed, use the terminal to navigate to the directory where you would like to build the software. Use:
cd <intended relative / absolute path>

3. Clone this git repository to your machine, using the following command. A directory will be created with a copy of the repository.
git clone https://github.com/WarriorMage/WM-File-Manager.git

4. Navigate into the directory and build the program using the given Makefile. Run this command:
make
If you don't use gcc you will have to change the compiler listed in makefile (CC).

5. (Optional) To install the software systemwide to use from any directory, run the following command. (You will need superuser access)
sudo make install

## How to use

- The program launches in default with the home directory of the logged in user.
- Highlight the required file by using up and down arrow keys and press ENTER to navigate into that directory.
- Files can be highlighted also by pressing their index number in the keyboard (ex. pressing '3' and '2' will highlight file number 32)
- Files are sorted in alphabetical ascending order.
- Use 's' to select a file / bunch of files.
- Pressing 'a' will select all entries in that directory.
- Use 'c' to copy files, 'x' to cut after selecting.
- Pressing 'v' will paste the files in the current directory.
- Use 'r' to rename a file
- Select the files and press 'd' to delete them.
- Press 'q' to quit the program.

## Future plans

To implement a search functionality
To give option to user to sort on the basis of their interest
Display more metadata about the files

## Disclaimer

This is an experimental software, use it in protected environment. The author will NOT be responsible for any loss of data due to any careless handling by the user.

## Author

Warrior Mage
Feedback is welcome. Made with lots of joys and debugging! :)
