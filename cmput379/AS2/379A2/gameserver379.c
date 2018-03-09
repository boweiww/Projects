#include "common.h"
#include <string.h>
#include <strings.h>

// struct player is defined in common.h
static int savenum[100];
static int sock;
static int dim = -1;
static double period = -1; // TODO alarm takes integer: can't do decimal values - setitimer or ualarm are possible solutions

// available ID to assign to a client.
static unsigned int available_ID = 0; 

// Holds information related to how a client's communicates with the server
// Only server has this
struct connection{
    int ID;      // client's unique ID
    int socket;
    int score;   // This is game related, but other clients don't need to know this
};

// A "client" is someone who has connected to the server
// A "connection" has information related to how that client communicates to the server - Only server needs to know this.
// A "player" has each client's game related information - their position and so on
// "player" structs are sent to ALL clients every update, so should be as small as possible
struct client_list {
    struct player *player_array;         // array of players (has position and game information)
    struct connection *connect_array;    // array of connections (has socket and connection information)
    int num;                             // number of players 
    int size;                            // number of spaces available for players in the array
};
static struct client_list all_clients;

// Checks if array needs to be reallocated and then doubles capacity if needed
void reallocate_client_list() {
    if(all_clients.num >= all_clients.size -1) {
        all_clients.size *= 2;
        all_clients.player_array = realloc(all_clients.player_array, all_clients.size*sizeof(struct player));
        all_clients.connect_array = realloc(all_clients.connect_array, all_clients.size*sizeof(struct connection));
    }
}
void check_target(int* shot){
    //printf ("shot at %d  %d\n",shot[0],shot[1]);
    struct player *pos;
    for(int i = 0; i < all_clients.num; ++i) {
        pos = &all_clients.player_array[i];
        if (shot[0] == pos->x  && shot[1] == pos->y){
            pos -> action = -2;
            //printf("destroyed!!!!!!!!!!!!!\n");
        }
        
    }
}
void shoot_action(struct player *pos ){
    int shot[2];
    switch(pos->facing) {
        case(UP):
            if(pos->y > 1) {
                shot[1] = pos->y - 1;
                shot[0] = pos->x;
                check_target(shot);
            }
            if(pos->y > 2) {
                shot[1] = pos->y - 2;
                shot[0] = pos->x;
                check_target(shot);
            }
            break;
        case(DOWN):
            if(pos->y < dim) {
                shot[1] = pos->y + 1;
                shot[0] = pos->x;
                check_target(shot);
            }
            if(pos->y < dim-1){
                shot[1] = pos->y + 2;
                shot[0] = pos->x;
                check_target(shot);
            }
            break;
        case(LEFT):
            if(pos->x > 1) {
                shot[1] = pos->y;
                shot[0] = pos->x - 1 ;
                check_target(shot);
            }
            if(pos->x > 2) {
                shot[1] = pos->y;
                shot[0] = pos->x - 2 ;
                check_target(shot);
            }
            break;
        case(RIGHT):
            if(pos->x < dim ) {
                shot[1] = pos->y;
                shot[0] = pos->x + 1 ;
                check_target(shot);
            }
            if(pos->x < dim - 1) {
                shot[1] = pos->y;
                shot[0] = pos->x + 2 ;
                check_target(shot);
            }
            break;

        
    }
}
int check_overlap(int* position){
    struct player *pos;
    for (int i = 0; i < all_clients.num; ++i){
        pos = &all_clients.player_array[i];
        if (pos->x == position[0] && pos->y == position[1]){
            return 0;
        }
    }
    return 1;
}
void resolve_player_actions(struct player *pos) {
    // Set new position based on action
    // Cannot move off of the grid
    int position[2];
    position[0] = pos->x;
    position[1] = pos->y;
    switch(pos->action) {
        case(-1):
            // This means that the client disconnected
            // TODO we should do something here
            break;

        case(UP):
            position[1] = position[1] - 1;
            if(pos->y > 1 && check_overlap(position)) {
                pos->y -= 1;
                pos->facing = UP;
            }
            break;
        case(DOWN):
            position[1] = position[1] + 1;
            if(pos->y < dim && check_overlap(position)) {
                pos->y += 1;
                pos->facing = DOWN;
            }
            break;
        case(LEFT):
            position[0] = position[0] - 1;
            if(pos->x > 1 && check_overlap(position)) {
                pos->x -= 1;
                pos->facing = LEFT;
            }
            break;
        case(RIGHT):
            position[0] = position[0] + 1;
            if(pos->x < dim && check_overlap(position)) {
                pos->x += 1;
                pos->facing = RIGHT;
            }
            break;
        
        
    }

    assert(pos->x > 0 && pos->x < dim+1 && pos->y > 0 && pos->y < dim+1);

    // Reset player action
    //pos->action = NONE;
}

// Event that happens every <period>
void game_update() {
    // update game

    // TODO mutex?

    // resolve player actions
    // TODO detect collisions
    // "the server game logic should not allow two players to move into the same grid point. If two (or more) players try to move to the same grid point, those move commands are ignored."

    // For each player, take it's action and move it
    struct player *pos;
    for(int i = 0; i < all_clients.num; ++i) {
        if (all_clients.player_array[i].ID == -1){
            continue;
        }
        //pos = &all_clients.player_array[i];
        resolve_player_actions(&all_clients.player_array[i]);
    }
    //estimate fire
    for (int i = 0; i < all_clients.num; ++i){
        if (all_clients.player_array[i].action ==  SHOOT){
            //printf ("fire !!!!!!\n");
            shoot_action(&all_clients.player_array[i] );
        }
    }
    // TODO is it possible to do the sends to all clients at once to get rid of the first for loop?

    // For each client, 
    int num_players = all_clients.num;
    int destroyed = -1;
    int client;
    for(client = 0; client < all_clients.num; ++client) {
        if (all_clients.player_array[client].ID == -1){
            continue;
        }
        // Tell client how many players there are
        if (all_clients.player_array[client].action == -2){
            send(all_clients.connect_array[client].socket, &destroyed, sizeof(num_players), 0);
        }
        else{
            send(all_clients.connect_array[client].socket, &num_players, sizeof(num_players), 0);
        }

        // For each player, send the client the new position
        for (int i = 0; i < all_clients.num; ++i){
            pos = &all_clients.player_array[i];
            //printf("i:%d x:%d y:%d\n",i,pos->x,pos->y);
            //printf ("handling: %d\n",client);
            send(all_clients.connect_array[client].socket, pos,sizeof(struct player),0);
        }
    }
    for(int i = 0; i < all_clients.num; ++i) {
        //pos = &all_clients.player_array[i];
        all_clients.player_array[i].action = 0;
    }

    //alarm(period);
}

void get_client_action(int playernum){
    // Get client's action (NONE, UP, DOWN, LEFT, RIGHT, or SHOOT)
    
    //struct timespec tim, tim2;
    //tim.tv_sec = 0;
    //tim.tv_nsec = 50000000;
    //nanosleep(&tim , &tim2)

    //printf("Current action player 1:%d\n",playernum);
    //in select to make unblock functions, the playernum should be saved in another place
    //because when goes into select, the address of playernum will be released( I think )
    //the value will be unknown
    int sock = all_clients.connect_array[playernum].socket;
    int action,tempaction,result ;
    if (all_clients.player_array[playernum].ID == -1){
        return;
    }
    //int savenum ;//= playernum;
   
    savenum[playernum] = playernum;

    fd_set readset;
    //struct player *pos = &all_clients.player_array[savenum];
    //it might be a little bit strange for the savenum[playernum], but it is necessary.
    
    struct timeval tv;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    action = -1;
    tempaction = -1;
    result = select(sock + 1, &readset, NULL, NULL, &tv);
    
    if (result > 0) {
        
        result = recv(sock, &action, 10, 0);
        if (action != ' '){
            all_clients.player_array[savenum[playernum]].action = action;
            
            if (result == 0) {
                all_clients.player_array[savenum[playernum]].ID = -1;
                all_clients.player_array[savenum[playernum]].x = -1;
                all_clients.player_array[savenum[playernum]].y = -1;
                close(sock);

                //exit, should be removed when two client is added.
                //exit(1);
            }
        }
    }
    
}
    
    /*
    fd_set readset;
    struct timeval tv;
    FD_ZERO(&readset);
    FD_SET(sock, &readset);
    tv.tv_sec = 0;
    tv.tv_usec = 500;
    action = -1;
    tempaction = -1;
    result = select(sock + 1, &readset, NULL, NULL, &tv);
    
    if (result > 0) {
        
        result = recv(sock, &action, 10, 0);
        if (action != ' '){
            
            all_clients.player_array[0].action = action;
            if (result == 0) {
     
                close(sock);
                 //exit, should be removed when two client is added.
                exit(1);
            }
        }
        
      */

// Listen for and add a new client to the game
void new_client(){
    // wait for someone to connect to us
    listen(sock, 5);

    int outnum =-1, number =-1;
    struct sockaddr_in from;
    int fromlength = sizeof(from);

    struct connection new_connect;

    new_connect.socket = accept(sock, (struct sockaddr*) & from, &fromlength);    
    if (new_connect.socket < 0) {
        perror("Server: accept failed");
        exit(1);
    }
    outnum = htonl(number);

    // We have one more player/client successfully connected
    all_clients.num += 1;
    const int array_num = all_clients.num - 1;
    
    // Send the dimensions of the board to the client
    printf("send dim: %d\n", dim);
    send(new_connect.socket, &dim,4,0);

    // Generate a unique "player ID" (stored in both player and connection arrays)
    int ID = available_ID;
    
    struct player new_player; 
    new_player.ID = ID;
    new_connect.ID = ID;

    // Send player ID
    send(new_connect.socket, &ID, sizeof(ID), 0);
    
    // Generate a random location to place the new player
    // Generate the position    
    new_player.x = (rand() % dim)+1, new_player.y = (rand() % dim)+1;
    new_player.facing = UP; // "The default initial direction of a player is 'up'".
    new_player.action = NONE;
    assert(new_player.x > 0 && new_player.x < dim+1 && new_player.y > 0 && new_player.y < dim+1);

    // Store information
    reallocate_client_list();                            // make sure arrays have enough space
    new_connect.score = 0;
    all_clients.connect_array[array_num] = new_connect;  // store socket in array
    all_clients.player_array[array_num] = new_player;
    available_ID += 1;


    // Send the number of players
    int num_players = all_clients.num;
    send(new_connect.socket, &num_players, sizeof(num_players),0);   
 
    // Send the positions of all players 
    struct player *pos;
    for (int i = 0; i < all_clients.num; ++i){
        pos = &all_clients.player_array[i];
        send(new_connect.socket, pos,sizeof(struct player),0);
    }

    printf("finish new_client()\n");
}

void print_usage(char *argv[]) {
    printf("\nUsage: %s <dimensions> <period> <port> <random_seed>\n", argv[0]);
    
    printf("<dimensions> are the dimensions of the square grid (16 = 16x16 grid). Should be less than 32767\n");
    printf("<period> is the update period in seconds (can be decimal).\n");
    printf("<port> is the port number where the server listens.\n");
    printf("<random_seed> is the seed for the random number generator.\n");
}



void* new_clientloop(){
    while(TRUE){
            new_client();
    
    }
}


//handle SIGTERM
void term(int signum)
{
    //printf("SIGTERM is triggered\n");
    int destroyed = -1;
    int num_players = all_clients.num;
    for (int i = 0; i < all_clients.num; ++i){
        if (all_clients.player_array[i].ID == -1){
            continue;
        }
         send(all_clients.connect_array[i].socket, &destroyed, sizeof(num_players), 0);
        
    }
    sleep(1);
    printf("All the client is disconnected\n");
    exit(1);
   
}


int main (int argc, char *argv[]) {
    // invoked as follows:
    // gameserver379 <dimensions> <period> <port> <random_seed>
  
    // Argument handling
    /////////////////
    int action;
    int port = -1;
    int seed = -1;
    char* address = "127.0.0.1";
    int cmd_line_error = FALSE;
  
    if (argc != 5) {
        cmd_line_error = TRUE;
    }
    else {
        dim = atoi(argv[1]);
        period = atof(argv[2]);
        port = atoi(argv[3]);
        seed = atoi(argv[4]);  

        if(dim < 0 | period < 0 | port < 0 | seed < 0) {
            printf("\nERROR: None of the inputs can be negative.\n");
            cmd_line_error = TRUE;
        } 
    }

    printf("dim: %d\n",dim);
    printf("period: %f\n",period);
    printf("port: %d\n",port);
    printf("seed: %d\n",seed);
    printf("address: %s",address);
    if (cmd_line_error) {
        print_usage(argv);
        return -1;
    }

    /////////////////

    // Set random number generator seed
    srand(seed);

    // CREATE SOCKETS AND CONNECT TO A CLIENT
    struct sockaddr_in master;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Server: cannot open master socket");
        exit(1);
    }

    master.sin_family = AF_INET;
    master.sin_addr.s_addr = inet_addr(address);
    master.sin_port = htons(port);

    if (bind (sock, (struct sockaddr*) &master, sizeof (master))) {
        perror ("Server: cannot bind master socket");
        exit (1);
    }

    // Allocate space for the first entry of player and connection arrays
    all_clients.player_array = malloc(sizeof(struct player));
    all_clients.connect_array = malloc(sizeof(struct connection));
    all_clients.size = 1; 
    all_clients.num = 0;


    pthread_t clientThreadID;
    pthread_create(&clientThreadID, NULL, new_clientloop, NULL);
    // Start alarm to call game_update every <period> seconds
    //struct sigaction alarm_action;
    //alarm_action.sa_flags = 0;
    //alarm_action.sa_handler = game_update;
    //sigaction(SIGALRM, &alarm_action, NULL);
    // TODO alarm takes integer: can't do decimal values - setitimer or ualarm are possible solutions
    //alarm(period);
    //printf("get action \n");

    
    struct sigaction sig_action;
    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_handler = term;
    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGALRM, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);

    struct timeval start, stop;
    double secs = 0;
    double time = 0;
    int i;
    double avg_time = 0;
    
    while (TRUE){
        
        while (TRUE){
            //set a clock, if clock is reached, handle all the clients' input
            gettimeofday(&start, NULL);
            //This loop will get all the clients' action, useful for multiple client
            for (i = 0; i < all_clients.num; ++i){
                get_client_action(i);
            }
            
            gettimeofday(&stop, NULL);
            secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
            if ( 0 < secs && secs< 10000){
                time = time +secs;
                //read input from client every 2 secs.
                if (time >= period - avg_time){
                    time = 0;
                    break;
                }
            }
            
        }
        //update and make time shorter.
        gettimeofday(&start, NULL);
        game_update();
        gettimeofday(&stop, NULL);
        secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
        avg_time = (avg_time + secs)/2;
        
    }
    
    
   
    // TODO do a "graceful exit" (it never reaches here)
    

    close (all_clients.connect_array[0].socket);
    

               
}
