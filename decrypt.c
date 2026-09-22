#include "bobcat.h"

int analyse_path(const char *path){
    struct stat path_stat;

    if (lstat(path, &path_stat) != 0) {
        fprintf(stderr, "This path doesn't exist or couldn't be accessed: %s\n", path);
        return 0;
    }

    if (S_ISDIR(path_stat.st_mode)) {
        return 1;
    } else if (S_ISREG(path_stat.st_mode)) {
        return 2;
    } else {
        return 0; 
    }
}

void decrypt_file(const char *filepath){
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
            fprintf(stderr, "ftell failed for file %s\n", filepath);
            break;
        }

        // read chunk into buffer / overwrites from the last iteration
        bytes_read = fread(buffer, 1, sizeof(buffer), current_file);

        if(bytes_read == 0){
            printf("EOF for file %s\n", filepath);
            break;
        }

        for(size_t i = 0; i < bytes_read; i++){
            buffer[i] = (unsigned char)(buffer[i] - SHIFT);
        }

        // shift position pointer back
        if(fseek(current_file, position, SEEK_SET) != 0){
            fprintf(stderr, "fseek backward failed for file %s\n", filepath);
            break;
        }

        // overwrite the original bytes with the encrypted bytes
        size_t bytes_written = fwrite(buffer, 1, bytes_read, current_file);

        if(bytes_written != bytes_read){ 
            fprintf(stderr, "fwrite error for file: %s\n", filepath);
            break;
        }

        // flush write buffer 
        fflush(current_file);

        // advance position pointer
        if(fseek(current_file, (position + (long) bytes_read), SEEK_SET) != 0){
            fprintf(stderr, "fseek forward failed for file: %s\n", filepath);
            break;
        }
    }

    fclose(current_file);
}

void decrypt_directory(const char *folder_path, int *files_count){
    DIR *dir = opendir(folder_path);

    if (dir == NULL){
        fprintf(stderr, "Error opening directory: %s\n", folder_path);
        return;
    }

    struct dirent *entry; 

    // entry holds info about each file or folder in the directory
    while((entry = readdir(dir)) != NULL){
        // skip the "." and ".." directly links that is present by default
        if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0)){
            continue;
        }

        char filepath[PATH_MAX];
        snprintf(filepath, sizeof(filepath), "%s/%s", folder_path, entry->d_name);
        
        // analyse_path returns 0 if the path doesn't exist or it's a special file, 1 if it's a folder, 2 if it's a file
        int path_type = analyse_path(filepath);

        if(path_type == 1){
            decrypt_directory(filepath, files_count);
        } else if(path_type == 2){
            decrypt_file(filepath);
            (*files_count)++;
        } else{
            continue;
        }
    }

    closedir(dir);
}

int main(void){
    int files_touched = 0;
    decrypt_directory("./test", &files_touched);
    printf("\n Went through %d files\n", files_touched);
    return 0;
}