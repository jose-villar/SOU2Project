#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define nOfDoctorsR1 4
#define nOfDoctorsR2 6
#define nOfLabsR1 3
#define nOfLabsR2 2

void *notifyDeaths(void *i){

  int nOfDeaths; //Simulation of new daily deaths 
  nOfDeaths = rand() % 100;

  char buf[] = "";
    
  long a = (long) i;
  int fd;//File descriptor

  printf("Escribiendo... el medico %ld \n", a);
  
  fd = open("./fichero.txt", O_WRONLY|O_APPEND);
  write(fd, buf, sizeof(buf)-1);
  close(fd);    
  pthread_exit(NULL);
}

void *notifyCases(){
  printf("Funcion laboratorio");
}

int main(int argc, char *argv[]) {
  pthread_t doctors [nOfDoctorsR1 + nOfDoctorsR2];
  pthread_t labs [nOfLabsR1 + nOfLabsR2];
  srand (time(NULL));

  //Creating doctor threads
  for(long i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++){
    
    printf("Soy el médico número %ld \n", i);    
    if(pthread_create(&doctors[i], NULL, notifyDeaths, (void *)i)){
      printf("Error en la creación del hilo");
      exit(-1);    
    }
  }
  // Wait for all the doctor threads to finish
  for(int i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++) {
    pthread_join(doctors[i], NULL);   
  }
  //Creating laboratories threads
  /*for(long i=0; i<nOfLabsR1 + nOfLabsR2; i++){
    printf("Soy el labortorio número %ld \n", i);
    if(pthread_create(&labs[i], NULL, notifyCases, (void*)i)){
      printf("Error en la creacion del hilo");
    }
  }*/

  return 0;
}