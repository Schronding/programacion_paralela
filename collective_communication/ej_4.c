/*

• Objetivo: Realizar operaciones matemáticas globales sobre la red. 

• Instrucciones: Haz que cada proceso defina un número aleatorio entre 1 y 100. 
Utiliza MPI_Reduce dos veces: una para encontrar el valor máximo de todos los 
procesos (MPI_MAX) y otra para sumar todos los valores (MPI_SUM), enviando el 
resultado al proceso 0 para que los imprima.

*/

#include <mpi.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define THREADS 16

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv); 

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int array[THREADS]; 
    int random_ceiling = 100; 
    int random_floor = 1; 

    int random_value = (rand() % (random_ceiling - random_floor + 1)) + random_floor; 

    MPI_Gather(&random_value, 1, MPI_INT, array, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int max; 
    int sum; 

    MPI_Reduce(&random_value, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); 
    MPI_Reduce(&random_value, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); 

    if (rank == 0){
        printf("I am rank %d and the array of random values is [", rank);
        for (int c = 0; c < THREADS; c++){
            printf("%d, ", array[c]);
        }
        printf("]\n");
        printf("Of those, the maximum is %d and the total sum is %d\n", max, sum);
    }

    MPI_Finalize();
}