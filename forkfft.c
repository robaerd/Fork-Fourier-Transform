/**
 * @file forkfft.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 12.12.18
 *
 * @brief Main program module.
 *
 * ------
 */

#include "forkfft.h"
#include <sys/types.h>
#include <sys/wait.h>

#define PI 3.141592654
#define STD_LINES 1024 //change

#define MAX_LINE_LENGTH 2048



int main(int argc, char **argv)
{
  //check if arguments are provided. if so terminate with exit code EXIT_FAILURE
  if(argc > 1){
    fprintf(stderr, "ERROR: Arguments provided. Do not provide Arguments.\n");
    exit(EXIT_FAILURE);
  }

    char lineBuf[MAX_LINE_LENGTH];
    int i = 0;
    char *ptr;
    float real = 0.0;
    float im = 0.0;
    
    pid_t pid_O;
    pid_t pid_E;
    int pipefd1[2]; //STD_OUT from first child to parent
    int pipefd2[2]; //from parent to stdin at first child
    int pipefd3[2]; //STD_OUT from second child to parent
    int pipefd4[2]; //from parent to stdin at second child
    if (pipe(pipefd1) < 0){
        fprintf(stderr, "error creating first pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd2) < 0){
        fprintf(stderr, "error creating second pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd3) < 0){
        fprintf(stderr, "error creating third pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd4) < 0){
        fprintf(stderr, "error creating fourth pipe");
        exit(EXIT_FAILURE);
    }
    
  _Complex float *X = (_Complex float *)malloc(sizeof(_Complex float) * 2);
    //_Complex float X[2];

  while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL && strcmp(lineBuf, "\n") != 0){

    real = strtof(lineBuf, &ptr);
    im = strtof(ptr, NULL);
    X[(i)%2] = (_Complex float) real+im*_Complex_I;
      i++;
      if(i % 2 == 0 && i > 2){
          dprintf(pipefd2[1], "%f %f*i\n", crealf(X[0]), cimagf(X[0]));
          dprintf(pipefd4[1], "%f %f*i\n", crealf(X[1]), cimagf(X[1]));
      }

      if(i == 2){
      
      pid_E = fork();

      if (pid_E == -1) {
        fprintf(stderr, "error during even fork");
        //free(P_E);
        //free(P_O);
        exit(EXIT_FAILURE);
      } else if (pid_E > 0) { //even parent
        close(pipefd2[0]); // closes reading side from parent. only need write end
          dprintf(pipefd2[1], "%f %f*i\n", crealf(X[0]), cimagf(X[0]));
        pid_O = fork();

        if (pid_O == -1) {
          fprintf(stderr, "error during odd fork");
          //free(P_E);
          //free(P_O);
          exit(EXIT_FAILURE);
        } else if (pid_O > 0) {
          close(pipefd4[0]); // closes reading side from parent. only need write end
          dprintf(pipefd4[1], "%f %f*i\n", crealf(X[1]), cimagf(X[1]));
        } else {
          //free(P_E);
          // free(P_O);
            close(pipefd2[1]); //error handling
            close(pipefd1[1]); // close writing end of p1
            if(dup2(pipefd3[1], STDOUT_FILENO) == -1) {
                fprintf(stderr, "Error duplicating STDOUT to write end of third pipe: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
              }
              close(pipefd3[1]);
              close(pipefd3[0]);

              if (dup2(pipefd4[0], STDIN_FILENO) == -1) {
                fprintf(stderr, "Error duplicating STDIN to read end of fourth pipe: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
              }
              close(pipefd4[0]);
              close(pipefd4[1]);

              execlp("./forkfft", "forkfft", NULL);
              //if execlp not succesfull
              fprintf(stderr, "exec error on odd part");
              exit(EXIT_FAILURE);
            }

      } else {
          
        if (dup2(pipefd1[1], STDOUT_FILENO) == -1) {
          fprintf(stderr, "Error duplicating STDOUT to write end of first pipe: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        close(pipefd1[1]);
        close(pipefd1[0]);

        if(dup2(pipefd2[0], STDIN_FILENO) == -1) {
          fprintf(stderr, "Error duplicating STDIN to read end of second pipe: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        close(pipefd2[0]);
        close(pipefd2[1]);
        close(pipefd2[0]);

        execlp("./forkfft", "forkfft", NULL);
        //if execlp succesfull, next lines will not be reached
        fprintf(stderr, "exec error on even part");
        exit(EXIT_FAILURE);
      }
    }
  }
    int actualValues;
    actualValues = i;
    //end of recursive call
    if(actualValues == 1) {
        writeToStdOut(X, 1);
        //fprintf()
        close(STDOUT_FILENO);
        exit(EXIT_SUCCESS);
    } else if ((actualValues % 2) != 0) {
        fprintf(stderr, "Amount of Complex float values is not even!\n");
        exit(EXIT_FAILURE);
    }

        _Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
        _Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
        
        close(pipefd2[1]); //error handling
        close(pipefd1[1]); // close writing end of p1
        close(pipefd4[1]); //error handling
        close(pipefd3[1]); // close writing end of p1

        int status1;
        pid_t waitStatus1;
        do {
            waitStatus1 = waitpid(pid_E, &status1, 0);
            if (waitStatus1 == -1) {
                if (errno == ECHILD){
                    fprintf(stderr,"ECHILD ERROR:  %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                } else if (errno == EINTR){
                    fprintf(stderr,"EINTR ERROR: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                } else{
                    fprintf(stderr,"ERROR: %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            
            if (WEXITSTATUS(status1) == EXIT_FAILURE) {
                fprintf(stderr,"exited with failure\n");
                exit(EXIT_FAILURE);
            }
            
        } while (!WIFEXITED(status1));
        readFdIn(P_E, pipefd1[0]);
        close(pipefd1[0]);
    
    
        int status2;
        pid_t waitStatus2;
        do {
            waitStatus2 = waitpid(pid_O, &status2, 0);
            if (waitStatus2 == -1) {
            if (errno == ECHILD){
                fprintf(stderr,"ECHILD ERROR:  %s\n", strerror(errno));
               exit(EXIT_FAILURE);
            } else if (errno == EINTR){
                fprintf(stderr,"EINTR ERROR: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            } else{
                fprintf(stderr,"ERROR: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            }
            
    if (WEXITSTATUS(status2) == EXIT_FAILURE) {
        fprintf(stderr,"exited with failure\n");
        exit(EXIT_FAILURE);
    }
            
        } while (!WIFEXITED(status2));
        readFdIn(P_O, pipefd3[0]);
        close(pipefd3[0]);
        

        //free(X);

        //calculate DFT
        _Complex float *R = (_Complex float *)malloc(sizeof(_Complex float) * actualValues);
        int k;
        for(k = 0; k < actualValues/2; k++){
          _Complex float omega = (cos(-((2*PI/actualValues)*k))+ sin(-((2*PI/actualValues)*k)) * _Complex_I);
          _Complex float second = omega * P_O[k];
          R[k] = P_E[k] + second;
          R[k+actualValues/2] = P_E[k] - second;
        }
        writeToStdOut(R, actualValues);
        free(R);
        free(P_E);
        free(P_O);
        exit(EXIT_SUCCESS);
    }

