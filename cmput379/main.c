//
//  main.c
//  379
//
//  Created by bowei on 2018-01-29.
//  Copyright © 2018 bowei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <sys/inotify.h>


int main( int argc, char **argv )
{
    int length, i = 0;
    int fd;
    int wd;
    //char buffer[BUF_LEN];
    
    fd = inotify_init();
    
    if ( fd < 0 ) {
        perror( "inotify_init" );
    }
    print (argc);
    print (argv);
    wd = inotify_add_watch( fd, "/home/strike",
                                                      IN_MODIFY | IN_CREATE | IN_DELETE );
    while TRUE{
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        
        time_t rawtime;
        struct tm * timeinfo;
        
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "Current local time and date: %s", asctime (timeinfo) );
        
        if ( event->len ) {
            if ( event->mask & IN_CREATE ) {
                printf( "The file %s was created.\n", event->name );
            }
            if ( event->mask & IN_DELETE ) {
                printf( "The file %s was deleted.\n", event->name );
            }
            if ( event->mask & IN_MODIFY ) {
                printf( "The file %s was modified.\n", event->name );
            }
        }
        time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
        if (sigaction(SIGCHLD,NULL,&old_action) != -1){
            printf("ctrl + c is pressed");
        }

        if (time_spent>=5.0){
            break;
    }
    }
    return 0;
}
