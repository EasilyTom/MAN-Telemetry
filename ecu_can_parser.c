
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>           
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>


#include "ecu_can_parser.h"

#define TARGET_CSV          "ecu_data.csv"
#define PIPE_PATH           "/tmp/can_data_pipe"
#define TEST_PIPE_PATH      "/tmp/dump_test"
#define SHARMEMNAME         "/ecu_data"         // Name of the shared memory
#define SEMNAME             "/ecu_data_sem"     // ECU data semaphore
#define SHARMEMSIZE         256                 // Size of the shared memory


#define PRINT_AFTER         10



/////////////////////////////// VARIABLES ///////////////////////////////////

struct ECU myECU = {0,0,0,0,0,0,0,0,0};
int iter_num = 0;

key_t key;
int shmid;
char *shmPtr;

sem_t *ecuDataSem;
char *shmPtr;


//////////////////////////////// FUNCTIONS //////////////////////////////////

void send_data_to_dash(struct ECU * somedata){
    char buffer[256];
    memset(buffer, 0x00, 256);
    /*
    Need to send:
    - RPM
    - Battery 
    - Throttle
    - Vehicle speed
    - Oil pressure
    - Oil temperature
    - Coolant temperature
    - Fuel pressure
    - ECU Temperature
    - Gear
    */
    int written_size = sprintf(buffer,"%d,%f,%d,%d,%f,%f,%f,%f,%f,%d", 
    somedata->rpm, 
    somedata->battery,  
    somedata->throttle,
    somedata->speeeed,
    somedata->oil_pres,
    somedata->oil_temp,
    somedata->cool_temp,
    somedata->fuel_pres,
    somedata->ecu_temp,
    somedata->gear
    
    );
    // Memory copy is more annoying in python. Therefore, we write string data and not raw data here
    
    
    // write_to_pipe(buffer, written_size);
    write_to_shared_memory(buffer, written_size);

}

int write_to_pipe(char* buffer, int size){
    int fd = open(PIPE_PATH, O_WRONLY);
    if(fd == -1){
        return EXIT_FAILURE;
    }
    if(write(fd , buffer, size) == -1){
        return EXIT_FAILURE;
    }
    close(fd);
    return 0;
}

void write_pipe_test(void){
    char buffer[200];
    memset(buffer, 0x00, 200);
    int written_size = sprintf(buffer,"%d,%f,%f,%d", 666, 5.55, 4.44, 33);
    // Memory copy is more annoying in python. Therefore, we write string data and not raw data here
    
    // fflush(PIPE_PATH); //This is not needed with named pipes
    int fd = open(TEST_PIPE_PATH, O_WRONLY);
    write(fd , buffer, written_size);
    // fprintf(fd, "%d,%f,%f,%d", 666, 5.55, 4.44, 33);
    close(fd);
}


int init_shared_memory(void){
    // Creating the shared memory object
    int sharedMemory = shm_open(SHARMEMNAME, O_CREAT | O_RDWR, 0644);
    if(sharedMemory == -1){
        perror("Error creating shared memory object");
        return EXIT_FAILURE;
    }

    // Set the size of the shared memory object

    if(ftruncate(sharedMemory, SHARMEMSIZE) == -1){
        perror("Error setting size of shared memory object");
        return EXIT_FAILURE;
    }

    // Map the shared memory object
    shmPtr = mmap(0, SHARMEMSIZE, PROT_WRITE, MAP_SHARED, sharedMemory, 0);
    if(shmPtr == MAP_FAILED){
        perror("Error mapping shared memory object");
        return EXIT_FAILURE;
    }

    close(sharedMemory);

    ecuDataSem = sem_open(SEMNAME, O_CREAT, 0644, 1);
    if(ecuDataSem == SEM_FAILED){
        perror("Error creating semaphore");
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}


void write_to_shared_memory(char * buffer, int size){


    if(sem_wait(ecuDataSem) == -1){
        // Skips writing if semaphore cannot be acquired    
        // Can be replaced with sem_timedwait, which will wait for some time 
        // before giving up. If this breaks, use that instead.
        return;
    }
    

    strncpy(shmPtr, buffer, size);
    
    
    sem_post(ecuDataSem);
}

void clean_shared_memory(void){
    munmap(shmPtr, SHARMEMSIZE);
    sem_close(ecuDataSem);
    sem_unlink(SEMNAME);
    shm_unlink(SHARMEMNAME);

     if (munmap(shmPtr, SHARMEMSIZE) == -1) {
        perror("munmap");
    }

    // Close and unlink semaphore
    if (sem_close(ecuDataSem) == -1) {
        perror("sem_close");
    }
    // Removes the semaphore
    if (sem_unlink(SEMNAME) == -1) {
        perror("sem_unlink");
    }

    // Unlink shared memory
    if (shm_unlink(SHARMEMNAME) == -1) {
        perror("shm_unlink");
    }
}   


int init_csv_file(void){
    FILE *fptr;

    fptr = fopen(TARGET_CSV, "w+");
    if(fptr == NULL){
        printf("Error opening CSV file\n");
        return 1;
    }

    if(fprintf(fptr,"RPM, Battery volt, Throttle, Manifold pressure, Coolant pressure") < 0){
        printf("Writing broken\n");
    } else {
        printf("Writing success\n");
    }
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
   fprintf(fptr,"%u,%.3f,%u,%.2f,%.2f \n", myECU.rpm, myECU.battery, myECU.throttle, myECU.man_pres, myECU.cool_pres); //Broke it fix it later

    fclose(fptr);
    return 0;
}


void ecu_parse_and_print(uint16_t ID, char * frame, int frame_len){
    write_pipe_test();
    if(frame_len < 8) return; //If the frame length is too small it will return
    
    
    switch (ID)
    {
    case 0x0360:
        myECU.rpm = frame[0]<<8|frame[1]; //This might need to be looked at once
        myECU.man_pres = (frame[2]<<8|frame[3])/10;
        myECU.throttle = (frame[4]<<8|frame[5]);
        myECU.cool_pres = ((frame[6]<<8|frame[7])/10) - 101.3; 
        printf("COOLP: %f\n", myECU.cool_pres);
        break;
        // The order of the bytes might be in the reverse order. We want to write it\
        to a csv file too so we can graph it. 
    case 0x0361:
        // Fuel pressure [0:1] // Float // Added
        // Oil pressure [2:3] // Float // Added
        myECU.oil_pres = ((frame[2]<<8|frame[3])/10) - 101.3;
        myECU.fuel_pres = ((frame[0]<<8|frame[1])/10) - 101.3;
    break;

    case 0x0370:
        // Vehicle speed [0:1] // int // Added
        myECU.speeeed = (int)((frame[0]<<8|frame[1])/10);
    break;
    case 0x0372:
        myECU.battery = (frame[0]<<8|frame[1])/10; //This too.
        break;

    case 0x03E0:
        // Coolant temperature [0:1] // Float // Added
        // Oil temperature [6:7] // Float // Added
        myECU.cool_temp = ((frame[0]<<8|frame[1])/10);
        myECU.oil_temp = ((frame[6]<<8|frame[7])/10);
        break;

    case 0x0469:
        // ecu_temperature [0:1] // Float  // Added
        myECU.ecu_temp = ((frame[0]<<8|frame[1])/10); 
        break;

    case 0x0470:
        myECU.gear = frame[7]; // enum lol // Added
        break;

    // case 0x0471:
    //     //throttle [2:3] // Float // Added
    //     myECU.throttle = ((frame[6]<<8|frame[7])/10);
    //     break;

    default:
        // return;
        break;
    }
     
    send_data_to_dash(&myECU);
    
    return;
    
}
