#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);
    // If I recall well the process number of process was defined in
    // the terminal with mpi_run... but how can I declare which 
    // process sends what? 
    int rank;
    int size; 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 
    int one_process = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (size != 2) {
        if (rank == 0) {
            printf("This program must be run with exactly 2 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }
    // I understand that is variable will be filled independetly by the amount of 
    // process that I have, but I still don't see how can I control that. I can 
    // imagine that `one_process` will always be filled with rank 0 first, but then
    // I don't see a reason why rank 2, 5, or whatever goes next... but if it was
    // forced to go sequentially, then I would have a much more predictable behavior
    // It seem it doesn't go sequentially, and that I might not be able to store the
    // value in a variable as I did in the first line of the else statement
    if (rank == 0){
        int var = 666;
        MPI_Send(&var, 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        // I don't see how MPI_INIT could be a data type... 
        // I assume it goes first the number of the destination 
        // procesor and then the value
        // I wonder if there is a VS Code extension for MPI, as the lack of 
        // intellisense makes things more difficult.
        // I thought that the communication functions were relatively minimalistic, 
        // but it seems it is all the opposite, as they require up to 6 or 7 parameters
        // in order to work correctly. Those parameters are:
        // 1. Pointer to the data
        // 2. Number of elements
        // 3. Type of data
        // 4. Destination or origin (what rank is the other process)
        // 5. Tag
        // 6. Communicator (I wonder if there is another one than `MPI_COMM_WORLD`)
        // 7. ONLY IN RECEIVE: an status
        MPI_Status status; 
        MPI_Recv(&var, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status); 
        printf("The value is %d\n", var); 
        // I assume this will be run only when there is an actual message being sent,
        // but I might get an error as there is no value at the first iteration
        // of the program; otherwise I might get a truncated trash value. 
    }
    else{
        int scnd_var; 
        MPI_Status scnd_status; 
        MPI_Recv(&scnd_var, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &scnd_status);
        // I assume I just put from whom I expect the message
        scnd_var += 1;
        MPI_Send(&scnd_var, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize(); 
    return 0;
}