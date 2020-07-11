
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define nOfDoctorsR1 2 // number of doctors in region 1
#define nOfDoctorsR2 1 // number of doctors in region 2
#define nOfLabsR1 2 // number of labs in region 1
#define nOfLabsR2 1 // number of labs in region 2
#define LENGTH 5

pthread_mutex_t mutex; //MUTEX


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
    //printf("[%d]", arr[i]);
    if(arr[i]!=-1 && contador != 0){
      potencia = pow(10, contador-1);
      numero=numero+arr[i]*potencia;
      contador-=1;
    }
  }

  return numero;
}

// Copies the first "size" elements of the first array into the second one
void copyArr(char arr1[], char arr2[], int size) {
  for(int i = 0; i < size; i++) {
    arr2[i] = arr1[i];
  }
}

// Changes all the elements of an int array into backspace characters
void clearArr(int arr[], int size) {
  for(int i = 0; i < size; i++) {
    arr[i] = -1;
  }
}

// Changes all the elements of a char array into backspace characters
void clearArr2(int arr[], int size) {
  for(int i = 0; i < size; i++) {
    arr[i] = ' ';
  }
}

// Compute the sum of all the int values in a file
int hola(char a[], char b[]) {
  FILE *fp;
  char c; 
  fp =  fopen("a.txt", "b");  
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

// Compute the sum of all the int values in a file
int computeSum(char file[]) {
  FILE *fp;
  char mode[] = "";
  char c; 
  fp =  fopen(file, mode);   
  int n;
  char arr[LENGTH]; 
  int i = 0;
  int sum = 0;

  while(1) {   
    c = fgetc(fp);
    
    // End of file
    if(feof(fp) ) { 
      break ;
    }
    arr[i] = c;   

    // next line
    if(c == '\n') {
      char nAsArray[i];
      copyArr(arr, nAsArray, i);
      n = atoi(nAsArray);
      sum += n;
      //clearArr2(nAsArray, i);
      //clearArr2(arr, LENGTH);
    }
    i++;    
  }
  
  fclose(fp);   

  return sum;
}

// Writes an int value into a file. If the file has any contents, they are erased.
void writeUniqueInt(char file[], int value) {
  FILE *fp;
  fp =  fopen(file, "w");
  fprintf(fp, "%d\n", value); 
  fclose(fp);
  
}

// Writes a new value into a file. It doesn't replace the  preexistent values.
void writeAppendInt(char file[], int value) {
  FILE *fp;
  fp =  fopen(file, "a+");
  fprintf(fp, "%d\n", value); 
  fclose(fp);
}

void writeDeaths(char file[], char mode[] ,int deaths){ 
  FILE *filePointer;
  filePointer = fopen(file, mode);  // open the file for writing in appending mode
  fprintf(filePointer, "%d \n", deaths); 
  //fprintf(filePointer, "%c", '\n');
  fclose(filePointer); // closes the file
}

// Function executed my each doctor thread. It registers the number of dead patients a doctor had in one day.
void *notifyDeaths(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;
  nOfDeaths = rand() % 100; // number between 0 and 99
  printf("The doctor number: %ld is registering %d deaths... \n", doctorId, nOfDeaths);
  //sleep(4);
  //pthread_mutex_lock(&mutex); //MUTEX LOCK

  writeDeaths("deaths.txt", "a+", nOfDeaths);

  //pthread_mutex_unlock(&mutex); //MUTEX UNLOCK

  pthread_exit(NULL);
}

void *notifyCases(void *i){
  int nOfCases; // new daily cases 
  long labId = (long) i;
  FILE *filePointer;

  nOfCases = rand() % 100; // number between 0 and 99 
  filePointer = fopen("cases.txt", "a+");  // open the file for writing in appending mode
  
  //pthread_mutex_lock(&mutex);
  printf("The lab number: %ld is registering %d new cases... \n", labId, nOfCases);
  //sleep(1); 
  
  fprintf(filePointer, "%d \n", nOfCases); 
  //fprintf(filePointer, "%c", '\n'); 
  fclose(filePointer); // closes the file

  //pthread_mutex_unlock(&mutex);

  pthread_exit(NULL);

}

int readAndWrite(char file[], char mode[], char file2[], char mode2[]){
  printf("healthServiceFunction \n");

  FILE *fp;
  char c; 
  fp =  fopen(file, mode); 

  //hola
  int n;
  int contador;
  int arr[5] = {-1,-1,-1,-1,-1};
  int suma=0;
  int i = 0;

  while(1) {
    c = fgetc(fp);
    if( feof(fp) ) { 
      break ;
    }
    if(c != '\n'){
      n = c - '0';
      if(n>=0){
        arr[i]=n;
        i++;
        //printf("numero: %d \n", n);
      }
    }
    else{
      //printf("Numero completo: %d\n", juntarNumeros(arr, 5));
      suma=suma+juntarNumeros(arr, 5);
      clearArr(arr, 5);
      i=0;
    }    
  }
  fclose(fp);
  printf("la suma es %d\n", suma);

  fp =  fopen(file2, mode2);
  fprintf(fp, "%d\n", suma);
  pthread_exit(NULL);
}

void *healthServiceFunction(void *p){
  //int toSleep = rand() % 10;
  if(1 == 1){
    readAndWrite("deaths.txt",
    "r","hServiceR1.txt","a+");

  }
  else{
    readAndWrite("deathsR2.txt","r","hServiceR2.txt","a+");
    
  }

  pthread_exit(NULL);

  //sleep(toSleep);
}

void *healthDepartmentFunction(){
  //int toSleep = rand() % 10;
  //printf("healthDepartmentFunction \n");
  //sleep(toSleep);
  pthread_exit(NULL);

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

  // Waits for all the doctor threads to finish
  for(int i = 0; i < nOfDoctorsR1 + nOfDoctorsR2; i++) {
    pthread_join(doctors[i], NULL);   
  }

  printf("--------------------------------------------\n");

  //Creating laboratory threads
  for(long i=0; i<nOfLabsR1 + nOfLabsR2; i++){
    //printf("I'm the lab number: %ld \n", i);
    if(pthread_create(&labs[i], NULL, notifyCases, (void*)i)){
      printf("Error during thread creation \n");
    }
  }

  // Wait for all the lab threads to finish
  for(int i = 0; i < nOfLabsR1 + nOfLabsR2; i++) {
    pthread_join(labs[i], NULL);   
  }
  
  //Creating Health Service threads
  pthread_create(&hServiceR1, NULL, healthServiceFunction, NULL);
  
  pthread_join(hServiceR1, NULL);

  //pthread_create(&hServiceR2, NULL, healthServiceFunction, NULL);
 
  //pthread_join(hServiceR2, NULL);

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
