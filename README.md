# prime-number-detector


# HOW TO RUN:

In order to create the executable file, you should run the following command in the terminal: 

### `$ g++ -fopenmp primedector.cpp -o output` 

Program takes 2 arguments from the command line, chunk size and the value of M. In order to run the program with chunk size of 100 and M as 1M you should run the following command:

### `$ ./output 100 1000000` 

The resulting .csv file contains timings and speed up values. All timing values are printed in milliseconds.

In order to print the prime values to the console open the primedetector.cpp file and uncomment line 49. 

