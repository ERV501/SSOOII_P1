/*Enrique Rodriguez Villafranca*/

#define _GNU_SOURCE

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
int scan_student_model();
void get_model(char model[],char dni[]);
void deliver_model(char modelo[],char dni[]);

/*'Main'*/
int main (int argc, char *argv[]) {

  installSignalHandler();
  scan_student_model();
  
  sleep(2);

  return EXIT_SUCCESS;
}


int scan_student_model() {

  char model[MAX_NAME_LEN],dni[MAX_NAME_LEN], unused[MAX_NAME_LEN],c;
  FILE *fp;

    if ((fp = fopen("estudiantes_p1.text", "r")) == NULL) {
        printf("[PB %d] Error opening file\n",getpid());
        exit(1);
    }
    
    while((c = fgetc(fp)) != EOF){                          /* Lee hasta el fin de archivo */
		if(c != ' ' || c != '\n'){
		    fscanf(fp, "%s %s %s", dni,model,unused);          /* Escanea y guarda el modelo de examen */
        get_model(model,dni);
      }
		}

    fclose(fp);
    return 0;
}

void get_model(char model[],char dni[]){

  for(int i=0;i<strlen(model);i++){
    switch(model[i]){
      case 'A':
        deliver_model("MODELOA.pdf",dni);
      break;
      case 'B':
        deliver_model("MODELOB.pdf",dni);
      break;
      case 'C':
        deliver_model("MODELOC.pdf",dni);
      break;
    }
     
  }
}


void deliver_model(char modelo[],char dni[]){
  FILE *fp1, *fp2;
    char ch, path_model[MAX_NAME_LEN],path_dni[MAX_NAME_LEN];
    int pos;

    sprintf(path_model,"%s/%s","./MODELOSEXAMEN",modelo);
    sprintf(path_dni,"%s/%s/%s","./Students",dni,modelo);


  if ((fp1 = fopen(path_model,"r")) == NULL)    {    
        printf("\n[PB %d] File cannot be opened",getpid());
        return;
    }
    
    fp2 = fopen(path_dni, "w");  
    fseek(fp1, 0L, SEEK_END); /* file pointer at end of file */
    pos = ftell(fp1);
    fseek(fp1, 0L, SEEK_SET); /* file pointer set at start */
    while (pos--)
    {
        ch = fgetc(fp1);  /* copying file character by character */
        fputc(ch, fp2);
    }    
    fcloseall();
}


/*Instalacion y manejador de señal*/
void installSignalHandler(){
	if(signal(SIGINT, signalHandler) == SIG_ERR)
	{
		fprintf(stderr, "[PB %d] Error instaling handler: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int signo){
	printf("[PB %d] terminated (SIGINT)\n", getpid());
	exit(EXIT_SUCCESS);
}