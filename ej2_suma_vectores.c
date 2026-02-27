#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define N 12 // Un número pequeño para ver claramente la salida en consola

int main() {
    int a[N], b[N], c[N];
    int i;
    int thread_id = 0;

    // 1. Inicializar los vectores (esto lo hace un solo hilo secuencialmente)
    for (i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i * 2;
    }

    printf("Iniciando la suma de vectores...\n");

    // 2. Región paralela: dividimos el bucle for
    // Nota: En un '#pragma omp parallel for', la variable iteradora 'i' 
    // se vuelve 'private' automáticamente por defecto.
    #pragma omp parallel for private(thread_id) shared(a, b, c)
    for (i = 0; i < N; i++) {
        #ifdef _OPENMP
        thread_id = omp_get_thread_num();
        #endif

        c[i] = a[i] + b[i];
        
        // Imprimimos qué hilo está haciendo qué iteración
        printf("Hilo %d calculando índice %d: %d + %d = %d\n", 
               thread_id, i, a[i], b[i], c[i]);
    }

    return 0;
}