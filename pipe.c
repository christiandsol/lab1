#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUF_SIZE 100 


int main(int argc, char *argv[]) {
    int pipefd[argc - 1][2]; // pipe file descriptors
    char buffer[BUF_SIZE];    
    ssize_t bytes_read;
    if (pipe(pipefd[0]) < 0){ //create 1st pipe
        fprintf(stderr, "pipe error");
        return 1;
    }
    int rv = fork(); //1st child
    if (rv < 0) {
        fprintf(stderr, "fork failed\n");
        return 1;
    } else if (rv == 0) {//child
        close(pipefd[0][0]); //close read end
        dup2(pipefd[0][1],STDOUT_FILENO); //redirect stdout to write end of pipe 
        if (execlp(argv[1], argv[1], NULL) < 0){
            fprintf(stderr, "bogus argument");
            return 1;
        }
    } else { //parent
        close(pipefd[0][1]); //close write end
        dup2(pipefd[0][0], STDIN_FILENO); //redirect stdin to read end of pipe
        wait(NULL); //wait for child
    }


    //middle programs
    for (int i = 1; i < argc - 2; i++) {
        if (pipe(pipefd[i]) < 0){ //create ith pipe
            fprintf(stderr, "pipe error");
            return 1;
        }
        rv = fork();
        if (rv < 0) {
            fprintf(stderr, "fork failed\n");
            return 1;
        } else if (rv == 0) { //child
            //data is in read end of pipe i-1
            close(pipefd[i][0]); //close read end
            dup2(pipefd[i - 1][0], STDIN_FILENO); //redirect stdin to read end of pipe i-1
            dup2(pipefd[i][1],STDOUT_FILENO); //redirect stdout to write end of pipe 
            if (execlp(argv[i + 1], argv[i + 1], NULL) < 0){
                fprintf(stderr, "bogus argument");
                return 1;
            }
        } else { //parent
            close(pipefd[i][1]); //close write end
            dup2(pipefd[i][0], STDIN_FILENO); //redirect stdin to read end of pipe
            wait(NULL); //wait for child
        }
    }

    //parent last program
    if (pipe(pipefd[argc - 2]) < 0){ //create ith pipe
        fprintf(stderr, "pipe error");
        return 1;
    } 
    rv = fork();
    if (rv < 0) {
        fprintf(stderr, "fork failed\n");
        return 1;
    } else if (rv == 0) { //child
        // redirect previous pipe output to stdin
        close(pipefd[argc - 2][0]); //close read end
        dup2(pipefd[argc - 3][1], pipefd[argc - 2][0]);
        if(execlp(argv[argc - 1], argv[argc - 1], NULL) < 0){
            fprintf(stderr, "bogus argument");
            return 1;
        }
    } else { //parent
        close(pipefd[argc - 2][1]); //close write end
        close(pipefd[argc - 2][0]); //close read end
        wait(NULL); //wait for child
    }

    bytes_read = read(pipefd[argc -  2][0], &buffer, sizeof(buffer));
    write(STDOUT_FILENO, &buffer, bytes_read);
    return 0;
}
