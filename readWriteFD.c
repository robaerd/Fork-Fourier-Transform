/**
 * @file readWriteFD.c
 * @autor Robert Sowula `e11708475@student.tuwien.ac.at>
 * @date 15.12.18
 *
 * read* functions are reading the filedescripors or stdin and parsing the input into the  _Complex float type.
 * write* functions are parsing _Complex floats type values into chars and are writing them into either a fd or stdout
 */

#include "forkfft.h"

#define MAX_LINE_LENGTH 2048

int readStdIn(_Complex float **X)
{
  char lineBuf[MAX_LINE_LENGTH];
  int i = 0;
  char *ptr;
  float real = 0.0;
  float im = 0.0;

  while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL && strcmp(lineBuf, "\n") != 0){
    if(i >= MAX_LINE_LENGTH)
      *X = realloc(*X, sizeof(_Complex float) * i*2);

    real = strtof(lineBuf, &ptr);
    im = strtof(ptr, NULL);
    (*X)[i++] = (_Complex float) real+im*_Complex_I;
  }
  if(i != MAX_LINE_LENGTH)
    *X = realloc(*X, sizeof(_Complex float) * i);

  return i;
}


void readFdIn(_Complex float *X, int fd)
{
  char readBuffer[MAX_LINE_LENGTH];
  int size = MAX_LINE_LENGTH; // size of strBuf. Will be changed during read() if neccessary
  memset(readBuffer, '\0', sizeof(char) * MAX_LINE_LENGTH);
  char *strBuf = (char *) malloc(sizeof(char *) * size);
  memset(strBuf, 0, sizeof(char *)*  MAX_LINE_LENGTH);

  char *ptr; // char after digit pointer for strof
  float real = 0.0;
  float im = 0.0;

  while(read(fd, readBuffer, MAX_LINE_LENGTH) > 0){
    size = size + MAX_LINE_LENGTH;
    strBuf = realloc(strBuf, sizeof(char*) * size);
    strcat(strBuf, readBuffer);
    memset(readBuffer, '\0', sizeof(char) * MAX_LINE_LENGTH);
  }
  strcat(strBuf, readBuffer);

  char buffer[MAX_LINE_LENGTH];
  int i = 0;
  int j = 0;
  int z = 0;
  while(strBuf[j] != '\0'){
    buffer[z] = strBuf[j];
    if(buffer[z] == '\n' || strBuf[j+1] == '\0'){
      real = strtof(buffer, &ptr);
      im = strtof(ptr, NULL);
      X[i++] = (_Complex float) (real+im*_Complex_I);
      memset(buffer, '\0', sizeof(char)*MAX_LINE_LENGTH);
      z = -1; // -1 beacuse of the following increment of z
    }
    z++;
    j++;
  }
  free(strBuf);
}


void writeToFD(_Complex float *X,int complexFl_length, int fd)
{
  int i;
  for (i = 0; i< complexFl_length; i++) {
    dprintf(fd, "%f %f*i\n", crealf(X[i]), cimagf(X[i]));
  }

}

void writeToStdOut(_Complex float *X,int complexFl_length)
{
  int i;
  for (i = 0; i< complexFl_length; i++) {
    printf("%f %f*i\n", crealf(X[i]), cimagf(X[i]));
  }
  fflush(stdout);
}
