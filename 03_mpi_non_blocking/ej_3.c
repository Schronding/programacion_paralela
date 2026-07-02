/*
Ejercicio 3: Gestión manual del tanque de agua (MPI_Bsend) 
• Objetivo: Dominar la asignación explícita de memoria. 
• Instrucciones: Haz que el proceso 0 envíe un arreglo de 1000 enteros al proceso 1 
usando MPI_Bsend. Debes usar MPI_Buffer_attach calculando el tamaño 
matemáticamente correcto: el tamaño del mensaje en bytes más la constante 
MPI_BSEND_OVERHEAD. Al final, libera la memoria con MPI_Buffer_detach.
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int data[1000];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) printf("At least 2 processes are required.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        for (int i = 0; i < 1000; i++) {
            data[i] = i;
        }

        int buffer_size = sizeof(int) * 1000 + MPI_BSEND_OVERHEAD;
        void *buffer = malloc(buffer_size);

        if (buffer == NULL) {
            printf("Error in malloc\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Buffer_attach(buffer, buffer_size);

        MPI_Bsend(data, 1000, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Process 0 sent the array using MPI_Bsend.\n");

        void *b_ptr;
        int b_size;
        MPI_Buffer_detach(&b_ptr, &b_size);
        free(buffer);
        printf("Process 0 detached its local buffer.\n");

    } else if (rank == 1) {
        MPI_Recv(data, 1000, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process 1 received the array correctly. data[999] = %d\n", data[999]);
    }

    MPI_Finalize();
    return 0;
}

