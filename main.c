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
#define MAX_LINES 1024 //change

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

/*
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
 */

int readStdIn(_Complex float *X, int fd)
{
	//char **S_Arr = (char **) malloc(MAX_LINES * sizeof(char **));
	char lineBuf[256];
	int i = 0;
	char *ptr;
	char *im_ptr;
	float real = 0.0;
	float im = 0.0;

	//FILE *fp = fdopen(fd, "r");

	while(fgets(lineBuf, MAX_LINE_LENGTH, stdin) != NULL && strcmp(lineBuf, "\n") != 0){

		real = strtof(lineBuf, &ptr);
		im = strtof(ptr, NULL);
		X[i] = (_Complex float) real+im*_Complex_I;

		//fprintf(stderr, "%f %f*i\n >%s<\n\n", creal(X[i]), cimag(X[i]), lineBuf);
		i++;
	}
	//*actualValues = i;
	fflush(stdin);
	return i;
	//fclose(fp);

}

int getLines(char * str){
	return 1;
}

void readFdIn(_Complex float *X, int actualValues, int fd)
{
	int bufsize = 200 * MAX_LINE_LENGTH;
	char *strBuf  = (char *) malloc(sizeof(char *)*  bufsize); //change 200
	memset(strBuf, 0, sizeof(char *)*  bufsize);

	int i = 0;
	char *ptr;
	//char *im_ptr;
	float real = 0.0;
	float im = 0.0;
	/**
	 change!!!!!
	 */

	/*
	 for (long n = 0; (n = read(fd, strBuf, bufsize)) > 0;){

	 }*/

	ssize_t r = read(fd, strBuf, bufsize);

	char buffer[bufsize];
	memset(buffer, '\0', bufsize);


	//char *bufPointer = strBuf;
	//char *buf;
	//fprintf(stderr, "≥≥%s≤≤", strBuf);
	//while(*buffer != '\n' && *buffer != '\0'){
	int j = 0;
	int z = 0;
	while(strBuf[j] != '\0'){
		buffer[z] = strBuf[j];
		if(buffer[z] == '\n' || strBuf[j+1] == '\0'){
			//buffer[j+1] = '\0';
			real = strtof(buffer, &ptr);
			im = strtof(ptr, NULL);
			//fprintf(stderr, "real: %f \tim: %f str:%s whole: %s\n\n", real, im, buffer, strBuf);
			//if(strcmp(curLine, "\n") == 0) fprintf(stderr, "help");
			//X[j] = (_Complex float) (real+im*_Complex_I);
			X[i] = (_Complex float) (real+im*_Complex_I);
			i++;
			//fprintf(stderr, "%f %f*i\n >%s<\n\n", creal(X[j]), cimag(X[j]), buffer);
			//fprintf(stderr, "%f %f*i\n >>%s<< real: %f \tim: %f §§i=%d !! buflen_ %lu\n\n", creal(X[j]), cimag(X[j]), buffer,real, im,j, strlen(buffer));
			//memset(buffer, '\0', sizeof(char)*bufsize);
			memset(buffer, '\0', bufsize);
			z = 0;
			z--;
		}
		z++;
		j++;

	}





	/*
	char *curLine = strBuf;



	while(curLine)
	{
		//fix this
		if(i >= actualValues){
		    //fprintf(stderr, "index error while reading file descriptor: index is invalid %d", i);
			break;
		//exit(EXIT_FAILURE);

		 }

		char * nextLine = strchr(curLine, '\n');
		//if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
 		//printf("curLine=[%s]\n", curLine);
		//fprintf(stderr, "%s\n\n", curLine);
		real = strtof(curLine, &ptr);
		im = strtof(ptr, NULL);
		if(strcmp(curLine, "\n") == 0) fprintf(stderr, "help");
		X[i] = (_Complex float) (real+im*_Complex_I);
		fprintf(stderr, "%f %f*i\n >>%s<< real: %f \tim: %f §§i=%d\n\n", creal(X[i]), cimag(X[i]), curLine,real, im,i);
		i++;
		//if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy
		curLine = nextLine ? (nextLine+1) : NULL;
	}
	*/
	free(strBuf);

	//fprintf(stderr, "%s\t%ld\n", strBuf, s);
}


void writeToFD(_Complex float *X,int complexFl_length, int fd){
	//FILE *fp = fdopen(fd, "w");
	int i;
	for (i = 0; i< complexFl_length; i++) {
		//fprintf(fp, "%f + i%f\n", creal(X[i]), cimag(X[i]));
		dprintf(fd, "%f %f*i\n", crealf(X[i]), cimagf(X[i]));
	}
	//fflush(fp);
	//fclose(fp);
}

void writeToStdOut(_Complex float *X,int complexFl_length){

	int i;
	for (i = 0; i< complexFl_length; i++) {
		printf("%f %f*i\n", crealf(X[i]), cimagf(X[i]));
	}
	fflush(stdout);
}

void writeToStdErr(_Complex float *X,int complexFl_length){

	int i;
	for (i = 0; i< complexFl_length; i++) {
		fprintf(stderr, "%f + i%f\n", crealf(X[i]), cimagf(X[i]));
	}
	fflush(stderr);
}

_Complex float complexMult(_Complex float a, _Complex float b)
{
	return crealf(a)*crealf(b) -cimagf(a)*cimagf(b)+ (crealf(a)*cimagf(b)+crealf(a)* cimagf(a) * _Complex_I);

}


int main()
{

	int n = 1024;
	int actualValues = 0;
	_Complex float *X = (_Complex float *)malloc(sizeof(_Complex float) * n);

	actualValues = readStdIn(X, 0);
	//actualValues = readStdIn(X, 0);
	//test
	if(actualValues == 1) {
		writeToStdOut(X, 1);
		close(STDOUT_FILENO);
		exit(EXIT_SUCCESS);
	}
	if(actualValues < 1) {
		fprintf(stderr, "error. actualValues = 0");
		exit(EXIT_FAILURE);
	}

	//test end


	/* actualValues even? if not exit with error */

	_Complex float *P_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
	_Complex float *P_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);

	_Complex float *R_E = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);
	_Complex float *R_O = (_Complex float *)malloc(sizeof(_Complex float) * actualValues/2);

	order1(X, actualValues, &P_E, &P_O);

	//test

	//if (actualValues <=1)
	fprintf(stderr, "actualValues: %d\n", actualValues);
	//end test

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



	//pid_t parent = getpid();

	//even part
	pid_t pid_E = fork();

	if (pid_E == -1)
	{
		fprintf(stderr, "error during even Fork");
		exit(EXIT_FAILURE);
	}
	else if (pid_E > 0)
	{

		close(pipefd2[0]); // closes reading side from parent. only need write end
		writeToFD(P_E, actualValues/2, pipefd2[1]); //
		close(pipefd2[1]); //error handling

		close(pipefd1[1]); // close writing end of p1

		//wait and receive result
		int status1;
		waitpid(pid_E, &status1, 0);

		// /2 added
		readFdIn(R_E, actualValues/2, pipefd1[0]); //is actualValues even right? //pipe is reading side from stdout of child to parent //also valid imo --> check notes
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

	//odd part
	pid_t pid_O = fork();

	if (pid_O == -1)
	{
		fprintf(stderr, "error during odd Fork");
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
		readFdIn(R_O, actualValues/2, pipefd3[0]); //is actualValues even right? //pipe is reading side from stdout of child to parent //also valid imo --> check notes
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


	_Complex float *R = (_Complex float *)malloc(sizeof(_Complex float) * actualValues);
	//_Complex float R[actualValues];
	int k;
	for(k = 0; k < actualValues/2; k++){
		_Complex float omega = (cos(-((2*PI/actualValues)*k))+ sin(-((2*PI/actualValues)*k)) * _Complex_I);

		//_Complex float second = complexMult(omega, R_O[k]);
		_Complex float second = omega * R_O[k];
		//R[k    ] = P_E[k] + omega * P_O[k];
		//R[k] = (_Complex float) (((creal(R_E[k]) + creal(second)) +  (cimag(R_E[k])+ cimag(second))*_Complex_I));
		R[k] = R_E[k] + second;
		//R[k+actualValues/2] = P_E[k] - omega * P_O[k];
		//R[k+actualValues/2] = (_Complex float) (((creal(R_E[k]) - creal(second)) +  (cimag(R_E[k])- cimag(second))*_Complex_I));
		R[k+actualValues/2] = R_E[k] - second;
		//R[k+actualValues/2] = P_E[k] - complexMult(omega, P_O[k]);
	}


	//write to stdout
	//writeToFD(R, actualValues, 1);
	writeToStdOut(R, actualValues);
	free(R);
	free(P_E);
	free(P_O);
	free(R_E);
	free(R_O);
	exit(EXIT_SUCCESS); //right?
}
