#include <cnet.h>
#include <stdio.h>
unsigned short msg_1[]= {10,20,30,40,50,60}, msg_2[]= {60,10,20,30,40,50};
void main(){
  printf(CNET_IP_checksum(msg_1));
  printf(CNET_IP_checksum(msg_2));
}
main();
