#include <unistd.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "parsers.h"

int inserir_artigo(char** args){
  int artigos, strings, stocks;
  int new_code, string_pos;
  char new_str[1024];


  artigos = open("artigos.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
  strings = open("strings.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);
  stocks = open("stocks.txt", O_WRONLY | O_APPEND | O_CREAT, 0666);

  if (artigos == -1 || strings == -1) {
    perror("Erro a abrir ficheiro.");
    return -1;
  }

  new_code = lseek(artigos, 0, SEEK_END);
  string_pos = lseek(strings, 0, SEEK_END);

  new_code /= 33;

  sprintf(new_str, "%s\n", args[1]);
  write(strings, new_str, strlen(new_str));

  sprintf(new_str, "%010d %010d %010d\n", new_code, string_pos, atoi(args[2]));
  write(artigos, new_str, strlen(new_str));

  sprintf(new_str, "%010d 0000000000\n", new_code);
  write(stocks, new_str, strlen(new_str));

  printf("Artigo inserido com o código: %d\n", new_code); 

  close(strings);
  close(stocks);
  close(artigos);

  return 1;
}


int mudar_nome(char **args){

  int artigos, strings;
  int string_pos, line_pos;
  char new_str[1024];

  artigos = open("artigos.txt", O_WRONLY, 0666);
  strings = open("strings.txt", O_WRONLY | O_APPEND, 0666);

  if (artigos == -1 || strings == -1) {
    perror("Erro a abrir ficheiro.");
    return -1;
  }

  line_pos = atoi(args[1]) * 33;
  if(lseek(artigos, line_pos, SEEK_SET) >= lseek(artigos, 0, SEEK_END)){
    printf("Erro: Artigo inexistente.\n");
    close(artigos);
    close(strings);
    return -1;
  }
  lseek(artigos, line_pos + 11, SEEK_SET);

  string_pos = lseek(strings, 0, SEEK_END);

  sprintf(new_str, "%s\n", args[2]);
  write(strings, new_str, strlen(new_str));


  sprintf(new_str, "%010d", string_pos);
  write(artigos, new_str, strlen(new_str));
  
  close(strings);
  close(artigos);

  printf("Nome do artigo alterado com sucesso.\n");


  return 1;
}


int mudar_preco(char **args){

  int artigos;
  int line_pos;
  char new_str[1024];

  artigos = open("artigos.txt", O_WRONLY, 0666);

  if (artigos == -1) {
    perror("Erro a abrir ficheiro.");
    return -1;
  }

  line_pos = atoi(args[1]) * 33;
  if(lseek(artigos, line_pos, SEEK_SET) >= lseek(artigos, 0, SEEK_END)){
    printf("Erro: Artigo inexistente.\n");
    close(artigos);
    return -1;
  }
  lseek(artigos, line_pos + 22, SEEK_SET);

  sprintf(new_str, "%010d", atoi(args[2]));
  write(artigos, new_str, strlen(new_str));

  printf("Preço do artigo alterado com sucesso.\n");

  close(artigos);

  return 1;
}



int main (int argc, char *argv[]){
	int nargs;
  char ** args = (char **) malloc(5 * sizeof(char*));
	char line[1024];
  ssize_t bufsize;
	
	do{
    
    if(readln(0, line) == -1){
      break;
    }

    nargs = parseCommand(&args, line);
    if(args[0] == NULL) continue;    

    if(!strcmp(args[0], "i")){

      if(nargs!=3){
        printf("Erro: número de argumentos incorreto.\n");
        continue;
      }

      if(!isNumber(strtok(args[2], "\n"))){
        printf("Preço inválido.\n");
        continue;
      }

      inserir_artigo(args);
    }

    else if(!strcmp(args[0], "n")){

      if(nargs!=3){
        printf("Erro: número de argumentos incorreto.\n");
        continue;
      }

      mudar_nome(args);
        
    }


    else if(!strcmp(args[0], "p")){

      if(nargs!=3){
        printf("Erro: número de argumentos incorreto.\n");
        continue;
      }

      if(!isNumber(strtok(args[2], "\n"))){
        printf("Preço inválido.\n");
        continue;
      }

      mudar_preco(args);
        
    }

    else printf("Comando inválido.\n");
      
	}while(1);

  free(args);

}
