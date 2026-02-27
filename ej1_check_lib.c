#include <stdio.h>

// Solo incluimos la librería si OpenMP está activado en el compilador
#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    int thread_id = 0;
    int num_threads = 1;

    // Si no usamos el flag, el compilador ignora esta línea como si fuera un comentario
    #pragma omp parallel private(thread_id)
    {
        // Solo ejecutamos las funciones de OpenMP si el flag está activo
        #ifdef _OPENMP
        thread_id = omp_get_thread_num();
        num_threads = omp_get_num_threads();
        #endif

        printf("¡Hola! Soy el hilo %d de un total de %d hilos.\n", thread_id, num_threads);
    }

    return 0;
}