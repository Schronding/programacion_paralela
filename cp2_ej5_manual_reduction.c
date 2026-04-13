#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank = -1;
    int size = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int tag = 0;

    /* Simula un resultado parcial local de cada proceso. */
    double parcial = 100.0 + rank;

    if (rank == 0) {
        double suma_total = 0.0;

        for (int i = 1; i < size; i++) {
            double aporte = 0.0;
            MPI_Status status;

            MPI_Recv(&aporte, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            suma_total += aporte;

            printf(
                "[rank 0] Received %.2f from rank %d\n",
                aporte,
                status.MPI_SOURCE
            );
        }

        printf("[rank 0] Final total sum: %.2f\n", suma_total);
    } else {
        MPI_Send(&parcial, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}