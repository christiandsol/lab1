#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 100 


int main(int argc, char *argv[]) {
    if (argc == 1){
        exit(EINVAL);
    }
    int pipefd[argc - 1][2]; // pipe file descriptors
    char buffer[BUF_SIZE];    
    ssize_t bytes_read;
    if (pipe(pipefd[0]) < 0){ //create 1st pipe
        fprintf(stderr, "pipe error");
        exit(1);
    }
    int rv = fork(); //1st child
    if (rv < 0) {
        fprintf(stderr, "fork failed\n");
        exit(errno);
    } else if (rv == 0) {//child
        close(pipefd[0][0]); //close read end
        dup2(pipefd[0][1],STDOUT_FILENO); //redirect stdout to write end of pipe 
        if (execlp(argv[1], argv[1], NULL) < 0){
            fprintf(stderr, "bogus argument");
            exit(errno);
        }
    } else { //parent
        close(pipefd[0][1]); //close write end
        dup2(pipefd[0][0], STDIN_FILENO); //redirect stdin to read end of pipe
        int status;
        wait(&status); //wait for child
        if (WEXITSTATUS(status) != 0) {
            fprintf(stderr, "child exited with status %d\n", WEXITSTATUS(status));
            exit(errno);
        }
    }


    //middle programs
    for (int i = 1; i < argc - 2; i++) {
        if (pipe(pipefd[i]) < 0){ //create ith pipe
            fprintf(stderr, "pipe error");
            exit(errno);
        }
        rv = fork();
        if (rv < 0) {
            fprintf(stderr, "fork failed\n");
            exit(errno);
        } else if (rv == 0) { //child
            //data is in read end of pipe i-1
            close(pipefd[i][0]); //close read end
            dup2(pipefd[i - 1][0], STDIN_FILENO); //redirect stdin to read end of pipe i-1
            dup2(pipefd[i][1],STDOUT_FILENO); //redirect stdout to write end of pipe 
            if (execlp(argv[i + 1], argv[i + 1], NULL) < 0){
                fprintf(stderr, "bogus argument");
                exit(errno);
            }
        } else { //parent
            close(pipefd[i][1]); //close write end
            dup2(pipefd[i][0], STDIN_FILENO); //redirect stdin to read end of pipe
            int status;
            wait(&status); //wait for child
            if (WEXITSTATUS(status) != 0) {
                printf("child exited with signal %d\n", WEXITSTATUS(status));
                exit(errno);
            }
        }
    }

    //parent last program
    if (pipe(pipefd[argc - 2]) < 0){ //create ith pipe
        fprintf(stderr, "pipe error");
        exit(errno);
    } 
    rv = fork();
    if (rv < 0) {
        fprintf(stderr, "fork failed\n");
        exit(errno);
    } else if (rv == 0) { //child
        // redirect previous pipe output to stdin
        close(pipefd[argc - 2][0]); //close read end
        dup2(pipefd[argc - 3][1], pipefd[argc - 2][0]);
        if(execlp(argv[argc - 1], argv[argc - 1], NULL) < 0){
            fprintf(stderr, "bogus argument");
            exit(errno);
        }
    } else { //parent
        close(pipefd[argc - 2][1]); //close write end
        close(pipefd[argc - 2][0]); //close read end
        int status;
        wait(&status); //wait for child
        if (WEXITSTATUS(status) != 0) {
            printf("child exited with signal %d\n", WEXITSTATUS(status));
            exit(errno);
        }
    }
    bytes_read = read(pipefd[argc -  2][0], &buffer, sizeof(buffer));
    write(STDOUT_FILENO, &buffer, bytes_read);
    return 0;
}

