#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    int rank;
    int size; 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    int one_process = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (size != 2) {
        if (rank == 0) {
            printf("Exercise 1 requires exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }
    if (rank == 0){
        int var = 666;
        MPI_Send(&var, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        MPI_Status status; 
        MPI_Recv(&var, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status); 
        printf("Final value is %d\n", var); 
    }
    else{
        int scnd_var; 
        MPI_Status scnd_status; 
        MPI_Recv(&scnd_var, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &scnd_status);
        scnd_var += 1;
        MPI_Send(&scnd_var, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); 
    return 0;
}