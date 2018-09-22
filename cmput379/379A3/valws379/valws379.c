#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

// convert input line to unsigned long
unsigned long get_address(const char* input){
    char *end;

    // 1. get length of address
    char *end_of_address = strstr(input, ",");
    if (end_of_address == NULL) {
        printf("error reading address\n");
        exit(-1);
    } 
    char length_of_address = end_of_address - (input+3);

    // 2. get just address string
    char address_str[length_of_address+1];             
    strncpy(address_str, input+3,length_of_address);
    address_str[length_of_address] = 0;               

    // 3. convert address string to unsigned long 
    return strtoul(address_str, &end, 16);
}

void print_usage(char *argv[]) {
    printf("\nUsage: %s [-i] <pgsize> <windowsize>\n", argv[0]);

    printf("pgsize: the assumed page size (always a power of 2 from 16 to 65,536).\n");
    printf("windowsize: the working set window in terms of memory references (a positive integer)\n");
    printf("-i: an optional parameter which instructs valws379 to ignore the memory references that are instruction fetches (type I in the output of valgrind).\n");
}

int main (int argc, char *argv[]) {
    // invoked as follows:
    // valws379 [-i] pgsize windowsize inputfilename
    
    /*
        Where pgsize is the assumed page size (always a power of 2 from 16 to 65,536),
    windowsize is the working set window in terms of memory references (a positive integer),
    and -i is an optional parameter which instructs valws379 to ignore the memory references that are instruction fetches (type I in the output of valgrind).
    */
  
    // Argument handling
    /////////////////
    // TODO do we need to actually check the values are valid and all that? the assignmen says something about input validation, so probably. In the bash script?
    unsigned long pg_size = 0;
    unsigned long window_size = 0;
    char ignore_instr = FALSE;
    char *filename;
 
    int cmd_line_error = FALSE;
    char *end;
  
    // Not enough arguments
    if (argc < 3) {
        cmd_line_error = TRUE;
    }
    // Too many arguments
    else if (argc > 5) {
        cmd_line_error = TRUE;
    }
    // no -i
    else if (argc == 4) {
        // if first argument IS -i, that's an error
        if (strcmp(argv[1],"-i") == 0) {
            cmd_line_error = TRUE;
        }
        else {
            pg_size = strtoul(argv[1], &end, 10);
            window_size = strtoul(argv[2], &end, 10);
            filename = argv[3];
        }
    }
    // -i included
    else if (argc == 5) {
        // if first argument is NOT -i, that's an error
        if (strcmp(argv[1],"-i") != 0) {
            cmd_line_error = TRUE;
        }
        else {
            ignore_instr = TRUE;
            pg_size = strtoul(argv[2], &end, 10);
            window_size = strtoul(argv[3], &end, 10);
            filename = argv[4];
        }
    }

    // print arguments
    /*
    printf("pg_size: %lu\n",pg_size);
    printf("window_size: %lu\n",window_size); 
    printf("filename: %s\n\n",filename); 
    */

    if (pg_size < 16 || pg_size > 65536 || pg_size % 2 != 0) {
        printf("pgsize must be a power of 2 from 16 to 65,536.\n");
        cmd_line_error = TRUE;
    }

    if (cmd_line_error) {
      print_usage(argv);
      return -1;
    }

    /////////////////
    
    // open file
    /////////////////
    char input[255];
    
    FILE *infile;
    infile = fopen(filename, "r");

    if (infile == NULL) {
        printf("Cannot open file %s\n",filename);
        exit(-1);
    }
    /////////////////
    
    int counter = 0;          // counter of how many references have been done this window
    unsigned long pages[window_size]; // the start addresses of all the pages accessed this window
    int used_pages = 0;               // how many pages are being used for this window

    char *status; 
    status = 'g'; // TODO want to set this to not NULL without getting warning
    
    while (status != NULL) {
        // get line
        status = fgets(input, 255, infile);

        // the input is information on a memory access from Valgrind's Lackey
        // Format is access type, address, and size
        // I  040019b6,2    

        // ignore if it's the unneccesary valgrind printing
        if(input[0] != '='){
            // ignore if i is TRUE and it is an instruction
            if (ignore_instr == FALSE || input[0] != 'I') {
                // get size of memory access
                char size = input[strlen(input)-2];

                unsigned long address = get_address(input);

                // "page_address" is the start of the page this belongs to
                // TODO is it? I'm assuming that it starts from zero and for each page (page % pg_size) = 0. I'm not actually sure this is correct?
                // TODO does size matter?
                unsigned long page_address = address - (address % pg_size);
               
                // print a bunch of stuff for debugging
                /*
                printf("input: %s",input);
                printf("size: %c\n",size);
                printf("address: %lu\n",address);
                printf("page_address: %lu\n\n",page_address);
                */

                // if this page was already accessed this window, don't do anything
                char already_in_pages = FALSE;
                for (int index = 0; index < used_pages; ++index) {
                    if (page_address == pages[index]){
                        already_in_pages = TRUE;
                        break;
                    }
                }
                
                // if this page was NOT already accessed in this window, add it to pages
                if (!already_in_pages){
                    // new page was accessed
                    pages[used_pages] = page_address;
                    ++used_pages;
                }

                // this is the "minimum implementation" version
                // when counter reaches window size, this window is done,
                // so we print out the value and reset everything
                // don't need to reset pages array because we should never access it past used_pages 
                ++counter;
                if (counter >= window_size) {
                    printf("%d\n",used_pages); // print current working set size

                    counter = 0;    // reset counter
                    used_pages = 0; // reset pages
                }
            }
        }
    }

    fclose(infile);
}
