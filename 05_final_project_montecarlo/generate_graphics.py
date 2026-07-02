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

def plot_combined_metrics(threads, times, mc_cycles):
    T_1 = times[0]
    speedup_empirico = [T_1 / tp if tp else 0 for tp in times]
    speedup_ideal = threads
    eficiencia = [(sp / p) * 100 for sp, p in zip(speedup_empirico, threads)]
    
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    
    ax1.plot(threads, speedup_ideal, 'k--', marker='s', label='Speedup Teórico Ideal')
    ax1.plot(threads, speedup_empirico, 'r-', marker='o', linewidth=2.5, label=f'Speedup Empírico')
    ax1.set_title(f'Curva de Escalabilidad (N={mc_cycles})', fontsize=14, fontweight='bold')
    ax1.set_xlabel('Número de Hilos (P)', fontsize=12)
    ax1.set_ylabel('Speedup (Sp)', fontsize=12)
    ax1.set_xticks(threads)
    ax1.grid(True, linestyle=':', alpha=0.7)
    ax1.legend(fontsize=11)
    
    ax2.plot(threads, eficiencia, 'b-', marker='^', linewidth=2.5, label='Eficiencia Empírica')
    ax2.set_title(f'Curva de Eficiencia (N={mc_cycles})', fontsize=14, fontweight='bold')
    ax2.set_xlabel('Número de Hilos (P)', fontsize=12)
    ax2.set_ylabel('Eficiencia (%)', fontsize=12)
    ax2.set_xticks(threads)
    ax2.set_ylim(-5, 105) 
    ax2.grid(True, linestyle=':', alpha=0.7)
    ax2.legend(fontsize=11)
    
    plt.tight_layout()
    filename = f'metricas_completas_N{mc_cycles}.png'
    plt.savefig(filename, dpi=300, bbox_inches='tight')
    print(f"\n¡Gráfico doble guardado exitosamente como {filename}!")

if __name__ == "__main__":
    f0, K, T_exp, sigma, mu = 5, 6, 9, 12, 52
    
    print("\nEjecutando escenario microscópico (N=52)...")
    threads_52, times_52 = run_openmp_benchmark(f0, K, T_exp, sigma, mu, 52)
    plot_combined_metrics(threads_52, times_52, 52)
    
    print("\nEjecutando escenario de carga media (N=10000)...")
    threads_10k, times_10k = run_openmp_benchmark(f0, K, T_exp, sigma, mu, 10000)
    plot_combined_metrics(threads_10k, times_10k, 10000)