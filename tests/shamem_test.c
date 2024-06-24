#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include <assert.h>
#include "../ecu_can_parser.h"

#define NUM_TESTS 100

char * writeStr = "Test string";
char readStr[12];

int single_test(){

    double timeTestOne = clock();
    if(write_to_shared_memory(writeStr, 11) != 0){
        printf("write_to_shared_memory() failed\n");
        return EXIT_FAILURE;
    }

    double timeTestTwo = clock();
    if(read_from_shared_memory(readStr, 11) != 0){
        printf("read_from_shared_memory() failed\n");
        return EXIT_FAILURE;
    }

    double timeTestThree = clock();
    assert(strcmp(writeStr, readStr) == 0);
    printf("Time to write: %f\n Time to read: %f\n ", (timeTestTwo - timeTestOne)/CLOCKS_PER_SEC, (timeTestThree - timeTestTwo)/CLOCKS_PER_SEC);
    return 0;
}

int read_write_multiple_times() {
    double totalTime = 0;
    for (int i = 0; i < NUM_TESTS; i++) {
        double startTime = clock();
        if (write_to_shared_memory(writeStr, 11) != 0) {
            printf("write_to_shared_memory() failed\n");
            return EXIT_FAILURE;
        }
        if (read_from_shared_memory(readStr, 11) != 0) {
            printf("read_from_shared_memory() failed\n");
            return EXIT_FAILURE;
        }
        double endTime = clock();
        assert(strcmp(writeStr, readStr) == 0);
        totalTime += (endTime - startTime) / CLOCKS_PER_SEC;
    }
    printf("Total time to read and write %d times: %f seconds\n", NUM_TESTS, totalTime);
    return EXIT_SUCCESS;
}


int main(){
    // Test init_shared_memory


    if(init_shared_memory() != EXIT_SUCCESS){
        printf("init_shared_memory() failed\n");
        return EXIT_FAILURE;
    }

    // Test write_to_shared_memory
    if(single_test() != EXIT_SUCCESS){
        printf("single rw failed\n");
        return EXIT_FAILURE;
    }
    

    if(read_write_multiple_times() != EXIT_SUCCESS){
        printf("multiple rw failed\n");
        return EXIT_FAILURE;
    }


    if(clean_shared_memory() != EXIT_SUCCESS){
        printf("clean_shared_memory() failed\n");
        return EXIT_FAILURE;
    }   
    
    printf("All tests passed\n");
    return 0;
}
