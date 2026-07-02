# Repositorio de Prácticas de Programación Paralela y Distribuida

Este repositorio documenta el desarrollo progresivo de aplicaciones concurrentes y de alto rendimiento utilizando arquitecturas de memoria compartida y distribuida. El código está escrito principalmente en C y CUDA C, haciendo uso intensivo de las APIs de **OpenMP** y **MPI (Message Passing Interface)** para la orquestación de hilos y paso de mensajes en clústeres.

Desarrollado como parte del plan de estudios de la carrera de Tecnología en la UNAM ENES Juriquilla.

## Stack Tecnológico y Entorno

* **Lenguajes:** C, C++, Python (para perfilamiento y graficación de datos).
* **APIs / Frameworks:** OpenMP (Memoria Compartida), OpenMPI (Memoria Distribuida).
* **Aceleración por Hardware:** CUDA (NVIDIA GPU Computing).
* **Entorno de Desarrollo:** Debian Linux (vía WSL) / VS Code.
* **Compiladores:** GCC (con flag `-fopenmp`), `mpicc`, `nvcc`.

## Índice de Proyectos

El repositorio está estructurado temáticamente, escalando desde la sincronización básica de hilos en CPU hasta algoritmos paralelos híbridos ejecutados en GPU.

### 1. Fundamentos de Memoria Compartida (OpenMP)
Ejercicios enfocados en la paralelización a nivel de hilos dentro de un solo nodo.
* **`01_openmp_fundamentals`**: 
  * Identificación y prevención de condiciones de carrera mediante directivas `#pragma omp atomic` y `critical`.
  * Estrategias de distribución de carga de trabajo (`schedule static` vs `dynamic`).
  * Implementación de operaciones de reducción global (ej. cálculo de Series Armónicas y aproximaciones de Pi).

### 2. Memoria Distribuida y Paso de Mensajes (MPI)
Ejercicios enfocados en la comunicación entre procesos independientes a través de una red.
* **`02_mpi_point_to_point`**: Fundamentos de comunicación bloqueante (`MPI_Send`, `MPI_Recv`). Resolución de interbloqueos (Deadlocks) en topologías de anillo usando `MPI_Sendrecv`, e implementación del patrón Maestro-Trabajador.
* **`03_mpi_non_blocking`**: Optimización del rendimiento solapando comunicación y cómputo (Overlapping) mediante primitivas no bloqueantes (`MPI_Isend`, `MPI_Irecv`, `MPI_Test`). Control estricto de buffers (`MPI_Bsend`).
* **`04_mpi_collective`**: Operaciones globales para la distribución y recolección de datos (`MPI_Bcast`, `MPI_Scatter`, `MPI_Gather`). Consenso y sincronización global mediante `MPI_Allreduce`.

### 3. Proyecto Final: Fijación de Precios de Opciones (Monte Carlo)
Implementación de un modelo financiero para calcular el precio de opciones europeas simulando trayectorias estocásticas (Movimiento Browniano Geométrico).
* **`05_final_project_montecarlo`**: 
  * Comparativa de rendimiento entre soluciones secuenciales, OpenMP (Multicore CPU) y CUDA (Manycore GPU).
  * Análisis empírico de la **Ley de Amdahl**, evaluando el *overhead* de orquestación y la latencia de transferencia de memoria PCI-Express frente a cargas de trabajo computacionalmente densas ($N \ge 10^7$).
  * Perfilamiento de datos y automatización de gráficas de escalabilidad con Python.

## Cómo compilar y ejecutar

1. Asegúrate de tener instalado el compilador correspondiente (`gcc`, `openmpi-bin`, `nvidia-cuda-toolkit`) en tu entorno Debian/WSL.
2. Navega al directorio del proyecto deseado.

**Para compilar OpenMP:**
```bash
gcc -fopenmp archivo.c -o archivo.bin
./archivo.bin
```

**Para compilar y ejecutar MPI**:
```Bash
mpicc archivo.c -o archivo.bin
mpirun -np 4 ./archivo.bin  # Ejecuta con 4 procesos
```

**Para compilar CUDA**:
```Bash
nvcc archivo.cu -o archivo.bin
./archivo.bin
```