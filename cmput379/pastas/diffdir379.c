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
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>


#define EVENT_SIZE ( sizeof (struct inotify_event) )
#define BUF_LEN ( 1024 * ( EVENT_SIZE + 16 ) )

int main(  )
{
  
    int length, i = 0;
    int fd;
    int wd;
    // char buffer[BUF_LEN];
    char buffer[16384];

    fd = inotify_init();
    
    if ( fd < 0 ) {
        perror( "inotify_init" );
    }
     wd = inotify_add_watch( fd, "/cshome/bowei2", IN_MODIFY | IN_CREATE | IN_DELETE );
    if(wd < 0)
    {
        fprintf(stderr, "inotify_add_watch %s failed\n", argv[1]);
        return -1;
    }
    while (true){
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        
        time_t rawtime;
        struct tm * timeinfo;
        
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
	// printf ( "Current local time and date: %s", asctime (timeinfo) );
        
            if ( event->mask & IN_CREATE ) {
                printf( "The file %s was created.\n", event->name );
            }
            if ( event->mask & IN_DELETE ) {
                printf( "The file %s was deleted.\n", event->name );
            }
            if ( event->mask & IN_MODIFY ) {
                printf( "The file %s was modified.\n", event->name );
            }
	    
	// if (sigaction(SIGINT ,NULL,&old_action) != -1){
	//   printf("ctrl + c is pressed");
	// }
	 //	sleep(5);
	
    }
    return 0;
}
