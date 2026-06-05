import subprocess
import re
import os
import matplotlib.pyplot as plt

def run_openmp_benchmark(f0, K, T, sigma, mu, mc_cycles):
    threads = [1, 2, 4, 8, 16]
    times = []
    
    print(f"--- Iniciando Benchmark OpenMP (N={mc_cycles}) ---")
    for t in threads:
        os.environ["OMP_NUM_THREADS"] = str(t)
        cmd = ["./final_proyect_openMP.bin", str(f0), str(K), str(T), str(sigma), str(mu), str(mc_cycles)]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        match = re.search(r"Parallel execution time:\s+([\d\.e-]+)\s+seconds", result.stdout)
        if match:
            exec_time = float(match.group(1))
            times.append(exec_time)
            print(f"Hilos: {t:2d} | Tiempo Paralelo: {exec_time:.6f} s")
        else:
            print(f"Error extrayendo tiempo para {t} hilos.")
            times.append(None)
            
    return threads, times

def plot_speedup(threads, times, mc_cycles):
    T_1 = times[0]
    speedup_empirico = [T_1 / tp if tp else 0 for tp in times]
    speedup_ideal = threads
    
    plt.figure(figsize=(10, 6))
    plt.plot(threads, speedup_ideal, 'k--', marker='s', label='Speedup Teórico Ideal (Lineal)')
    plt.plot(threads, speedup_empirico, 'r-', marker='o', linewidth=2, label=f'Speedup Empírico (N={mc_cycles})')
    
    plt.title(f'Curva de Escalabilidad en OpenMP\n(Simulación Monte Carlo N={mc_cycles})', fontsize=14)
    plt.xlabel('Número de Hilos (P)', fontsize=12)
    plt.ylabel('Speedup (Sp)', fontsize=12)
    plt.xticks(threads)
    plt.grid(True, linestyle=':', alpha=0.7)
    plt.legend(fontsize=11)
    
    filename = f'curva_escalabilidad_N{mc_cycles}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f"¡Gráfico guardado exitosamente como {filename}!\n")

if __name__ == "__main__":
    
    f0, K, T_exp, sigma, mu = 5, 6, 9, 12, 52
    
    threads, times_52 = run_openmp_benchmark(f0, K, T_exp, sigma, mu, 52)
    plot_speedup(threads, times_52, 52)