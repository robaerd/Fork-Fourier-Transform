/**
 * @file forkfft.h
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 15.12.18
 * @brief provides all necessary header files for forkfft.c and readwriteFD.c
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LINE_LENGTH 2048

void readFdIn(_Complex float *, int);

void writeToStdOut(_Complex float *X, int);

void writeToFD(_Complex float *X, int, int);
