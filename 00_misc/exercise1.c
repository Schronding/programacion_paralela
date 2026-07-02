#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(){
    int thread_id = 0;
    int num_threads = 1;

    #pragma omp parallel private(thread_id)
    {
        #ifdef _OPENMP
        thread_id = omp_get_thread_num();
        num_threads = omp_get_num_threads();
        #endif

        printf("Hi! I am the thread %d of a total of %d threads\n", thread_id, num_threads); 
    }

    return 0; 
}