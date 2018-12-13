//
//  main.c
//  fft
//
//  Created by Robert Sowula on 12.12.18.
//  Copyright © 2018 Robert Sowula. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#define PI 3.141592654
#define MAX_LINE_LENGTH 256
#define MAX_LINES 1024//change

void order(_Complex float *C, int N){
  //_Complex double *temp = (_Complex double *)malloc(sizeof(_Complex double) * (double) N/2);
  _Complex float temp[N];

  int i;
  for(i = 0; i < N; i++) // copy even numbers in temp
    *(temp+i) = *(i < N/2 ? C+i*2 : C+(i-N/2)*2+1);
  for(; i > 0; i--)
    //memcpy(X, temp, sizeof(_Complex double)* (N-1));
    C[i-1] = temp[i-1];

  //free(temp);
}


void order1(_Complex float *EandO, int EandO_length,_Complex float **E,_Complex float **O)
{
  int i;
  for(i = 0; i < EandO_length/2; i++){
    *(*E+i) = EandO[i*2];
    *(*O+i) = EandO[(i-EandO_length/2)*2+1];
  }
}


void fft(_Complex float *P, int N){
  if (N <= 1) {

  } else {

    order(P, N);

    fft(P, N/2);
    fft((P+N/2), N/2);

    int k;
    for(k = 0; k < N/2; k++){
      _Complex float omega = (cos(-((2*PI/N)*k))+ I* sin(-((2*PI/N)*k)));
      _Complex float e = P[k    ];   // even
      _Complex float o = P[k+N/2];
      P[k    ] = e + omega * o;
      P[k+N/2] = e - omega * o;
    }

  }
}


void readFdIn(_Complex float *X, int *actualValues, int fd)
{
  //char **S_Arr = (char **) malloc(MAX_LINES * sizeof(char **));
  char lineBuf[256];
  int i = 0;
  char *ptr;
  char *im_ptr;
  float real = 0.0;
  float im = 0.0;

  FILE *fp = fdopen(fd, "r");

  while(fgets(lineBuf, MAX_LINE_LENGTH, fp) != NULL){
    real = strtof(lineBuf, &ptr);
    im = strtof(ptr, &im_ptr);
    X[i] = (_Complex float) real+im*I;
    //strcpy(S_Arr[i], lineBuf);
    i++;
  }
  *actualValues = i;
  fclose(fp);

}


void writeToFD(_Complex float *X,int complexFl_length, int fd){
  FILE *fp = fdopen(fd, "w");
  int i;
  for (i = 0; i< complexFl_length; i++) {
    fprintf(fp, "%f + i%f\n", creal(X[i]), cimag(X[i]));
  }
  fclose(fp);
}


int main()
{

  int n = 1024;
  int actualValues = 0;
  _Complex float *X = (_Complex float *)malloc(sizeof(_Complex float) * n);


  /*readStdIn(X, &actualValues,0); */
  readFdIn(X, &actualValues, 0);
  //writeToFD(X, actualValues, 1); //works :) dont leave this here pls


  /* actualValues even? if not exit with error */

  _Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
  _Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
  order1(X, actualValues, &P_E, &P_O);
  free(X);

  //before fork ^^^^^

  int pipefd1[2]; //STD_OUT from first child to parent
  int pipefd2[2]; //from parent to stdin at first child
  int pipefd3[2]; //STD_OUT from second child to parent
  int pipefd4[2]; //from parent to stdin ad second
  int pipe(int pipefd1[2]);
  int pipe(int pipefd2[2]);
  int pipe(int pipefd3[2]);
  int pipe(int pipefd4[2]);


  pid_t parent = getpid();

  //even part
  pid_t pid_E = fork();

  if (pid_E == -1)
    {
      // error, failed to fork()
    }
  else if (pid_E > 0)
    {
      /**
       * write even array to first child
       */
      dup2(pipefd2[0], STDIN_FILENO);
      close(pipefd2[0]); // closes reading side from parent. only need write end
      writeToFD(P_E, actualValues/2, pipefd2[1]); //is valid imo
      close(pipefd2[1]); //error handling

      //wait and receive result
      int status1;
      waitpid(pid_E, &status1, 0);



      //_Complex float *R_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues); //maybe only actualValues/2

      readFdIn(P_E, &actualValues, pipefd1[0]); //is actualValues even right? //pipe is reading side from stdout of child to parent //also valid imo --> check notes
      //needs R_O
      //            int k;
      //            for(k = 0; k < actualValues/2; k++){
      //                _Complex float omega = (cos(-((2*PI/actualValues)*k))+ I* sin(-((2*PI/actualValues)*k)));
      //                //_Complex float e = P[k    ];   // even
      //                //_Complex float o = P[k+N/2];
      //                R_E[k    ] = P_E[k] + omega * P_O[k+actualValues/2];
      //                R_E[k+actualValues/2] = P_E[k] - omega * P_O[k+actualValues/2];
      //            }
      //
      //            writeToFD(R_E, actualValues, 1); //1 is fd of stdout



      //recveing end and stopping
      //            int status1;
      //            waitpid(pid_E, &status1, 0);
    }
  else
    {
      //should be done after fork in child process
      dup2(pipefd1[1], STDOUT_FILENO);
      close(pipefd1[1]);
      close(pipefd1[0]);

      if(actualValues > 1){
        execlp("forkfft", "forkfft", NULL);
      } else {
        writeToFD(P_E, actualValues, 1); // actualValues = 1
        exit(EXIT_SUCCESS);
      }


      //maybe free R?

      //imo also needs to close write end


      // we are the child
      //execve(...);
      //exit(EXIT_FAILURE);   // exec never returns
    }

  //odd part
  pid_t pid_O = fork();

  if (pid_O == -1)
    {
      // error, failed to fork()
    }
  else if (pid_O > 0)
    {


      // write even array to second child

      dup2(pipefd4[0], STDIN_FILENO);
      close(pipefd4[0]); // closes reading side from parent. only need write end
      writeToFD(P_O, actualValues/2, pipefd4[1]);
      close(pipefd4[1]); //error handling

      //wait and receive result
      int status2;
      waitpid(pid_O, &status2, 0);

      //_Complex float *R_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues); //maybe only actualValues/2

      readFdIn(P_O, &actualValues, pipefd3[0]);
    }
  else
    {


      //should be done after fork in child process
      dup2(pipefd3[1], STDOUT_FILENO);
      close(pipefd3[1]);
      close(pipefd3[0]);

      if(actualValues > 1){
        execlp("forkfft", "forkfft", NULL);
      } else {
        writeToFD(P_E, actualValues, 1); // actualValues = 1
        exit(EXIT_SUCCESS);
      }
      // we are the child
      //execve(...);
      //exit(EXIT_FAILURE);   // exec never returns
    }








  //after fork_______
  _Complex float *R = (_Complex float *)malloc(sizeof(_Complex float) * actualValues);
  int k;
  for(k = 0; k < actualValues/2; k++){
    _Complex float omega = (cos(-((2*PI/actualValues)*k))+ I* sin(-((2*PI/actualValues)*k)));
    //_Complex float e = P[k    ];   // even
    //_Complex float o = P[k+N/2];
    R[k    ] = P_E[k] + omega * P_O[k+actualValues/2];
    R[k+actualValues/2] = P_E[k] - omega * P_O[k+actualValues/2];

  }


  //write to stdout
  //writeToFD(R, actualValues, 1);
  writeToFD(R, actualValues, 1);

  //}

  //order(X, actualValues);

  //free(X);



  //printf("%d", actualValues);
  /*
    int i;
    for(i = 0; i < n; i++)
    printf("%f + i%f\n", creal(X[i]), cimag(X[i]));
  */


  //order(X, 8);
  //fft(X, actualValues);
  //printf("sadfgasgsegasdgasegadsgds");

  //int i;
  //for(i = 0; i < actualValues; i++)
  //printf("%f + i%f\n", creal(X[i]), cimag(X[i]));

  //free(X);


}
