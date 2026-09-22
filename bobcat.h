#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <limits.h>

#define BUFFER_SIZE 8000
#define SHIFT 7

void encrypt_file(const char *filepath);
void encrypt_directory(const char *folder_path, int *file_count);

// analyse_path returns 0 if the path doesn't exist and 1 if it's a folder, 2 if it's a file
int analyse_path(const char *path);