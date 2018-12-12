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
      /*
      pid_t parent = getpid();
      //pid_t pid = fork();

      if (pid == -1)
        {
          // error, failed to fork()
        }
      else if (pid > 0)
        {
          int status;
          waitpid(pid, &status, 0);
        }
      else
        {
          // we are the child
          //execve(...);
          //exit(EXIT_FAILURE);   // exec never returns
          } */
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

void readStdIn(_Complex float *X, int *actualValues)
{
  //char **S_Arr = (char **) malloc(MAX_LINES * sizeof(char **));
  char lineBuf[256];
  int i = 0;
  char *ptr;
  char *im_ptr;
  float real = 0.0;
  float im = 0.0;
  while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL){
    real = strtof(lineBuf, &ptr);
    im = strtof(ptr, &im_ptr);
    X[i] = (_Complex float) real+im*I;
    //strcpy(S_Arr[i], lineBuf);
    i++;
  }
  *actualValues = i;

}

int complexArrToStArr(_Complex float *X, int length)
{
  char *complexStrArr[length];
  //for()
}


int main()
{

  int n = 1024;
  int actualValues = 0;
  _Complex float *X = (_Complex float *)malloc(sizeof(_Complex float) * n);


  //int pid = fork();

  readStdIn(X, &actualValues);
  if(actualValues <= 1){
    //recursion end
  } else {

  //actualValues even? if not exit with error

  _Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
  _Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
  order1(X, actualValues, &P_E, &P_O);
  free(X);

  //before fork ^^^^^

  int pipefd1[2]; //STD_OUT from child to parent
  int pipefd2[2]; //from parent to stdin at child
  int pipe(int pipefd1[2]);
  int pipe(int pipefd2[2]);


  //should be don in parent
  /*dup2(pipefd2[0], STDIN_FILENO);
  close(pipefd2[0]);
   close(pipefd2[1]);

  */



  pid_t parent = getpid();
  pid_t pid_E = fork();
  pid_t pid_O = fork();

  //even part
  if (pid_E == -1)
    {
      // error, failed to fork()
    }
  else if (pid_E > 0)
    {
      dup2(pipefd2[0], STDIN_FILENO);
      close(pipefd2[0]); // closes reading side from parent. only need write end
      //dont forget to close after writing to pipefd2[1]
      write()

      int status;
      waitpid(pid_E, &status, 0);
    }
  else
    {
      //should be done after fork in child process
      dup2(pipefd1[1], STDOUT_FILENO);
      close(pipefd1[1]);
      close(pipefd1[0]);

      //imo also needs to close write end


      // we are the child
      //execve(...);
      //exit(EXIT_FAILURE);   // exec never returns
    }

  //odd part

  if (pid_O == -1)
    {
      // error, failed to fork()
    }
  else if (pid_O > 0)
    {
      int status;
      waitpid(pid_O, &status, 0);
    }
  else
    {
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
}
