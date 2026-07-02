#include <stdio.h>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char ** argv){
    MPI_Init(&argc, &argv);
    int rank; 
    int size; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int value = rank * 10;
    sleep(rank);
    printf("Hello World \nI am proccess %d of %d and my value is %d\n", rank, size, value);
    if (rank == 0){
        printf("I am the administrator and I coordinate the work \n");
    }
    else{
        printf("I am a worker and I follow orders \n");
    }

    printf("\n");
    MPI_Finalize();
    return 0; 
}