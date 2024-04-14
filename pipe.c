#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    int command_n = 1;
    char *command_s = NULL;
    int pipefd[2]; // pipe file descriptors
    while (command_n < argc) {
        command_s = argv[command_n];
        fprintf(stderr, "commandn: %d\n", command_n);
        fprintf(stderr, "commands: %s\n", command_s);
        int pipe_val = pipe(pipefd); // create pipe
        if (pipe_val == -1) {
            fprintf(stderr, "pipe failed\n");
            exit(1);
        }
        int fork_rv = fork(); // return value: -1 if failure, 0 in child,
        // PID in parent
        // handle error
        if (fork_rv < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (fork_rv == 0) {
            // handle child
            printf("I am the child");
            close(pipefd[1]); // close write end
            execlp(command_s, command_s, NULL); // shouldn't be outputted, checking
            // nothing should print
        } else {
            printf("hello, I am parent(pid:%d)\n", (int)getpid());
            close(pipefd[0]); //close read end
            // wait for child to finish
            int wait_rv = wait(NULL);
            if (wait_rv == -1) {
                printf("wait failed\n");
            }
        }
        command_n++;
    } // command 1 printing
    return 0;
}
