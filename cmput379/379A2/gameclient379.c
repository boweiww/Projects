#include "common.h"
#include <netdb.h>
#include <strings.h>
#include <ncurses.h>
#include <sys/select.h>

static int sock;    // socket to server
static int action; // action this client will take on the next update
static int dim;

// mutex for modifying action or player position (since these are generally checked at the same time)
static pthread_mutex_t mutex; 

static int our_ID = -1;


// Terminate this client
// TODO should message server to let it know we are disconnecting - or can the server figure that out itself?
// TODO should print out score
void terminate(){
    printf("Terminating client.\n");
    close(sock);  // close socket to server
    endwin();     // end ncurses window
    exit(0);
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

void print_player(struct player *curr) {
    if (curr->action == SHOOT){
        switch(curr->facing) {
        case(UP):
            if(curr->y > 1) {
                mvprintw(curr->y - 1,curr->x,"O");
            }
            if(curr->y > 2) {
                mvprintw(curr->y - 2,curr->x,"O");
            }
            break;
        case(DOWN):
            if(curr->y < dim) {
                mvprintw(curr->y + 1,curr->x,"O");
            }
            if(curr->y < dim-1){
                mvprintw(curr->y + 2,curr->x,"O");
            }
            break;
        case(LEFT):
            if(curr->x > 1) {
                mvprintw(curr->y,curr->x - 1,"O");
            }
            if(curr->x > 2) {
                mvprintw(curr->y,curr->x - 2,"O");
            }
            break;
        case(RIGHT):
            if(curr->x < dim ) {
                mvprintw(curr->y,curr->x + 1,"O");
            }
            if(curr->x < dim - 1) {
                mvprintw(curr->y,curr->x + 2,"O");
            }
            break;
        }
    }
    switch(curr->facing){
        case(NONE):
            break;
        case(UP):
            mvprintw(curr->y,curr->x,"^");
            break;
        case(DOWN):
            mvprintw(curr->y,curr->x,"v");
            break;
        case(LEFT):
            mvprintw(curr->y,curr->x,"<");
            break;
        case(RIGHT):
            mvprintw(curr->y,curr->x,">");
            break;
        default:
            perror("Error printing the players\n");
            terminate();
    }

}

/*
    Print out the board and all the players
    Called by listen_server()

    Action does not actually occur until the server's game update.
   "No more than one command per client will be handled by the server in each update period"
    So this will not print out our latest move.

    At one point it would print out our move we expect to have before we move, but that looked weird and resulted in jumping around.
*/ 
void print_board(int num_players, struct player *player_array){
    // Print an empty board first, then add other objects.
    
    clear(); // clear ncurses screen

    // border_line is something like "+-----------+"
    char border_line[dim+3];
    for (int i = 1; i < dim+1; ++i){
        border_line[i] = '-';
    }
    border_line[0] = '+';
    border_line[dim+1] = '+';
    border_line[dim+2] = '\0';
        
    // Print borders at top and bottom
    mvprintw(0,0,"%s\n",border_line);
    mvprintw(dim+1,0,"%s\n",border_line);
    

    // blank_line is something like "|            |"
    char blank_line[dim+3];
    for (int i = 1; i < dim+1; ++i){
        blank_line[i] = ' ';
    }
    blank_line[0] = '|';
    blank_line[dim+1] = '|';
    blank_line[dim+2] = '\0';

    // Print blank_line dim times in the middle of the border lines
    for (int i = 1; i < dim+1; ++i){
        mvprintw(i,0,"%s\n",blank_line);
    }
    
    // TODO shots

    
    printf("our_ID: %d\n",our_ID);
    // Then, print players on the board
    // bold the character that represents ourselves
    for (int i = 0; i < num_players; ++i) {
        struct player *curr = &player_array[i];
        
        if(our_ID == curr->ID) {
            attron(A_BOLD);
        }

        print_player(curr);

        if(our_ID == curr->ID) {
            attroff(A_BOLD);
        }
    }

    // After drawing, refresh the ncurses window to display results
    refresh();
}

// Waits for server to send a message
// When it does, update player positions
void listen_server(fd_set readset) {
    // http://developerweb.net/viewtopic.php?id=2933

    // Wait for message
    int result = select(sock + 1, &readset, NULL, NULL, NULL);

    if (result > 0) {
        if (FD_ISSET(sock, &readset)) {
            // There is data to read            
            // first server tells us how big the player array is
            // TODO check result here and in the loop
            int num_players = -1;
            result = recv(sock, &num_players, sizeof(num_players), 0);
            if (num_players == -1){
                
                //destroyed
                terminate();
            }

            // make sure we can store all the info
            struct player player_array[num_players*sizeof(struct player)];
            
            // Read in information about all player's positions
            for (int i = 0; i < num_players; ++i) {
                result = recv(sock, &player_array[i], sizeof(struct player), 0);
            }

             // reset action for next update

            // Print the new state of the board
            print_board(num_players, player_array);
            action = NONE;
        }
    } 
    else { 
        perror("An error occured listening to server.\n");
        terminate();
    }

    /*
    int result,tempaction ;
    //fd_set readset;
    struct timeval tv;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    tv.tv_sec = 2;
    tv.tv_usec = 50;
    
    result = select(sock + 1, &readset, NULL, NULL, &tv);
    if (result > 0) {
        result = recv(sock, &tempaction, sizeof(struct player), 0);
        if (tempaction != ' '){
            action = tempaction;
            printf("I Got IT\n");
            
            if (result == 0) {
                
                close(sock);
                //exit, should be removed when two client is added.
                exit(1);
            }
        }
    }
*/
}


// calls listen_server forever
// should be safer and more efficient than calling self recursively
void * listen_server_loop() {
    fd_set readset;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);

    while(TRUE){
        listen_server(readset);        
    }
}

/*
   Reads input and sets action correspondingly.
   Actions can be NONE, UP, DOWN, LEFT, RIGHT, or SHOOT
   Action does not actually occur until the server's game update.
   "No more than one command per client will be handled by the server in each update period"
*/
void read_input() {
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
            action = UP;
            break;

        // move down
        case 'k':
            action = DOWN;
            break;

        // move left
        case 'j':
            action = LEFT;
            break;

        // move right
        case 'l':   
            action = RIGHT; 
            break;
            
        // shoot
        case ' ':
            action = SHOOT;  
            break;
    }
    pthread_mutex_unlock(&mutex);

    // send action to server
    send(sock, &action, 10, 0);
    
}

// calls read_input forever
// should be safer and more efficient than calling self recursively
void * read_input_loop() {
    
    while(TRUE) {
        read_input();
    }
}

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
	
    // get dimensions of the board
    recv(sock,&dim,4,0);
    printf("dim: %d\n",dim);

    // get our unique player ID
    recv(sock,&our_ID,sizeof(our_ID),0);
    printf("ID: %d\n",our_ID);

    // Initiate ncurses user interface
    initscr();
    cbreak(); 
    noecho();

    /*
         TODO this is copy-pasted out of listen server, so this part should probably 
     be broken into a function
    */
    ///////
    // first server tells us how big the player array is
    // TODO check result here and in the loop
    int num_players = -1;
    int result = recv(sock, &num_players, sizeof(num_players), 0);

    // make sure we can store all the info
    struct player player_array[num_players*sizeof(struct player)];
            
    // Read in information about all player's positions
    for (int i = 0; i < num_players; ++i) {
        result = recv(sock, &player_array[i], sizeof(struct player), 0);
    }

    print_board(num_players, player_array);
    ///////

    /////////////////
    // Initalize mutex
    pthread_mutex_init(&mutex, NULL);

    // start input thread
    // gets input from keyboard
    pthread_t inputThreadID;
    pthread_create(&inputThreadID, NULL, read_input_loop, NULL);

    // start server thread
    // listens the server socket for position updates
    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, listen_server_loop, NULL);

    while(TRUE) {

    }
    
}

