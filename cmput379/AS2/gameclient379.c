#include "common.h"
#include <netdb.h>
#include <strings.h>
#include <ncurses.h>
#include <pthread.h>
#include <sys/select.h>
#include <curses.h>
// Contains any declarations or functions common to both client and server


// Holds information related to a client's character

// All clients are sent player structs for each player in the game, so this should be as small as possible


static int sock;    // socket to server
static int action; // action this client will take on the next update
static int dim;

// mutex for modifying action or player position (since these are generally checked at the same time)
static pthread_mutex_t mutex; 

// Terminate this client
// TODO should message server to let it know we are disconnecting - or can the server figure that out itself?
// TODO should print out score
void terminate(){
    printf("Terminating client.\n");
    close(sock);  // close socket to server
    endwin();     // end ncurses window
    exit(0);
}

/*
   Reads input and sets action correspondingly.
   Action does not actually occur until the server's game update.
   "No more than one command per client will be handled by the server in each update period"
*/
void print_board(char *border_line, char *blank_line, struct player *us){
    // Print an empty board first, then add other objects.
    
    clear(); // clear ncurses screen
    
    // border_line is something like "+-----------+" corresponding to dim
    // Print borders at top and bottom
    mvprintw(0,0,"%s\n",border_line);
    mvprintw(dim+1,0,"%s\n",border_line);
    
    // blank_line is something like "|            |" corresponding to dim
    // Print blank_line dim times in the middle of the border lines
    for (int i = 1; i < dim+1; ++i){
        mvprintw(i,0,"%s\n",blank_line);
    }
    
    // Get our current position and next action
    //pthread_mutex_lock(&mutex);
    char curr_action = action;
    int x = us->x, y = us->y, facing = us->facing;
    //pthread_mutex_unlock(&mutex);
    
    // TODO we only print ourselves
    // TODO shots
    
    // Then, print ourselves on the board
    // bold the character that represents ourself
    attron(A_BOLD);
    switch(facing){
        case(NONE):
            break;
        case(UP):
            mvprintw(y,x,"^");
            break;
        case(DOWN):
            mvprintw(y,x,"v");
            break;
        case(LEFT):
            mvprintw(y,x,"<");
            break;
        case(RIGHT):
            mvprintw(y,x,">");
            break;
        default:
            perror("Error printing the players\n");
            terminate();
    }
    attroff(A_BOLD);
    
    // After drawing, refresh the ncurses window to display results
    refresh();
}
void print_board_loop(struct player *us){
    // border_line is something like "+-----------+"
    char border_line[dim+3];
    for (int i = 1; i < dim+1; ++i){
        border_line[i] = '-';
    }
    border_line[0] = '+';
    border_line[dim+1] = '+';
    border_line[dim+2] = '\0';
    
    // blank_line is something like "|            |"
    char blank_line[dim+3];
    for (int i = 1; i < dim+1; ++i){
        blank_line[i] = ' ';
    }
    blank_line[0] = '|';
    blank_line[dim+1] = '|';
    blank_line[dim+2] = '\0';
    
    while (TRUE){
        print_board(border_line, blank_line, us);
    }
}
void read_input(struct player *us) {
    char ch = getch();
    pthread_mutex_lock(&mutex);
    switch(ch) {
        // terminate
        // If terminate is called, mutex will stay locked. This is fine because the program is terminating anyway, right?
        case 'x':
            terminate();
            break;

        // move up
        case 'i':   
            if (us->y > 1){
                action = UP;
            }
                
            break;

        // move down
        case 'k':
            if (us->y < dim){
                action = DOWN;
            }
            break;

        // move left
        case 'j':
            if (us->x > 1){
                action = LEFT;
            }
            break;

        // move right
        case 'l':   
            if (us->x < dim){
                action = RIGHT;
            }    
            break;
            
        // shoot
        case ' ':
            action = SHOOT;  
            break;
    }
    pthread_mutex_unlock(&mutex);
    printf("%d\n" ,action);
    // send action to server
    send(sock, &action, 10, 0);
    
}

// calls read_input forever
// should be safer and more efficient than calling self recursively
void * read_input_loop(void * arg) {
    struct player *us = arg;
    
    while(TRUE) {
        read_input(us);
    }
}

// Waits for server to send a message
// When it does, update our position
void user_action (struct player *us , int action){
    //int x = us->x, y = us->y, facing = us->facing;
    switch(action){
        case(0):
            return;
        case(UP):
            us->y = us->y + 1;

            break;
        case(DOWN):
            us->y = us->y - 1;
            break;
        case(LEFT):
            us->x = us->x - 1;
            break;
        case(RIGHT):
            us->x = us->x + 1;
            break;
        case(SHOOT):
            printf("shoot\n");
            return;
    }
    us->facing = action;
}
void listen_server(struct player *us, fd_set readset) {
    // http://developerweb.net/viewtopic.php?id=2933

    // Wait for message
    /*int result = select(sock + 1, &readset, NULL, NULL, NULL);

    if (result > 0) {
        if (FD_ISSET(sock, &readset)) {
            // There is data to read
            pthread_mutex_lock(&mutex);
            result = recv(sock, us, 10, 0);
            action = NONE; // reset action for next update
            pthread_mutex_unlock(&mutex);
        }
    } 
    else { 
        perror("An error occured listening to server.\n");
        terminate();
    }
    */
    
    int result,tempaction ;
    //fd_set readset;
    struct timeval tv;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    tv.tv_sec = 0;
    tv.tv_usec = 50;
    
    result = select(sock + 1, &readset, NULL, NULL, &tv);
    if (result > 0) {
        result = recv(sock, &tempaction, sizeof(struct player), 0);
        if (tempaction != ' '){
            action = tempaction;
            printf("I Got IT\n");
            user_action(us, action);
            if (result == 0) {
                
                close(sock);
                //exit, should be removed when two client is added.
                exit(1);
            }
        }
    }
}


// calls listen_server forever
// should be safer and more efficient than calling self recursively
void * listen_server_loop(void * arg) {
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);

    struct player *us = arg;
    while(TRUE){
        listen_server(us, readset);
        
        //print_board_loop(us);
        
    }
}

// Print out the shots
void print_shots(int x, int y, int facing){
/*
    int x_change=0, y_change=0;
    
    switch(facing){
        case(UP):
            y_change=-1;
            break;
        case(DOWN):
            y_change=1;
            break;
        case(LEFT):
            x_change=-1;
            break;
        case(RIGHT):
            x_change=1;
            break;
    }

    int new_y = y;
    int new_x = x;
    for (int i = 0; i < 2; ++i) {
        new_y += y_change;
        new_x += x_change;
        
        // Make sure shots can't go over the board
        if(new_x >= 1 && new_x <= dim && new_y >= 1 && new_x <= dim) {
            mvaddch(new_y, new_x, 'o');
        }
    }
*/
}

/*
    Print out the board and all the players

    Action does not actually occur until the server's game update.
   "No more than one command per client will be handled by the server in each update period"
    So this will not print out our latest move.

    At one point it would print out our move we expect to have before we move, but that looked weird and resulted in jumping around.
*/ 


// Setup the lines of the empty board first
// Then calls print_board forever
// print_board seg faults if it calls itself recursively a lot


void print_usage(char *argv[]) {
    printf("\nUsage: %s <host> <port>\n", argv[0]);

    printf("<hostname> is the hostname OR IP address of the host on which the server is running.\n");
    printf("<port> is the port number where the server listens.\n");
    
    printf("\nUse localhost as the <hostname> for testing on the same computer.\n");
}

int main (int argc, char *argv[]) {
    // invoked as follows:
    // gameclient379 host port
  
    // Argument handling
    /////////////////

    char *hostname = 0;
    int port = -1;
 
    int cmd_line_error = FALSE;
  
    if (argc != 3) {
      cmd_line_error = TRUE;
    }
    else {
      hostname = argv[1];
      port = atoi(argv[2]);

      if(port < 0) {
          printf("\nERROR: port cannot be negative\n");
          cmd_line_error = TRUE;
      }
    }

    if (cmd_line_error) {
      print_usage(argv);
      return -1;
    }

    /////////////////
    printf("Attempting to connect to host '%s' at port '%d'...\n", hostname, port);
    
    struct hostent *host;
    host = gethostbyname (hostname);
    if (host == NULL) {
        perror ("Client: cannot get host description");
        exit (1);
    }

    sock = socket (AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror ("Client: cannot open socket");
        exit (1);
    }

    // catch iterrupts to gracefully terminate
    struct sigaction terminate_action;
    terminate_action.sa_flags = 0;
    terminate_action.sa_handler = terminate;
    sigaction(SIGINT, &terminate_action, NULL);

    struct sockaddr_in server;
    bzero (&server, sizeof (server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons (port);
    if (connect (sock, (struct sockaddr*) & server, sizeof (server))) {
        perror ("Client: cannot connect to server");
        exit (1);
    }
	
    // get dimensions of the board and x,y position
    struct player us;
    recv(sock,&dim,4,0);
    printf("dim: %d\n",dim);
    recv(sock,&us,sizeof(struct player),0);

    printf("dim: %d, x: %d, y :%d\n",dim, us.x, us.y);
    assert(dim > 0 && us.x > 0 && us.y > 0);
    assert(us.x < dim+1 && us.y < dim+1);

    printf("Successfully connected to host.\n");
    /////////////////

    // Initiate ncurses user interface
    initscr();
    cbreak(); 
    noecho();

    // Initalize mutex
    pthread_mutex_init(&mutex, NULL);

    // start input thread
    // gets input from keyboard
    pthread_t inputThreadID;
    
    pthread_create(&inputThreadID, NULL, read_input_loop, (void *) &us);

    // start server thread
    // listens the server socket for position updates
    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, listen_server_loop, (void *) &us);

    // start output thread
    // prints the current status of the board constantly
    print_board_loop(&us);
    
}

