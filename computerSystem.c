/*
 By Ruoxian Weng on 02/2023. 
 This program simulates a simple computer system consisting a CPU and Memory. In addition, the interrupt feature is supported.
 It uses fork() to create two separate processes and pipe() for interprocess communication. 

 Command line arguments: 
        -argv[1] = input filename
        -argv[2] = timer value for interrupt
*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

        int fd_memory[2]; //memory to cpu
        int fd_cpu[2]; //cpu to memory
        pid_t pid;

        //check pipe()
        if (pipe(fd_memory) != 0) {
                printf("The memory pipe failed.\n");
                exit(-1);
        }
        if (pipe(fd_cpu) != 0) {
                printf("The cpu pipe failed.\n");
                exit(-1);
        }

        pid = fork();
        if (pid < 0) { //fork() failed
                printf("The fork failed.\n");
                exit(-1);
        }
        else if (pid == 0) { //memory process goes here
                //close unused file descriptors
                close(fd_memory[0]); //no read from fd_memory
                close(fd_cpu[1]); //no write to fd_cpu
                memory(argv[1], fd_memory[1], fd_cpu[0]);
                exit(0);
        }
        else { //CPU process goes here
                //close unused file descriptors
                close(fd_cpu[0]); //no read from fd_cpu
                close(fd_memory[1]); //no write to fd_memory
                int timer = atoi(argv[2]);
                if (timer <= 0) {
                        printf("Invalid timer value.\n");
                        exit(-1);
                }
                cpu(timer, fd_memory[0], fd_cpu[1]);
                waitpid(pid, NULL, 0);
                exit(0);
        }
}
