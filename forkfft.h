/**
 * @file forkfft.h
 * @author Robert Sowula <e11708475@student.tuwien.ac.at>
 * @date 15.12.18
 * @brief provides all necessary header files for forkfft.c and readwriteFD.c and declares the read and write functions for FD's and stdout
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define MAX_LINE_LENGTH 2048 /* max length an input line, containing the complex float value, can have */

/**
 * @brief reads a filedescriptor and and parsed the input into an array of _Complex float values
 */
void readFdIn(_Complex float *, int);

/**
 * @brief writes _Complex float array in string representation into stdout
 */
void writeToStdOut(_Complex float *X, int);


/**
 * @brief writes _Complex float array in string representation into provided filedescripor
 */
void writeToFD(_Complex float *X, int, int);
