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

// Function that each doctor thread executes.
void *notifyDeaths(void *i){
  int nOfDeaths; // new daily deaths 
  nOfDeaths = rand() % 100; // number between 0 and 99

  int length = (int)((ceil(log10(nOfDeaths))+1)*sizeof(char)); //length of the buffer to store the number of deaths as an array of characters
  char buff[length];
 // char buff2[] = "\n"; 
//  sprintf(buff, "%d", nOfDeaths);
  long doctorId = (long) i;
  int fd;//File descriptor

  printf("The doctor number: %ld is registering %d deaths... \n", doctorId, nOfDeaths);
  /*
  fd = open("./deaths.txt", O_WRONLY|O_APPEND);
  write(fd, buff, sizeof(buff)-1);
 // write(fd, buff2, sizeof(buff2)-1);
  close(fd); */   
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
    pthread_join(doctors[i], NULL);
  }
  /*
  // Wait for all the doctor threads to finish
  for(int i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++) {
    pthread_join(doctors[i], NULL);   
  }
*/
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