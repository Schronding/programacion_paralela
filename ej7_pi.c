#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// 100 millones de pasos para asegurar un buen tiempo de cómputo y precisión
#define NUM_STEPS 100000000 

int main() {
    int i;
    double x, pi, sum = 0.0;
    double step = 1.0 / (double)NUM_STEPS;
    double inicio, fin;

    printf("Calculando Pi con %d pasos...\n\n", NUM_STEPS);

    // -----------------------------------------------------------------
    // VERSIÓN SECUENCIAL
    // -----------------------------------------------------------------
    sum = 0.0;
    inicio = omp_get_wtime();
    for (i = 0; i < NUM_STEPS; i++) {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    pi = step * sum;
    fin = omp_get_wtime();
    printf("SECUENCIAL:\n");
    printf("  Pi = %.15f\n", pi);
    printf("  Tiempo = %f segundos\n\n", fin - inicio);

    // -----------------------------------------------------------------
    // VERSIÓN PARALELA
    // -----------------------------------------------------------------
    sum = 0.0;
    inicio = omp_get_wtime();

    // TRAMPA: 'x' DEBE ser privada. Si fuera compartida, 
    // los hilos se sobreescribirían la posición del rectángulo.
    // 'sum' DEBE ser una reducción para evitar la condición de carrera al acumular el área.
    
    #pragma omp parallel for private(x) reduction(+:sum)
    for (i = 0; i < NUM_STEPS; i++) {
        // 1. Calcular el centro del rectángulo actual
        x = (i + 0.5) * step;
        
        // 2. Calcular la altura del rectángulo (la función 4 / (1 + x^2))
        // 3. Acumular en la suma parcial
        sum = sum + 4.0 / (1.0 + x * x);
    }

    // 4. Multiplicar la suma total de las alturas por el ancho base (step)
    pi = step * sum;
    fin = omp_get_wtime();
    printf("PARALELA (%d hilos):\n", omp_get_max_threads());
    printf("  Pi = %.15f\n", pi);
    printf("  Tiempo = %f segundos\n", fin - inicio);

    return 0;
}