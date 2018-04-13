#include <cnet.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <time.h>
/*
# ------------------------------------------------------------
# Bowei Wang 1462495
# ePoint is used for different cases in Lanc manager to handle.
#- Struct LANC holds the following values:
#       - lasttimer:  the timer ID can be null if ther is no current timer
#	    - link: current link number
#	    - f, fLen, fTime: Information about frame, ftime is the time that needed to transmit information,
#         fLen is the length of the frame
#       - cs_flag: if 0, the controller does not use carrier sensing before transmitting  frame, this can be modified
#	      in the cnet file outside 
#       - state: lanc state that controls the state of lanc
#       - backoff: used to save te number of backoff happened
#	    - backoff_rn: This is a random number based on backoff
#       
# -LANC_start_timer is very useful it will trigger data after interval time
# so we can use this to handle some cases tat happens in future.
#
#- Function LANC_manager includes most of the protocol's logic.
#-For app case, do several asserts, then set state to busy, start a timer for cs.
#-For collosion case, printf the debug information, do several assert and stop timer.
#-if backoff is not less than max backoff, drop the frame by reinit the lanc, implement the  dropped frame number then enable all application layer.
#-if backoff is less than max backoff, set state to collieded, implement the backoff number, generate a new backoff_rn, start a new timer for backoff.
#-For cs case, get the temp frame length.
#	-if cs_flag == 1, check whether the link is available, if so, send current frame and start a timer for frame transmit. If link is not available, set a timer for next cs.
#	-if cs_flag is not 1. Send current frame and start      	 a timer for frame transmit.
#-For backoff case,set a timer for cs.
#-For frame transmit case, enable application layer for all nodes. Implement success frame number, then reinitial the lanc.
#-For default case, print error happened, show debug info, then exit the process.
#
#
#    - Variable 'lanc' (of type struct LANC) holds the current controller's
#      information which is very important in this program. Variable 'prev-lanc' is used to hold a 'previous'
#      copy of 'lanc'.
#
#    - Variable 'lanc_stats' (of type struct LANC_STATS) is intended to
#      hold protocol statistics about tx_frames,rx_frames, success frames, dropped frames.
#
#    - Printing routines (e.g., LANC_print, and LANC_STATS_print) takes
#      an argument and printf the debug infromation. 
#      
#    - When transmitting a frame on a LAN, remember that
#      transmit frames of length should be least LAN_MINPACKET bytes each.
#      So if the fLen is less than LAN_MINPACKET we should implement the number of fLen
#     
#
# Remarks:
#     - When all frames use the broadcast address as the destination address
#       and the GUI is not used, CNET reports zeros in the following fields:
#	- msgs_received (in struct CnetNodeStats)
#	- rx_frames     (in struct CnetLinkStats)
#	- rx_frames_corrupted, rx_frames_collisions (in struct CnetLinkStats)
#	The stats collected by the program should be more accurate.
#	 
# ------------------------------------------------------------ 
*/

#define FRAME_HEADER_SIZE  (sizeof(FRAME) - sizeof(MSG))
#define FRAME_SIZE(f)      (FRAME_HEADER_SIZE + (f).msgLen) //Don't use 'f.msgLen'

#define LAN_SLOTTIME   52	   //in microseconds
#define LAN_TINYSLOT    5	   
#define MAX_BACKOFF     7          

typedef enum { LC_idle, LC_busy, LC_collided } LC_STATE;
char *LC_STATE_str[]= { "LC_idle", "LC_busy", "LC_collided" };

enum { EP_app, EP_collision, EP_cs, EP_backoff, EP_frame_transmit };
char *EP_str[] = { "EP_app", "EP_collision", "EP_cs", "EP_backoff",
     	       	    "EP_frame_transmit" };
		      	   
typedef struct { char data[MAX_MESSAGE_SIZE]; } MSG;

typedef struct {
    CnetNICaddr	 destNIC, srcNIC;
    CnetAddr     dest, src;
    size_t	 msgLen;       
    int          checksum;  	
    		 		
    MSG          msg;		// the msg field must come last to transmit
    		 		// a padded frame
} FRAME;

// a LANC struct stores all key information of a single LAN controller
//
typedef struct {
    CnetTimerID	 lasttimer;
    int		 link;
    FRAME	 f;
    size_t	 fLen;
    int		 fTime;			// frame transmission time (usec)
    int		 cs_flag;		// carrier sense flag
    LC_STATE	 state;
    int		 backoff, backoff_rn;
} LANC;

// a LANC_STATS stores some key statistics about the protocol
//
typedef struct {
    int	   tx_frames;		// number of original frames attempted
    int	   success_frames;	// number of frames we think have succeeded
    int	   dropped_frames;	// number of frames dropped
    int	   rx_frames;		// number of frames correctly received
} LANC_STATS;
 
LANC        lanc;	   // only one LAN controller is supported for now
LANC        prev_lanc;     // used for debugging
LANC_STATS  lanc_stats;    // holds protocol stats

int   CS_FLAG ;             // 0 =  no carrier sensing 


int  make_frame (FRAME *f, CnetAddr dest, MSG *msg, size_t msgLen)
{
    size_t  fLen;		// frame length

    memcpy ( f->destNIC, NICADDR_BCAST, sizeof(CnetNICaddr) );
    memcpy ( f->srcNIC,  NICADDR_ZERO,  sizeof(CnetNICaddr) );

    f->dest = dest; f->src = nodeinfo.address;
    f->msgLen = msgLen;

    memset ( &(f->msg), 0, sizeof(MSG) );
    if (msg != NULL)  memcpy( &(f->msg), msg, (int)msgLen );

    // Note. Normally, cnet does not report corrupted frames to the receiving
    // host ('-E' overrides this behaviour). So, the sender should include an
    // error detection code.

    f->checksum = 0;
    fLen = FRAME_SIZE(*f);		// header + msg
    f->checksum =  CNET_ccitt( (unsigned char *)f,
    		   	(int) (fLen >= LAN_MINPACKET)? fLen: LAN_MINPACKET);
    return (fLen);
}

// ------------------------------------------------------------
// FRAME_init - use make_frame() with suitable values to initialize a frame
//
void FRAME_init (FRAME *f) { make_frame (f, -1 , NULL, 0);  } 

void FRAME_print (char *prefix, FRAME *f) {
    char    destNIC[20], srcNIC[20];

    CNET_format_nicaddr (destNIC, f->destNIC);
    CNET_format_nicaddr (srcNIC,  f->srcNIC);

    // printf ("(destNIC= %s, srcNIC= %s) \n", destNIC, srcNIC);
    printf ("%s %s: (dest= %d, src= %d, msgLen= %d) \n",
    	     prefix, nodeinfo.nodename, f->dest, f->src, f->msgLen);
}

// ------------------------------------------------------------
// LANC_init - initialize a LAN controller installed on the specified 'link'
//
void LANC_init (int link) {

    assert (linkinfo[link].linktype == LT_LAN ||
    	    linkinfo[link].linktype == LT_WLAN );

    lanc.lasttimer= NULLTIMER;
    lanc.link= link;
    lanc.state = LC_idle; lanc.cs_flag= CS_FLAG;
    lanc.backoff = lanc.backoff_rn = 0;
    FRAME_init (& lanc.f);
    lanc.fLen= FRAME_SIZE(lanc.f); lanc.fTime= LAN_SLOTTIME;
}

// ------------------------------

void LANC_print (char *prefix) {
    CnetData  timerData;

    if (lanc.lasttimer == NULLTIMER ) { timerData= -1; }
    else { CHECK( CNET_timer_data (lanc.lasttimer, &timerData) ); }

    printf ("%s --------------------\n", prefix);
    printf ("%s %s: LAN controller (time= %s, link= %d, cs_flag= %d)\n",
    	     prefix, nodeinfo.nodename, 
	     CNET_format64 (nodeinfo.time_in_usec), lanc.link, lanc.cs_flag );
    printf ("%s %s: LC_state= %s (Backoff= %d, Backoff_rn= %d) \n",
    	     prefix, nodeinfo.nodename,
    	     LC_STATE_str[lanc.state], lanc.backoff, lanc.backoff_rn );
    printf ("%s %s: timerData= %s, frame_time= %d \n",
    	     prefix, nodeinfo.nodename,
	     (timerData >= 0)? EP_str[timerData] : "NULL",
	     lanc.fTime);
    FRAME_print (prefix, & lanc.f); printf("\n");
} 
// ------------------------------
void LANC_STATS_init () {
     lanc_stats.tx_frames = 0;
     lanc_stats.success_frames = 0;
     lanc_stats.dropped_frames = 0;
     lanc_stats.rx_frames = 0;
}

void LANC_STATS_print (char *prefix) {

   printf ("%s --------------------\n", prefix);

   printf ("%s %s: LANC statistics (time= %s)\n",
    	    prefix, nodeinfo.nodename, CNET_format64 (nodeinfo.time_in_usec));
   printf ("%s %s: tx_frames= %d, success_frames= %d, dropped_frames= %d \n",
            prefix, nodeinfo.nodename, lanc_stats.tx_frames,
	    lanc_stats.success_frames, lanc_stats.dropped_frames);
   printf ("%s %s: rx_frames= %d \n",
            prefix, nodeinfo.nodename, lanc_stats.rx_frames);

   printf("\n");
}

// ------------------------------

void LANC_start_timer (CnetTime interval, CnetData data) {
    // Avoid starting a timer with a zero value
    // 
    if (interval > 0 ) {
        lanc.lasttimer = CNET_start_timer (EV_TIMER1, interval, data);
    }
    else {
        lanc.lasttimer = CNET_start_timer (EV_TIMER1, LAN_TINYSLOT, data);
    }
}

void LANC_stop_timer () {
    if (lanc.lasttimer != NULLTIMER)  {
        CNET_stop_timer (lanc.lasttimer); lanc.lasttimer = NULLTIMER;
    }
}
// ------------------------------------------------------------
void LANC_manager_debug () {
    // print debugging information if there is a key change in
    // the lanc struct

    if ( (prev_lanc.state      != lanc.state)   ||
         (prev_lanc.backoff    != lanc.backoff) ||
	 (prev_lanc.backoff_rn != lanc.backoff_rn) ) LANC_print ("debug: ");
    prev_lanc = lanc; 
}
// ------------------------------------------------------------
void LANC_manager (int ePoint)
{
    size_t   fLen;
    switch(ePoint){ 
        case EP_app:
            assert ( lanc.state == LC_idle );
            assert (lanc.f.dest != nodeinfo.address);
            lanc.state = LC_busy;
            LANC_start_timer (LAN_TINYSLOT, EP_cs);
            break;
               
        case EP_collision:
            printf("debug: LANC_manager : collision\n");
            LANC_manager_debug ();
            assert((lanc.state == LC_busy) || (lanc.state == LC_collided));
            assert(lanc.lasttimer != NULLTIMER);
           
            LANC_stop_timer ();
            if(lanc.backoff >= MAX_BACKOFF){
                LANC_init (lanc.link);
                lanc_stats.dropped_frames++;
                CNET_enable_application(ALLNODES);  
                break;
            }
            else{
                lanc.state = LC_collided;
                lanc.backoff = lanc.backoff + 1;
                lanc.backoff_rn = CNET_rand()% ((2^lanc.backoff)+1);
                LANC_start_timer (lanc.backoff_rn*LAN_SLOTTIME, EP_backoff);
                break;
            }
              
        case EP_cs:
            fLen = (lanc.fLen >= LAN_MINPACKET)? lanc.fLen: LAN_MINPACKET;
            if (CS_FLAG == 1){
                
                if (CNET_carrier_sense(lanc.link) == 1){
                    LANC_start_timer (LAN_TINYSLOT,EP_cs);
                    break;
                }
                else{
                    CNET_write_physical(lanc.link,&lanc.f,&fLen);
                    LANC_start_timer(lanc.fTime,EP_frame_transmit);
                    break;
                }
                
            }
            else{
                CNET_write_physical(lanc.link,&lanc.f,&fLen);
                LANC_start_timer(lanc.fTime,EP_frame_transmit);
                break;
            }
                
    
        case EP_backoff:
           
            LANC_start_timer(LAN_TINYSLOT,EP_cs);
            break;
        
        case EP_frame_transmit:
            CNET_enable_application(ALLNODES); 
            lanc_stats.success_frames = lanc_stats.success_frames + 1;
            LANC_init (lanc.link);
            break;
    
        default:
            printf (" Error: unknown entry point; exit with error");
            LANC_manager_debug();
            exit(1);
    }

}

// ------------------------------------------------------------
void NODE_print_stats (char *prefix) {

   CnetNodeStats  stats;

   CNET_get_nodestats (&stats);

   printf ("%s --------------------\n", prefix);

   printf ("%s %s: Node statistics (time= %s, nEvents= %ld, nErrors= %ld)\n",
    	    prefix, nodeinfo.nodename, CNET_format64 (nodeinfo.time_in_usec),
	     stats.nevents, stats.nerrors );
   printf ("%s %s: msgs generated= %ld, msgs received= %ld \n",
            prefix, nodeinfo.nodename,
	    stats.msgs_generated, stats.msgs_received );

   printf("\n");
}

// ------------------------------
void LINK_print_stats (char *prefix, int link) {

   CnetLinkStats  stats;

   CNET_get_linkstats (link, &stats);

   printf ("%s --------------------\n", prefix);
   printf ("%s %s: Link (= %d) statistics (time= %s)\n",
    	    prefix, nodeinfo.nodename, link, 
	    CNET_format64 (nodeinfo.time_in_usec) );
   printf ("%s %s: tx_frames= %ld, rx_frames= %ld \n",
            prefix, nodeinfo.nodename, stats.tx_frames, stats.rx_frames );
   printf ("%s %s: rx_frames_corrupted= %ld, rx_frames_collisions= %ld \n",
            prefix, nodeinfo.nodename,
	    stats.rx_frames_corrupted, stats.rx_frames_collisions );

   printf("\n");
}

// ------------------------------------------------------------

EVENT_HANDLER(application_ready)
{
    CnetAddr  destaddr;
    size_t    msgLen, fLen;
    MSG	      lastMsg;
    FRAME     lastFrame;

    msgLen  = sizeof(MSG);
    CHECK(CNET_read_application(&destaddr, &lastMsg, &msgLen));
    CNET_disable_application (ALLNODES);

    // make frame for storage
    fLen= make_frame(&lastFrame, destaddr, &lastMsg, msgLen);

    // store frame information in LAN controller (store a complete frame
    // of size 'sizeof(FRAME)' with struct msg padded with zeros)
    //
    assert (fLen <= LAN_MAXPACKET );
    lanc.fLen= fLen;
    memcpy ( &lanc.f, &lastFrame, sizeof(FRAME) );

    lanc.fTime= (int) (8.0 * lanc.fLen) /
	              (linkinfo[lanc.link].bandwidth / 1000000.00);

    // Request the LAN manager to transmit the frame (with struct msg possibly
    // padded with zeros)
    //
    assert ( lanc.state == LC_idle );
    lanc_stats.tx_frames++;    // increment number of frames attempted
    LANC_manager (EP_app);
}
// ------------------------------------------------------------
EVENT_HANDLER(physical_ready)
{
    FRAME        f;
    size_t	 msgLen, fLen;
    int          link, stored_checksum, computed_checksum;

    fLen         = sizeof(FRAME);
    CHECK(CNET_read_physical(&link, &f, &fLen));

    // Ignore if the frame is not for this node
    //
    if ( f.dest != nodeinfo.address ) return;

    // else, check the integrity of the received frame
    //
    stored_checksum    = f.checksum;
    f.checksum  = 0;
    computed_checksum= CNET_ccitt((unsigned char *)&f,
    		       (int) (fLen >= LAN_MINPACKET)? fLen: LAN_MINPACKET);

    if ( stored_checksum != computed_checksum) {
        printf("\t\tBAD checksum - frame ignored (stored= %d,computed= %d)\n",
	        stored_checksum, computed_checksum);
        return;
    }

    // else, deliver
    //
    msgLen= f.msgLen;
    lanc_stats.rx_frames++;   // increment number of received frames
    CHECK( CNET_write_application( &f.msg, &msgLen) );
}


// ------------------------------------------------------------

// Recall: 'EVENT_HANDLER(name)' expands to
//          void name(CnetEvent ev, CnetTimerID timer, CnetData data)

static EVENT_HANDLER(LANC_timeouts) 
{
    lanc.lasttimer= NULLTIMER;  LANC_manager (data);
} 

static EVENT_HANDLER(LANC_collisions) 
{
    if ( (lanc.state == LC_busy) || (lanc.state == LC_collided) ) {
        LANC_manager (EP_collision);   // inform manager
    }
    else {
        printf ("[%s] LANC_collisions: caught a LAN collision \n",
	         nodeinfo.nodename);
    }
} 

static EVENT_HANDLER(LANC_showstate)
{
    LANC_print("showstate: ");
    LANC_STATS_print ("showstate: ");
}

// ------------------------------
static EVENT_HANDLER(LANC_periodic)
{
    NODE_print_stats ("periodic: ");
    LINK_print_stats ("periodic: ", lanc.link);
    LANC_STATS_print ("periodic: ");
}

// ------------------------------
static EVENT_HANDLER(LANC_shutdown)
{
    NODE_print_stats ("shutdown: ");
    LINK_print_stats ("shutdown: ", lanc.link);
    LANC_STATS_print ("shutdown: ");
}
// ------------------------------------------------------------

EVENT_HANDLER(reboot_node)
{

    if ( (linkinfo[1].linktype != LT_LAN ) &&
    	 (linkinfo[1].linktype != LT_WLAN) ) {
	fprintf(stderr,"link 1 is neither a LAN nor a WLAN!\n");
	exit(1);
    }

    if (nodeinfo.nodetype != NT_ROUTER) {
         CHECK(CNET_set_handler( EV_APPLICATIONREADY, application_ready, 0));
    }

    CHECK(CNET_set_handler( EV_PHYSICALREADY,    physical_ready, 0));
    CHECK(CNET_set_handler( EV_TIMER1,           LANC_timeouts, 0));
    CHECK(CNET_set_handler( EV_FRAMECOLLISION,   LANC_collisions, 0));

    CHECK(CNET_set_handler( EV_SHUTDOWN,	 LANC_shutdown, 0));

    CHECK(CNET_set_handler( EV_PERIODIC,         LANC_periodic, 0));
    CHECK(CNET_set_handler( EV_DEBUG0,           LANC_showstate, 0));
    CHECK(CNET_set_debug_string( EV_DEBUG0, "LANC State"));

    CNET_enable_application(ALLNODES);

    CNET_srand (nodeinfo.time_of_day.sec + nodeinfo.nodenumber);

    // LANC specific initialization
    //

    CS_FLAG = atoi (CNET_getvar ("cs_flag"));
    assert ( CS_FLAG == 0 || CS_FLAG == 1);

    LANC_init (1);       // initialize LAN controller

    LANC_STATS_init ();  // initialize protocol's stats

    prev_lanc= lanc;	 // 'prev_lanc' is used for debugging to detect
    	       		 // changes of certain key values in the 'lanc'
}
