/*
Ejercicio 4: Solapamiento básico (Overlapping con MPI_Isend e MPI_Irecv) 
• Objetivo: Lograr solapar la comunicación de red con el cómputo intensivo de la CPU. 
• Instrucciones: El proceso 0 y el proceso 1 deben intercambiar un arreglo grande de 
datos. Usa MPI_Isend y MPI_Irecv para iniciar el intercambio. Antes de llamar a 
MPI_Wait para sincronizar, escribe un bucle for que imprima los números del 1 al 
1000 simulando un "cálculo de CPU paralelo".
*/

#include <mpi.h>
#include <stdio.h>

#define ARRAY_SIZE 100000

int main(int argc, char *argv[]) {
    int rank, size;
    int send_data[ARRAY_SIZE];
    int recv_data[ARRAY_SIZE];
    MPI_Request reqs[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) printf("At least 2 processes are required.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 0 || rank == 1) {
        int partner = (rank == 0) ? 1 : 0;
        
        for (int i = 0; i < ARRAY_SIZE; i++) {
            send_data[i] = rank * ARRAY_SIZE + i;
        }

        MPI_Irecv(recv_data, ARRAY_SIZE, MPI_INT, partner, 0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Isend(send_data, ARRAY_SIZE, MPI_INT, partner, 0, MPI_COMM_WORLD, &reqs[1]);

        printf("Process %d computing in CPU while sending...\n", rank);
        for (int i = 1; i <= 1000; i++) {
            printf("%d ", i);
        }
        printf("\n");

        MPI_Waitall(2, reqs, MPI_STATUSES_IGNORE);

        printf("Process %d finished exchange. First received data: %d\n", rank, recv_data[0]);
    }

    MPI_Finalize();
    return 0;
}

