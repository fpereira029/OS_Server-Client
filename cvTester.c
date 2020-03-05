#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#define n_processes 20

int main(int argc, char *argv[]) {
	
	int i, stdin, stdout;
    int pid;

    remove("cvOutput.txt");

    for(i=0; i<n_processes; i++){
        pid = fork();

        if(!pid){
            stdin = open("cvInput2.txt", O_RDONLY);
            dup2(stdin, 0);

            stdout = open("cvOutput.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
            dup2(stdout,1); 

            close(stdin);
            close(stdout);
            
            execlp("./cv", "./cv", (char *)NULL);
        }
    }

    for(i=0; i<n_processes; i++){
        wait(0);
    }
}
