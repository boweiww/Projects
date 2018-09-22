#include <cnet.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
# ------------------------------------------------------------
# Copyright Notice:
#    Copyright by CMPUT 313, U. of Alberta, course instructor (E. Elmallah).
#    All rights reserved. Do not post any part on a publicly-available Web site.
#
# Important:
#     Your submitted solution based on using parts of the code should
#     - Replace the provided documentation with your own documentation
#     - Omit the provided suggested steps
#     - Acknowledge parts of this code used in your solution
#
# Purpose:
#     This file implements a sliding window protocol with the following features:
#     - Sender buffers "sendWin" pkts 
#     - Sender keeps one timeout period (for the oldest transmitted but not yet
#     	ack'ed pkt). On timeout, only the timed out packet is retransmitted.
#     - Receiver buffers "rcvWin" pkts (it stores out-of-order pkts if	
#        rcvWin > 1)
#     - Receiver sends cumulative ACKs; the ack number is the sequence number
#       of the last correctly received in-order pkt
#     - Each frame carries a 'seq' number and an 'ack' number; these numbers
#       count whole frames (transmitted or received). There is no 'kind' field
#	in a frame.
# 
# Implementation remarks:
#     - For a transmitted frame f:
#       - If (f.seq == -1) the frame carries no data in the 'msg' field.
#	  Else, if (f.seq >= 0) the frame carries a 'msg' whose sequence number
#	  is 'f.seq'.
#       - If (f.ack == -1) the frame carries no acknowledgement.
#	  Else if (f.ack >= 0), the frame confirms that the sender has correctly
#	  received all frames in the range [0,ack] (cumulative ACK).
#
#     - So, a frame can be one of the following types:
#       - (seq >= 0)  && (ack >= 0):  a frame with valid data and 'ack' number
#	- (seq >= 0)  && (ack == -1): a data only frame
#	- (seq == -1) && (ack >= 0):  an ACK only frame
#	- (seq == -1) && (ack == -1): neither a data frame nor an ACK frame
#
#     - For simplicity, the sender's buffer is implemented as an array of
#       frames: sendBuf[0, 1, ..., sendWin-1]. Each frame is initialized by
#	FRAME_init() to be a "null" frame.
#
#     - 'sendBase' is the sequence number of the oldest transmitted but unack'ed
#        pkt in array sendBuf[] (the sender's window)
#
#     - 'nextseqnum' is the sequence number used if a new frame is transmitted
#
#     - At any instant, the relation between the sequence numbers of frames
#       stored in array sendBuf[] and the indexes of these frames is:
#     	    sendBuf[0] <--> sendBase
#	    sendBuf[1] <--> sendBase + 1
#	    ...
#	    sendBuf[sendWin-1] <--> sendBase + sendWin - 1
#
#     - Likewise, for simplicity, the receiver's buffer is implemented as an
#       array of frames: rcvBuf[0,1, ..., rcvWin-1]. Each frame is initialized
#	by FRAME_init().
#
#     - 'rcvBase' is the next sequence number expected by the receiver
#
#     - At any instant, the relation between the sequence numbers of frames
#       stored in array rcvBuf[] and the indexes of these frames is:
#	    rcvBuf[0] <--> rcvBase
#	    rcvBuf[1] <--> rcvBase + 1
#	    ...
#	    rcvBuf[rcvWin-1] <--> rcvBases + rcvWin - 1 
#
#     - The topology file should define variables "sendWin" and "rcvWin", e.g.,
#       ...
#       var sendWin= "4"
#	var rcvWin = "3"
#	...
#
# Tips:
#     - Given a frame index, say 'fIdx', in sendBuf[0, ..., sendWin-1] (or,
#       rcvBuf[0, ..., rcvWin-1]) where fIdx > 0, sometimes we want to relocate
#	the frames at positions (fIdx, fIdx+1, fIdx+2, ...) to the new positions
#	(0, 1, 2, ...), respectively. This can be done by copying the frames
#	to the new positions, and initializing frames at the vacated positions.
#
# Status:
#    - Currently, the protocol works only for 2-node networks
#    - There is one instance of the CONN struct, so the protocol handles
#      only one sender-receiver connection
#
# ------------------------------------------------------------ 
*/

#define FRAME_HEADER_SIZE  (sizeof(FRAME) - sizeof(MSG))
#define FRAME_SIZE(f)      (FRAME_HEADER_SIZE + (f).msgLen) //Don't use 'f.msgLen'

typedef struct {  char data[MAX_MESSAGE_SIZE]; } MSG;

typedef struct {
    CnetAddr     src,dest; 
    size_t	 msgLen;       	// the length of the msg field only
    int          checksum;  	// checksum of the whole frame
    int          seq;       	// for a data pkt, seq > 0 (else -1)
    int		 ack;		// for a valid ack, ack > 0 (else, -1)
    MSG          msg;
} FRAME;

// a CONN struct stores all key information of a single connection
//
typedef struct {
    CnetAddr     dest;
    CnetTimerID	 lasttimer;
    int          sendWin, rcvWin;	// size of sender and receiver windows
    int          sendBase, nextseqnum;	// sender variables
    int          rcvBase;		// receiver variables
    FRAME        *sendBuf, *rcvBuf;   	// sender and receiver buffers (arrays)  
} CONN;

static CONN  conn;	// only one connection is supported in this version

static CnetTime  CONN_timeout;
// ------------------------------------------------------------
// FRAME_init - initialize a frame with 'seq= -1' and 'ack= -1'. 
//

void FRAME_init (FRAME *f) {
    f->src = f->dest = -1;
    f->msgLen = 0;
    f->checksum = 0;
    f->seq = f->ack = -1;
    memset ( &(f->msg), 0, sizeof(MSG) );
}   
// ------------------------------

void FRAME_print (FRAME *f) {
    printf ("(src= %d, dest= %d, seq= %d, ack= %d, msgLen= %d) \n",
     	    f->src, f->dest, f->seq, f->ack, f->msgLen);
}

// ------------------------------
// CONN_init - initialize a connection with the specified 'sendWin' and
//     'rcvWin' buffer sizes
//
void CONN_init (int sendWin, int rcvWin) {
    int idx;

    conn.dest = -1;
    conn.lasttimer = NULLTIMER;
    conn.sendWin= sendWin; conn.rcvWin= rcvWin;  

					// start with zero sequence numbers
    conn.sendBase = conn.nextseqnum = conn.rcvBase = 0;
    
    conn.sendBuf = (FRAME *) calloc (sendWin, sizeof(FRAME)); // array of FRAMEs
    conn.rcvBuf  = (FRAME *) calloc (rcvWin,  sizeof(FRAME)); // array of FRAMEs 

    for (idx= 0; idx < sendWin; idx++) FRAME_init ( &(conn.sendBuf[idx]) );

    for (idx= 0; idx < rcvWin; idx++)  FRAME_init ( &(conn.rcvBuf[idx]) );
}
// ----------

void CONN_print () {
    FRAME_print(conn.sendBuf);
    FRAME_print(conn.rcvBuf);
    // Suggested Steps:
    // - Complete this function. You may use FRAME_print() to print information
    //   about each frame in sendBuf[] and rcvBuf[]

    // ...
}
// ----------

void CONN_stop_timer () {
    if (conn.lasttimer != NULLTIMER)  {
        CNET_stop_timer (conn.lasttimer); conn.lasttimer = NULLTIMER;
    }
}
// ----------

void CONN_start_timer () {
    conn.lasttimer = CNET_start_timer (EV_TIMER1, CONN_timeout, 0);
}

// ----------
// make_frame - compose a frame with the specified parameters.
// Note: a frame can be composed, stored in 'sendBuf', and transmitted.
//      To re-transmit the frame at some future time, the 'ack'
//	field has to be updated, and consequently the 'checksum' field
//	has to be recomputed. So, the frame should be re-composed again.
//
static int  make_frame (FRAME *f, CnetAddr dest, int seq, int ack,
       	    	        MSG *msg, size_t msgLen)
{
    size_t  fLen;		// frame length

    f->src = nodeinfo.address;  f->dest = dest;
    f->msgLen = msgLen;
    f->seq = seq; f->ack = ack;
    if (msg != NULL)  memcpy( &(f->msg), msg, (int)msgLen );

    // Note. Normally, cnet does not report corrupted frames to the receiving
    // host ('-E' overrides this behaviour). So, the sender should include an
    // error detection code.

    f->checksum = 0;
    fLen = FRAME_SIZE(*f);
    f->checksum =  CNET_ccitt((unsigned char *)f, (int)fLen);
    return (fLen);
}

// ------------------------------------------------------------

static EVENT_HANDLER(application_ready)
{
    CnetAddr  destaddr;
    int       outLink;
    size_t    msgLen, fLen;
    MSG	      lastMsg;
    FRAME     lastFrame;
    int	      lastFrame_idx;	// index in sendBuf[] to store a frame carrying
    	      			// a new AL msg

    msgLen  = sizeof(MSG);
    CHECK(CNET_read_application(&destaddr, &lastMsg, &msgLen));

    if (conn.dest == -1) conn.dest = destaddr;	 // initialize conn.dest    
    if (conn.dest != destaddr) return;		 // ignore msgs not to conn.dest 

    lastFrame_idx = conn.nextseqnum - conn.sendBase;
    //assert (lastFrame_idx < conn.sendWin);

    // Disable AL if the sendBuf becomes full after inserting a new frame
    //
    if ( (lastFrame_idx + 1) == conn.sendWin ) {
         CNET_disable_application (destaddr);
    }	  	 

    // make frame for storage
    fLen= make_frame(&lastFrame, destaddr, conn.nextseqnum, -1, &lastMsg, msgLen);
    memcpy ( &(conn.sendBuf[lastFrame_idx]), &lastFrame, fLen );
    printf("transmitted, dest=%d\n", destaddr);
    // transmit frame
    outLink = 1;
    CHECK ( CNET_write_physical (outLink, &lastFrame, &fLen) );
    
    // Suggested remaining steps:
    // - If (nextseqnum == sendBase) restrat timer. First, stop the running
    //   timer (if any), then start the timer.
    // - Update 'nextseqnum'
    if (conn.nextseqnum == conn.sendBase){
        CONN_stop_timer ();
        CONN_start_timer ();
    }
    conn.nextseqnum = conn.nextseqnum + 1;
    // ....
    // .... 
}

// ------------------------------------------------------------

static EVENT_HANDLER(physical_ready)
{
    FRAME        f, fout;
    FRAME      *newbuff;
    size_t	  fLen;
    int          link, computed_checksum,stored_checksum;
    int		 outLink, fIdx;
    //CnetAddr  destaddr;
    
    fLen         = sizeof(FRAME);
    CHECK(CNET_read_physical(&link, &f, &fLen));

    // check the integrity of the received frame
     // - If the frame fails the integrity check return to caller

    stored_checksum    = f.checksum;
    f.checksum  = 0;
    computed_checksum= CNET_ccitt((unsigned char *)&f, (int)fLen);
    if(computed_checksum != stored_checksum) {
        printf("\t\t\t\tBAD checksum - frame ignored\n");
        return;           // bad checksum, ignore frame
    }
   
    printf("get information\n");
    printf("seq = %d dest=%d\n" ,f.seq,f.dest);
    printf("nodenumber:%d\n",nodeinfo.nodenumber);
    printf("rcvBase:%d, max:%d\n",conn.rcvBase,conn.rcvBase + conn.rcvWin);
    // - If 'conn.dest' is still uninitialized, initialize it

   // CHECK(CNET_read_application(&destaddr, &lastMsg, &msgLen));

    if (conn.dest == -1) conn.dest = f.dest;	 // initialize conn.dest    

    printf("seq = %d dest=%d\n" ,f.seq,f.dest);
    // - If (the received frame is a data frame, and it is for this host, and
    //   it can be stored in rcvBuf[]) then

    //	     - Set 'fIdx' to the position in rcvBuf[] where the frame should 
    //	       be stored
    // 	     - If (rcvBuf[fIdx] is empty) then copy the frame there.
    //	       Else, check that the frame in rcvBuf[fIdx] has the same sequence
    //	       number as the one in the received frame.
    if (f.seq >= 0){
         printf(" DATA transmitted, seq=%d\n", f.seq);
             if(nodeinfo.nodenumber == f.dest){
                
              if(conn.rcvBase <= f.seq){ 
                
                if(f.seq <= (conn.rcvBase + conn.rcvWin)){
                 fIdx = f.seq - conn.rcvBase - 1;
                 if (conn.rcvBuf[fIdx].ack != -1){
                    
                     //assert(f.seq == conn.rcvBuf[fIdx].seq);
                 }
                 else{
                      conn.rcvBuf[fIdx] = f;
                 }
                 int i,j;
                 j = 0;
    
                 for (i = 0; i<conn.rcvWin; i = i + 1){
                     if (conn.rcvBuf[i].ack != -1){
                         CHECK(CNET_write_application(&conn.rcvBuf[i].msg, &conn.rcvBuf[i].msgLen));
                         conn.rcvBase = conn.rcvBuf[i].seq;
                         j = j + 1;
                         continue;
                     }
                     break;
                 }
                j= j+1;
                newbuff  = (FRAME *) calloc (conn.rcvWin,  sizeof(FRAME));
                for (i = 0; i < (conn.rcvWin - j); i = i +1 ){
                    newbuff[i] = conn.rcvBuf[i];
                }
                conn.rcvBuf = newbuff;
                outLink = 1;
                printf("dest : %d\n",f.src);
                fLen = make_frame(&fout, f.src, -1, conn.rcvBase, 0, 0);
                CHECK ( CNET_write_physical (outLink, &fout, &fLen) );
             }
             }
             }
    }

    if (f.ack >= conn.sendBase){
        printf("ACK transmitted, seq=%d\n", f.ack);
        int i;
       // j = 0;
        assert(f.ack <= conn.nextseqnum );
        int basenum = (f.ack - conn.sendBase)+1;
        newbuff  = (FRAME *) calloc (conn.sendWin, sizeof(FRAME));
        for (i = basenum;i <= conn.sendWin;i++){
            newbuff[i-basenum] = conn.sendBuf[i];
            //&conn.sendBuf = (&conn.sendBuf + sizeof(FRAME));
        }
        conn.sendBuf = newbuff;
        CONN_stop_timer ();
        CONN_start_timer ();
        CNET_enable_application(ALLNODES);
    }

    // Suggested remaining steps:

    //	     - If rcvBuf[] has a sequence of frames whose messages can be
    //	       delivered in-order to the AL then process these frames by
    //	       delivering their messages to the AL.
    //	       Then left shift rcvBuf[] to replace the sequence of processed
    //	       frames with frames that follow the shifted out sequence.
    //	       Initialize rcvBuf[] frames whose contents have been relocated
    //	        during the shifting operation.
    //	     - Update 'rcvBase'
    // - If (the received frame is a data frame) send a cumulative ACK

    // - If (the received frame has an ACK number >= sendBase) then
    //       - assert (frame's ack number <= nextseqnum)
    //	     - left shift sendBuf[] and replace the sequence of ack'ed
    //	       frames with frames that follow the shifted out sequence.
    //	       Initialize frames in sendBuf[] whose contents have been relocated
    //	       during the shifting operation.
    //	     - Restart the timer, if needed
    //	     - Enable the AL to generate messages to the destination

    //  ....
    //  ....
}
// ------------------------------------------------------------

static EVENT_HANDLER(CONN_timeouts) 
{
    int		outLink= 1;
    FRAME	f;
    size_t	fLen;

    		// a timeout occurs only if (sendBase < nextseqnum)
    assert (conn.sendBase < conn.nextseqnum);

    // Retransmit the pkt at sendBase[0]. Re-pack the pkt because the 'ack'
    // number may have changed since the time we stored the frame.
    //
    printf("timeout\n");
    fLen= make_frame (&f, conn.sendBuf[0].dest, conn.sendBuf[0].seq,
    	              conn.rcvBase - 1, &(conn.sendBuf[0].msg),
		      conn.sendBuf[0].msgLen );
    CHECK ( CNET_write_physical (outLink, &f, &fLen)  );

    // restrat timer
    CONN_start_timer();
} 

// ------------------------------------------------------------

static EVENT_HANDLER(showstate)
{
    CONN_print();
}

// ------------------------------------------------------------

EVENT_HANDLER(reboot_node)
{
    int sendWin, rcvWin;

    if(nodeinfo.nodenumber > 1) {
	fprintf(stderr,"This is not a 2-node network!\n");
	exit(1);
    }

    if (nodeinfo.nodetype == NT_HOST) {
         CHECK(CNET_set_handler( EV_APPLICATIONREADY, application_ready, 0));
    }

    CHECK(CNET_set_handler( EV_PHYSICALREADY,    physical_ready, 0));
    CHECK(CNET_set_handler( EV_TIMER1,           CONN_timeouts, 0));

    CHECK(CNET_set_handler( EV_DEBUG0,           showstate, 0));
    CHECK(CNET_set_debug_string( EV_DEBUG0, "CONN State"));

    CNET_enable_application(ALLNODES);

    // connection specific initialization
    //
    // The timeout period follows the stop-and-wait.c protocol
    //
    CONN_timeout= sizeof(FRAME) * ( (CnetTime) 8000000 / linkinfo[1].bandwidth)
    	          + linkinfo[1].propagationdelay;
    CONN_timeout=  3 * CONN_timeout;
    
    sendWin = atoi (CNET_getvar ("sendWin")); assert (sendWin > 0);
    rcvWin  = atoi (CNET_getvar ("rcvWin"));  assert (rcvWin  > 0);

    CONN_init (sendWin, rcvWin);
}
