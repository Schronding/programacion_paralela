#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(){
    #pragma omp parallel
    printf("Hola desde el hilo %i\n", omp_get_thread_num());
    return 0;
}