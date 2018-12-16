/**
 * @file forkfft.c
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 12.12.18
 *
 * @brief Main program module.
 *
 * Calculates the fft of stdin input floats, where the input float can either only contain an real part or both.
 * The Amount of input values provided should be a power of two, otherwise the program exits with exit code EXIT_FAILURE
 * The fft is calculated recursively. The programm forks at each recursion step and calls itself.
 * The parent then writes the neccessary splitted (in even an odd incices) complex float array into the childs stdin.
 * The child then calculates the dft and writes it into his stdout, where the parent can read it.
 *
 * @details input line amount should be a power of 2
 */

#include "forkfft.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#define PI 3.141592654 /* approximated value of pi */

/**
 * @brief checks if an input line is valid and writes the real and imaginary part into their provided adresses
 * a valid input lines has following form: <value containing only numbers>  and optional <backspace> followed by the <imaginary value> and <"*i"> to explicitly set it as imgaginary part.
 */
static int checkAndParse(char *, float *, float *);

/**
 * is called if closing an file descriptor resulted in an error
 * @brief prints an error message and exits the programm with exit code EXIT_FAILURE
 */
static void closeError(char *);

/**
 * First checks if no arguments are provided, then initializes the pipes.
 * pipefd1 redirects stdout from the first child into his parent, pipefd3 from the second child
 * pipef2d and pipef4d redirects the stdin from the childs to the parent
 *
 * The program reads the input values and buffers two complex float values. Every two input lines read by the program, the array value with indice 0 (even) is send to the even child and the array value with indice 1 (odd) is send to the odd child
 * If the input line parsing loop is run  exactly two times, the two childs are created an wait for their input lines to
 * be completly send. During the wait, the childs already parse their received input lines.
 * If only one complex Value is received, it is immediatly printed to stdout, so the parent receives it (bottom of recursion).
 *
 * The DFT is calculated recursively by forking itself every recursion step and calling forkfft. The input values from the two children are received through stdin from the parent. After calculating the DFT the program writes the result into stdout where the parent can parse it.
 */
int main(int argc, char **argv)
{
  //check if arguments are provided. if so terminate with exit code EXIT_FAILURE
  if(argc > 1){
    fprintf(stderr, "ERROR: Arguments provided. Do not provide Arguments.\n");
    exit(EXIT_FAILURE);
  }

  char lineBuf[MAX_LINE_LENGTH]; /* line buffer where a stdin input line is stored */
  int i = 0; /* counter for the amount of lines being parsed from stdin */
  float real = 0.0;
  float im = 0.0;

  pid_t pid_O;
  pid_t pid_E;
  int pipefd1[2]; /* STD_OUT from first child to parent */
  int pipefd2[2]; /* from parent to stdin at first child */
  int pipefd3[2]; /* STD_OUT from second child to parent */
  int pipefd4[2]; /* from parent to stdin at second child */
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

  /* _Complex float buffer of size two. Value at indice 0 is send two frist child, the other one two the second child */
  _Complex float X[2];
  /* input line parsing followed by sending the first two paresed lines directly into the childs */
  while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL && strcmp(lineBuf, "\n") != 0){

    if(checkAndParse(lineBuf, &real, &im) == 0){
      fprintf(stderr, "Wrong input format.\n Possible format example: ./forkfft 1.0 0.5*i OR ./forkfft 2.00000000 \n");
      exit(EXIT_FAILURE);
    }

    X[i%2] = (_Complex float) real+im*_Complex_I;
    i++;
    if(i % 2 == 0 && i > 2){
      dprintf(pipefd2[1], "%f %f*i\n", crealf(X[0]), cimagf(X[0]));
      dprintf(pipefd4[1], "%f %f*i\n", crealf(X[1]), cimagf(X[1]));
    }

    if(i == 2){ /* more than one input lines read -> recursion goes on and two childs are created wich executes the program again */

      pid_E = fork();

      if (pid_E == -1) {
        fprintf(stderr, "error during even fork");
        exit(EXIT_FAILURE);
      } else if (pid_E > 0) {
        if(close(pipefd2[0]) == -1) closeError("pipefd2[0]"); // closes reading side from parent. only need write end
        dprintf(pipefd2[1], "%f %f*i\n", crealf(X[0]), cimagf(X[0]));
        pid_O = fork();

        if (pid_O == -1) {
          fprintf(stderr, "error during odd fork");
          exit(EXIT_FAILURE);
        } else if (pid_O > 0) {
          if(close(pipefd4[0]) == -1) closeError("pipefd4[0]"); // closes reading side from parent. only need write end
          dprintf(pipefd4[1], "%f %f*i\n", crealf(X[1]), cimagf(X[1]));
        } else {
          if(close(pipefd2[1]) == -1) closeError("pipefd2[1]");
          if(close(pipefd1[1]) == -1) closeError("pipefd1[1]");
          if(dup2(pipefd3[1], STDOUT_FILENO) == -1) {
            fprintf(stderr, "Error duplicating STDOUT to write end of third pipe: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
          }
          if(close(pipefd3[1]) == -1) closeError("pipefd3[1]");
          if(close(pipefd3[0]) == -1) closeError("pipefd3[0]");

          if (dup2(pipefd4[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "Error duplicating STDIN to read end of fourth pipe: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
          }
          if(close(pipefd4[0]) == -1) closeError("pipefd4[0]");
          if(close(pipefd4[1]) == -1) closeError("pipefd4[1]");
          execlp("./forkfft", "forkfft", NULL);
          //if execlp not succesfull:
          fprintf(stderr, "exec error on odd part");
          exit(EXIT_FAILURE);
        }

      } else {
        if (dup2(pipefd1[1], STDOUT_FILENO) == -1) {
          fprintf(stderr, "Error duplicating STDOUT to write end of first pipe: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if(close(pipefd1[1]) == -1) closeError("pipefd1[1]");
        if(close(pipefd1[0]) == -1) closeError("pipefd1[0]");

        if(dup2(pipefd2[0], STDIN_FILENO) == -1) {
          fprintf(stderr, "Error duplicating STDIN to read end of second pipe: %s\n", strerror(errno));
          exit(EXIT_FAILURE);
        }
        if(close(pipefd2[0]) == -1) closeError("pipefd2[0]");
        if(close(pipefd2[1]) == -1) closeError("pipefd2[1]");

        execlp("./forkfft", "forkfft", NULL);
        //if execlp succesfull, next lines will not be reached
        fprintf(stderr, "exec error on even part");
        exit(EXIT_FAILURE);
      }
    }
  }
  int actualValues;
  actualValues = i;
  /* end of recursive call */
  if(actualValues == 1) {
    writeToStdOut(X, 1);
    if(close(STDOUT_FILENO) == -1) closeError("STDOUT_FILENO");
    exit(EXIT_SUCCESS);
  } else if ((actualValues % 2) != 0) {
    fprintf(stderr, "Amount of Complex float values is not even!\n");
    exit(EXIT_FAILURE);
  }

  _Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
  _Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);

  if(close(pipefd2[1]) == -1) closeError("pipefd2[1]");
  if(close(pipefd1[1]) == -1) closeError("pipefd1[1]");
  if(close(pipefd4[1]) == -1) closeError("pipefd4[1]");
  if(close(pipefd3[1]) == -1) closeError("pipefd3[1]");

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
  if(close(pipefd1[0]) == -1) closeError("pipefd1[0]");

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
  if(close(pipefd3[0]) == -1) closeError("pipefd3[0]");

  /* calculate DFT */
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

/**
 * @brief checks if an input line is valid and writes the real and imaginary part into their provided adresses '*real' and '*im'
 * a valid input lines has following form: <value containing only numbers>  and optional <backspace> followed by the <imaginary value> and <"*i"> to explicitly set it as imgaginary part.
 * @param str string to check validity
 * @param real adress of float value where the parsed real part is written into
 * @param im adress of float vlaue wehre the parsed im part is written into
 */
int checkAndParse(char *str, float *real, float *im){
  if(isalpha(*str)) return 0;
  char *ptr;
  char *imPtr;
  *real = strtof(str, &ptr);
  if(*ptr == '\n'){

    return 1;
  }
  if(*ptr != ' ') return 0;
  if(isalpha(*(ptr++))) return 0;

  *im = strtof(ptr, &imPtr);
  if(strcmp(imPtr, "*i\n") != 0) return 0;

  return 1;
}

/**
 * is called if closing an file descriptor resulted in an error
 * @brief prints an error message and exits the programm with exit code EXIT_FAILURE
 * @param fd filedescriptor where the error happend
 */
void closeError(char *fd){
  fprintf(stderr, "error closing %s: %s",fd, strerror(errno));
  exit(EXIT_FAILURE);
}
