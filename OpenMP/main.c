#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

double getRandomNumber(unsigned int *seed) {
    return (double) rand_r(seed) * 2 / (double) (RAND_MAX) - 1;
}

long double SequentialPi(long long iterations) {
    long long numberInCircle = 0;
    unsigned int seed = (unsigned int) time(NULL);
    
    for (long long int i = 0; i < iterations; i++) {
        double x = getRandomNumber(&seed);
        double y = getRandomNumber(&seed);
        double distanceSquared = x*x + y*y;
        
        if (distanceSquared <= 1) 
        	numberInCircle++;
    }

    return 4 * numberInCircle / ((double) iterations);
}

long double ParallelPi(long long iterations) {
    long long numberInCircle = 0;
	#pragma omp parallel num_threads(8)
	{
    /* Since rand() is not thread-safe, we generate a seed per thread and use rand_r() to 
		retrieve a thread-safe random value. */
    	unsigned int seed = (unsigned int) time(NULL) + (unsigned int) omp_get_thread_num();
    	#pragma omp for reduction(+: numberInCircle)
        for (long long int i = 0; i < iterations; i++) {
            double x = getRandomNumber(&seed);
            double y = getRandomNumber(&seed);
            double distanceSquared = x*x + y*y;
            	
            if (distanceSquared <= 1) 
            	numberInCircle++;
        }
    }
    return 4 * numberInCircle/((double) iterations);
}

int main() {
    struct timeval start, end;

    double time1, time2;

    long long iterations = 10000000000;

    printf("\nTIMING SEQUENTIAL: \n");
    gettimeofday(&start, NULL);
    long double sequentialPi = SequentialPi(iterations);
    gettimeofday(&end, NULL);
    time1 = end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000;
    printf("Took %f seconds\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("\nTIMING PARALLEL: \n");
    gettimeofday(&start, NULL);
    long double parallelPi = ParallelPi(iterations);
    gettimeofday(&end, NULL);
    time2 = end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000;
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Sequential: Estimated π = %.10Lf \n", sequentialPi);
    printf("Parallel: Estimated π = %.10Lf \n", parallelPi);

    printf("Speed up: %f\n", time1/time2);

    return 0;
}
