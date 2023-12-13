/* 
    Author: Malhaar Karandikar
    Org:    Manchester Stingers
    Brief:  All functions in this file modifies the fucntionality of the original CAN module driver to work with
            a driver dash and a local data store.    
*/


#ifndef _ECU_CAN_PARSER_H_
#define _ECU_CAN_PARSER_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>           // Only for timing


#define TARGET_CSV          "ecu_data.csv"
#define PIPE_PATH           "/tmp/can_data_pipe"
#define PRINT_AFTER         10


struct ECU{
  uint16_t rpm;
  float man_pres;
  uint16_t throttle;
  float cool_pres;
  float battery;
  float boost_target;
  float baro_pres;
};

struct ECU myECU = {0,0,0,0,0,0,0};
int iter_num = 0;

/// @brief Write data to the named pipe.
/// @param somedata Data structure containing all the data
/// @return Nothing right now. Need to add error checks
void write_pipe(struct ECU * somedata){
    char buffer[200];
    memset(buffer, 0x00, 200);
    int written_size = sprintf(buffer,"%d,%f,%f,%d", somedata->rpm, somedata->battery, somedata->man_pres, somedata->throttle);
    // Memory copy is more annoying in python. Therefore, we write string data and not raw data here
    
    // fflush(PIPE_PATH); //This is not needed with named pipes
    int fd = open(PIPE_PATH, O_WRONLY);
    write(fd , buffer, written_size);
    close(fd);
}


/// @brief Empties the old written csv file
/// @param  
/// @return Returns 0 on success and 1 on failure
int init_csv_file(void){
    FILE *fptr;

    fptr = fopen(TARGET_CSV, "w+");
    if(fptr == NULL){
        printf("Error opening CSV file\n");
        return 1;
    }

    fprintf(fptr,"RPM, Battery volt, Throttle, Manifold pressure, Coolant pressure");
    fclose(fptr); 
    return 0;

}

/// @brief Writes received CAN data to a csv file
/// @param  
/// @return 0 on success and 1 on failure
int ecu_write_to_csv(void){
    int num;
    FILE *fptr;

    fptr = fopen(TARGET_CSV, "a");

    if(fptr == NULL){
        printf("Error opening CSV file");
        return 1;
    }
   fprintf(fptr,"%u,%.3f,%u,%.2f,%.2f \n", myECU.rpm, myECU.battery, myECU.throttle, myECU.man_pres, myECU.cool_pres); //Broke it fix it later

    fclose(fptr);
    return 0;
}


/// @brief Parses received can frames into 
/// @param ID Received CAN ID
/// @param frame Data bytes received on CAN
/// @param frame_len Length of received data 
void ecu_parse_and_print(uint16_t ID, char * frame, int frame_len){
    if(frame_len <= 8) return; //If the frame length is too small it will return
    
    
    switch (ID)
    {
    case 0x0360:
        myECU.rpm = frame[0]<<8|frame[1]; //This might need to be looked at once
        myECU.man_pres = (frame[2]<<8|frame[3])/10;
        myECU.throttle = (frame[4]<<8|frame[5]);
        myECU.cool_pres = ((frame[6]<<8|frame[7])/10) - 101.3; 
        break;
        // The order of the bytes might be in the reverse order. We want to write it\
        to a csv file too so we can graph it. 
    case 0x0372:
        myECU.battery = (frame[0]<<8|frame[1])/10; //This too.
        break;
    default:
        // return;
        break;
    }
     
    write_pipe(&myECU);
    if(iter_num == PRINT_AFTER){
        iter_num = 0;
            // printf("\nRPM: %u Throttle: %u \n Battery: %f",myECU.rpm,  myECU.throttle, myECU.battery); 
	        // system("clear");
        if(ecu_write_to_csv()){
           printf("Write operation failed");
        }
    }
    else{
        iter_num ++;
    }
    return;
    
}



#endif
