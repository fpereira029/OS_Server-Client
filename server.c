#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#include "parsers.h"

volatile int reading = 0;
volatile int run = 1;
pid_t pid;

void term_handler(){
    kill(pid, SIGTERM);
    if(reading == 0){
        exit(0);
    }else{
        run = 0;
    }
}

char * getFormatedTime(){
    time_t timer;
    char *formatedTime = malloc (sizeof (char) * 28);
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(formatedTime, 28, "%Y-%m-%dT%H:%M:%S", tm_info);

    return formatedTime;
}

int code_parser(char **args){
    int stocks, artigos, server_to_client, line_pos;
    char buffer[1024];
    char envio[1024];
    char stock[1024];
    char ** args2 = (char **) malloc(4 * sizeof(char*));

    server_to_client = open(args[1], O_WRONLY);
    if (server_to_client == -1) {
        free(args2);
        perror ("Erro a abrir fifo server_to_client");
        return -1;
    }

    if(!(isNumber(args[0]))){
        sprintf(envio, "Codigo invalido.");
        write(server_to_client, envio, strlen(envio) +1);
        close(server_to_client);
        free(args2);
        return -1;
    }

    stocks = open("stocks.txt", O_RDONLY, 0666);

    line_pos = atoi(args[0]) * 22;
    if(lseek(stocks, line_pos, SEEK_SET) >= lseek(stocks, 0, SEEK_END)){
        strcpy(envio, "Erro: Artigo inexistente.");
        write(server_to_client, envio, strlen(envio) +1);
        close(server_to_client);
        close(stocks);
        free(args2);
        return -1;
    }
    lseek(stocks, line_pos, SEEK_SET);

    readln(stocks, buffer);
    parseArtigo(&args2, buffer);
    strtok(args2[1], "\n");
    strcpy(stock, args2[1]);

    close(stocks);

    artigos = open("artigos.txt", O_RDONLY, 0666);

    line_pos = atoi(args[0]) * 33;
    lseek(artigos, line_pos, SEEK_SET);

    readln(stocks, buffer);
    parseArtigo(&args2, buffer);
    strtok(args2[1], "\n");

    sprintf(envio, "Stock existente: %d Preço: %d", atoi(stock), atoi(args2[1]));

    close(artigos);

    write(server_to_client, envio, strlen(envio) +1);
    close(server_to_client);

    free(args2);
    
    return 1;
}

int atualizar_stock(char **args){
    int stocks, vendas, artigos, server_to_client, new_stock;
    int line_pos, price;
    char buffer[1024];
    char envio[1024];
    char new_str[1024];
    char ** args2 = (char **) malloc(4 * sizeof(char*));

    server_to_client = open(args[2], O_WRONLY);
    if (server_to_client == -1) {
        free(args2);
        perror ("Erro a abrir fifo server_to_client");
        return -1;
    }

    if(!(isNumber(args[1]) || !(isNumber(args[0])))){
        sprintf(envio, "Alteração inválida.");
        write(server_to_client, envio, strlen(envio) +1);
        close(server_to_client);
        free(args2);
        return -1;
    }

    artigos = open("artigos.txt", O_RDWR, 0666);

    line_pos = atoi(args[0]) * 33;
    if(lseek(artigos, line_pos, SEEK_SET) >= lseek(artigos, 0, SEEK_END)){
        strcpy(envio, "Erro: Artigo inexistente.");
        write(server_to_client, envio, strlen(envio) +1);
        close(server_to_client);
        close(artigos);
        free(args2);
        return -1;
    }

    lseek(artigos, line_pos, SEEK_SET);

    readln(artigos, buffer);
    parseArtigo(&args2, buffer);
    price = atoi(args2[2]);

    close(artigos);


    stocks = open("stocks.txt", O_RDWR, 0666);

    line_pos = atoi(args[0]) * 22;
    lseek(stocks, line_pos, SEEK_SET);

    readln(stocks, buffer);
    parseArtigo(&args2, buffer);

    lseek(stocks, line_pos + 11, SEEK_SET);

    new_stock = atoi(args2[1]) + atoi(args[1]);
    if(new_stock >= 0){
        sprintf(new_str, "%010d\n", new_stock);
        sprintf(envio, "Stock atual do artigo: %d", new_stock);

        write(stocks, new_str, strlen(new_str));

        if(atoi(args[1]) <0){   //Caso de venda
            vendas = open("vendas.txt", O_WRONLY | O_APPEND| O_CREAT, 0666);
            if (lseek(vendas, 0, SEEK_END) == 0) write(vendas, "0000000011\n", 11);

            sprintf(new_str, "%s %d %d\n", args[0], abs(atoi(args[1])), abs(atoi(args[1])) * price);
            write(vendas, new_str, strlen(new_str));
            close(vendas);
        }
    }else{
        strcpy(envio, "Stock insuficiente");
    }

    close(stocks);

    write(server_to_client, envio, strlen(envio) +1);
    close(server_to_client);

    free(args2);

    return 1;
}


void agregate_listener(){
    char line[1024];
    int fd[2], fd2[2], vendas;
    int n, max_read, total_read, min_read, n_processes;
    char *formartedTime = NULL;
    char buffer[2048];          //Valor adequado??????
    char new_str[1024];
    char ** args2 = (char **) malloc(4 * sizeof(char*));

    while(1){
        readln(0, line);
        if(strcmp(line, "agregar")){
            printf("Comando inválido.\n");
            continue;
        }

        vendas = open("vendas.txt", O_RDWR, 0666);
        if(vendas == -1){
            perror("Ficheiro vendas inexistente.");
            continue;
        }
        max_read = lseek(vendas, 0, SEEK_END);
        total_read = max_read;
        lseek(vendas, 0, SEEK_SET);

        readln(vendas, buffer);
        parseArtigo(&args2, buffer);
        min_read = atoi(args2[0]);

        max_read -= min_read;

        lseek(vendas, min_read, SEEK_SET);
        //clock_t t;          debug
        //t = clock();       debug

        pipe(fd2);

        if(!fork()){
            close(fd2[0]);
            n_processes =0;

            while(1){
                pipe(fd);
                n_processes++;

                if(!fork()){
                    close(vendas);
                    close(fd[1]);
                    dup2(fd[0], 0);
                    close(fd[0]);

                    dup2(fd2[1], 1);
                   
                    execlp("./ag", "./ag", (char *) 0);
                    _exit(0);
                }

                close(fd[0]);
                if(max_read <= sizeof(buffer)){
                    n = read(vendas, buffer, max_read);
                    write(fd[1], buffer, n);
                    break;
                }else{
                    n = read(vendas, buffer, sizeof(buffer));
                    max_read -= n;
                    write(fd[1], buffer, n);
                    n = readln(vendas,buffer);
                    write(fd[1], buffer, n),
                    write(fd[1], "\n", 1);
                    max_read -= n+1;
                }
                
                close(fd[1]);

            }
            for(int i=1; i<n_processes; i++){
                wait(0);
            }
            close(fd2[1]);
            _exit(0);
        }
        close(fd2[1]);

        pipe(fd);        

        if(!fork()){
            close(vendas);
            close(fd2[0]);
            close(fd[1]);
            dup2(fd[0], 0);
            close(fd[0]);

            formartedTime =  getFormatedTime();
            int stdout = open(formartedTime, O_WRONLY | O_TRUNC | O_CREAT, 0666);
            free(formartedTime);
            dup2(stdout,1);
            close(stdout);
            
            execlp("./ag", "./ag", (char *) 0);
            _exit(0);
        }

        close(fd[0]);
        while(1){
            n = read(fd2[0], buffer, sizeof(buffer));
            if(n<=0){
                break;
            }
            write(fd[1], buffer, n);
        }
        close(fd[1]);
        close(fd2[0]);

        lseek(vendas, 0, SEEK_SET);
        sprintf(new_str, "%010d\n", total_read);
        write(vendas, new_str, strlen(new_str));

        //t = clock() - t;         debug
        //double time_taken = ((double)t)/CLOCKS_PER_SEC;  debug
    
        //printf("agregate took %f seconds to execute \n", time_taken);  debug

        close(vendas);
        wait(0);

    }

    free(args2);
}


int main(int argc, char *argv[]) {
    int client_to_server, n, nargs;
    int i;
    char ** args = (char **) malloc(5 * sizeof(char*));
    char buffer[2048];

    signal(SIGTERM, term_handler);

    pid = fork();
    if(!pid){
        agregate_listener();
        _exit(0);
    }

	char *fifo1 = "/tmp/client_to_server";
    mkfifo(fifo1, 0666);

    client_to_server = open(fifo1, O_RDONLY);
    if (client_to_server == -1) {
        perror ("Erro a abrir fifo client_to_server");
        exit(0);
    }
    
    while (run){
        
        i = 0;
        int end = 0;
        reading = 0;
        while(1){
            n = read(client_to_server, &buffer[i], 1);
            reading = 1;
            if(end == 1){
                buffer[i-1] = '\0';
                break;
            }
            if(n<=0) break;
            char c = buffer[i];
            if(c=='\n'){
                end = 1;
            }
            i++;
        }
        
        if(n<=0){
          continue;  
        }
        
        nargs = parseCommand(&args, buffer);

        if(nargs == 2){
            code_parser(args);

        }

        if(nargs == 3){
            atualizar_stock(args);

        }

	}
    //unlink(fifo1); Em caso de crash do servidor, não se irão perder os pedidos dos clientes. 
    //Porém o fifo pode ser apagado manualmente se desejar cancelar os pedidos dos clientes.
    close(client_to_server);
    free(args);
}
