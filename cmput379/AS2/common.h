// Contains any declarations or functions common to both client and server

#include <stdio.h>    
#include <stdlib.h>    
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>

#define FALSE 0
#define TRUE 1

#define NONE 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define SHOOT 5

// Holds information related to a client's character

// All clients are sent player structs for each player in the game
struct player{
    int ID;       // client's unique ID
    int x;        // X position of player
    int y;        // Y position of player 
    int score;

    char facing;   // direction player is facing (UP, DOWN, LEFT, or RIGHT)
    char action;   // action client wants to take (NONE, UP, DOWN, LEFT, RIGHT or SHOOT)
};
