// Objetivo: Dividir y distribuir un conjunto de datos equitativamente.
/* Instrucciones: Supongamos que tienes 4 procesos. En el proceso 0, inicializa un 
arreglo de 16 números (del 1 al 16). Utiliza MPI_Scatter para repartir 4 números a 
cada proceso. Haz que cada proceso imprima el sub-arreglo que le tocó.*/

#include <mpi.h>
#include <stdio.h>
#define ARRAY_SIZE 16

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    int size; 
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int complete_array[ARRAY_SIZE];
    int sub_array[ARRAY_SIZE / 4];

    for (int c = 0; c < ARRAY_SIZE; c++){
        complete_array[c] = c + 1;
    }

    MPI_Scatter(complete_array, 
        4, MPI_INT, sub_array, 4,
        MPI_INT, 0, MPI_COMM_WORLD);

    printf("I am rank %d and I have the array [%d,%d,%d,%d]\n", rank, 
        sub_array[0], sub_array[1], sub_array[2], sub_array[3]);

    MPI_Finalize();
}