// Author:  Malhaar Karandikar
// Org:     Manchester Stingers
/*  
    This was used to test "dash.py". We create a named pipe and write known data to 
    We can test the functionality of the display by running both simultaneously. We 
    are using python for GUI because GUI development is faster in python and we can
    get a working prototype faster.
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define PIPE_PATH       "/tmp/can_data_pipe"

/// @brief 
/// @param 
struct testData{
    int rpm;
    int throttle;
    int battery;
    int something_else;
};

/// @brief Write data to the named pipe.
/// @param somedata Data structure containing all the data
void send_data_to_dash(struct testData * somedata){
    char buffer[200];
    memset(buffer, 0x00, 200);
    int written_size = sprintf(buffer,"%d,%d,%d,%d", somedata->rpm, somedata->battery, somedata->something_else, somedata->throttle);
    // Memory copy is more annoying in python. Therefore, we write string data and not raw data here
    // fflush(PIPE_PATH); //This is not needed with named pipes
    int fd = open(PIPE_PATH, O_WRONLY);
    write(fd , buffer, written_size);
    close(fd);
}

int generate_random(int l, int r) { //this will generate random number in range l and r
   int rand_num = (rand() % (r - l + 1)) + l;
    return rand_num;
}

int main(){

    mkfifo(PIPE_PATH, 0666);
    struct testData myData = {0,0,0,0}; 
    
    while(1){
            if(myData.battery >= 100){
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
                myData.rpm = 1000;
            }
            else{
                myData.rpm += 100;
            }
            send_data_to_dash(&myData); 
            usleep(50000);
    }

    return 0;

}