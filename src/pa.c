/*Enrique Rodriguez Villafranca*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_NAME_LEN 256

/*'Cabeceras'*/
void installSignalHandler();
void signalHandler(int signo);
int scan_student_dni();
void create_stud_dir(char dni[], char path[]);

/*'Main'*/
int main (int argc, char *argv[]) {

  installSignalHandler();
  scan_student_dni();
  
  sleep(2);
  
  return EXIT_SUCCESS;
}


int scan_student_dni() {

  char dni[MAX_NAME_LEN], unused[MAX_NAME_LEN],path[MAX_NAME_LEN],c;
  FILE *fp;
    if ((fp = fopen("estudiantes_p1.text", "r")) == NULL) {
        printf("[PA %d]Error opening file\n",getpid());
        exit(1);
    }
    
    mkdir("Students",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    while((c = fgetc(fp)) != EOF){                          /* Lee hasta el fin de archivo */
		  if(c != ' ' || c != '\n'){
		    fscanf(fp, "%s %s %s", dni,unused,unused);          /* Escanea y guarda el dni */
        create_stud_dir(dni,path);
      }
		}

    fclose(fp);
    return 0;
}

void create_stud_dir(char dni[],char path[]){
  sprintf(path,"%s/%s","./Students",dni);
  if((mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0){
    printf("[PA %d] Error: %s creating directory %s at %s\n",getpid(),strerror(errno),dni,path);
    exit(EXIT_FAILURE);
  } 
}

/*Instalacion y manejador de señal*/
void installSignalHandler()
{
	if(signal(SIGINT, signalHandler) == SIG_ERR)
	{
		fprintf(stderr, "[PA %d] Error instaling handler: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int signo)
{
	printf("[PA %d] terminated (SIGINT)\n", getpid());
	exit(EXIT_SUCCESS);
}