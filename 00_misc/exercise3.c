#ifdef _OPENMP
#include <omp.h>
#endif
#include <stdio.h>

int main(){
    int id_thread; 
    int shared_variable = 100;

    printf("Before parallelism: shared_variable = %d\n\n", shared_variable);

    #pragma omp parallel private(id_thread) shared (shared_variable)
    {
        #ifdef _OPENMP
        id_thread = omp_get_thread_num();
        #else
        id_thread = 0;
        #endif

        int my_calc = id_thread * 10; 

        printf("I am the thread %d, My private calc is %d, I read the information %d\n", id_thread, my_calc, shared_variable);
    }

    return 0; 

}