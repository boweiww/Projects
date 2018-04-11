
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    char *filename;
    int acces = 0;
    for (int k=1; k<=5;k++){
        FILE *infile;
        filename = "out.h"
        filename = filename + string(k);
        infile = fopen(filename, "r");
        char *status; 
        status = 'g'; // TODO want to set this to not NULL without getting warning
    
        while (status != NULL) {
        // get line
            status = fgets(input, 255, infile);
            if(input[0] == 's' && input[16] == 'A'){
                printf("get in\n");
                acces = 1;
            }
    }

}