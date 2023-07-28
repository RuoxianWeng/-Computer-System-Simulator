/*
 Implementation of memory.
 Read the entire input file into an array before any fetching.
 Process CPU signals.
 
 Memory array: size of 2000
        -entries 0-999: user program
        -entries 1000-1999: system code
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constants
#define READ_SIGNAL 0
#define WRITE_SIGNAL 1
#define END_SIGNAL -1

//Function Prototypes
void readFile();
void readCpu();
void writeMemory();

void memory(char* filename, int fd_write, int fd_read) {

        int memory[2000];
        int signal;
        readFile(memory, filename);

        //read for signal until cpu ends
        while (1) {
                //read signal from cpu
                read(fd_read, &signal, sizeof(signal));

                if (signal == READ_SIGNAL) { //read address from cpu and send value at that address back
                        readCpu(fd_read, fd_write, memory);
                }

                else if (signal == WRITE_SIGNAL) { //read value from cpu and write to array
                        writeMemory(fd_read, memory);
                }

                else if (signal == END_SIGNAL) { //end the memory 
                        break;
                }

                else { //invalid signal 
                        printf("Invalid signal from cpu\n");
                        exit(-1);
                }
        }
}

//This function stores the whole input file into array
void readFile(int memory[], char* filename) {

        //open input file
        FILE* inFile = fopen(filename, "r");
        if (inFile == NULL) {
                printf("fopen failed.");
                exit(-1);
        }

        //read from file
        char line[1000];
        int i=0;
        while (fgets(line, 1000, inFile) != NULL) { //stop when end of file

                //check if load address need to be changed
                if (line[0] == '.') {
                        char newLine[100];
                        strncpy(newLine,&line[1],4);
                        sscanf(newLine, "%d", &i); //store new load address to i
                        continue;
                }

                //store to array and increment i
                if (sscanf(line, "%d", &memory[i]) == 1)
                        i++;
        }

        //close input file
        fclose(inFile);
}

//This function retrieves value at given address sent by the cpu
void readCpu(int fd_read, int fd_write, int memory[]) {

        int address, value;
        read(fd_read, &address, sizeof(address)); //read address
        value = memory[address]; //get value at that address
        write(fd_write, &value, sizeof(value)); //write value back
}

//This function stores given value at given address
void writeMemory(int fd_read, int memory[]) {

        int address, value;
        read(fd_read, &address, sizeof(address)); //read address
        read(fd_read, &value, sizeof(value)); //read value
        memory[address] = value; //store value at the address
}
