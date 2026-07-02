#include <stdio.h>
#include <unistd.h> // Para la función sleep()

#ifdef _OPENMP
#include <omp.h>
#endif

// Solo 16 iteraciones, pero algunas serán "pesadas"
#define N 16 

int main() {
    int i;
    double inicio, fin;

    printf("Iniciando bucle desbalanceado...\n\n");

    // ==========================================================
    // ESCENARIO A: Planificación Estática (Por defecto)
    // Reparte bloques fijos. Si a un hilo le tocan las tareas 
    // pesadas, los demás hilos terminan y se quedan sin hacer nada.
    // ==========================================================
    inicio = omp_get_wtime();
    
    #pragma omp parallel for schedule(static)
    for (i = 0; i < N; i++) {
        // Solo las primeras 4 iteraciones son pesadas (100ms),
        // el resto son ligeras (5ms). Con bloques estáticos,
        // el hilo 0 cargará con casi todo el trabajo pesado.
        if (i < 4) {
            usleep(100000); 
        } else {
            usleep(5000);   
        }
        
        printf("Hilo %d terminó la iteración %d\n", omp_get_thread_num(), i);
    }
    fin = omp_get_wtime();
    printf("--> Tiempo TOTAL Estático: %f segundos\n\n", fin - inicio);

    // ==========================================================
    // ESCENARIO B: Planificación Dinámica (Trabajo bajo demanda)
    // Los hilos toman una iteración. Cuando terminan, piden otra.
    // Nadie se queda de brazos cruzados.
    // ==========================================================
    inicio = omp_get_wtime();
    
    #pragma omp parallel for schedule(dynamic, 1)
    for (i = 0; i < N; i++) {
        if (i < 4) {
            usleep(100000); 
        } else {
            usleep(5000);   
        }
    }
    fin = omp_get_wtime();
    printf("--> Tiempo TOTAL Dinámico: %f segundos\n", fin - inicio);

    return 0;
}