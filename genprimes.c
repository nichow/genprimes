#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/* declare our variables for the command line args and our array */
int N, threads;
int* a;
/* function declarations */
void local_primes();

int main (int argc, char* argv[]) {
	/* declare time variables and grab command line args */
	double tstart = 0.0, ttaken;
	N = atoi(argv[1]);
	threads = strtol(argv[2], NULL, 10);
	/* allocate space for the array, will hold every number up to N */
	a = (int*)malloc(N * sizeof(int));
	/* declare the filename with N and the pointer */
	char filename[255] = {0};
	sprintf(filename, "%d.txt", N);
	FILE* f = fopen(filename, "w");
	/* N is decremented for implementation purposes */
	N -= 2;
	tstart = omp_get_wtime(); 
	local_primes();
	ttaken = omp_get_wtime() - tstart;
	printf("Time taken for the main part: %f\n", ttaken);
	/* sequentially write each one to the file */
	int prev = 2;
	int counter = 1;
	int i;
	for(i = 0; i < N; ++i) {
		if(a[i] != 1) {
			fprintf(f, "%d, %d, %d\n", counter, a[i], a[i] - prev);
			prev = a[i];
			counter++;
		}
	}
	free(a);
	fclose(f);

	return 0;
}

void local_primes() {
	int i, j;

	/* fill the array with ints from 2 to N */
#	pragma omp parallel for num_threads(threads)
	for(i = 0; i < N; ++i)
		a[i] = i + 2;

	int sentinel = (N + 1) / 2;

#	pragma omp parallel for num_threads(threads)
	for(i = 0; i < sentinel; ++i) {
		if(a[i] != 1) {
#			pragma omp parallel for num_threads(threads)
			for(j = i + 1; j < N; ++j) {
				/* test the remaining values of the list against the value of index i 
				   if the element of the list is divisible by that value, set it to 0 
				   and if the value we're looking at is 0 we skip over it*/
				if(a[i] != 1 && a[j] % a[i] == 0 && a[j] != 1) {
					a[j] = 1;
				}
			}
		}
	}
}

