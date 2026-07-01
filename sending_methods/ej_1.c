/*

• Objetivo: Comprender el paso de mensajes estándar y evitar interbloqueos simples. 

• Instrucciones: Escribe un programa donde N procesos formen un anillo. El proceso 
0 envía un número entero (ej. 100) al proceso 1; el proceso 1 le suma 1 y lo envía al 
2, y así sucesivamente hasta que el último proceso se lo devuelve al proceso 0. 
Imprime el resultado final. 

*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int number;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            printf("More than one process is required for this exercise.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        number = 100;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        MPI_Recv(&number, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process 0 received the final result: %d\n", number);
    } else {
        MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        number++;
        int dest = (rank + 1) % size;
        MPI_Send(&number, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
