#include "diffdir379.h"

// initialize global variables
static int period = 10;
static char *path = 0;
static int main_fd, main_wd= -1;
static int sub_fd = -1;
static struct sub_watches sub_wd;

static struct sigaction user_action, interrupt_action, alarm_action;
static int exit_signal = FALSE; // This indicates if the program should end next time possible

// checks if array needs to be reallocated and then doubles capacity if needed
void reallocate() {
    if(sub_wd.num >= sub_wd.size -1) {
        sub_wd.size *= 2;
        sub_wd.wds = realloc(sub_wd.wds, sub_wd.size*sizeof(struct watch));
    }
}

// Tell program to print then exit
void catch_stop_waiting() {
    exit_signal = TRUE;
    print_changes();
}

// Tell program to exit next time it reaches end of printing
void catch_stop_printing() {
    exit_signal = TRUE;
}

// When user signal recieved, print changes.
void catch_user() {
    print_changes();
}

// When alarm signal recieved, print changes and set another alarm.
void catch_alarm() {
    print_changes();

    alarm(period);
}

// Postpone signal handling
void block_signals() {
    // postpone interrupt signals while printing
    interrupt_action.sa_handler = catch_stop_printing;
    sigaction(SIGINT, &interrupt_action, NULL);

    // ignore user/alarm signals while printing
    user_action.sa_handler = SIG_IGN;
    alarm_action.sa_handler = SIG_IGN;     
    sigaction(SIGUSR1, &user_action, NULL);
    sigaction(SIGALRM, &alarm_action, NULL);
}

// Resume signal handling and resolve blocked signals
void restore_signals() {
    // if recieved an interrupt signal that was previously ignored,
    // exit program now.
    if(exit_signal) {
        exit(0);
    }

    // set signal actions back to normal
    interrupt_action.sa_handler = catch_stop_waiting;
    sigaction(SIGINT, &interrupt_action, NULL);

    // restore user/alarm signal actions
    user_action.sa_handler = catch_user;  
    alarm_action.sa_handler = catch_alarm;
    sigaction(SIGUSR1, &user_action, NULL); 
    sigaction(SIGALRM, &alarm_action, NULL);
}

// print the current time
void print_time() {
    int time_string_len = 256;
    char time_string[time_string_len];

    time_t curr_time = time(NULL);            // get time in seconds
    struct tm *date = localtime(&curr_time);  // split time into day, month, year, etc.
    
    // format and print time
    // Sun Nov 5 13:10:50 MST 2017
    strftime(time_string, time_string_len, "%a %b %d %H:%M:%S %Z %Y", date);
    printf("%s\n",time_string);
}

void add_sub_watch(char *name) {
    struct watch new_dir;
    new_dir.name = name;
 
    char filepath[1024];
    strcpy(filepath, path);
    strcat(filepath, "/");
    strcat(filepath, name); 
            
    new_dir.wd = inotify_add_watch(sub_fd, filepath, IN_CREATE | IN_MODIFY 
                                 | IN_ATTRIB | IN_DELETE
                                 | IN_DELETE_SELF | IN_MOVE_SELF
                                 | IN_MOVED_FROM | IN_MOVED_TO);
            
    reallocate();
    sub_wd.wds[sub_wd.num] = new_dir;
    ++sub_wd.num;
}

// Called once at the beginning of the program
// prints out the contents of the directory
void print_initial_state() {
    block_signals();

    print_time();
    
    DIR *dr = opendir(path);
    struct dirent *entry;
     
    if (dr == NULL) {
        printf("Could not open directory\n");
        exit(-1);
    }

    // shows some additions and modifications but doesn't show deletion right now
    while ((entry = readdir(dr)) != NULL) {
        // We are supposed to ignore "." and ".."
        // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=928062
        if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name,"..") == 0) {
            continue;
        }

        if(entry->d_type == DT_REG) {
            printf("+ %s\n", entry->d_name);
        } else
        if(entry->d_type == DT_DIR) {
            printf("d %s\n", entry->d_name);
            add_sub_watch(entry->d_name);
        }
        else {
           printf("o %s\n", entry->d_name);
        }

    }

    closedir(dr);

    restore_signals();
}

// Finds if an entry already exists in arr and returns it
struct entry *find_entry(char *name, struct entry *arr, int used_entries) {
    for(int i = 0; i < used_entries; ++i) {
        if (strcmp(name, arr[i].name) == 0) {
            return &arr[i];
        }
    }
    return NULL;
}

// read inotify events
// used in print_subdirectories and print_changes
int read_inotify(int fd, char *buffer, unsigned int avail) {
    int length =0;
    int flags, nfds = 0;
    fd_set read_fdst;
    struct timeval tv;

    //set fd to check read
    FD_ZERO(&read_fdst);
    FD_SET(fd,&read_fdst);
    //set tv for maximun read time
    tv.tv_sec = 0;
    tv.tv_usec = 100;
    //set select
    nfds = select (fd+1,&read_fdst,NULL,NULL,&tv);
    //select cases
    switch(nfds){
    case -1:
        printf("Something wrong happened");
        exit(-1);
    case 0:
        return -1;
    default:
        length = read(fd, buffer, avail);
    }

    return length;
}

// Check if there have been any changes in the subdirectories
// If so, only print that the subdirectory has been modified
void print_subdirectories() {

    unsigned int avail = 0;
    ioctl(sub_fd, FIONREAD, &avail);
    char buffer[avail];
    int length = read_inotify(sub_fd, buffer, avail);

    if (length == -1) {
        return;
    }

    /*
     The problem is we are not supposed to print redundant information. 
     For example, if a subdirectory is modified many times

     arr is an array of all the subdirectories in the directory.
     arr is reset every time print_subdirectories() is called.
     the status of the entries are kept when reading through the 
     inotify events for this report to avoid printing redundant information.
     The number of inotify events is >= the number of directory entries.
    */
    struct entry arr[avail];
    int used_entries = 0;

    int i = 0; 
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];

        // find which subdirectory it is from and print that it is modified
        for (int j = 0; j < sub_wd.num; ++j) {
            if(event->wd == sub_wd.wds[j].wd) {
                struct entry *curr = find_entry(sub_wd.wds[j].name, arr, used_entries);

                /* DIRECTORY REMOVED */
                // if deleted, ignore messages. print_changes will handle it.
                if ((event->mask & IN_DELETE_SELF) | (event->mask & IN_MOVE_SELF)) {
                    if(curr == NULL) {
                        // if entry doesn't already exist in arr, create it
                        struct entry new;
                        new.name = sub_wd.wds[j].name; 
                        new.status = DELETED;
                        arr[used_entries] = new;
                        ++used_entries;   
                        break;
                    } else {
                        curr->status = DELETED;
                    }
                } 
                /* DIRECTORY MODIFIED */
                else {
                    if(curr == NULL) {
                        // if entry doesn't already exist in arr, create it
                        struct entry new;
                        new.name = sub_wd.wds[j].name; 
                        new.status = MODIFIED;
                        arr[used_entries] = new;
                        ++used_entries;   
                        break;
                    } 
                }
                
            }
        }

        i += EVENT_SIZE + event->len;
    }

    // print which directories have been m
    for (int i = 0; i < used_entries; ++i) {
        if(arr[i].status == MODIFIED) { 
            printf("* %s\n", arr[i].name); 
        }
   }
}

// finds changes using inotify and prints out the relevant ones
void print_changes() {
    block_signals();

    print_time();

    print_subdirectories();
    
    unsigned int avail = 0;
    ioctl(main_fd, FIONREAD, &avail);
    char buffer[avail];
    int length = read_inotify(main_fd, buffer, avail);
 
    /*
     The problem is we are not supposed to print redundant information. 
     For example, if a file is created then modified, it should simply print created.
     Or, if a file is modified many times, it should just print modified.

     arr is an array of all the entries in the directory.
     arr is reset every time print_changes() is called.
     the status of the entries are kept when reading through the 
     inotify events for this report to avoid printing redundant information.
     The number of inotify events is >= the number of directory entries.
    */
    struct entry arr[avail];
    int used_entries = 0;

    int i=0;
    //inotify to check modification of file
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        // printf("%s\n",event->name); // line for testing
        int exists = FALSE;        

        /* DIRECTORY REMOVED */
        if ((event->mask & IN_DELETE_SELF) | (event->mask & IN_MOVE_SELF)) {
            printf("Directory no longer exists at that location\n");
            exit(-1);
        } 

        if (event->len) {
            /* CREATING */
            if ((event->mask & IN_CREATE) | (event->mask & IN_MOVED_TO)) {
                int location = -1;
                struct entry *curr = find_entry(event->name, arr, used_entries);

                if(curr == NULL) {
                    // if entry doesn't already exist in arr, create it
                    struct entry new;
                    new.name = event->name; new.status = CREATED;
                    arr[used_entries] = new;
                    ++used_entries;    
                } else 
                if (curr->status == DELETED) {
                    // if entry was already deleted, special case.
                    // We must report both deletion and re-creation
                    curr->status = DELETED_THEN_CREATED;
                } else
                if (curr->status == CREATED_THEN_DELETED) {
                    // if entry was created, deleted, then created again,
                    // treat as if it was just created
                    curr->status = CREATED;
                }
            } else
            /* MODIFYING */
            if ((event->mask & IN_MODIFY) | (event->mask & IN_ATTRIB)) {
                struct entry *curr = find_entry(event->name, arr, used_entries);

                // if entry doesn't already exist in arr, create it
                // otherwise it doesn't matter
                if(curr == NULL) {
                    struct entry new;
                    new.name = event->name; new.status = MODIFIED;
                    arr[used_entries] = new;
                    ++used_entries;
                }
            } else
            /* DELETING */
            if ((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)) {
                struct entry *curr = find_entry(event->name, arr, used_entries);

                if(curr == NULL) {
                    // if entry doesn't already exist in arr, create it
                    struct entry new;
                    new.name = event->name; new.status = DELETED;
                    arr[used_entries] = new;
                    ++used_entries;
                } else
                if(curr->status == MODIFIED) {
                    // if entry was already modified, just report deleted
                    curr->status = DELETED;
                } else
                if(curr->status == CREATED) {
                    // if entry was already created, special case.
                    // We should not report anything as this happened between reports.
                    curr->status = CREATED_THEN_DELETED;
                } else
                if(curr->status == DELETED_THEN_CREATED) {
                    // if entry was deleted, created, then deleted again,
                    // treat as if it was just deleted
                    curr->status = DELETED;
                }
            }

       }

       i += EVENT_SIZE + event->len;
    }

    // Go through arr and print out the report
    struct stat file_info;
    for (int i = 0; i < used_entries; ++i) {
        if (arr[i].status == CREATED) {   
            char filepath[strlen(path)+strlen(arr[i].name)+1];
            
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, arr[i].name);
            stat(filepath, &file_info); 

            char *dirpath;
            dirpath = (char *) malloc(strlen(arr[i].name)*strlen(arr[i].name));
            strcpy(dirpath, arr[i].name);
                                     
            // print differently depending on type
            switch(file_info.st_mode & S_IFMT) {  
            case S_IFREG: printf("+ %s\n",arr[i].name); break;
            case S_IFDIR: printf("d %s\n",arr[i].name);
                          // A new directory, so we need to add a watch to it
                          add_sub_watch(dirpath);
                          break;
            default: printf("o %s\n",arr[i].name);      break;
            }  
        } else
        if (arr[i].status == MODIFIED) {
            printf("* %s\n", arr[i].name); 
        } else
        if (arr[i].status == DELETED) {
            printf("- %s\n", arr[i].name);
        } else
        if (arr[i].status == DELETED_THEN_CREATED) {

            // Must print deletion and creation consecutively
            // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=935452
            printf("- %s\n", arr[i].name);
            printf("+ %s\n", arr[i].name);
        }
       
   }

    restore_signals();
}

int main (int argc, char *argv[]) {
  // invoked as follows:
  // diffdir379 period path
  
  // Input handling
  /////////////////
 
  int cmd_line_error = FALSE;
  
  if (argc != 3) {
    cmd_line_error = TRUE;
  }
  else {
    period = atoi(argv[1]);
    path = argv[2];
  }

  // If path is not a valid directory this will be handled in print_initial_state
  
  if (cmd_line_error) {
    printf("Usage: %s <period> <path>\n", argv[0]);
    
    printf("<period> is the reporting period in seconds\n");
    printf("<path> is the path to the directory to monitor\n");

    printf("\ndiffdir is a program that periodically reports the changes to the contents of a directory. It will run forever until terminated.\n");
    return -1;
  }
  /////////////////
 
  // actual main part of the program

  user_action.sa_flags = 0;
  alarm_action.sa_flags = 0;
  // this sets the action to call the appropriate function
  user_action.sa_handler = catch_user;    
  alarm_action.sa_handler = catch_alarm; 
  // this sets so when the signal SIGUSR1 is sent, user's action is called (catch_user function)
  sigaction(SIGUSR1, &user_action, NULL); 
  sigaction(SIGALRM, &alarm_action, NULL);

  interrupt_action.sa_handler = catch_stop_waiting;
  interrupt_action.sa_flags = 0;
  sigaction(SIGINT, &interrupt_action, NULL);

  main_fd = inotify_init();
  sub_fd = inotify_init();
  if ((main_fd < 0) | (sub_fd < 0)) {
      printf("error initializing inotify\n");
      return -1;
  }
  main_wd = inotify_add_watch(main_fd, path, IN_CREATE | IN_MODIFY | IN_ATTRIB | IN_DELETE 
                                 | IN_DELETE_SELF | IN_MOVE_SELF
                                 | IN_MOVED_FROM | IN_MOVED_TO);

  sub_wd.wds = malloc(START_SUB_SIZE * sizeof(struct watch));  
  sub_wd.size = START_SUB_SIZE;
  sub_wd.num = 0;

  // print out the contents of the directory
  print_initial_state();      

  alarm(period);

  while (TRUE) {
      pause();
  }
}


