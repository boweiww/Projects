#include <stdio.h>
#include <cnet.h>
unsigned short msg_1[]= {10,20,30,40,50,60}, msg_2[]= {60,10,20,30,40,50};
int32_t  CNET_IP_checksum(*msg_1, 6);
int32_t  CNET_IP_checksum(*msg_2, 6);