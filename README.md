# Fork-Fourier-Tranform
A Cooley-Tukey fast fourier transform algorithm which instead of calling a function recursively, calls itself (forks, executes itself and pipes input and output)
#### Info
- (amount of inputvalues) * 2 +1 processes will be created
- This can lead at around 4096 input lines to errors because the max limit of processes on a few systems is reached (will be fixed)
- In the future the program will only executes itself only around 100 times and will compute the DFT recursively.
#### Building from source
For development:
```sh
$ make all
```
Remove all executables and object files with:
```sh
$ make clean
```
#### Usage Examples
##### Sinus wave:
```sh
$ cat sine.txt
0.000000
0.707107
1.000000
0.707107
0.000000
-0.707107
-1.000000
-0.707107
```
```sh
$ ./forkFFT < sine.txt
0.000000 0.000000*i
0.000000 -4.000001*i
0.000000 0.000000*i
0.000000 -0.000001*i
0.000000 0.000000*i
-0.000000 0.000001*i
0.000000 0.000000*i
-0.000000 4.000001*i
```
##### Complex values can olso be provided as input:
- SYNOPSIS:
```sh
$ ./forkFFT
REALTERM IMAGTERM*i
...
...
```
- EXAMPLE:
```sh
$ ./forkFFT
0.000000 -1.000000*i
0.400000 -0.2000001*i
```
