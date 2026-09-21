#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

void encrypt_file(const char *filepath);
void encrypt_directory(const char *folder_path);