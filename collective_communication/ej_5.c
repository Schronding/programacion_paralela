/* 

• Objetivo: Distribuir el resultado de una reducción a todos los nodos.

• Instrucciones: Cada proceso tiene una variable que representa su "carga de 
trabajo" (puede ser simplemente su rango + 1). Usa MPI_Allreduce para calcular la 
suma total de la carga de trabajo, de modo que todos los procesos conozcan el 
resultado final. Cada proceso debe calcular e imprimir su porcentaje individual 
respecto a la carga total.

*/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv); 
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int work_load = rank + 1; 
    int total_work;

    MPI_Allreduce(&work_load, &total_work, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    float percentage = (work_load  * 100.0 / total_work);
    printf("I am rank %d and my percentaje of the total work load is %.2f %%\n", rank, percentage);

    MPI_Finalize();
}