#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <semaphore.h>
#include <sys/mman.h>

#include "parsers.h"

int main(int argc, char *argv[]) {
    int client_to_server, server_to_client, n, nargs;
    ssize_t bufsize;
    char ** args = (char **) malloc(5 * sizeof(char*));
    char line[1024];
    char buffer[1024];
    
	char *fifo1 = "/tmp/client_to_server";

    char fifo2[35];
    sprintf(fifo2, "/tmp/server_to_client%d", (int)getpid());
    if(mkfifo(fifo2, 0666) == -1){
        perror ("Erro a criar server_to_client");
        exit(0);
    }

    while(1){
        if(readln(0, line) == -1){
            break;
        }
        
        nargs = parseCommand(&args, line);

        if(args[0] == NULL) continue;
        if(nargs != 1 && nargs != 2){
            printf("Comando inválido\n");
            continue;
        }
        sprintf(line, "%s %s\n", line, fifo2);

        client_to_server = open(fifo1, O_WRONLY);

        if (client_to_server == -1) {
            perror ("Erro a abrir fifo");
            break;
        }

        n = write(client_to_server, line, strlen(line) +1);

        if(n<=0){
          perror ("Erro a enviar comando ao servidor.");  
          continue;
        }

        server_to_client = open(fifo2, O_RDONLY);
        if (server_to_client == -1) {
            perror ("Erro a abrir fifo");
            break;
        }

        while(1){
            n = read(server_to_client, buffer, sizeof(buffer));
            if(n==0){
                break;
            }
            if(n == -1){
                perror ("Erro a ler resposta do servidor");
            break;
            }
        }
    
        close(server_to_client);

        printf("%s\n", buffer);

    }

    close(client_to_server);
    unlink(fifo2);
    free(args);
}
