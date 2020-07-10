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

pthread_mutex_t mutex; //MUTEX

// Function executed my each doctor thread. It registers the number of dead patients a doctor had in one day.
void *notifyDeaths(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;
  FILE *filePointer;

  nOfDeaths = rand() % 100; // number between 0 and 99 
  filePointer = fopen("deaths.txt", "a+");  // open the file for writing in appending mode
  
  pthread_mutex_lock(&mutex); //MUTEX LOCK
  printf("The doctor number: %ld is registering %d deaths... \n", doctorId, nOfDeaths);
  sleep(4);

  fprintf(filePointer, "%d \n", nOfDeaths); 
  //fprintf(filePointer, "%c", '\n');
  fclose(filePointer); // closes the file

  pthread_mutex_unlock(&mutex); //MUTEX UNLOCK

  pthread_exit(NULL);
}


void *notifyCases(void *i){
  int nOfCases; // new daily cases 
  long labId = (long) i;
  FILE *filePointer;

  nOfCases = rand() % 100; // number between 0 and 99 
  filePointer = fopen("cases.txt", "a+");  // open the file for writing in appending mode
  
  pthread_mutex_lock(&mutex);
  printf("The lab number: %ld is registering %d new cases... \n", labId, nOfCases);
  sleep(4); 
  
  fprintf(filePointer, "%d \n", nOfCases); 
  //fprintf(filePointer, "%c", '\n'); 
  fclose(filePointer); // closes the file

  pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);

}

void *healthServiceFunction(){
  int toSleep = rand() % 10;
  printf("healthServiceFunction");
  sleep(toSleep);
}

void *healthDepartmentFunction(){
  int toSleep = rand() % 10;
  printf("healthDepartmentFunction");
  sleep(toSleep);
}

int main(int argc, char *argv[]) {
  pthread_t doctors [nOfDoctorsR1 + nOfDoctorsR2];
  pthread_t labs [nOfLabsR1 + nOfLabsR2];

  pthread_t hServiceR1; 
  pthread_t hServiceR2;

  pthread_t healthDepartment;
  
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

  printf("--------------------------------------------\n");

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

  //Creating Health Service threads
  pthread_create(&hServiceR1, NULL, healthServiceFunction, NULL);
  pthread_join(hServiceR1, NULL);

  pthread_create(&hServiceR2, NULL, healthServiceFunction, NULL);
  pthread_join(hServiceR2, NULL);

  pthread_create(&healthDepartment, NULL, healthDepartmentFunction, NULL);

  return 0;
}


/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void clearArr(int arr[], int size) {
  for(int i = 0; i < size; i++) {
    arr[i] = -1;
  }
}

int juntarNumeros(int arr[], int size){
  int numero=0;
  int contador = 0;
  int potencia;

  for(int i=0; i < size; i++){
    if(arr[i]!=-1){
      contador+=1;
    }
  }
  for(int i=0; i < size; i++){
    if(arr[i]!=-1){
      potencia = pow(10, contador-1);
      numero=numero+arr[i]*potencia;
      contador-=1;
    }
  }

  return numero;
}


int main(void) {
  FILE *fp;
  char c; 
  fp =  fopen("deaths.txt", "r");  
  //hola
  int n;
  int contador;
  int arr[5] = {-1,-1,-1,-1,-1};
  int suma=0;
  //int arr1[5] = {3,7,4,8,-1};
  //int numero = juntarNumeros(arr1, 5);
  //printf("el valor es %d \n", numero);

  int i = 0;

  while(1) {
    c = fgetc(fp);
    if( feof(fp) ) { 
      break ;
    }
    //printf("%c", c);
    if(c != '\n'){
      n = c - '0';
      arr[i]=n;
      i++;
      printf("numero: %d \n", n);
    }
    else{
      printf("El valor leido es %d\n", juntarNumeros(arr, 5));
      suma=suma+juntarNumeros(arr, 5);
      printf("la suma es %d\n", suma);
      clearArr(arr, 5);

    }    
  }
  
  fclose(fp);

  fp =  fopen("deaths.txt", "w");
  fprintf(fp, "%d\n", suma); 
 

  
  return 0;
}
*/
