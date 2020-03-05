#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int isNumber (const char *s)
{
     char *ep = NULL;
     double f = strtod (s, &ep);

     if (!ep  ||  *ep)
         return 0;

     return 1;
}

int readln(int fd, char *buffer){

    int i = 0;
    while(1){
        int n = read(fd, &buffer[i], 1);
        if(n<=0) break;
        char c = buffer[i];
        if(c=='\n'){
            buffer[i] = '\0';
            return i;
        }else{
            i++;
        }
    }

    return -1;
}


int parseCommand(char *** dest, char * orig){
	char *copy = strdup(orig);
	int onQuotes = 0;
	int onParameter = 0;
	
	int i=0, j=0;
	while(1){
		if(copy[i]=='\0' || copy[i]=='\n'){
			(*dest)[j]=NULL;
			return j;
		}
		else if(copy[i]=='\"'){
			if(onQuotes){
				onQuotes = 0;
				copy[i] = '\0';
				i++;
			}
			else{
				onQuotes = 1;
				copy[i]='\0';
				i++;
				(*dest)[j]=copy+i;
				j++;
				onParameter = 1;
			}
		}
		else if(copy[i]==' '){
			if(onQuotes)
				i++;
			else{
				if(onParameter == 1)
					onParameter = 0;
				copy[i]='\0';
				i++;
			}
		}
		else{
			if(onParameter == 1)
				i++;
			else{
				(*dest)[j]=copy+i;
				j++;
				i++;
				onParameter = 1;
			}
		}
	}
	return j;
}

void parseArtigo(char *** dest, char * orig){
	char *copy = strdup(orig);
	int onParameter = 0;
	
	int i=0, j=0;
	while(1){
		if(copy[i]=='\0' || copy[i]=='\n'){
			(*dest)[j]=NULL;
			break;
		}
		else if(copy[i]==' '){
      if(onParameter == 1)
        onParameter = 0;
      copy[i]='\0';
      i++;
		}
		else{
			if(onParameter == 1)
				i++;
			else{
				(*dest)[j]=copy+i;
				j++;
				i++;
				onParameter = 1;
			}
		}
	}
}