/*
 Implementation of CPU. 
 Fetch and execute instructions until the program ends.
 Registers: PC, SP, IR, AC, X, Y
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Constants
#define USER_MODE 0
#define KERNEL_MODE 1
#define READ_SIGNAL 0
#define WRITE_SIGNAL 1  
#define END_SIGNAL -1

//Function Prototypes
int readFromMemory();
void writeToMemory();
void execute();

//Global Variables 
int PC = 0;
int IR = 0;
int SP = 1000;
int AC = 0;
int X = 0;
int Y = 0;
int mode = USER_MODE;
int signalNum = 2;

void cpu(int timer, int fd_read, int fd_write) {

        int instructionCount=0;

        //loop for fetch and execute
        while (1) {
                IR = readFromMemory(PC, fd_read, fd_write); //fetch instruction to IR
                PC++;
                execute(fd_read, fd_write); //execute instruction
                instructionCount++;
                if (instructionCount >= timer && mode != KERNEL_MODE) { //timer interrupt processing 
                        mode = KERNEL_MODE;
                        int userSP = SP;
                        SP = 1999; //beginning of system stack
                        writeToMemory(SP, userSP, fd_write); //push user SP to stack
                        SP--;
                        writeToMemory(SP, PC, fd_write); //push return PC to stack
                        PC = 1000;
                        instructionCount = 0; //reset count
                }
        }
}

//This function makes the memory to send the value at the given address and return it
int readFromMemory(int address, int fd_read, int fd_write) {

        int value;

        if (mode == USER_MODE && address > 999) { //check memory violation in user mode
                printf("Memory violation: accessing system address %d in user mode\n",address);
                exit(-1);
        }

        signalNum = READ_SIGNAL;
        write(fd_write, &signalNum, sizeof(signalNum)); //send READ signal 
        write(fd_write, &address, sizeof(address)); //send address
        read(fd_read, &value, sizeof(value)); //read value at the address
        return value;
}

//This function makes the memory to store given value at given address
void writeToMemory(int address, int value, int fd_write) {

        if (mode == USER_MODE && address > 999) { //check memory violation in user mode
                printf("Memory violation: accessing system address %d in user mode\n",address);
                exit(-1);
        }

        signalNum = WRITE_SIGNAL;
        write(fd_write, &signalNum, sizeof(signalNum)); //send WRITE signal
        write(fd_write, &address, sizeof(address)); //send address
        write(fd_write, &value, sizeof(value)); //send value
}

//This function executes the instruction fetched into IR
void execute(int fd_read, int fd_write) {

        int value=0;

        switch (IR) {
                case 1: //load value
                        AC = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        break;
                case 2: //load addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        AC = readFromMemory(value, fd_read, fd_write);
                        break;
                case 3: //loadInd addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        value = readFromMemory(value, fd_read, fd_write);
                        AC = readFromMemory(value, fd_read, fd_write);
                        break;
                case 4: //loadIdxX addr
                        value = readFromMemory(PC, fd_read, fd_write) + X;
                        PC++;
                        AC = readFromMemory(value, fd_read, fd_write);
                        break;
                case 5: //loadIdxY addr
                        value = readFromMemory(PC, fd_read, fd_write) + Y;
                        PC++;
                        AC = readFromMemory(value, fd_read, fd_write);
                        break;
                case 6: //loadSpX
                        AC = readFromMemory(SP+X, fd_read, fd_write);
                        break;
                case 7: //store addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        writeToMemory(value, AC, fd_write);
                        break;
                case 8: //get
                        srand(time(NULL));
                        AC = rand() % 100 + 1;
                        break;
                case 9: //put port
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        if (value == 1)
                                printf("%d", AC);
                        else if (value == 2)
                                printf("%c", AC);
                        else
                                printf("invalid port number.\n");
                        break;
                case 10: //addX
                        AC += X;
                        break;
                case 11: //addY
                        AC += Y;
                        break;
                case 12: //subX
                        AC -= X;
                        break;
                case 13: //subY
                        AC -= Y;
                        break;
                case 14: //copyToX
                        X = AC;
                        break;
                case 15: //copyFromX
                        AC = X;
                        break;
                case 16: //copyToY
                        Y = AC;
                        break;
                case 17: //copyFromY
                        AC = Y;
                        break;
                case 18: //copyToSp
                        SP = AC;
                        break;
                case 19: //copyFromSp
                        AC = SP;
                        break;
                case 20: //jump addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC = value;
                        break;
                case 21: //jumpIfEqual addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        if (AC == 0)
                                PC = value;
                        break;
                case 22: //jumpIfNotEqual addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        if (AC != 0)
                                PC = value;
                        break;
                case 23: //call addr
                        value = readFromMemory(PC, fd_read, fd_write);
                        PC++;
                        SP--;
                        writeToMemory(SP, PC, fd_write);
                        PC = value;
                        break;
                case 24: //ret
                        PC = readFromMemory(SP, fd_read, fd_write);
                        SP++;
                        break;
                case 25: //incX
                        X++;
                        break;
                case 26: //decX
                        X--;
                        break;
                case 27: //push
                        SP--;
                        writeToMemory(SP, AC, fd_write);
                        break;
                case 28: //pop
                        AC = readFromMemory(SP, fd_read, fd_write);
                        SP++;
                        break;
                case 29: //int
                        if (mode == KERNEL_MODE)
                                break;
                        mode = KERNEL_MODE;
                        int userSP = SP;
                        SP = 1999; //beginning of system stack
                        writeToMemory(SP, userSP, fd_write); //push user SP to stack
                        SP--;
                        writeToMemory(SP, PC, fd_write); //push return PC to stack
                        PC = 1500;
                        break;
                case 30: //iRet
                        PC = readFromMemory(SP, fd_read, fd_write); //pop return PC
                        SP++;
                        SP = readFromMemory(SP, fd_read, fd_write); //pop user SP
                        mode = USER_MODE;
                        break;
                case 50: //end
                        signalNum = END_SIGNAL;
                        write(fd_write, &signalNum, sizeof(signalNum));
                        exit(0);
                default:
                        printf("invalid instruction.\n");
                        exit(-1);
        }
}
