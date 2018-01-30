#define false 0
#define true 1
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

char buffer[EVENT_BUF_LEN]
// holds the previous sample of the directory in memory
struct content_arr{
    struct dirent *arr;
    int    size;
    int    used;
    
};

// checks if content_arr needs to be reallocated and then double capacity if neccessary
void reallocate(struct content_arr *contents) {
    if (contents->used >= contents->size -1) {
        contents->size *= 2;
        contents->arr = realloc(contents->arr, contents->size*sizeof(struct dirent));
    }
}

int print_dir(int period, const char *path, int length) {
    // 1. print the current time
    // http://zetcode.com/articles/cdatetime/
    int i = 0;
    int time_string_len = 256;
    char time_string[time_string_len];
    
    
    time_t curr_time = time(NULL);            // get time in seconds
    struct tm *date = localtime(&curr_time);  // split time into day, month, year, etc.
    
    // format and print time
    // Sun Nov 5 13:10:50 MST 2017
    strftime(time_string, time_string_len, "%a %b %d %H:%M:%S %Z %Y", date);
    printf("%s\n",time_string);
    
    // 2. check if we have to print anything new
    // https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
    //DIR *dr = opendir(path);
    
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        if (event->len) {
            if (event->mask & IN_CREATE) {
                printf("New file %s created.\n", event->name);
            } else if (event->mask & IN_DELETE) {
                printf("File %s deleted.\n", event->name);
                
            }
        }
        i += EVENT_SIZE + event->len;
    }
    struct dirent *entry;
    
    if (dr == NULL) {
        printf("Could not open directory\n");
        return 1;
    }
    
    // for now, simply print out the contents of the directory
    // shows some additions and modifications but doesn't show removal right now
    
    return 0;
}

int main (int argc, char *argv[]) {
    // invoked as follows:
    // diffdir379 period path
    
    // Input handling
    int period = 5;
    char *path = 0;
    struct sigaction sa;
    int fd;
    int wd;
    int cmd_line_error = false;
    fd = inotify_init();
    
    /*checking for error*/
    if (fd < 0) {
        perror("inotify_init");
    }
    int length = read(fd, buffer, EVENT_BUF_LEN);

    if (argc != 3) {
        cmd_line_error = true;
    }
    else {
        //period = atoi(argv[1]);
        path = argv[2];
    }
    
    
    // If path is not a valid directory this will be handled in print_dir
    
    if (cmd_line_error) {
        printf("Usage: %s <period> <path>\n", argv[0]);
        
        printf("<period> is the reporting period in seconds\n");
        printf("<path> is the path to the directory to monitor\n");
        
        printf("\ndiffdir is a program that periodically reports the changes to the contents of a directory. It will run forever until terminated.\n");
        return -1;
    }
    wd = inotify_add_watch( fd, path, IN_MODIFY | IN_CREATE | IN_DELETE );

    // actual main part of the program
    struct content_arr contents;
    int start_arr_size = 256;
    contents.arr = malloc(start_arr_size * sizeof(struct dirent));
    contents.size = start_arr_size;
    contents.used = 0;
    
    while(true) {
        length = read(fd, buffer, EVENT_BUF_LEN);
        if (print_dir(period, path, length) != 0) {
            return 1;
        }
        sleep(period);
        
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            break;
        }
    }
    return 0;
}


