/* 

• Objetivo: Distribuir el resultado de una reducción a todos los nodos.

• Instrucciones: Cada proceso tiene una variable que representa su "carga de 
trabajo" (puede ser simplemente su rango + 1). Usa MPI_Allreduce para calcular la 
suma total de la carga de trabajo, de modo que todos los procesos conozcan el 
resultado final. Cada proceso debe calcular e imprimir su porcentaje individual 
respecto a la carga total.

*/

#include <mpi.h>
#include <stdlib.h>
