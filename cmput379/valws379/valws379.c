#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

void print_wws(const unsigned long *accesses, unsigned long window_size,int xnumber, FILE *infile, long int *occurance) {
    // calculate how many unique pages were added 
    unsigned long pages[window_size];   
    int used_pages = 0;

    // for each access, first out if it is from a new page
    // if so, add it to pages array, else ignore it
    for (int a = 0; a < window_size; ++a) {
        char already_in_pages = FALSE; 
        for (int p = 0; p < used_pages; ++p) {
            if (accesses[a] == pages[p]) {
                already_in_pages = TRUE;
                break;
            }
        }
                        
        // if not already in pages array, add it
        if (!already_in_pages) {
            pages[used_pages] = accesses[a];
            ++used_pages;
        }
    }

    //printf("\n");
    // debug: print the accesses
    //for (int index = 0; index < window_size; ++index) {
    //    printf("%d: %lu\n",index, accesses[index]);
    //}

    // debug: print the pages
    //for (int index = 0; index < used_pages; ++index) {
    //    printf("%d: %lu\n",index, pages[index]);
    //}

    // print working set size
    
   
    fprintf (infile, "%d  %d\n",xnumber,used_pages);
    
    occurance[used_pages]++;
    printf("%d\n",used_pages);    

    return;
}

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
    
    
    int xnumber = 1;
    //number for x axis of data in time series
    
    //char *filename;
 
    int cmd_line_error = FALSE;
    char *end;
    printf("argc:%d\n",argc);
    // Not enough arguments
    if (argc < 3) {
        cmd_line_error = TRUE;
    }
    // Too many arguments
    else if (argc >= 5) {
        cmd_line_error = TRUE;
    }
    // no -i
    else if (argc == 3) {
        // if first argument IS -i, that's an error
        if (strcmp(argv[1],"-i") == 0) {
            cmd_line_error = TRUE;
        }
        else {
            pg_size = strtoul(argv[1], &end, 10);
            window_size = strtoul(argv[2], &end, 10);
            //filename = argv[3];
        }
    }
    // -i included
    else if (argc == 4) {
        // if first argument is NOT -i, that's an error
        if (strcmp(argv[1],"-i") != 0) {
            cmd_line_error = TRUE;
        }
        else {
            ignore_instr = TRUE;
            pg_size = strtoul(argv[2], &end, 10);
            window_size = strtoul(argv[3], &end, 10);
            //filename = argv[4];
        }
    }

    // print arguments
    /*
    printf("pg_size: %lu\n",pg_size);
    printf("window_size: %lu\n",window_size); 
    printf("filename: %s\n\n",filename); 
    */

    if (pg_size < 16 || pg_size > 65536 || pg_size % 2 != 0) {
        printf("page size: %lu  \n", pg_size);
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
    /*
    FILE *infile;
    
    printf("filename: %s\n\n",filename);
    if (infile == NULL) {
        printf("Cannot open file %s\n",filename);
        exit(-1);
    } */
    /////////////////
    FILE *infile;
    infile = fopen("plotdata1.dat", "w");
    //first temp file, save the time series date
    
    //long occurance[window_size + 1];
    long *occurance = malloc((window_size + 1) * sizeof(long));
    for (int i; i <= window_size; i++){
        occurance[i] = 0;
    }
    
    int counter = 0;                     // counter of how many accesses have been done
    unsigned long accesses[window_size]; // the addresses of all the accesses this window
    

    char *status; 
    // TODO want to set this to not NULL without getting warning
    
    while (fgets(input, 255 , stdin)) {
        // get line
        
        //printf("this is input: %s",input);
        // the input is information on a memory access from Valgrind's Lackey
        // Format is access type, address, and size
        // I  040019b6,2    

        // ignore if it's the unneccesary valgrind printing
        if(input[0] != '=' && input[0] != '-' && input[0] != 'w'){
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

                // if sliding window is not full yet, just add to the end
                if (counter < window_size) {
                    accesses[counter] = page_address;
                    ++counter;
                }
                else {
                    // print first
                    print_wws(accesses, window_size,xnumber,infile,occurance);
                    xnumber++;
                    // Then, we need to add the new address
                    // shift everything over
                    for (int index = 1; index < window_size; ++index) {
                        accesses[index-1] = accesses[index];
                    }

                    // then replace with new address
                    accesses[window_size-1] = page_address;
                }

            }
        }
    }

    // print again at the end because in the loop the print comes first
    print_wws(accesses, window_size,xnumber,infile,occurance);
    fclose (infile);
    
    FILE *freq;
    freq = fopen("plotdata2.dat", "w");
    //second temp file, save frequency data
    long double mean = 0;
    long double temp;
    long double variance = 0;
    for (int k = 0;k <= window_size;k = k+1){
        fprintf(freq,"%d   %ld \n",k,occurance[k]);
        temp = 1.0 * occurance[k] / xnumber;
        mean = mean + (temp * k);
    }
    for (int k = 0;k <= window_size;k = k+1){
        temp = 1.0 * occurance[k] / xnumber;
        variance = variance + temp*(mean - k)*(mean - k);
    }
    fclose(freq);
    free(occurance);
    printf("=========== analyse finished ===========\n");
    printf("the mean is: %Lf\n", mean);
    printf("the variance is: %Lf\n", variance);

    printf("making plot.....\n\n");

    


     
    //fclose(infile);
}
