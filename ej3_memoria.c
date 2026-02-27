#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main() {
    // Variable declarada ANTES de la región. Por defecto sería compartida.
    int id_hilo; 
    int variable_compartida = 100;

    printf("Antes del paralelo: variable_compartida = %d\n\n", variable_compartida);

    // Forzamos a que 'id_hilo' sea una copia privada para cada hilo, 
    // pero mantenemos 'variable_compartida' accesible para todos.
    #pragma omp parallel private(id_hilo) shared(variable_compartida)
    {
        #ifdef _OPENMP
        id_hilo = omp_get_thread_num();
        #else
        id_hilo = 0;
        #endif

        // Cada hilo tiene su PROPIA copia de 'id_hilo'
        int mi_calculo = id_hilo * 10; 

        // CRÍTICO: Todos los hilos intentan leer la misma variable compartida simultáneamente
        printf("Soy el hilo %d. Mi calculo privado es %d. Leo la compartida: %d\n", 
               id_hilo, mi_calculo, variable_compartida);
               
        // OJO: Si aquí hiciéramos variable_compartida++ sin protección, 
        // causaríamos una condición de carrera (race condition).
    }

    return 0;
}