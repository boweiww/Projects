#define false 0
#define true 1
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <sys/time.h>

// holds the previous sample of the directory in memory
/*struct content_arr{
    struct dirent *arr;
    int    size;
    int    used;

};

// initialize global variables for use in print_dir


// checks if content_arr needs to be reallocated and then doubles capacity if neccessary
void reallocate() {
    if (contents.used >= contents.size -1) {
        contents.size *= 2;
        contents.arr = realloc(contents.arr, contents.size*sizeof(struct dirent));
    }
}*/
static int period = 5;
static char *path = 0;
//static struct content_arr contents;
char buffer[EVENT_BUF_LEN];
char lastbuffer[EVENT_BUF_LEN];
int length, i =0;
int fd,flags,nfds;
fd_set read_fdst;
struct timeval tv;


int print_dir() {
    // 1. print the current time
    // http://zetcode.com/articles/cdatetime/
    int time_string_len = 256;
    char time_string[time_string_len];


    time_t curr_time = time(NULL);            // get time in seconds
    struct tm *date = localtime(&curr_time);  // split time into day, month, year, etc.

    // format and print time
    // Sun Nov 5 13:10:50 MST 2017

    strftime(time_string, time_string_len, "%a %b %d %H:%M:%S %Z %Y", date);
    printf("%s\n",time_string);


    length =0;
/*
    FD_ZERO(&read_fdst);
    FD_SET(0,&read_fdst);
    tv.tv_sec = 1;
    tv.tv_usec = 100;
    nfds = select (1,&read_fdst,NULL,NULL,&tv);
    switch(nfds){
    case -1:
        printf("Something wrong happened");
    case 0:
        return 0;
    default:
        //printf("I am in");
        length = read(fd, buffer, EVENT_BUF_LEN);
    }*/
    //return read(fd, buffer, EVENT_BUF_LEN);


    //The program will be stuck at here

    length = read(fd, buffer, EVENT_BUF_LEN);

    //fwrite(buffer, EVENT_BUF_LEN, 1, stdout);
    //length = read(fd, buffer, EVENT_BUF_LEN);




    i=0;
    while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                        printf("+ %s \n", event->name);
                    }
                }
                if (event->mask & IN_DELETE) {
                        printf("- %s\n", event->name);
                    }
                if (event->mask & IN_MODIFY) {
                        printf("* %s\n", event->name);
                    }



            i += EVENT_SIZE + event->len;
        }


    strncpy(lastbuffer, buffer, EVENT_BUF_LEN);


    return 0;
}

int main (int argc, char *argv[]) {
  // invoked as follows:
  // diffdir379 period path

  // Input handling
  /////////////////

  //daljdhsajkhd
  int wd;
  int cmd_line_error = false;

  if (argc != 3) {
    cmd_line_error = true;
  }
  else {
    //period = atoi(argv[1]);
    path = argv[2];
  }

  // If path is not a valid directory this will be handled in print_dir
  if (cmd_line_error) {
    printf("\nUsage: %s <period> <path>\n", argv[0]);

    printf("\n<period> is the reporting period in seconds\n");
    printf("\n<path> is the path to the directory to monitor\n");

    printf("\ndiffdir is a program that periodically reports the changes to the contents of a directory. It will run forever until terminated.\n");
    return -1;
  }
  /////////////////

  // actual main part of the program

  /*int start_arr_size = 256;
  contents.arr = malloc(start_arr_size * sizeof(struct dirent));
  contents.size = start_arr_size;
  contents.used = 0;*/

  fd = inotify_init();

    /*checking for error*/
  if (fd < 0) {
        perror("inotify_init");
    }
  wd = inotify_add_watch(fd, path, IN_MODIFY | IN_CREATE | IN_DELETE);


  // sigaction is confusing
  // https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/apis/sigactn.htm
  //struct sigaction sa;
  //struct sigaction sa;
  //sa.sa_flags = 0;
  // this sets the action for sa to call the print_dir function
  //sa.sa_handler = print_dir;
  // this sets so when the signal SIGUSR1 is sent, sa's action is called (print_dir function)
  //sigaction(SIGUSR1, &sa, NULL);

  // this sends the signal to test if it works

  //kill(getpid(), SIGUSR1);

    //printf("%d",buffer[fd]);
  /*if (flags = fcntl(STDIN_FILENO,F_GETFL,0)<0){
    printf("F_GETFL error");
    exit(0);
  }

  flags |= O_NONBLOCK;

  if( fcntl( STDIN_FILENO, F_SETFL, flags) < 0){
       printf("F_SETFL error");
        exit(0);
  }*/
  //fcntl(fd, F_SETFL, O_NONBLOCK);
  while(true) {
      //printf("This is d %d",buffer[fd]);

      print_dir();
      sleep(period);
      //break;

  }
  return 0;
}


