// Author:  Malhaar Karandikar
// Org:     Manchester Stingers
/*  
    This was used to test "dash.py". We create a named pipe and write known data to 
    We can test the functionality of the display by running both simultaneously. We 
    are using python for GUI because GUI development is faster in python and we can
    get a working prototype faster.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "../ecu_can_parser.h"

#define PIPE_PATH       "/tmp/can_data_pipe"


int generate_random(int l, int r) { //this will generate random number in range l and r
   int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num;
}

int main(){

    mkfifo(PIPE_PATH, 0666);
    struct ECU myData = {0,0,0,0,0,0,0,0,0}; 
    
    while(1){
            if(myData.battery >= 13){
                myData.battery = 0;
            }
            else{
                myData.battery ++;
            }
            if(myData.throttle >= 100){
                myData.throttle = 0;
            }
            else{
                myData.throttle ++;
            }
            if(myData.rpm >= 10000){
                myData.rpm -= 6000;
            }
            else{
                myData.rpm += 1000;
            }
            write_pipe(&myData); 
            usleep(50000);
    }

    return 0;

}