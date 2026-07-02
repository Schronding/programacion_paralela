#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank = -1;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            printf("Exercise 2 requires at least 2 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;
    const int tag = 0;

    const int count = 16 * 1024 * 1024;
    int *buffer = (int *)malloc((size_t)count * sizeof(int));
    if (buffer == NULL) {
        fprintf(stderr, "[rank %d] Error allocating memory.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < count; i++) {
        buffer[i] = rank;
    }

    printf("[rank %d] Sending to rank %d...\n", rank, next);
    fflush(stdout);
    MPI_Send(buffer, count, MPI_INT, next, tag, MPI_COMM_WORLD);

    MPI_Status status;
    printf("[rank %d] Waiting for message from rank %d...\n", rank, prev);
    fflush(stdout);
    MPI_Recv(buffer, count, MPI_INT, prev, tag, MPI_COMM_WORLD, &status);

    printf("[rank %d] Received from rank %d.\n", rank, status.MPI_SOURCE);

    free(buffer);
    MPI_Finalize();
    return 0;
}