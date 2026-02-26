#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define N 100000

int main(){
    int total_suma = 0;
    int index; 

    printf("Trying to sum %d unos...\n\n", N); 

    /* UNSAFE: multiple threads share total_suma with no protection.
     * "total_suma = total_suma + 1" looks like one operation but it is actually three:
     *   1. READ  total_suma from memory
     *   2. ADD   1 to it
     *   3. WRITE the result back
     *
     * Two threads can both execute step 1 at the same time, getting the same value.
     * Both then add 1 and write back the same result, so one increment is lost.
     * This is called a RACE CONDITION. The final value will be less than N. */
    #pragma omp parallel for shared(total_suma)
    for (index = 0; index < N; index++){
        total_suma = total_suma + 1;
    }

    printf("Result without protection %d INCORRECT\n", total_suma);

    total_suma = 0;

    /* SAFE: "atomic" makes the read-modify-write uninterruptible.
     * While one thread is executing those 3 steps on total_suma,
     * all other threads that want to touch total_suma must WAIT.
     * This guarantees no increment is ever lost.
     *
     * atomic is cheaper than "critical" (which locks an entire block of code)
     * because it only protects a single memory operation, and modern CPUs
     * have hardware instructions designed exactly for this. */
    #pragma omp parallel for shared(total_suma)
    for (index = 0; index < N; index ++){
        #pragma omp atomic
        total_suma = total_suma + 1;
    }

    printf("Result with atomic: %d CORRECT\n", total_suma);

    total_suma = 0;

    #pragma omp parallel for reduction(+:total_suma)
    for (index = 0; index < N; index++){
        total_suma = total_suma + 1;
    }

    printf("Result with 'Reduction': %d (CORRECT AND OPTIMIZED)\n", total_suma);

    return 0;
}