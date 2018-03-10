#include "common.h"
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>

// TODO mutex

// struct player is defined in common.h
static int sock;
static int dim = -1;
static double period = -1; 
static unsigned long int micro_period = -1;


// available ID to assign to a client.
static unsigned int available_ID = 0; 

// Holds information related to how a client's communicates with the server
// Only server has this
struct connection{
    int ID;      // client's unique ID
    int socket;
};

// A "client" is someone who has connected to the server
// A "connection" has information related to how that client communicates to the server - Only server needs to know this.
// A "player" has each client's game related information - their position and so on
// "player" structs are sent to ALL clients every update, so should be as small as possible
struct client_list {
    struct player *player_array;         // array of players (has position and game information)
    struct connection *connect_array;    // array of connections (has socket and connection information)
    pthread_t *input_threads;            // array of threads    

    int num;                             // number of players 
    int size;                            // number of spaces available for players in the array
};
static struct client_list all_clients;

static pthread_mutex_t mutex;

// Checks if array needs to be reallocated and then doubles capacity if needed
// mutex is locked when this is called
void reallocate_client_list() {
    if(all_clients.num >= all_clients.size -1) {
        all_clients.size *= 2;
        all_clients.player_array = realloc(all_clients.player_array, all_clients.size*sizeof(struct player));
        all_clients.connect_array = realloc(all_clients.connect_array, all_clients.size*sizeof(struct connection));
        all_clients.input_threads = realloc(all_clients.input_threads, all_clients.size*sizeof(pthread_t));
    }
}

// called by check_target and get_client_action
// mutex is locked when called
void termination_message(int array_location) {
    // -1 means player died
    int message = -1;    
    int socket = all_clients.connect_array[array_location].socket;

    int result = send(socket, &message, sizeof(message), 0);
    if (result < 0) {
        perror("Error sending client termination message");
        close(socket);
        return;
    }

    close(socket);
}

// called by shoot_action
// mutex is locked when this is called
int check_target(int* shot){
    int hits = 0;
    struct player *pos;
    for(int i = 0; i < all_clients.num; ++i) {
        pos = &all_clients.player_array[i];
        if (shot[0] == pos->x  && shot[1] == pos->y){
            termination_message(i);
            ++hits;
        }
    }
    return hits;
}

// called by game_update
// mutex is locked when this is called
int shoot_action(struct player pos ){
    int shot[2];
    int hits = 0;
    switch(pos.facing) {
        case(UP):
            if(pos.y > 1) {
                shot[1] = pos.y - 1;
                shot[0] = pos.x;
                hits = check_target(shot);
            }
            if(pos.y > 2) {
                shot[1] = pos.y - 2;
                shot[0] = pos.x;
                hits = check_target(shot);
            }
            break;
        case(DOWN):
            if(pos.y < dim) {
                shot[1] = pos.y + 1;
                shot[0] = pos.x;
                hits = check_target(shot);
            }
            if(pos.y < dim-1){
                shot[1] = pos.y + 2;
                shot[0] = pos.x;
                hits = check_target(shot);
            }
            break;
        case(LEFT):
            if(pos.x > 1) {
                shot[1] = pos.y;
                shot[0] = pos.x - 1 ;
                hits = check_target(shot);
            }
            if(pos.x > 2) {
                shot[1] = pos.y;
                shot[0] = pos.x - 2 ;
                hits = check_target(shot);
            }
            break;
        case(RIGHT):
            if(pos.x < dim ) {
                shot[1] = pos.y;
                shot[0] = pos.x + 1 ;
                hits = check_target(shot);
            }
            if(pos.x < dim - 1) {
                shot[1] = pos.y;
                shot[0] = pos.x + 2 ;
                hits = check_target(shot);
            }
            break;
    }

    return hits;
}

// called by resolve_player_actions and new_client
// mutex is locked when this is called
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

// called by game_update
// mutex is locked when this is called
void resolve_player_actions(struct player *pos) {
    // Set new position based on action
    // Cannot move off of the grid
    int position[2];
    position[0] = pos->x;
    position[1] = pos->y;
    switch(pos->action) {
        case(UP):
            position[1] = position[1] - 1;
            if(pos->y > 1 && check_overlap(position)) {
                pos->y -= 1;
            }
            pos->facing = UP;
            break;
        case(DOWN):
            position[1] = position[1] + 1;
            if(pos->y < dim && check_overlap(position)) {
                pos->y += 1;
            }
            pos->facing = DOWN;
            break;
        case(LEFT):
            position[0] = position[0] - 1;
            if(pos->x > 1 && check_overlap(position)) {
                pos->x -= 1;
            }
            pos->facing = LEFT;
            break;
        case(RIGHT):
            position[0] = position[0] + 1;
            if(pos->x < dim && check_overlap(position)) {
                pos->x += 1;
            }
            pos->facing = RIGHT;
            break;   
    }

    assert(pos->x > 0 && pos->x < dim+1 && pos->y > 0 && pos->y < dim+1);

}

// Event that happens every <period>
// Locks and unlocks mutex
void game_update() {
    // update game
    pthread_mutex_lock(&mutex);   // LOCK

    // resolve player actions

    // For each player, take its action and move it
    struct player *pos;
    for(int i = 0; i < all_clients.num; ++i) {
        resolve_player_actions(&all_clients.player_array[i]);
    }
    //estimate fire
    int hits = 0;
    for (int i = 0; i < all_clients.num; ++i){
        if (all_clients.player_array[i].action ==  SHOOT){
            struct player pos = all_clients.player_array[i];
            hits = shoot_action(pos);
            all_clients.player_array[i].score += hits;
        }
    }

    // For each client, 
    int num_players = all_clients.num;
    int result = -1;

    for(int client = 0; client < all_clients.num; ++client) {
        int socket = all_clients.connect_array[client].socket;
        // Tell client how many players there are
        result = send(socket, &num_players, sizeof(num_players), 0);
        if (result < 0) {
            //perror("Error sending num_players message");
            close(socket);
            continue;
        }

        // For each player, send the client the new position
        for (int i = 0; i < all_clients.num; ++i){
            pos = &all_clients.player_array[i];
            result = send(socket, pos,sizeof(struct player),0);
            if (result < 0) {
                //perror("Error sending player positions");
                close(socket);
                break;
            } 
        }
    }

    // Reset player actions
    for (int i = 0; i < all_clients.num; ++i) {
        all_clients.player_array[i].action = NONE;
    }

    pthread_mutex_unlock(&mutex); // UNLOCK

}

//handle SIGTERM
void term(int signum)
{
    int message = -1;
    for(int i = 0; i < all_clients.num; ++i) {
        send(all_clients.connect_array[i].socket, &message, sizeof(message), 0);
    }

    for(int i = 0; i < all_clients.num; ++i) {
        close(all_clients.connect_array[i].socket);
    }

    free(all_clients.connect_array);
    free(all_clients.player_array);
    free(all_clients.input_threads);

    exit(1); 
}

// called only by the clients get_client_action thread when it cannot connect to the client anymore
// generally, if a client is to be removed, its socket is closed, so the thread deletes it and returns
// mutex is locked when called 
void remove_client(int ID) {
    int array_location = -1;
    for (int i = 0; i < all_clients.num; ++i) {
        if (all_clients.connect_array[i].ID == ID){
            array_location = i;
            break;
        }
    }
    if (array_location < 0) {
        printf("Could not find client to remove\n");
        term(-1);
    }

    pthread_t thread_ID = all_clients.input_threads[array_location];
    // remove client
    // move each ahead of it back 1
    for (int i = array_location+1; i < all_clients.num; ++i) {
        all_clients.player_array[i-1] = all_clients.player_array[i];
        all_clients.connect_array[i-1] = all_clients.connect_array[i];
        all_clients.input_threads[i-1] = all_clients.input_threads[i];   
    }
    all_clients.num -= 1; 
}

// locks and unlocks mutex
void * get_client_action(void *arg){
    int ID = *(int *) arg;
    int array_location = -1;

    pthread_mutex_lock(&mutex);   // LOCK
    for (int i = 0; i < all_clients.num; ++i) {
        if (all_clients.connect_array[i].ID == ID) {
            array_location = i;
        }
    }
    
    int socket = all_clients.connect_array[array_location].socket;
    pthread_mutex_unlock(&mutex); // UNLOCK    

    int connection_error = FALSE;

    while(!connection_error) {
        char action = 0;

        fd_set readset;
        FD_ZERO(&readset);
        FD_SET(socket, &readset);

        struct timeval tv = {1};
        int result = select(socket + 1, &readset, NULL, NULL, &tv);
    
        if (result >= 0) {
            // Get client's action (NONE, UP, DOWN, LEFT, RIGHT, or SHOOT)
            result = recv(socket, &action, sizeof(action), 0);
            if (result < 0) {
                printf("Lost connection to client during action input.\n");
                connection_error = TRUE;
                close(socket);
                printf("connect err\n");
            } else {

                if(action == -1) {
                    connection_error = TRUE;
                }
               
                pthread_mutex_lock(&mutex);   // LOCK.

                // make sure we have the right array location
                for (int i = 0; i < all_clients.num; ++i) {
                    if (all_clients.connect_array[i].ID == ID) {
                        array_location = i;
                    }
                }

                all_clients.player_array[array_location].action = action;
            
                pthread_mutex_unlock(&mutex); // UNLOCK
            }
        } else {
            printf("Lost connection to client during select.\n");
            connection_error = TRUE;
            close(socket);
            printf("connection error\n");
        }    
    }

    // remove this client from everything and let the thread return
    pthread_mutex_lock(&mutex);
    remove_client(ID);
    pthread_mutex_unlock(&mutex);
    
}
    
// Listen for and add a new client to the game
// called only by new_client_loop
// locks and unlocks mutex
void new_client(){
    // wait for someone to connect to us
    int result = listen(sock, 5);
    if (result != 0) {
        perror("Error listening for new clients failed");
        term(1);
    }

    int outnum =-1, number =-1;
    struct sockaddr_in from;
    int fromlength = sizeof(from);

    struct connection new_connect;

    new_connect.socket = accept(sock, (struct sockaddr*) & from, &fromlength);    
    if (new_connect.socket < 0) {
        perror("Error accept failed");
        term(1);
    }
    outnum = htonl(number);
 
    pthread_mutex_lock(&mutex);   // LOCK
    // If game is already full, send -1
    if (dim*dim == all_clients.num) {
        int full = -1;
        result = send(new_connect.socket, &full, sizeof(dim), 0);
        close(new_connect.socket);
        // we don't really care about the result because we don't want them anyways
    }
    // If game not full, add client to the game
    else {
        // We have one more player/client connected
        
        const int array_num = all_clients.num;
    
        // Send the dimensions of the board to the client
        send(new_connect.socket, &dim,sizeof(dim),0);
        if (result < 0) {
            perror("Error: sending dimensions");
            close(new_connect.socket);
            return;
        }

        // Generate a unique "player ID" (stored in both player and connection arrays)
        int ID = available_ID;
    
        struct player new_player; 
        new_player.ID = ID;
        new_connect.ID = ID;

        // Send player ID
        int result = send(new_connect.socket, &ID, sizeof(ID), 0);
        if (result < 0) {
            perror("Error sending ID");
            close(new_connect.socket);
            return;
        }
        // Generate a random location to place the new player
        // Generate the position    
        int position[2];
        position[0] = (rand() % dim)+1;
        position[1] = (rand() % dim)+1;
        // Cannot start where someone already is
        while (!check_overlap(position)){
            position[0] = (rand() % dim)+1;
            position[1] = (rand() % dim)+1;
        }

        new_player.x = position[0], new_player.y = position[1];
        new_player.facing = UP; // "The default initial direction of a player is 'up'".
        new_player.action = NONE;
        new_player.score = 0;
        assert(new_player.x > 0 && new_player.x < dim+1 && new_player.y > 0 && new_player.y < dim+1);

        // Store information
        reallocate_client_list();   // make sure arrays have enough space
        all_clients.connect_array[array_num] = new_connect;  // store socket in array
        all_clients.player_array[array_num] = new_player;
        available_ID += 1;
        all_clients.num += 1;
        
        // Send the number of players
        int num_players = all_clients.num;
        send(new_connect.socket, &num_players, sizeof(num_players),0); 
        if (result < 0) {
            perror("Error sending new client num_players");
            close(new_connect.socket);
            return;
        }  
 
        // Send the positions of all players 
        struct player *pos;
        for (int i = 0; i < all_clients.num; ++i){
            pos = &all_clients.player_array[i];
            send(new_connect.socket, pos,sizeof(struct player),0);
            if (result < 0) {
                perror("Error sending new client starting positions");
                close(new_connect.socket);
                return;
            }
        }

        // Start input thread
        pthread_create(&all_clients.input_threads[array_num], NULL, get_client_action, (void *) &ID);
    }

    pthread_mutex_unlock(&mutex); // UNLOCK
}

void* new_client_loop(){
    while(TRUE){
            new_client();
    }
}

void print_usage(char *argv[]) {
    printf("\nUsage: %s <dimensions> <period> <port> <random_seed>\n", argv[0]);
    
    printf("<dimensions> are the dimensions of the square grid (16 = 16x16 grid). Should be less than 32767\n");
    printf("<period> is the update period in seconds (can be decimal).\n");
    printf("<port> is the port number where the server listens.\n");
    printf("<random_seed> is the seed for the random number generator.\n");
}

void daemonize(){
    pid_t pid;
    
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
 
}

int main (int argc, char *argv[]) {
    daemonize();

    // invoked as follows:
    // gameserver379 <dimensions> <period> <port> <random_seed>
  
    // Argument handling
    /////////////////
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

        if((dim < 0) | (period < 0) | (port < 0)) {
            printf("\nERROR: None of the inputs can be negative.\n");
            cmd_line_error = TRUE;
        } 
    }

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
        exit(1);
    }

    // Allocate space for the first entry of player and connection arrays
    all_clients.player_array = malloc(sizeof(struct player));
    all_clients.connect_array = malloc(sizeof(struct connection));
    all_clients.input_threads = malloc(sizeof(pthread_t));
    all_clients.size = 1; 
    all_clients.num = 0;

    // Initalize mutex
    pthread_mutex_init(&mutex, NULL);

    pthread_t clientThreadID;
    pthread_create(&clientThreadID, NULL, new_client_loop, NULL);
    
    // Start timer to call game_update every <period> seconds
    struct sigaction alarm_action;
    alarm_action.sa_flags = 0;
    alarm_action.sa_handler = game_update;
    sigaction(SIGALRM, &alarm_action, NULL);
    
    
    int seconds = (int)floor(period);
    int microseconds = (period - seconds)*1000000;

    struct itimerval timer;
    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = microseconds;
    timer.it_interval.tv_sec = seconds;
    timer.it_interval.tv_usec = microseconds;

    setitimer ( ITIMER_REAL, &timer, NULL );

    struct sigaction sig_action;
    memset(&sig_action, 0, sizeof(sig_action));
    sig_action.sa_handler = term;
    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);

    // ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);
    
    pthread_join(clientThreadID, NULL);


    // TODO daemonization             
}
