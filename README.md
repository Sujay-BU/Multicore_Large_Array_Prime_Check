# Multicore_Large_Array_Prime_Check
This repository contains code that spawns multiple child processes, each of which spawn multiple threads to count how many elements of an extremely large array are prime.  

The parent will create 'n' processes, where 'n' is the number of cores in the machine. Each child process will create n/2 pthreads to do the processing.  
'n' is not constant and is not hard coded, but rather acquired from the operating system.
