/**
 * @file readWriteFD.c
 * @autor Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 15.12.18
 *
 * readFD is reading a filedescripor and parsing the input into the  _Complex float type.
 * write* functions are parsing _Complex floats type values into chars and are writing them into either a file descriptor or stdout
 */

#include "forkfft.h"

/**
 * @brief reads a filedescriptor and and parsed the input into an array of _Complex float values
 * @param X adress to _Complex float array where the parsed complex float values are written into
 * @param fd filedescripor which will be parsed
 *
 */
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

  //int strsize = 0;
  while(read(fd, readBuffer, MAX_LINE_LENGTH) > 0){
    size = size + MAX_LINE_LENGTH;
    strBuf = realloc(strBuf, sizeof(char*) * size);
    strBuf = strcat(strBuf, readBuffer);
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

/**
 * @brief writes _Complex float array in string form into filedescripor
 * @param X complex float array
 * @param complexFl_length size of complex float array X
 * @param fd filedescripor where the string representation of X is written into
 */
void writeToFD(_Complex float *X,int complexFl_length, int fd)
{
  int i;
  for (i = 0; i< complexFl_length; i++)
    dprintf(fd, "%f %f*i\n", crealf(X[i]), cimagf(X[i]));
}
/**
 * @brief writes _Complex float array in string form into stdout
 * @param X complex float array
 * @param complexFl_length size of complex float array X
 */
void writeToStdOut(_Complex float *X,int complexFl_length)
{
  int i;
  for (i = 0; i< complexFl_length; i++)
    printf("%f %f*i\n", crealf(X[i]), cimagf(X[i]));

  fflush(stdout);
}
