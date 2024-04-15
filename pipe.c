#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 100 


int main(int argc, char *argv[]) {
    int pipefd[argc - 1][2]; // pipe file descriptors
    if (pipe(pipefd[0]) < 0){ //create 1st pipe
        fprintf(stderr, "pipe error");
        exit(1);
    }
    int rv = fork(); //1st child
    if (rv < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rv == 0) {//child
        close(pipefd[0][0]); //close read end
        dup2(pipefd[0][1],STDOUT_FILENO); //redirect stdout to write end of pipe 
        printf("Process child id: %d\n", getpid());
        execlp(argv[1], argv[1], NULL);
    } else { //parent
        close(pipefd[0][1]); //close write end
        dup2(pipefd[0][0], STDIN_FILENO); //redirect stdin to read end of pipe
        wait(NULL); //wait for child
        char buffer[BUF_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0][0], buffer, sizeof(buffer))) > 0) {
            // Write the read data to the parent's standard output
            write(STDOUT_FILENO, buffer, bytesRead);
        }
    }
    // if (pipe(pipefd[argc - 1]) < 0){ //create ith pipe
    //     fprintf(stderr, "pipe error");
    //     exit(1);
    // } 
    // printf("Process parent id: %d\n", getpid());
    // printf("here");
    // //first thing done
    // rv = fork();
    // if (rv < 0) {
    //     fprintf(stderr, "fork failed\n");
    //     exit(1);
    // } else if (rv == 0) { //child
    //     dup2(pipefd[argc - 2][1], pipefd[argc - 1][0]);
    //     execlp(argv[argc - 1], argv[argc - 1], NULL);
    // } else {    
    //     close(pipefd[argc - 1][1]); //close write end
    //     close(pipefd[argc - 1][0]); //close read end
    //     wait(NULL); //wait for child
    // }
    // return 0;
}
