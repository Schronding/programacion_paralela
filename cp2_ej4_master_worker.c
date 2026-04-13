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
            printf("Exercise 4 requires at least 2 processes.\n");
        }
        MPI_Finalize();
        return 0;
    }

    const int tag = 0;

    if (rank == 0) {
        int workers = size - 1;
        int *tasks = (int *)malloc((size_t)workers * sizeof(int));
        int *results = (int *)malloc((size_t)workers * sizeof(int));

        if (tasks == NULL || results == NULL) {
            fprintf(stderr, "[master] Error allocating memory.\n");
            free(tasks);
            free(results);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 0; i < workers; i++) {
            tasks[i] = (i + 1) * 10;
            MPI_Send(&tasks[i], 1, MPI_INT, i + 1, tag, MPI_COMM_WORLD);
        }

        for (int i = 0; i < workers; i++) {
            int processed_value = 0;
            MPI_Status status;

            MPI_Recv(&processed_value, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            results[status.MPI_SOURCE - 1] = processed_value;

            printf(
                "[master] Received %d from rank %d\n",
                processed_value,
                status.MPI_SOURCE
            );
        }

        printf("[master] Final summary by worker:\n");
        for (int i = 0; i < workers; i++) {
            printf("  rank %d -> %d\n", i + 1, results[i]);
        }

        free(tasks);
        free(results);
    } else {
        int value = 0;
        MPI_Status status;

        MPI_Recv(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        value *= 2;
        MPI_Send(&value, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}