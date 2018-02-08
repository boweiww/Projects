#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16) )
#define FILEPATH_SIZE 1024
#define ENTRY_ARR_LEN (1024 * sizeof (struct entry))

#define FALSE 0
#define TRUE 1

#define CREATED 1
#define MODIFIED 2
#define DELETED 3
#define CREATED_THEN_DELETED 4
#define DELETED_THEN_CREATED 5

// a representation of an entry in the directory
struct entry {
    char *name;  // name of the entry ex. "diffdir379.h"
    int status;  // status of the entry ex. CREATED
};

// Keeps an inotify watch_descriptor with the corresponding directory name
struct watch {
    int wd;
    char *name;
};

// a array of watch objects. Contains all of the subdirectories that are being watched
struct sub_watches {
    struct watch wds[1024 * sizeof(struct watch)];
    int num;
};

/* SIGNAL FUNCTIONS */
// Tell program to exit next time it reaches end of printing
void catch_stop();
// When user signal recieved, print changes.
void catch_user();
// When alarm signal recieved, print changes and set another alarm.
void catch_alarm();

// Postpone signal handling
void block_signals();
// Resume signal handling and resolve blocked signals
void restore_signals();
/*                  */

/* PRINTING FUNCTIONS */
// print the current time
void print_time();

// Called once at the beginning of the program
// prints out the contents of the directory
void print_initial_state();

/* Print changes and its functions */
// Finds if an entry already exists in arr and returns it
struct entry *find_entry(char *name, struct entry *arr, int used_entries);
// read inotify events
// used in print_subdirectories and print_changes
int read_inotify(int fd, char *buffer);
// Check if there have been any changes in the subdirectories
// If so, only print that the subdirectory has been modified
void print_subdirectories();
// finds changes using inotify and prints out the relevant ones
void print_changes();
/*                    */               


