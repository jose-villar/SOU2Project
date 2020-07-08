#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define nOfDoctorsR1 4 // number of doctors in region 1
#define nOfDoctorsR2 6 // number of doctors in region 2
#define nOfLabsR1 3 // number of labs in region 1
#define nOfLabsR2 2 // number of labs in region 2

pthread_mutex_t mutex;

// Function executed my each doctor thread. It registers the number of dead patients a doctor had in one day.
void *notifyDeaths(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;
  FILE *filePointer;

  nOfDeaths = rand() % 100; // number between 0 and 99 
  filePointer = fopen("deaths.txt", "a+");  // open the file for writing in appending mode
  
  pthread:pthread_mutex_lock(&mutex);
  printf("The doctor number: %ld is registering %d deaths... \n", doctorId, nOfDeaths);
  sleep(4);

  fprintf(filePointer, "%d \n", nOfDeaths); 
  //fprintf(filePointer, "%c", '\n');
  fclose(filePointer); // closes the file

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
}


void *notifyCases(void *i){
  int nOfCases; // new daily cases 
  long labId = (long) i;
  FILE *filePointer;

  nOfCases = rand() % 100; // number between 0 and 99 
  filePointer = fopen("cases.txt", "a+");  // open the file for writing in appending mode
  
  pthread:pthread_mutex_lock(&mutex);
  printf("The lab number: %ld is registering %d new cases... \n", labId, nOfCases);
  sleep(4); 
  
  fprintf(filePointer, "%d \n", nOfCases); 
  //fprintf(filePointer, "%c", '\n'); 
  fclose(filePointer); // closes the file

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);
  
}



int main(int argc, char *argv[]) {
  pthread_t doctors [nOfDoctorsR1 + nOfDoctorsR2];
  pthread_t labs [nOfLabsR1 + nOfLabsR2];
  srand (time(NULL));
  pthread_mutex_init(&mutex, NULL);

  //Creating doctor threads
  for(long i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++){
    
    if(pthread_create(&doctors[i], NULL, notifyDeaths, (void *)i)){
      printf("Error during thread creation \n");
      exit(-1);    
    }
  }

  // Wait for all the doctor threads to finish
  for(int i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++) {
    pthread_join(doctors[i], NULL);   
  }

  printf("---------------------------------------------- \n"); 
  
  //Creating laboratories threads
  for(long i=0; i<nOfLabsR1 + nOfLabsR2; i++){
    //printf("I'm the lab number: %ld \n", i);
    if(pthread_create(&labs[i], NULL, notifyCases, (void*)i)){
      printf("Error during thread creation \n");
    }
  }
  // Wait for all the labs threads to finish
  for(int i = 0; i < nOfLabsR1 + nOfLabsR2; i++) {
    pthread_join(labs[i], NULL);   
  }
  
  return 0;
}