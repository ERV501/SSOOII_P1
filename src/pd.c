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
#include <dirent.h>

#define MAX_NAME_LEN 256

/*'Cabeceras'*/
int remove_dir(char path[]);

/*'Main'*/
int main (int argc, char *argv[]) { 

 if(remove_dir("./Students") != 0){ 
     printf("[PD %d] Error removing files\n",getpid());
	 exit(EXIT_FAILURE); 
    }

 return EXIT_SUCCESS;
}

int remove_dir(char path[]){
   char rm[MAX_NAME_LEN];

   sprintf(rm,"rm -r ./Students");

   system(rm);

  return EXIT_SUCCESS;
}