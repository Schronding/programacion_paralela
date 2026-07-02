// Objetivo: Distribuir información idéntica a todos los nodos de forma eficiente.
/* Instrucciones: El proceso raíz (0) debe inicializar una variable entera con un valor 
ingresado por el usuario (o hardcodeado). Usa MPI_Bcast para enviar este valor a 
todos los demás procesos en MPI_COMM_WORLD. Haz que cada proceso imprima 
su rango (rank) y el valor que acaba de recibir. */ 

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);

    int rank;
    int size; 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = 420; 

    MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD); 

    printf("I am %d and the value I just received is %d\n", rank, value);

    MPI_Finalize();

    return 0;
}