#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

#include "parsers.h"


struct node {
   int codigo;
   int quantidade;
   int preco;
   struct node *next;
};

struct node *head = NULL;

char buffer[1024];
char venda[1024];

struct node* update_vendas(char *** data)
{
    if(head == NULL){
        struct node* nova_venda = (struct node*)malloc(sizeof(struct node));
        nova_venda->codigo = atoi((*data)[0]);
        nova_venda->quantidade = atoi((*data)[1]);
        nova_venda->preco = atoi((*data)[2]);

        head = nova_venda;
        return head;
    }

    struct node *cursor = head;
    while(cursor != NULL){
        if(atoi((*data)[0]) == cursor->codigo){
            cursor->quantidade += atoi((*data)[1]);
            cursor->preco += atoi((*data)[2]);
            return head;
        }else{
            if(cursor->next == NULL) break;
            cursor = cursor->next;
        }
    }
    
    struct node* nova_venda = (struct node*)malloc(sizeof(struct node));
    nova_venda->codigo = atoi((*data)[0]);
    nova_venda->quantidade = atoi((*data)[1]);
    nova_venda->preco = atoi((*data)[2]);

    cursor->next = nova_venda;
 
    return head;
}

void print_vendas()
{
    struct node* cursor = head;
    while(cursor != NULL)
    {
        memset(venda, 0, sizeof(venda));
        sprintf(venda, "%d %d %d\n", cursor->codigo, cursor->quantidade, cursor->preco);
        write(1, venda, strlen(venda));
        cursor = cursor->next;
    }
}


int main(int argc, char *argv[]) {

    char ** args = (char **) malloc(4 * sizeof(char*));
    int i = 0;
    while(1){
        int n = read(0, &buffer[i], 1);
        if(n<=0) break;
        if(buffer[0] == '\n') break;
        char c = buffer[i];
        if(c=='\n'){
            buffer[i] = '\0';
            i = 0;
            parseArtigo(&args, buffer);
            strtok(args[2], "\n");
            update_vendas(&args);
        }else{
            i++;
        }
    }
    print_vendas();

}