#include "bobcat.h"

int main(void){
    int files_touched = 0;
    encrypt_directory("./test", &files_touched);
    printf("\n Went through %d files\n", files_touched);
    return 0;
}