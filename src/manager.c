/*Enrique Rodriguez Villafranca*/

#define _POSIX_SOURCE

#include <errno.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

/*Definiciones*/
#define PATH_A "./exec/pa"
#define PATH_B "./exec/pb"
#define PATH_C "./exec/pc"
#define PATH_D "./exec/pd"

#define PIPE_READING 0
#define PIPE_WRITING 1

#define MAX_NAME_LEN 256

/* Total number of processes */
int g_nProcesses=4;          
/* 'Process table' de tamaño 4 dado que solo vamos a lanzar 4 procesos */
pid_t proccess_table[4];

/* Precesses classes */
enum ProcessClass_t {PA,PB,PC,PD};

/*Headers*/
void installSignalHandler();
void signalHandler(int signo);
void create_processes_by_class(enum ProcessClass_t class, char *const arg[]);
pid_t create_single_process(const char *path, const char *str_process_class, char *const arg[]);
void wait_processes_finalize(int n_processes);
void terminate_processes(void);
void read_pipe_mean(int fd);

/* Main */
int main(int argc, char *argv[]) {

  int fd[2];
  int log;

  installSignalHandler();
  
  if (((log = open("./log.txt",  O_WRONLY|O_CREAT|O_TRUNC, 0666))) < 0){
        printf("[MANAGER] Error creating log\n");
        exit(1);
    }

  pipe(fd);
  close(1); /*Close stdout*/
  dup(log);

  printf("\n-----  Log del sistema ----- \n");
  printf("[Manager] PA created\n");

  char *a1 = PATH_A;
  char *args_a[]={a1,NULL};
  create_processes_by_class(PA,args_a);
  wait_processes_finalize(1);
  printf("[Manager] PA ended\n");

  char *b1 = PATH_B;
  char *args_b[]={b1,NULL};
  create_processes_by_class(PB,args_b);
  
  char arg_int_pc[MAX_NAME_LEN],arg_char_pc[MAX_NAME_LEN];
  sprintf(arg_int_pc,"%d",fd[PIPE_WRITING]);
  sprintf(arg_char_pc,"%s",arg_int_pc);
  char *s1 = PATH_C, *s2 = arg_char_pc;
  char *args_c[]={s1,s2,NULL};

  create_processes_by_class(PC,args_c);

  sleep(2);

  wait_processes_finalize(2);
  printf("[Manager] PB and PC ended\n");

	read_pipe_mean(fd[PIPE_READING]);

  close(fd[0]);
  close(fd[1]);
  close(log);

  return EXIT_SUCCESS;
}


void create_processes_by_class(enum ProcessClass_t class, char *const arg[]){
	pid_t pid;
	
	switch(class){
		case PA:
			pid = create_single_process(PATH_A, "pa",arg);
			break;
		case PB:
			pid = create_single_process(PATH_B, "pb",arg);
			break;
		case PC:
			pid = create_single_process(PATH_C, "pc",arg);
			break;
		case PD:
			pid = create_single_process(PATH_D, "pd",arg);
			break;
		default:
			return;
	}
	
	if(pid == -1){
    terminate_processes();
		exit(EXIT_FAILURE);
	}

proccess_table[class]=pid; /*Guardamos pid en la tabla de procesos*/

}


pid_t create_single_process(const char *path, const char *str_process_class, char *const arg[]) {
  pid_t pid;

  switch (pid = fork()) {
  case -1 :
    fprintf(stderr, "[MANAGER] Error creating %s process: %s.\n", 
	    str_process_class, strerror(errno));
    terminate_processes();

    exit(EXIT_FAILURE);
    /* Child process */
  case 0 : 
    if (execv(path, arg) == -1) {
      fprintf(stderr, "[MANAGER] Error using execl() in %s process: %s.\n", 
	      str_process_class, strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

  /* Parent process */
  return pid;
}

void wait_processes_finalize(int n_processes){

   int i;
  pid_t pid;

  /* Wait for the termination of n processes */
  while (n_processes > 0) {
    /* Wait for any process */
    pid = wait(NULL); 
    for (i = 0; i < g_nProcesses; i++) {
      if (pid == proccess_table[i]) {
	/* Update the 'process table' */
        proccess_table[i]= 0;           
	/* Decrement the number of running processes */

	  n_processes--; 
	/* Child process found */
        break; 
      }
    }
  }
}

void terminate_processes(void) {
  int i;
  
  printf("\n[MANAGER] Terminating running child processes\n");
  for (i = 0; i < g_nProcesses; i++) {
    /* Child process alive */
    if (proccess_table[i] != 0) { 
      printf("[MANAGER] Terminating %d process...\n", 
	     proccess_table[i]);
      if (kill(proccess_table[i], SIGINT) == -1) {
	fprintf(stderr, "[MANAGER] Error using kill() on process %d: %s.\n", 
		proccess_table[i], strerror(errno));
      }
    }
  }
}

void read_pipe_mean(int fd){
	char buf[MAX_NAME_LEN];
	
	read(fd, buf, sizeof(buf));
  fprintf(stdout,"The average students grade result is: %s\n",buf);

}

/*Instalacion y manejador de señal*/
void installSignalHandler(){
	if(signal(SIGINT, signalHandler) == SIG_ERR)
	{
		fprintf(stderr, "[Manager %d] Error instaling handler: %s\n", getpid(), strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void signalHandler(int signo){

  terminate_processes();
  char *s1 = PATH_D;
  char *args_d[]={s1,NULL};
  create_processes_by_class(PD,args_d);
  wait(NULL);
  
	printf("[Manager %d] terminated (SIGINT)\n", getpid());
  
	exit(EXIT_SUCCESS);
}