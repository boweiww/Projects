#include "diffdir379.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16) )
#define FILEPATH_SIZE 1024

#define CREATED 1
#define MODIFIED 2
#define DELETED 3
#define CREATED_THEN_DELETED 4
#define DELETED_THEN_CREATED 5

#define ENTRY_ARR_LEN (1024 * sizeof (struct entry))

// initialize global variables
static int period = 10;
static char *path = 0;
static int wd,fd = -1;
int wdlist[1000];
char directoryname[1000][1000];
static int directorynum = 0;
static struct sigaction user_action, interrupt_action, alarm_action;
static int exit_signal = FALSE; // This indicates if the program should end next time possible
// Tell program to exit next time it reaches end of printing
void catch_stop() {
    exit_signal = TRUE;
}

void catch_user() {
    print_dir();
}

void catch_alarm() {
    print_dir();

    alarm(period);
}

void block_signals() {
    // postpone interrupt signals while printing
    sigaction(SIGINT, &interrupt_action, NULL);
    sigaction(SIGQUIT, &interrupt_action, NULL);
    sigaction(SIGTSTP, &interrupt_action, NULL);

    // ignore user/alarm signals while printing
    user_action.sa_handler = SIG_IGN;
    alarm_action.sa_handler = SIG_IGN;
    sigaction(SIGUSR1, &user_action, NULL);
    sigaction(SIGALRM, &alarm_action, NULL);
}

void restore_signals() {
    // if recieved an interrupt signal that was previously ignored,
    // exit program now.
    if(exit_signal) {
        exit(0);
    }

    // set signal actions back to normal
    interrupt_action.sa_handler = SIG_DFL;
    sigaction(SIGINT, &interrupt_action, NULL);
    sigaction(SIGQUIT, &interrupt_action, NULL);
    sigaction(SIGTSTP, &interrupt_action, NULL);

    // restore user/alarm signal actions
    user_action.sa_handler = catch_user;
    alarm_action.sa_handler = catch_alarm;
    sigaction(SIGUSR1, &user_action, NULL);
    sigaction(SIGALRM, &alarm_action, NULL);
}

void print_dir() {
    block_signals();

    /* Do printing */
    print_time();
    print_changes();

    restore_signals();
}

void print_time() {
    // print the current time
    // http://zetcode.com/articles/cdatetime/
    int time_string_len = 256;
    char time_string[time_string_len];


    time_t curr_time = time(NULL);            // get time in seconds
    struct tm *date = localtime(&curr_time);  // split time into day, month, year, etc.

    // format and print time
    // Sun Nov 5 13:10:50 MST 2017
    strftime(time_string, time_string_len, "%a %b %d %H:%M:%S %Z %Y", date);
    printf("%s\n",time_string);
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

// Called once at the beginning of the program
// prints out the contents of the directory
void print_initial_state() {
    block_signals();

    print_time();

    // https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
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

            char filepath[FILEPATH_SIZE];
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, entry->d_name);
            wdlist[directorynum] = inotify_add_watch(fd, filepath, IN_CREATE | IN_MODIFY | IN_DELETE);
            strcpy(directoryname[directorynum],entry->d_name);
            directorynum  = directorynum +1;
        }
        else {
           printf("o %s\n", entry->d_name);
        }

    }

    closedir(dr);

    restore_signals();
}

// finds changes using inotify
// and prints out the relevant ones
void print_changes() {

    printf ("%d",wd);
    // first check if directory still exists
    DIR *dir = opendir(path);
    if (!dir) {
        printf("Directory no longer exists\n", path);
        exit(0);
    }
    closedir(dir);
    //new file is used to remember new created file to not show modify in the same process
    char newfile[20][100];
    int length =0;
    int flags, nfds = 0;
    fd_set read_fdst;
    struct timeval tv;
    char buffer[EVENT_BUF_LEN];

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
        return;
    default:
        length = read(fd, buffer, EVENT_BUF_LEN);
    }


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
    struct entry arr[ENTRY_ARR_LEN];
    int used_entries = 0;

    int f,j,i=0;
    bool creat_mod = false;
    //inotify to check modification of file
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        // printf("%s\n",event->name); // line for testing
        int exists = FALSE;

        if (event->len) {
            /* CREATING */
            if (event->mask & IN_CREATE) {
                if (event -> wd != wd){
                int m = 0;
                char name[1000];
                for (m; m < directorynum; m += 1){
                    if (wdlist[m] == event -> wd){
                        strcpy(name,directoryname[m]);
                        directoryname[m][0] = '\0';
                        wdlist[m] = 0;
                    }
                }
                creat_mod = false;
                for( j = 0; j < f; j = j + 1 ){
                    if (strcmp(newfile[j], name) != 0){
                        creat_mod = true;
                    }
                }
                if(creat_mod) {
                    struct entry new;
                    new.name = name; new.status = MODIFIED;
                    arr[used_entries] = new;
                    ++used_entries;
                }
                }
                int location = -1;
                struct entry *curr = find_entry(event->name, arr, used_entries);

                strcpy(newfile[f], event->name);

                f += 1;
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
            }
            /* MODIFYING */
            if (event->mask & IN_MODIFY) {
                struct entry *curr = find_entry(event->name, arr, used_entries);
                if (event -> wd != wd){
                int m = 0;
                char name[1000];
                for (m; m < directorynum; m += 1){
                    if (wdlist[m] == event -> wd){
                        strcpy(name,directoryname[m]);
                        directoryname[m][0] = '\0';
                        wdlist[m] = 0;
                    }
                }
                creat_mod = false;
                for( j = 0; j < f; j = j + 1 ){
                    if (strcmp(newfile[j], name) != 0){
                        creat_mod = true;
                    }
                }
                if( creat_mod) {
                    struct entry new;
                    new.name = name; new.status = MODIFIED;
                    arr[used_entries] = new;
                    ++used_entries;
                }
                }
                // if entry doesn't already exist in arr, create it
                // otherwise it doesn't matter
                creat_mod = false;
                for( j = 0; j < f; j = j + 1 ){
                    if (strcmp(newfile[j], event->name) != 0){
                        creat_mod = true;
                    }
                }
                if(curr == NULL && creat_mod) {
                    struct entry new;
                    new.name = event->name; new.status = MODIFIED;
                    arr[used_entries] = new;
                    ++used_entries;
                }
            }
            /* DELETING */
            if (event->mask & IN_DELETE) {
                struct entry *curr = find_entry(event->name, arr, used_entries);
                if (event -> wd != wd){
                int m = 0;
                char name[1000];
                for (m; m < directorynum; m += 1){
                    if (wdlist[m] == event -> wd){
                        strcpy(name,directoryname[m]);
                        directoryname[m][0] = '\0';
                        wdlist[m] = 0;
                    }
                }
                creat_mod = false;
                for( j = 0; j < f; j = j + 1 ){
                    if (strcmp(newfile[j], name) != 0){
                        creat_mod = true;
                    }
                }
                if( creat_mod) {
                    struct entry new;
                    new.name = name; new.status = MODIFIED;
                    arr[used_entries] = new;
                    ++used_entries;
                }
                }
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
    char filepath[FILEPATH_SIZE];
    struct stat file_info;
    for (int i = 0; i < used_entries; ++i) {
        switch(arr[i].status) {
            case CREATED:

                strcpy(filepath, path);
                strcat(filepath, "/");
                stat(strcat(filepath, arr[i].name), &file_info);
                // print differently depending on type
                switch(file_info.st_mode & S_IFMT) {
                    case S_IFREG: printf("+ %s\n",arr[i].name); break;
                    case S_IFDIR:
                        printf("d %s\n",arr[i].name);
                        wdlist[directorynum] = inotify_add_watch(fd, filepath, IN_CREATE | IN_MODIFY | IN_DELETE);
                        strcpy(directoryname[directorynum],arr[i].name);
                        directorynum = directorynum + 1;
                        break;
                    default: printf("o %s\n",arr[i].name);      break;
                }
                break;

            case MODIFIED:
                printf("* %s\n", arr[i].name);
                break;

            case DELETED:
                printf("- %s\n", arr[i].name);
                break;

            // Must print deletion and creation consecutively
            // https://eclass.srv.ualberta.ca/mod/forum/discuss.php?d=935452
            case DELETED_THEN_CREATED:
                printf("- %s\n", arr[i].name);
                printf("+ %s\n", arr[i].name);
                break;

            // if not one of these cases, print nothing

       }
   }

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

  // If path is not a valid directory this will be handled in print_dir

  if (cmd_line_error) {
    printf("Usage: %s <period> <path>\n", argv[0]);

    printf("<period> is the reporting period in seconds\n");
    printf("<path> is the path to the directory to monitor\n");

    printf("\ndiffdir is a program that periodically reports the changes to the contents of a directory. It will run forever until terminated.\n");
    return -1;
  }
  /////////////////

  // actual main part of the program

  // https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/apis/sigactn.htm
  user_action.sa_flags = 0;
  alarm_action.sa_flags = 0;
  // this sets the action to call the appropriate function
  user_action.sa_handler = catch_user;
  alarm_action.sa_handler = catch_alarm;
  // this sets so when the signal SIGUSR1 is sent, user's action is called (print_dir function)
  sigaction(SIGUSR1, &user_action, NULL);
  sigaction(SIGALRM, &alarm_action, NULL);

  interrupt_action.sa_handler = catch_stop;
  interrupt_action.sa_flags = 0;

  fd = inotify_init();
  if (fd < 0) {
      printf("error initializing inotify\n");
      return -1;
  }
  wd = inotify_add_watch(fd, path, IN_CREATE | IN_MODIFY | IN_DELETE);

  // print out the contents of the directory
  print_initial_state();

  alarm(period);

  while (TRUE) {

      print_time();
      print_changes();
      sleep(5);
  }
}


