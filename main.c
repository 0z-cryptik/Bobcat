#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void list_directory(const char *folder_path){
    DIR *dir = opendir(folder_path);

    if (dir == NULL){
        perror("Error opening directory");
        exit(1);
    }
    
    struct dirent *entry; 

    // entry holds info about each file or folder in the directory
    while((entry = readdir(dir)) != NULL){
        // skip the "." and ".." directly links that is present by default
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
            continue;
        }
        printf("%s\n", entry->d_name);
    }
}

int main(void){
    list_directory("./test");
    return 0;
}