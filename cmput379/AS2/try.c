#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
int main(){
    int fd=0;
    int flags;
    flags=fcntl(fd,F_GETFL);
    flags|=O_NONBLOCK;
    if(fcntl(fd,F_SETFL,flags)==-1){
        exit(1);
    }
    char buf[10];
    while(1){
        int num=read(fd,buf,sizeof(buf));
        for(int i=0;i<num;i++){
            if(buf[i]=='q'){
                exit(0);
            }
            printf("char:%c",buf[i]);
        }
        sleep(1);
        //printf("next\n");
    }
}
