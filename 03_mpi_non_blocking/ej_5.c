/*
Ejercicio 5: El escáner de red (MPI_Test) 
• Objetivo: Liberar a la CPU para tareas útiles mientras el paquete llega. 
• Instrucciones: El proceso 1 simula un trabajo largo haciendo un sleep(3) y luego 
envía un dato al proceso 0. El proceso 0 inicia un MPI_Irecv y entra en un bucle 
while. Dentro del bucle, usa MPI_Test actualizando una bandera (flag). Si la bandera 
es 0, imprime "Trabajando en otra cosa... " y espera medio segundo; si es diferente 
de 0, rompe el bucle e imprime el dato recibido.
*/

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int data = 0;
    MPI_Request request;
    int flag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) printf("At least 2 processes are required.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank == 1) {
        sleep(3);
        data = 42;
        MPI_Send(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Process 1 sent the data.\n");
    } else if (rank == 0) {
        MPI_Irecv(&data, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        
        while (!flag) {
            MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
            if (!flag) {
                printf("Working on something else...\n");
                usleep(500000); 
            }
        }
        
        printf("Process 0 received the data: %d\n", data);
    }

    MPI_Finalize();
    return 0;
}
