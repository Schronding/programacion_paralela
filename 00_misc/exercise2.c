#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define N 8

int main(){
    int a[N];
    int b[N];
    int c[N];

    int index;
    int thread_id = 0;

    for (index = 0; index < N; index++){
        a[index] = index;
        b[index] = index * 2;
    }

    printf("Beginning the sum of vectors \n");

    /* The number of threads OpenMP uses is controlled by the environment variable
     * OMP_NUM_THREADS (with an S at the end).
     * If you set it wrong (e.g. OMP_NUM_THREAD without the S), OpenMP ignores it
     * and uses as many threads as CPU cores the machine has.
     *
     * Correct way to limit to 3 threads before running:
     *   export OMP_NUM_THREADS=3
     *
     * "parallel for" tells OpenMP to split the loop iterations across threads.
     * - private(thread_id): each thread gets its own copy of thread_id (not shared).
     * - shared(a, b, c):    all threads read/write the same arrays (safe here because
     *                       each thread only touches its own index).
     */
    #pragma omp parallel for private(thread_id) shared(a, b, c)
    for (index = 0; index < N; index++)
    {
        #ifdef _OPENMP
        thread_id = omp_get_thread_num(); /* get the ID of the current thread (0-based) */
        #endif

        c[index] = a[index] + b[index];
        printf("Thread %d calculating index %d: %d + %d = %d\n", thread_id, index, a[index], b[index], c[index]); 
    }
    
    return 0; 
}