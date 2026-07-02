#include <stdio.h>
#include <stdlib.h> // Para malloc y free

#ifdef _OPENMP
#include <omp.h>
#endif

#define N 50000000 // 50 millones de iteraciones para que el procesador "sude"

int main() {
    double inicio, fin;
    long long suma_total = 0;
    int i;

    printf("Calculando suma de %d elementos...\n\n", N);

    // -----------------------------------------------------------------
    // PRUEBA 1: Secuencial puro (Nuestro tiempo base)
    // -----------------------------------------------------------------
    inicio = omp_get_wtime();
    for (i = 0; i < N; i++) {
        suma_total += 1;
    }
    fin = omp_get_wtime();
    printf("1. Tiempo SECUENCIAL : %f segundos (Suma: %lld)\n", fin - inicio, suma_total);

    // -----------------------------------------------------------------
    // PRUEBA 2: Paralelo con 'critical' (El cuello de botella)
    // -----------------------------------------------------------------
    suma_total = 0;
    inicio = omp_get_wtime();
    #pragma omp parallel for shared(suma_total)
    for (i = 0; i < N; i++) {
        #pragma omp critical
        {
            suma_total += 1;
        }
    }
    fin = omp_get_wtime();
    printf("2. Tiempo CRITICAL   : %f segundos (Suma: %lld)\n", fin - inicio, suma_total);

    // -----------------------------------------------------------------
    // PRUEBA 3: Paralelo con 'reduction' (La forma correcta)
    // -----------------------------------------------------------------
    suma_total = 0;
    inicio = omp_get_wtime();
    #pragma omp parallel for reduction(+:suma_total)
    for (i = 0; i < N; i++) {
        suma_total += 1;
    }
    fin = omp_get_wtime();
    printf("3. Tiempo REDUCTION  : %f segundos (Suma: %lld)\n", fin - inicio, suma_total);

    return 0;
}