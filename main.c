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
#define MAX_LINE_LENGTH 2048
#define STD_LINES 1024 //change

void writeToFD(_Complex float *X,int complexFl_length, int fd);
void writeToStdErr(_Complex float *X,int complexFl_length);


void order1(_Complex float *EandO, int EandO_length,_Complex float **E,_Complex float **O)
{
	int i;
	for(i = 0; i < EandO_length/2; i++){
		*(*E+i) = EandO[i*2];
		*(*O+i) = EandO[(i*2+1)];
	}
}


int readStdIn(_Complex float **X)
{
	char lineBuf[MAX_LINE_LENGTH];
	int i = 0;
	char *ptr;
	float real = 0.0;
	float im = 0.0;
	
	while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL && strcmp(lineBuf, "\n") != 0){
		if(i >= MAX_LINE_LENGTH){
			*X = realloc(*X, sizeof(_Complex float) * i*2);
		}
		real = strtof(lineBuf, &ptr);
		im = strtof(ptr, NULL);
		(*X)[i++] = (_Complex float) real+im*_Complex_I;
	}
	if(i != MAX_LINE_LENGTH){
		*X = realloc(*X, sizeof(_Complex float) * i);
	}
	return i;
}


void readFdIn(_Complex float *X, int actualValues, int fd)
{
	//int bufsize = MAX_LINE_LENGTH; //change!
	char readBuffer[MAX_LINE_LENGTH];
	memset(readBuffer, '\0', sizeof(char) * MAX_LINE_LENGTH);
	char *strBuf  = (char *) malloc(sizeof(char *)*  MAX_LINE_LENGTH); //change 200
	memset(strBuf, 0, sizeof(char *)*  MAX_LINE_LENGTH);
	
	char *ptr;
	float real = 0.0;
	float im = 0.0;
	
	int size = MAX_LINE_LENGTH;
	int realocFlag = 0;
	ssize_t r;
	while((r = read(fd, readBuffer, MAX_LINE_LENGTH)) > 0){
			size = size + MAX_LINE_LENGTH;
			strBuf = realloc(strBuf, sizeof(char*) * size);
			realocFlag = 1;
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
			//buffer[j+1] = '\0';
			real = strtof(buffer, &ptr);
			im = strtof(ptr, NULL);
			X[i++] = (_Complex float) (real+im*_Complex_I);
			memset(buffer, '\0', sizeof(char)*MAX_LINE_LENGTH);
			z = -1; //beacuse of the following increment
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


int main()
{
	int actualValues = 0;
	_Complex float *X = (_Complex float *)malloc(sizeof(_Complex float) * STD_LINES);
	
	actualValues = readStdIn(&X);

	if(actualValues == 1) {
		writeToStdOut(X, 1);
		close(STDOUT_FILENO);
		exit(EXIT_SUCCESS);
	}
	if(actualValues < 1) {
		fprintf(stderr, "error. actualValues < 0");
		exit(EXIT_FAILURE);
	}
	
	
	_Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
	_Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
	
	order1(X, actualValues, &P_E, &P_O);
	
	free(X);
	
	
	int pipefd1[2]; //STD_OUT from first child to parent
	int pipefd2[2]; //from parent to stdin at first child
	int pipefd3[2]; //STD_OUT from second child to parent
	int pipefd4[2]; //from parent to stdin ad second
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
	
	
	//even part
	pid_t pid_E = fork();
	
	if (pid_E == -1)
	{
		fprintf(stderr, "error during even Fork");
		free(P_E);
		free(P_O);
		exit(EXIT_FAILURE);
	}
	else if (pid_E > 0)
	{
		
		close(pipefd2[0]); // closes reading side from parent. only need write end
		writeToFD(P_E, actualValues/2, pipefd2[1]); //
		close(pipefd2[1]); //error handling
		
		close(pipefd1[1]); // close writing end of p1
		
		//odd part
		pid_t pid_O = fork();
		
		if (pid_O == -1)
		{
			fprintf(stderr, "error during odd Fork");
			free(P_E);
			free(P_O);
			exit(EXIT_FAILURE);
			// error, failed to fork()
		}
		else if (pid_O > 0)
		{
			// write even array to second child
			close(pipefd4[0]); // closes reading side from parent. only need write end
			writeToFD(P_O, actualValues/2, pipefd4[1]); //
			close(pipefd4[1]); //error handling
			
			close(pipefd3[1]); // close writing end of p1
			
			//wait and receive result
			int status2;
			waitpid(pid_O, &status2, 0);
			
			// /2 added
			readFdIn(P_O, actualValues/2, pipefd3[0]); //is actualValues even right? //pipe is reading side from stdout of child to parent //also valid imo --> check notes
			close(pipefd3[0]);
		}
		else
		{
			free(P_E);
			free(P_O);
			dup2(pipefd3[1], 1);
			close(pipefd3[1]);
			close(pipefd3[0]);
			
			dup2(pipefd4[0], 0);
			close(pipefd4[0]);
			close(pipefd4[1]);
			
			execlp("./forkfft", "forkfft", NULL);
			fprintf(stderr, "exec error on odd part");
			exit(EXIT_FAILURE);
			
		}
		//wait and receive result
		int status1;
		waitpid(pid_E, &status1, 0);
		
		// /2 added
		readFdIn(P_E, actualValues/2, pipefd1[0]); //is actualValues even right? //pipe is reading side from stdout of child to parent //also valid imo --> check notes
		close(pipefd1[0]);
		
	}
	else
	{
		free(P_E);
		free(P_O);
		dup2(pipefd1[1], STDOUT_FILENO);
		close(pipefd1[1]);
		close(pipefd1[0]);
		
		if(dup2(pipefd2[0], STDIN_FILENO) == -1){
			fprintf(stderr, "error duplicating pipfefd2-0");
		}
		close(pipefd2[0]);
		close(pipefd2[1]);
		close(pipefd2[0]); //neccessary?
		
		
		execlp("./forkfft", "forkfft", NULL);
		//if execlp succesfull, next lines will not be reached
		fprintf(stderr, "exec error on even part");
		exit(EXIT_FAILURE);
	}
	
	_Complex float *R = (_Complex float *)malloc(sizeof(_Complex float) * actualValues);
	//_Complex float R[actualValues];
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
	exit(EXIT_SUCCESS); //right?
}

