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
            printf("Exercise 3 requires at least 2 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;
    const int tag = 0;

    const int count = 16 * 1024 * 1024;
    int *send_buffer = (int *)malloc((size_t)count * sizeof(int));
    int *recv_buffer = (int *)malloc((size_t)count * sizeof(int));
    if (send_buffer == NULL || recv_buffer == NULL) {
        fprintf(stderr, "[rank %d] Error allocating memory.\n", rank);
        free(send_buffer);
        free(recv_buffer);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < count; i++) {
        send_buffer[i] = rank;
        recv_buffer[i] = -1;
    }

    MPI_Status status;

    MPI_Sendrecv(
        send_buffer,
        count,
        MPI_INT,
        next,
        tag,
        recv_buffer,
        count,
        MPI_INT,
        prev,
        tag,
        MPI_COMM_WORLD,
        &status
    );

    printf(
        "[rank %d] sent %d ints to rank %d and received %d ints from rank %d (sample=%d)\n",
        rank,
        count,
        next,
        count,
        status.MPI_SOURCE,
        recv_buffer[0]
    );

    free(send_buffer);
    free(recv_buffer);

    MPI_Finalize();
    return 0;
}