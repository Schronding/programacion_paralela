// 'DND:' means DO NOT DELETE. That is not a comment that I want to get rid of. 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>

double NoD(double x);

static double now_seconds(void) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

__global__ void init_curand_states(curandState *states, unsigned long seed, int n) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		curand_init(seed, i, 0, &states[i]);
	}
}

__global__ void compute_payoff(curandState *states,
							   double *payoff,
							   int n,
							   double f0,
							   double K,
							   double T,
							   double sigma,
							   double mu,
							   double pi) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		curandState local_state = states[i];
		double a = curand_uniform_double(&local_state);
		double b = curand_uniform_double(&local_state);
		if (a <= 0.0 || a > 1.0 || b <= 0.0 || b > 1.0) {
			printf(" Random number generator problem : %g %g\n", a, b);
			payoff[i] = 0.0;
			states[i] = local_state;
			return;
		}
		/* Standardized normal distribution */
		double eps = sqrt(-2.0 * log(a)) * cos(2.0 * pi * b);
		double S = f0 * exp((mu - 0.5 * sigma * sigma) * T + sigma * eps * sqrt(T));
		double p = exp(-mu * T) * (S - K);
		if (p < 0.0) {
			p = 0.0;
		}
		payoff[i] = p;
		states[i] = local_state;
	}
}

__global__ void reduce_mean(const double *payoff, int n, double *mean) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		atomicAdd(mean, payoff[i]);
	}
}

__global__ void reduce_variance(const double *payoff, int n, double mean, double *variance) {
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		double diff = mean - payoff[i];
		atomicAdd(variance, diff * diff);
	}
}

int main(int argc, char *argv[]) {
	if ( argc != 7 ) {
		printf(" Usage: bs <f0><K><T to expiry><sigma><mu><Monte Carlo cycles>\n");
		exit(0);
	}
	double f0 = atof(argv[1]);
	double K = atof(argv[2]);
	double T = atof(argv[3]);
	double sigma = atof(argv[4]);
	double mu = atof(argv[5]);
	int mc_cycles = atoi(argv[6]);
	printf("-----------------------------\n");
	printf(" Parameter choices:\n");
	printf("-----------------------------\n");
	printf(" Spot: %g\n",f0);
	printf(" Strike: %g\n",K);
	printf(" Time to expiry: %g\n",T);
	printf(" sigma: %g\n",sigma);
	printf(" mu: %g\n",mu);
	printf("Monte Carlo cycles: %d\n",mc_cycles);
	double pi = 4.*atan(1.0);
	double *payoff;
	payoff = (double *) malloc(mc_cycles*sizeof(double));
	// Timing
	double wall_begin = now_seconds();
	clock_t begin = clock();
	double variance, mean;
	// DND: All up to this moment these are just declarations that take O(1); constant
	// time to be executed. 
	/* DND:  All of these initializations correspond to the serial part, being all of the following
	that seem to calculate some arrays, strictly parallel */

	double parallel_begin = now_seconds();

	double *d_payoff = NULL;
	double *d_mean = NULL;
	double *d_variance = NULL;
	curandState *d_states = NULL;
	cudaMalloc((void **)&d_payoff, mc_cycles * sizeof(double));
	cudaMalloc((void **)&d_mean, sizeof(double));
	cudaMalloc((void **)&d_variance, sizeof(double));
	cudaMalloc((void **)&d_states, mc_cycles * sizeof(curandState));

	int threads_per_block = 256;
	int blocks = (mc_cycles + threads_per_block - 1) / threads_per_block;

	init_curand_states<<<blocks, threads_per_block>>>(d_states, (unsigned long)time(NULL), mc_cycles);
	compute_payoff<<<blocks, threads_per_block>>>(
		d_states,
		d_payoff,
		mc_cycles,
		f0,
		K,
		T,
		sigma,
		mu,
		pi
	);
	/* DND: This loop is just O(n) where n is equal to the number of montacarlo 
	cycles `mc_cycles`. */

	/* find the mean and variance of the payoff */
	mean = 0.0;
	variance = 0.0;
	cudaMemset(d_mean, 0, sizeof(double));
	reduce_mean<<<blocks, threads_per_block>>>(d_payoff, mc_cycles, d_mean);
	cudaMemcpy(&mean, d_mean, sizeof(double), cudaMemcpyDeviceToHost);
	/* DND: Again O(n) */

	double mean_value = mean / mc_cycles;
	cudaMemset(d_variance, 0, sizeof(double));
	reduce_variance<<<blocks, threads_per_block>>>(d_payoff, mc_cycles, mean_value, d_variance);
	cudaMemcpy(&variance, d_variance, sizeof(double), cudaMemcpyDeviceToHost);
	/* DND: O(n) */

	double parallel_end = now_seconds();
	double parallel_time = parallel_end - parallel_begin;

	/* Here is where the parallel part ends and the sequential one starts again. To calculate
	the speed up I would need to know how much time it takes for the sequential and parallel
	parts to be executed. */
	variance /= mc_cycles;
	mean /= mc_cycles;
	double std = sqrt(variance);
	double error = std/sqrt(mc_cycles);
	clock_t end = clock();
	double wall_end = now_seconds();
	double total_time = wall_end - wall_begin;
	double serial_time = total_time - parallel_time;
	double time_to_solution = (double) (end-begin)/CLOCKS_PER_SEC;
	printf(" Mean: %g Estimated Error: %g std dev: %g\n",mean,error,std);
	printf(" True price should be in range: %g to %g with 0.95 confidence.\n",
	mean-1.96*error,mean+1.96*error);
	printf(" Time to solution (CPU Time): %g seconds\n",time_to_solution);
	printf(" Total Wall Time (Real Time): %g seconds\n", total_time);
	printf("   -> Parallel execution time: %g seconds\n", parallel_time);
	printf("   -> Serial execution time:   %g seconds\n", serial_time);
	/* Analytic result */
	double d1 = ( log(f0/K)+(mu+0.5*sigma*sigma)*T )/( sigma*sqrt(T) );
	double d2 = d1 - sigma*sqrt(T);
	double price = f0*NoD(d1) - K*exp(-mu*T)*NoD(d2);
	printf(" Analytic BS result: %g\n",price);
	free(payoff);

	cudaFree(d_payoff);
	cudaFree(d_mean);
	cudaFree(d_variance);
	cudaFree(d_states);

	/* DND: O(1) */
	return 0;
}
/* DND: The overall complexity of the algorithm is O(n) 
In the parallel version we have that the temporal complexity is
dropped to O(1). */

// Cumululative Normal Distribution Function
double NoD(double x) {
	double gamma = 0.2316419;
	double a1 = 0.319381530;
	double a2 = -0.356563782;
	double a3 = 1.781477937;
	double a4 = -1.821255978;
	double a5 = 1.330274429;
	double pi = 4.0*atan(1.0);
	double k;
	double N;
	double Nprime;
	if ( x >= 0.0 ) {
		k = 1./(1.+gamma*x);
		Nprime = 1./sqrt(2.*pi)*exp(-0.5*x*x);
		N = 1. - Nprime*(a1*k
		+a2*k*k
		+a3*k*k*k
		+a4*k*k*k*k
		+a5*k*k*k*k*k);
		return N;
	} else {
		x *= -1.0;
		k = 1./(1.+gamma*x);
		Nprime = 1./sqrt(2.*pi)*exp(-0.5*x*x);
		N = 1. - Nprime*(a1*k
		+a2*k*k
		+a3*k*k*k
		+a4*k*k*k*k
		+a5*k*k*k*k*k);
		return 1.-N;
	}
}
/* DND: This part is O(1) */
/*  ¿La latencia del acceso a la memoria global en la 
 GPU limitó los GFLOPS?
 
 Yo creo que si. Practicamente todas mis funciones hacen uso del keyword
 `__global__` y eso me hace pensar que al ser un programa "tan pequenio" este 
 trayecto realmente no vale la pena y hace que el programa sea lento para este
 caso en particular: cuando solo ejecutamos la ecuacion una sola vez en lugar 
 de hacerlo una y otra vez. 
 */
