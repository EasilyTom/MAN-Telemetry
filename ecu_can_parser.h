#ifndef _ECU_CAN_PARSER_H_
#define _ECU_CAN_PARSER_H_

#include <stdio.h>
#include <stdint.h>
#define TARGET_CSV          "ecu_data.csv"
#define PRINT_AFTER         10


struct ECU{
  uint16_t RPM;
  float man_pres;
  uint16_t throttle;
  float cool_pres;
  float bat_volt;
  float boost_target;
  float baro_pres;
};

struct ECU myECU = {0,0,0,0,0,0,0};
int iter_num = 0;

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

int ecu_write_to_csv(void){
    int num;
    FILE *fptr;

    fptr = fopen(TARGET_CSV, "a");

    if(fptr == NULL){
        printf("Error opening CSV file");
        return 1;
    }
//    fprintf(fptr,"%.2f \n", myECU.throttle); //Broke it fix it later

    fclose(fptr);
    return 0;
}


void ecu_parse_and_print(uint16_t ID, char * frame, int frame_len){
    if(frame_len <= 8) return; //If the frame length is too small it will return
    
    
    switch (ID)
    {
    case 0x0360:
        myECU.RPM = frame[0]<<8|frame[1]; //This might need to be looked at once
        myECU.man_pres = (frame[2]<<8|frame[3])/10;
        myECU.throttle = (frame[4]<<8|frame[5]);
        myECU.cool_pres = ((frame[6]<<8|frame[7])/10) - 101.3; 
        break;
        // The order of the bytes might be in the reverse order. We want to write it\
        to a csv file too so we can graph it. 
    case 0x0372:
        myECU.bat_volt = (frame[0]<<8|frame[1])/10; //This too.
        break;
    default:
        // return;
        break;
    }
     
    if(iter_num == PRINT_AFTER){
        iter_num = 0;
            printf("\n Throttle: %u",myECU.throttle); 
	system("clear");
        //if(ecu_write_to_csv()){
         //   printf("Write operation failed");
        //}
    }
    else{
        iter_num ++;
    }
    return;
    
}



#endif
