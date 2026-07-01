/* 
Objetivo: Reconstruir datos procesados en un solo nodo central. 

Instrucciones: Extiende el ejercicio anterior. Una vez que cada proceso recibe sus 
4 números, debe multiplicarlos por 10. Luego, usa MPI_Gather para recolectar 
todos estos nuevos valores y agruparlos en un nuevo arreglo dentro del proceso 0. 
Imprime el arreglo resultante en el proceso 0.

*/

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

    for (int c = 0; c < ARRAY_SIZE / 4; c++){
        sub_array[c] *= 10;
    }

    MPI_Gather(sub_array, 4, MPI_INT, complete_array,
                4, MPI_INT, 0, MPI_COMM_WORLD); 

    if (rank == 0){
        printf("I am rank %d and the array I have is: [", rank);
    
        for (int c = 0; c < ARRAY_SIZE; c++){
            printf("%d, ", complete_array[c]);
        }
        printf("]\n");
    }

    MPI_Finalize();
}