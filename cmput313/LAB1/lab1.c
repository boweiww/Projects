#include <cnet.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
// This file is based on "protocol.c"
// ------------------------------

typedef enum { DL_HELLO, DL_HELLO_ACK } FRAMEKIND;

typedef struct {
  char         data[MAX_NODENAME_LEN];
} MSG;

typedef struct {
  FRAMEKIND    kind;
  CnetAddr     srcAddr;
  CnetTime     time_send;
  MSG          msg;
  int          linknum;
} FRAME;
int hello_num = 0;
int ackhello_num = 0;
int currentlink = 0;
int readylink = 0;
char names[32][10];
int addresses[32];
#define MAX_DEGREE  32
// --------------------
// This function is defined in 'protocol.c' but is not used here.
//
static EVENT_HANDLER(application_ready)
{
    CnetAddr	destaddr;
    char	buffer[MAX_MESSAGE_SIZE];
    size_t	length;

    length = sizeof(buffer);
    CNET_read_application(&destaddr, buffer, &length);
    printf("\tI have a message of %4d bytes for address %d\n",
			    length, (int)destaddr);
}
// ------------------------------

static EVENT_HANDLER(button_pressed)
//This function is for information output when button is pressed.
{
    int j;
    if (readylink > MAX_DEGREE){
		printf("The neighbour number is over MAX_DEGREE\n");
		return;

    		}
    printf("\n Node name       : %s\n",	nodeinfo.nodename);
    printf(" Node number     : %d\n",	nodeinfo.nodenumber);
    printf(" Node address    : %d\n",	nodeinfo.address);
    printf(" Number of links : %d\n",	nodeinfo.nlinks);
    if (nodeinfo.nodetype == NT_HOST) { 
	printf(" Node type       : host\n");
	}else if (nodeinfo.nodetype == NT_ROUTER){
	printf(" Node type       : router\n");
	}else{
	printf(" unknown type\n");
	}
    printf(" Received Hello : %d\n", hello_num);
    printf(" Sent Hello_ack : %d\n", ackhello_num);
    for (j = 1; j <= readylink; j = j+1){
	printf(" link %d => %s, %d\n",j,names[j],addresses[j]);
	if (names[j+1] == NULL){
	break;
}
}
    printf("\n");
}
// ------------------------------
static EVENT_HANDLER(physical_ready)
   // Recall: the macro expands to the following function declaration:
   // static void physical_ready(CnetEvent ev, CnetTimerID timer, CnetData data)
{

  int    link;
  size_t len;
  FRAME  f;

  len= sizeof(FRAME);
  CHECK ( CNET_read_physical (&link, (char *) &f, &len) );

  switch (f.kind) {
  case DL_HELLO:
      hello_num = hello_num + 1;
      //assert ( link == link );
      f.kind = DL_HELLO_ACK;
      //f.msg.data = nodeinfo.nodename;
      f.srcAddr = nodeinfo.address;
     
      strcpy(f.msg.data, nodeinfo.nodename);
      // echo on the same link
      len= sizeof(f);
      CHECK( CNET_write_physical(link, (char *) &f, &len) );
      
      ackhello_num = ackhello_num + 1;
      break;

  case DL_HELLO_ACK: 
      //if the frame is corrput posibility is not 0 it will return a large negative address
      //this if satement will handle this case.
      if ( f.srcAddr > -10000){
	
      	addresses[link] = f.srcAddr;
	
      	if( link > readylink){
		readylink = link;
}

      	strcpy(names[link], f.msg.data);
}
	



      break;
  }
}
// ------------------------------
static EVENT_HANDLER(timer1_send_hello)
   // Recall: the macro expands to the following function declaration:
   // static void timer1_send_hello (CnetEvent ev, CnetTimerID timer, CnetData data)
{
  int   link=1;
  int   i;
  int   max_possible = MAX_DEGREE;
  size_t  len;
  FRAME f;

  f.kind= DL_HELLO;
  f.srcAddr= nodeinfo.address;
  f.time_send =  nodeinfo.time_in_usec;    // get current time
  f.linknum = currentlink;
  len = sizeof(f);
//refresh the maximun number of the valid link
  for( i = 0; i < max_possible; i = i + 1){
     CHECK( CNET_write_physical(link + i, (char *) &f, &len)) ;
     if (CNET_write_physical(link + i+1, (char *) &f, &len)==-1){
	max_possible = i+1;

	}
  }
  CNET_start_timer (EV_TIMER1, 1000000, 0);   // send continuous hello
}

// ------------------------------
EVENT_HANDLER(reboot_node)
{
//  Interested in hearing about:
//    - the Physical Layer has a frame to deliver
//    - timer 1
//
    //int a = 0;
    CNET_set_handler(EV_APPLICATIONREADY, application_ready, 0);

    CNET_set_handler(EV_DEBUG0, button_pressed, 0);
    CNET_set_debug_string(EV_DEBUG0, "Node Info");

    CNET_set_handler(EV_PHYSICALREADY, physical_ready, 0);
  //  for(a = 0; a < 2; a = a + 1){

    CNET_set_handler(EV_TIMER1, timer1_send_hello, 0);
    //CNET_set_handler(EV_TIMER1, timer1_send_hello, 0);
    // Request EV_TIMER1 in 1 sec, ignore return value
    CNET_start_timer (EV_TIMER1, 1000000, 0);

    // if (nodeinfo.nodetype == NT_HOST) { CNET_enable_application(ALLNODES); }

}
