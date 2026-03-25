#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <math.h>

//Initializing the array along with it's size and a mutex variable
const int size_of_array = 1000000;
static int my_array_of_numbers[1000000];
pthread_mutex_t lock;

//Creating the thread structure (start and end points of each thread)
struct thread_structure {
	int start, end;
};

//Function to check if a given set of numbers is prime or not
void *isPrime(void *parameter) {

	//Casting the parameter passed to the function into the structure thread_structure
	//Initializing the file variable amongst others
	struct thread_structure *threads = (struct thread_structure*) parameter;
	int number_of_primes = 0, number_of_factors = 0;
	FILE *fptr;

	//For loop to chek the number of factors of the number in the array and increments a counter if the number of factors is 0
	for(int i = threads->start; i < threads->end; i++) {
		
		//If loop to check if the value being tested for primality is lesser than 2 since the numbers lesser than 2 aren't prime
		if (my_array_of_numbers[i] >= 2) {

			//For loop to check primality, 1 is ignored since it is common for all numbers
			//For loop ends at the integer square root of the number + 1 for efficiency purposes
			for(int j = 2; j < floor(sqrt(my_array_of_numbers[i])) + 1; j++)
				if(my_array_of_numbers[i] % j == 0)
					++number_of_factors;

			if(number_of_factors == 0)
				++number_of_primes;

			number_of_factors = 0;

		}

	}

	//Locking the code that opens and writes to a file since overwriting/appending of values might take place
	pthread_mutex_lock(&lock);
	fptr = fopen("numbers.txt", "a"); //Opening a file and appending values to it
	fprintf(fptr, "%d\n", number_of_primes);
	fclose(fptr);
	pthread_mutex_unlock(&lock);
}



int main() {

//Variable declaration
const int number_of_cores = sysconf(_SC_NPROCESSORS_ONLN);
int number_of_threads = number_of_cores/2;
pid_t process_pid[number_of_cores];
pthread_t thread_tid[number_of_cores * number_of_threads];

int start, end;
struct thread_structure thread_blocks[number_of_threads];

//Initializing the array of numbers
for(int i = 0; i < size_of_array; i++)
	my_array_of_numbers[i] = 11;

//For loop to create many processes
for(int i = 0; i < number_of_cores; i++) {

	//Assigning start and end values for each process created
	start = i * size_of_array / number_of_cores;
	end = (i + 1) * size_of_array / number_of_cores;

	//Assigning the start and end values of each thread in a process
	thread_blocks[i].start = i * (end-start) / number_of_threads;
	thread_blocks[i].end = (i + 1) * (end-start) / number_of_threads;

	if(process_pid[i] = fork() != 0) {

	}
	else {

		//Creating threads inside the child process
		for(int j = 0; j < number_of_threads; j++)
			pthread_create(&thread_tid[i], NULL, isPrime, &thread_blocks[i]);

		//Waiting for the threads to finish executing
		for(int k = 0; k < number_of_threads; k++)
			pthread_join(thread_tid[i], NULL);

		exit(0);
	}

}

//Waiting for processes to finish to make sure it isn't executed sequentially
for(int i = 0; i < number_of_cores; i++)
	wait(&process_pid[i]);

//Destroying the mutex lock variable
pthread_mutex_destroy(&lock);



//Displaying how many prime numbers were found by gathering data written to the text file
int intermediate_value = 0, primes = 0;
FILE *ptr;

ptr = fopen("numbers.txt", "r");

//Reading values from the text file and inserting it into the variable intermediate_value
for(int i = 0; i < number_of_cores; i++) {
	fscanf(ptr, "%d", &intermediate_value);
	primes += intermediate_value;
}

fclose(ptr);

//Deleting the file numbers.txt created by the program
remove("numbers.txt");

printf("The total number of primes in the array is: %d\n", primes);

return 0;
}
