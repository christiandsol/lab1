#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid:%d)\n", (int) getpid());
    int command_n = 1;
    char *command1 = argv[1];
    //command 1 printing 
    fprintf(stderr, "command1: %s\n", command1);
    int fork_rv = fork(); //return value: -1 if failure, 0 in child, PID in parent
    //handle error
    if (fork_rv < 0){
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (fork_rv == 0) {
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        //handle child
        printf("I am the child");
        execlp(command1, command1, NULL);
        //shouldn't be outputted, checking for errors
        printf("this shouldn't print");
    } else {
        printf("hello, I am parent(pid:%d)\n", (int) getpid());
        //parent
        //wait for child to finish
        int wait_rv = wait(NULL);
        if (wait_rv == -1) {
            printf("wait failed\n");
        }
    }
    return 0;
}
