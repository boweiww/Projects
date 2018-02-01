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
#include <stdbool.h>

#define FALSE 0
#define TRUE 1

struct entry {
    char *name;
    int status;
};

void catch_stop();
void catch_user();
void catch_alarm();

void block_signals();
void restore_signals();

void print_dir();

void print_time();

struct entry *find_entry(char *name, struct entry *arr, int used_entries);

void print_changes();


