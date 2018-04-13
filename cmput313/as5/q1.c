#include <stdio.h>
#include <stdlib.h>

int main(){
    int X[100];
    int N = 30;
    int node[N];
    double d;
    int count,countx;
    double p = 0.5;
    int total = 0;
    for(int i = 1; i <= 100; i++){
        countx = 0;
        for (int j = 1; j <= 10000; j++){
            count = 0;
            for(int k = 1; k <= N; k++){
                
                d = (double)rand() / (double)RAND_MAX ;
                //printf("%f\n",d);
                if (d < p){
                    //printf("%d\n",count);
                    count = count+1;
                }
            }
            if (count < 2 && countx >= 1){
                break;
            }
            else{
                //printf("%d\n",countx);
                countx = countx+1;
            }
        }
        X[i] = countx;
        total = total + countx;
    }
    d = 1.0*total/100;
    printf("average x[n]: %f\n",d);
}