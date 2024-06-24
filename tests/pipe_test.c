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
#include <assert.h>
#include "../ecu_can_parser.h"

#define NUM_TESTS       100

char * writeStr = "Test string";
char readStr[12];

int single_test(void){

    double timeTestOne = clock();
    if(write_to_pipe(writeStr, 11) != 0){
        printf("Pipe writing failed\n");
        return EXIT_FAILURE;
    }

    double timeTestTwo = clock();
    printf("Time to write: %f\n", (timeTestTwo - timeTestOne)/CLOCKS_PER_SEC);

    double timeTestThree = clock();
    if(read_from_pipe(readStr, 11) != 0){
        printf("Pipe reading failed\n");
        return EXIT_FAILURE;
    }

    double timeTestFour = clock();
    printf("Time to read: %f\n", (timeTestFour - timeTestThree)/CLOCKS_PER_SEC);
    return EXIT_SUCCESS;
}

int read_write_multiple_times(void){
    double totalTime = 0;   
    for (int i = 0; i < NUM_TESTS; i++) {
        double startTime = clock();
        if (write_to_pipe(writeStr, 11) != 0) {
            printf("Pipe writing failed\n");
            return EXIT_FAILURE;
        }
        if (read_from_pipe(readStr, 11) != 0) {
            printf("Pipe reading failed\n");
            return EXIT_FAILURE;
        }
        double endTime = clock();

        totalTime += (endTime - startTime) / CLOCKS_PER_SEC;
    }

    printf("Total time to read and write %d times: %f seconds\n", NUM_TESTS, totalTime);
    return EXIT_SUCCESS;
}



int main(){
    
    if(init_pipe() != 0){
        printf("Pipe initialization failed\n");
        return EXIT_FAILURE;
    }
    printf("Pipe initialized\n");

    if(single_test() != 0){
        printf("Single test failed\n");
        return EXIT_FAILURE;
    }


    if(read_write_multiple_times() != 0){
        printf("Multiple tests failed\n");
        return EXIT_FAILURE;
    }

    if(clean_pipe() != 0){
        printf("Pipe cleaning failed\n");
        return EXIT_FAILURE;
    }
    printf("Pipe cleaned\n");

    printf("Tests successful!\n");
    return 0;

}

