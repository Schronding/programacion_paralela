#!/bin/bash
set -e
echo "--- Testing Ejercicio 1 (Ping-Pong) ---"
mpirun -np 2 ./cp2_ej1_ping_pong.bin

echo -e "\n--- Testing Ejercicio 2 (Deadlock) ---"
echo "Expecting timeout..."
timeout 3s mpirun -np 4 --oversubscribe ./cp2_ej2_ring_deadlock.bin || echo "Timeout successful (Deadlock confirmed)"

echo -e "\n--- Testing Ejercicio 3 (SendRecv) ---"
mpirun -np 4 --oversubscribe ./cp2_ej3_ring_sendrecv.bin | head -n 4

echo -e "\n--- Testing Ejercicio 4 (Master/Worker) ---"
mpirun -np 4 --oversubscribe ./cp2_ej4_master_worker.bin

echo -e "\n--- Testing Ejercicio 5 (Reduction) ---"
mpirun -np 4 --oversubscribe ./cp2_ej5_manual_reduction.bin
