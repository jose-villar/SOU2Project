#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define nOfDoctorsR1 3 // number of doctors in region 1
#define nOfDoctorsR2 2 // number of doctors in region 2
#define nOfLabsR1 2 // number of labs in region 1
#define nOfLabsR2 3 // number of labs in region 2
#define LENGTH 6 // size of an auxiliary array

int keepGoing = 2; // 0 is false, 1 is true, 2 is true in verbose mode

/*The resource counter is a proxy variable to protect
the access to the critical sections (files). 
0 means the resource is free,
>1 means the amount of readers
-1 means there's a writer.*/
int resourceCounterDR1 // resource counter for Doctor file region 1
int resourceCounterLR1; // resource counter for Lab file region 1
int resourceCounterDR2 // resource counter for Doctor file region 2
int resourceCounterLR2; // resource counter for Lab file region 2
int resourceCounterHSR1; // resource counter for health service file region 1
int resourceCounterHSR2; // resource counter for health service file region 2

int wReadersDoctorFilesR1; //number of threads waiting to read the doctors file in region 1 (it may be either a health service or the health department)
int wWritersDoctorFilesR1; //number of threads waiting to write into the doctors file in region 1
int wReadersDoctorFilesR2; //number of threads waiting to read the doctors file in region 2 (it may be either a health service or the health department)
int wWritersDoctorFilesR2; //number of threads waiting to write into the doctors file in region 2

int wReadersHealthServiceR1; // number of threads waiting to read the health service file in region 1
int wReadersHealthServiceR1; // number of threads waiting to read the health service file in region 2

pthread_mutex_t mutexLR1; // mutex for lab file region 1
pthread_mutex_t mutexLR2; // mutex for lab file region 2
pthread_mutex_t mutexDR1; // mutex for doctor file region 1
pthread_mutex_t mutexDR2; // mutex for doctor file region 2
pthread_mutex_t mutexHSR1; // mutex for health service file region 1
pthread_mutex_t mutexHSR2; // mutex for health service file region 2

pthread_cond_t readCondLR1; // condition for reading lab file region 1
pthread_cond_t writeCondLR1; // codition for writing into lab file region 1

pthread_cond_t readCondLR2; // condition for reading lab file region 2
pthread_cond_t writeCondLR2; // codition for writing into lab file region 2

pthread_cond_t readCondDR1; // condition for reading doctor file region 1
pthread_cond_t writeCondDR1; // codition for writing into doctor file region 1

pthread_cond_t readCondDR2; // condition for reading doctor file region 2
pthread_cond_t writeCondDR2; // codition for writing into doctor file region 2

pthread_cond_t readCondHSR1; // condition for reading health service file region 1
pthread_cond_t writeCondHSR1; // codition for writing into health service file region 1

pthread_cond_t readCondHSR2; // condition for reading health service file region 2
pthread_cond_t writeCondHSR2; // codition for writing into health service file region 2

char *getCurrentTimeAndDate() {
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );  
  timeinfo = localtime (&rawtime); 
  return strtok(asctime (timeinfo), "\n"); //removes the "\n" in the end of the string
}

// Copies the first "size" elements of the first array into the second one
void copyArr(char arr1[], char arr2[], int size) {
  for(int i = 0; i < size; i++) {
    arr2[i] = arr1[i];
  }
}

// Changes all the elements of a char array into backspace characters
void clearArr(char arr[], int size) {
  for(int i = 0; i < size; i++) {
    arr[i] = ' ';
  }
}

// Gets the number located in the specified line of a file that has only one number per line.
int getNumberFromFile(char file[], int line) {
  int number = 0;
  FILE *fp;
  char mode[] = "r";
  fp =  fopen(file, mode);
  char arr[LENGTH]; 
  int n;
  char c;
  int count = 0;
  int i = 0;
  while(1) {   
    c = fgetc(fp);
    arr[i] = c;
    
    // next line
    if(c == '\n') {
      count++;
      char nAsArray[i];
      copyArr(arr, nAsArray, i);
      n = atoi(nAsArray);
      clearArr(nAsArray, i);
      clearArr(arr, LENGTH);
      i = 0;
      if(count == line) {
        number = n;
        break;
      }
    }
    // End of file
    if(feof(fp) ) { 
      break ;
    }
    i++;    
  }
  
  fclose(fp);
  return number;
}

// Computes the sum of all the int values in a file
int computeSum(char file[]) {
  FILE *fp;
  char mode[] = "r";
  char c; 
  fp =  fopen(file, mode);   
  int n;
  char arr[LENGTH]; 
  int i = 0;
  int sum = 0;

  while(1) {   
    c = fgetc(fp);
    arr[i] = c;
    
    // next line
    if(c == '\n') {
      char nAsArray[i];
      copyArr(arr, nAsArray, i);
      n = atoi(nAsArray);
      sum += n;
      clearArr(nAsArray, i);
      clearArr(arr, LENGTH);
      i = 0;
    }
    // End of file
    if(feof(fp) ) { 
      break ;
    }
    i++;    
  }
  
  fclose(fp);   

  return sum;
}

// Deletes all the contents of a file.
void eraseFileContents(char file[]) {
  FILE *fp;
  fp =  fopen(file, "w");
  fprintf(fp, "%s", ""); 
  fclose(fp);
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

// Function executed by each doctor thread from region 1. It registers the number of dead patients a doctor with id *i had in one day.
void *registerDeathsR1(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;

  nOfDeaths = rand() % 100; // number between 0 and 99 
  
  //Critical section ***************************************
  pthread_mutex_lock (&mutex);
  writeAppendInt("files/deathsR1.txt", nOfDeaths);
  pthread_cond_signal(&readCond); // If there's any reader waiting, it will have priority.
  pthread_cond_signal(&writeCond); //.

  pthread_mutex_unlock(&mutex_monitor);
  // End Critical section **********************************
  
  if(keepGoing == 2) { // if verbose mode is active
    printf("The doctor %ld of region 1 is registering %d deaths...\n", doctorId, nOfDeaths);
  }
  pthread_exit(NULL);
}

// Function executed my each doctor thread from region 2. It registers the number of dead patients a doctor had in one day.
void *registerDeathsR2(void *i){
  int nOfDeaths; // new daily deaths 
  long doctorId = (long) i;

  nOfDeaths = rand() % 100; // number between 0 and 99 
  
  // Critical section **************************************
  writeAppendInt("files/deathsR2.txt", nOfDeaths);
  // End Critical section **********************************
  
  if(keepGoing == 2) { // if verbose mode is active
    printf("The doctor %ld of region 2 is registering %d deaths...\n", doctorId, nOfDeaths);
  }
  pthread_exit(NULL);
}

// Function executed my each lab thread from region 1. It notifies about the number of cases the lab registered in one day.
void *registerCasesR1(void *i){
  int nOfCases; // new daily cases 
  long labId = (long) i;

  nOfCases = rand() % 1000 + 100; // number between 100 and 1099

  if(keepGoing == 2) {
    printf("The lab %ld of region 1 is registering %d new cases... \n", labId, nOfCases);
  }

  // Critic section ********************************** 
  writeAppendInt("files/casesR1.txt", nOfCases);
  // End critic section ******************************

  pthread_exit(NULL);

}

// Function executed my each lab thread from region 2. It notifies about the number of cases the lab registered in one day.
void *registerCasesR2(void *i){
  int nOfCases; // new daily cases 
  long labId = (long) i;

  nOfCases = rand() % 1000 + 100; // number between 100 and 1099

  if(keepGoing == 2) {
    printf("The lab %ld of region 2 is registering %d new cases... \n", labId, nOfCases);
  }

  // Critic section ********************************** 
  writeAppendInt("files/casesR2.txt", nOfCases);
  // End critic section ******************************

  pthread_exit(NULL);

}

// Reads the regional data to coumpute the national statistics
void *healthServiceFunction(void *i) {
  long region = (long) i;
  int totalDeaths = 0;
  int totalCases = 0;
  int randomTime = rand() % 5;

  switch(region) {
    case 1:
      // Critical section ******************************
      totalDeaths = computeSum("files/deathsR1.txt");
      eraseFileContents("files/deathsR1.txt");
      totalCases = computeSum("files/casesR1.txt");
      eraseFileContents("files/casesR1.txt");
      // End Critical section **************************
      
      // Critical section *****************************
      writeUniqueInt("files/hServiceR1.txt", totalCases);
      writeAppendInt("files/hServiceR1.txt", totalDeaths);
      // End critical section *************************
      break;	
    case 2:
      // Critical section ******************************
      totalDeaths = computeSum("files/deathsR2.txt");eraseFileContents("files/deathsR2.txt");
      totalCases = computeSum("files/casesR2.txt");
      eraseFileContents("files/casesR2.txt");
      // End Critical section *************************
      
      // Critical section
      writeUniqueInt("files/hServiceR2.txt", totalCases);
      writeAppendInt("files/hServiceR2.txt", totalDeaths);
      // End critical section
      break;
  }  

  randomTime = rand() % 5;



  pthread_exit(NULL);
}

void *healthDepartmentFunction(void * i) {
  // Reads the current national statistics
  int nationalCases = getNumberFromFile("files/hDepartment.txt", 1);
  int nationalDeaths = getNumberFromFile("files/hDepartment.txt", 2);

  // Reads the information from region 1
  // Critical section ******************************
  nationalCases += getNumberFromFile("files/hServiceR1.txt", 1); // gets the number in line 1 of the file
  nationalDeaths += getNumberFromFile("files/hServiceR1.txt", 2); // gets the number in line 2 of the file
  // End Critical section ******************************

  // Reads the information from region 2
  //Critical section **********************************
  nationalCases += getNumberFromFile("files/hServiceR2.txt", 1); //gets the number in line 1 of the file
  nationalDeaths += getNumberFromFile("files/hServiceR2.txt", 2); // gets the number in line 2 of the file
  // End Critical section ******************************
  
  // Writes the national statistics
  writeUniqueInt("files/hDepartment.txt", nationalCases);
  writeAppendInt("files/hDepartment.txt", nationalDeaths);

  if(keepGoing == 2) {
    printf("Total deaths in the country: %d\n", nationalDeaths);
    printf("Total cases in the country: %d\n", nationalCases);
  }

  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t doctorsR1 [nOfDoctorsR1];
  pthread_t doctorsR2 [nOfDoctorsR2];
  pthread_t labsR1 [nOfLabsR1];
  pthread_t labsR2 [nOfLabsR2];
  pthread_t hServiceR1; 
  pthread_t hServiceR2;
  pthread_t healthDepartment;
  int dayNumber = 0;

  srand (time(NULL));
  // pthread_mutex_init(&mutex, NULL);
  eraseFileContents("files/deathsR1.txt");

  // The program will keep running until the user decides to stop it
  while(1) {    
    
    //Creates doctor threads from region 1
    for(long i = 0; i < nOfDoctorsR1; i++) {    
      if(pthread_create(&doctorsR1[i], NULL, registerDeathsR1, (void *)i)){
        printf("Error creating doctors from region 1.\n");
        exit(-1);    
      }
      pthread_join(doctorsR1[i], NULL);   
    }  
    
    //Creates doctor threads from region 2
    for(long i = 0; i < nOfDoctorsR2; i++) {    
      if(pthread_create(&doctorsR2[i], NULL, registerDeathsR2, (void *)i)){
        printf("Error creating doctors from region 2.\n");
        exit(-1);    
      }
      pthread_join(doctorsR2[i], NULL);   

    }

    // Creates lab threads from region 1
    for(long i = 0; i < nOfLabsR1; i++) {
      if(pthread_create(&labsR1[i], NULL, registerCasesR1, (void*)i)){
        printf("Error creating labs from region 1.\n");
      }
      pthread_join(labsR1[i], NULL);
    }

    //Creates lab threads from region 2
    for(long i = 0; i < nOfLabsR2; i++) {
      if(pthread_create(&labsR2[i], NULL, registerCasesR2, (void*)i)){
        printf("Error creating labs from region 2.\n");
      }
      pthread_join(labsR2[i], NULL);
    }
   
    // Creates Health Service thread from region 1
    pthread_create(&hServiceR1, NULL, healthServiceFunction, (void *) 1);
    pthread_join(hServiceR1, NULL);

    // Creates Health Service thread from region 2
    pthread_create(&hServiceR2, NULL, healthServiceFunction, (void *) 2);       pthread_join(hServiceR2, NULL);
 
    // Creates health department thread
    pthread_create(&healthDepartment, NULL, healthDepartmentFunction, NULL);
    pthread_join(healthDepartment, NULL);

    /************** Join all the threads *************/
/*
    for(int i = 0; i < nOfDoctorsR1; i++) {
      pthread_join(doctorsR1[i], NULL);   
    }
 
    for(int i = 0; i < nOfDoctorsR2; i++) {
      pthread_join(doctorsR1[i], NULL);   
    }

    for(int i = 0; i < nOfLabsR1; i++) {
      pthread_join(labsR1[i], NULL);   
    }

    for(int i = 0; i < nOfLabsR2; i++) {
      pthread_join(labsR2[i], NULL);   
    }

    pthread_join(hServiceR2, NULL);
    pthread_join(healthDepartment, NULL);
    */

    printf("%s GMT: Simulation of day %d finished.\nEnter 1 to continue, 2 to continue in verbose mode or 0 to exit >> ", getCurrentTimeAndDate(), dayNumber);
    
    scanf("%d", &keepGoing);
    printf("\n");
    if(keepGoing == 0) {
      break;
    }
    dayNumber++;
  }
  return 0;
}