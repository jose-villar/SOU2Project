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

// Function executed my each doctor thread. It registers the number of dead patients a doctor had in one day.
void *notifyDeaths(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;
  FILE *filePointer;

  nOfDeaths = rand() % 100; // number between 0 and 99 
  filePointer = fopen("deaths.txt", "a+");  // open the file for writing in appending mode
 
  fprintf(filePointer, "%d", nOfDeaths); 
  fprintf(filePointer, "%c", '\n'); 

  printf("The doctor number: %ld is registering %d deaths... \n", doctorId, nOfDeaths);

  fclose(filePointer); // closes the file
  pthread_exit(NULL);
}

void *notifyCases(void *i){
  int nOfCases; //Simulation of new daily cases 
  nOfCases = rand() % 100;
  int length = (int)((ceil(log10(nOfCases))+1)*sizeof(char));
  char buff[length];
  char buff2[] = "\n"; 
  sprintf(buff, "%d", nOfCases);

  long labId = (long) i;
  int fd;//File descriptor

  printf("The lab number: %ld is writing... \n", labId);
  
  //Writing in the cases file
  fd = open("./cases.txt", O_WRONLY|O_APPEND);
  write(fd, buff, sizeof(buff)-1);
  write(fd, "\n", sizeof(buff2)-1);
  close(fd);      
  
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t doctors [nOfDoctorsR1 + nOfDoctorsR2];
  pthread_t labs [nOfLabsR1 + nOfLabsR2];
  srand (time(NULL));

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
  /*
  //Creating laboratories threads
  for(long i=0; i<nOfLabsR1 + nOfLabsR2; i++){
    printf("I'm the lab number: %ld \n", i);
    if(pthread_create(&labs[i], NULL, notifyCases, (void*)i)){
      printf("Error during thread creation \n");
    }
  }
  // Wait for all the labs threads to finish
  for(int i = 0; i < nOfLabsR1 + nOfLabsR2; i++) {
    pthread_join(labs[i], NULL);   
  }
  */
  return 0;
}