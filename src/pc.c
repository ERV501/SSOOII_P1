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
int scan_student_mark();
void required_mark(char *mark,char dni[]);
void send_mark(int result,char dni[]);
int calculate_mean(char *mark, double mean);
void send_mean(double mean, int fd);

/*'Main'*/
int main (int argc, char *argv[]) {
  int mean = 0;
  int fd = atoi(argv[1]);
  
  installSignalHandler();

  mean = scan_student_mark();
  send_mean(mean,fd);

  sleep(2);

  return EXIT_SUCCESS;
}


int scan_student_mark() {

  char mark[MAX_NAME_LEN], unused[MAX_NAME_LEN],dni[MAX_NAME_LEN],c;
  double mean = 0, average;
  FILE *fp;

    if ((fp = fopen("estudiantes_p1.text", "r")) == NULL) {
        printf("[PC %d] Error opening file\n",getpid());
        exit(1);
    }
    
    while((c = fgetc(fp)) != EOF){                          /* Lee hasta el fin de archivo */
		if(c != ' ' || c != '\n'){
		    fscanf(fp, "%s %s %s", dni,unused,mark);          /* Escanea y guarda el modelo de examen y la nota */
            required_mark(mark,dni);
            average = calculate_mean(mark,mean);

      }
		}




    fclose(fp);
    return average;
}

void required_mark(char *mark,char dni[]){
  int result;
  for(int i=0;i < strlen(mark);i++){
   result = (10 - atoi(&mark[i]));
   send_mark(result,dni);
 } 
}

void send_mark(int result,char dni[]){
  FILE *fp;
    char path_dni[MAX_NAME_LEN];

    sprintf(path_dni,"%s/%s/%s","./Students",dni,"notas.txt");


  if ((fp = fopen(path_dni,"w")) == NULL)    
    {    
        printf("\n [PC %d] File cannot be opened",getpid());
        return;
    }

    fprintf(fp, "The mark you must obtain to pass the test is: %d. \n",result);

    fcloseall();
}

int calculate_mean(char *mark, double mean){
  int result;
  for(int i=0;i < strlen(mark);i++){
   result = (result + atoi(&mark[i]));
 } 
  return mean = result/strlen(mark);

}

void send_mean(double mean, int fd){
  char mean_log[MAX_NAME_LEN];
  
	sprintf(mean_log,"%.2f",mean);

	write(fd, mean_log, sizeof(mean_log));
}

void installSignalHandler()
{
	if(signal(SIGINT, signalHandler) == SIG_ERR)
	{
		fprintf(stderr, "[PC %d] Error instaling handler: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int signo)
{
	printf("[PC %d] terminated (SIGINT)\n", getpid());
	exit(EXIT_SUCCESS);
}