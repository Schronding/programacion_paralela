#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

// Usamos un número muy grande para garantizar que los hilos colisionen
#define N 100000 

int main() {
    int suma_total = 0;
    int i;

    printf("Intentando sumar %d unos...\n\n", N);

    // =================================================================
    // CASO 1: EL DESASTRE (Violando las Condiciones de Bernstein)
    // =================================================================
    // Múltiples hilos intentan leer y escribir 'suma_total' simultáneamente.
    // Esto viola W1 ∩ W2 = ∅ (Write-After-Write) y W1 ∩ R2 = ∅ (Read-After-Write).
    
    #pragma omp parallel for shared(suma_total)
    for (i = 0; i < N; i++) {
        // En lenguaje máquina, esto son 3 pasos: LEER, SUMAR, ESCRIBIR.
        // Si el Hilo 1 y el Hilo 2 LEEN al mismo tiempo, ambos ven el mismo valor,
        // ambos suman 1, y ambos ESCRIBEN el mismo resultado. ¡Perdimos una suma!
        suma_total = suma_total + 1; 
    }
    
    printf("Resultado SIN proteccion: %d (INCORRECTO)\n", suma_total);

    // =================================================================
    // CASO 2: LA SOLUCIÓN SEGURA PERO LENTA (critical)
    // =================================================================
    suma_total = 0; // Reiniciamos la variable

    #pragma omp parallel for shared(suma_total)
    for (i = 0; i < N; i++) {
        // 'critical' crea un embudo: solo un hilo puede entrar a la vez.
        // Resuelve el problema matemático, pero destruye el rendimiento paralelo
        // porque serializa esta parte del código.
        #pragma omp critical
        {
            suma_total = suma_total + 1;
        }
    }
    printf("Resultado CON 'critical': %d (CORRECTO)\n", suma_total);

    // =================================================================
    // CASO 3: LA SOLUCIÓN DE HARDWARE (atomic)
    // =================================================================
    suma_total = 0;

    #pragma omp parallel for shared(suma_total)
    for (i = 0; i < N; i++) {
        // 'atomic' es como 'critical' pero optimizado a nivel de procesador.
        // Solo funciona para operaciones matemáticas súper simples (++, +=, -, etc).
        #pragma omp atomic
        suma_total = suma_total + 1;
    }
    printf("Resultado CON 'atomic'  : %d (CORRECTO)\n", suma_total);

    // =================================================================
    // CASO 4: LA SOLUCIÓN ELEGANTE Y NATIVA DE OPENMP (reduction)
    // =================================================================
    suma_total = 0;

    // 'reduction' le da a cada hilo una copia privada local temporal para sumar sin estorbarse.
    // Al final del bloque for, OpenMP recolecta todas las copias privadas y las suma al 'suma_total' global.
    // ¡Cero colisiones, máximo rendimiento!
    #pragma omp parallel for reduction(+:suma_total)
    for (i = 0; i < N; i++) {
        suma_total = suma_total + 1;
    }
    printf("Resultado CON 'reduction': %d (CORRECTO Y OPTIMIZADO)\n", suma_total);

    return 0;
}