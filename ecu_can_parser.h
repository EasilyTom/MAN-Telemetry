/* 
    Author: Malhaar Karandikar
    Org:    Manchester Stingers
    Brief:  All functions in this file modifies the fucntionality of the original CAN module driver to work with
            a driver dash and a local data store.    
*/


#ifndef _ECU_CAN_PARSER_H_
#define _ECU_CAN_PARSER_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <pthread.h>


struct ECU{
  
  uint16_t rpm;
  float man_pres;
  uint16_t throttle;
  float cool_pres;
  float battery;
  float boost_target;
  float baro_pres;
  uint8_t gear;
  float ecu_temp;
  float cool_temp;
  int speeeed;
  float oil_temp;
  float oil_pres;
  float fuel_pres;
  
};


/* Function prototypes */
/// @brief Write data to the named pipe.
/// @param somedata Data structure containing all the data
/// @return Nothing right now. Need to add error checks
void send_data_to_dash(struct ECU * somedata);

/// @brief Test function to write data to the named pipe
/// @param  
void write_pipe_test(void);

/// @brief Initializes the named pipe
/// @return 0 on success and 1 on failure
int init_pipe(void);

/// @brief Writes data to the named pipe.
/// @param buffer Sting to be written to the shared memory
/// @param size Size of the string
/// @return 0 on Success and 1 on failure
int write_to_pipe(char* buffer, int size);

/// @brief Reads data from the named pipe
/// @param buffer buffer to read the memory to
/// @param size Size of data to be read
/// @return 0 on success and 1 on failure
int read_from_pipe(char* buffer, int size);

/// @brief Cleans the named pipe
/// @return 0 on success and 1 on failure
int clean_pipe(void);

/// @brief Creates semaphore and shared memory instances to write to 
/// @return 0 on success and 1 on failure
int init_shared_memory(void);

/// @brief Instead of writing to a named pipe, write to shared memory. This should be faster than the 
///        named pipe. It will not write to the shared mempry if it cannot acquire the semaphore needed
/// @param buffer Sting to be written to the shared memory
/// @param size Size of the string
/// @return 0 on success and 1 on a failure
int write_to_shared_memory(char * buffer, int size);

/// @brief Reads data from the shared memory
/// @param buffer buffer to read the memory to
/// @param size Size of data to be read
/// @return 0 on success and 1 on failure
int read_from_shared_memory(char * buffer, int size);

/// @brief Empties the old written csv file or creates one if it does not exist
/// @param  
/// @return Returns 0 on success and 1 on failure
int init_csv_file(void);

/// @brief Writes received CAN data to a csv file
/// @param  
/// @return 0 on success and 1 on failure
int ecu_write_to_csv(void);

/// @brief Parses received can frames into the data structure
/// @param ID Received CAN ID
/// @param frame Data bytes received on CAN
/// @param frame_len Length of received data 
void ecu_parse_and_print(uint16_t ID, char * frame, int frame_len);

/// @brief Cleans shared memory and associated semaphores
/// @param  
int clean_shared_memory(void);


#endif
