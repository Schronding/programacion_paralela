/*
Ejercicio 2: El apretón de manos estricto (MPI_Ssend) 
• Objetivo: Usar comunicación síncrona para depurar o forzar un orden estricto. 
• Instrucciones: Crea un "Ping-Pong" entre el proceso 0 y el proceso 1. Intercambien 
un mensaje 5 veces usando exclusivamente MPI_Ssend. Observa y comenta en el 
código por qué esto garantiza que ningún proceso se adelante al otro.
*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) printf("At least 2 processes are required.\n");
        MPI_Finalize();
        return 0;
    }

    /*
     * El uso de MPI_Ssend (Synchronous Send) garantiza que la función no retorne
     * en el proceso emisor hasta que el proceso receptor haya comenzado a recibir
     * el mensaje con un MPI_Recv que concuerde. Esto asegura que ambos 
     * procesos están sincronizados en ese punto de ejecución exacto, evitando que un 
     * emisor sature la red o avance demasiado rápido.
     */
    
    for (int i = 0; i < 5; i++) {
        if (rank == 0) {
            data = i;
            MPI_Ssend(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Process 0 completed ping-pong %d\n", i);
        } else if (rank == 1) {
            MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Ssend(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("Process 1 completed ping-pong %d\n", i);
        }
    }

    MPI_Finalize();
    return 0;
}

