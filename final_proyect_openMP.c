#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
double NoD(double x);
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
    double a,b,result,eps;
    double S;
    int i;
    double *payoff;
    payoff = (double *) malloc(mc_cycles*sizeof(double));

    double wall_begin = omp_get_wtime();
    clock_t begin = clock();
    double variance, mean;
    // All up to this moment these are just declarations that take O(1); constant time to be executed. 
    /* All of these initializations correspond to the serial part, being all of the following
    that seem to calculate some arrays, strictly parallel */

    double parallel_begin = omp_get_wtime();
#pragma omp parallel private ( i )
    {
    #pragma omp for
            for (i=0;i<mc_cycles;i++) {
                result = rand();
                a = (1.0*result)/RAND_MAX;
                result = rand();
                b = (1.0*result)/RAND_MAX;
                if ( a <= 0.0 || a > 1.0 ||
                b <= 0.0 || b > 1.0 ) {
                    printf(" Random number generator problem : %g %g\n",a,b);
                    exit(0);
                }

                eps = sqrt(-2.*log(a))*cos(2.*pi*b);
                S = f0*exp( (mu-0.5*sigma*sigma)*T + sigma*eps*sqrt(T) );
                payoff[i] = exp(-mu*T)*(S - K);
                if ( payoff[i] < 0.0 ) payoff[i] = 0.0;
            }
            /* This loop is just O(n) where n is equal to the number of montacarlo cycles `mc_cycles`. */
    }

    mean = 0.0;
    variance = 0.0;
#pragma omp parallel private ( i )
    {
    #pragma omp for reduction ( + : mean)
            for (i=0;i<mc_cycles;i++) {
                mean += payoff[i];
                /*Again O(n) */
            }
    }

#pragma omp parallel private ( i )
    {
    #pragma omp for reduction ( + : variance)
            for (i=0;i<mc_cycles;i++) {
                variance += (mean/mc_cycles-payoff[i])*(mean/mc_cycles-payoff[i]);
            }
            /* O(n) */
    }
    double parallel_end = omp_get_wtime();
    double parallel_time = parallel_end - parallel_begin;

    /* Here is where the parallel part ends and the sequential one starts again. To calculate
    the speed up I would need to know how much time it takes for the sequential and parallel
    parts to be executed. */
    variance /= mc_cycles;
    mean /= mc_cycles;
    double std = sqrt(variance);
    double error = std/sqrt(mc_cycles);
    clock_t end = clock();
    double wall_end = omp_get_wtime();
    double total_time = wall_end - wall_begin;
    double serial_time = total_time - parallel_time;
    double time_to_solution = (double) (end-begin)/CLOCKS_PER_SEC;
    printf(" Mean: %g Estimated Error: %g std dev: %g\n",mean,error,std);
    printf(" True price should be in range: %g to %g with 0.95 confidence.\n",
    mean-1.96*error,mean+1.96*error);
    printf(" Time to solution (CPU Time): %g seconds\n",time_to_solution);
    printf(" Total Wall Time (Real Time): %g seconds\n", total_time);
    printf("   ↳ Parallel execution time: %g seconds\n", parallel_time);
    printf("   ↳ Serial execution time:   %g seconds\n", serial_time);
    double d1 = ( log(f0/K)+(mu+0.5*sigma*sigma)*T )/( sigma*sqrt(T) );
    double d2 = d1 - sigma*sqrt(T);
    double price = f0*NoD(d1) - K*exp(-mu*T)*NoD(d2);
    printf(" Analytic BS result: %g\n",price);
    free(payoff);

    /* All of these take O(1) */
    return 0;
}
/* The overall complexity of the algorithm is O(n). In the parallel version we have that the temporal complexity is
dropped to O(N/P) where P is the number of processors. . */

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
/* This part is O(1) */


/* Execution times below */

/* DND: As I got 
 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.496049 seconds
 Total Wall Time (Real Time): 0.0353166 seconds
   ↳ Parallel execution time: 0.0353098 seconds
   ↳ Serial execution time:   6.86e-06 seconds
 Analytic BS result: 5
 
 That means that my formula goes to 
 
 0.0353166 / (6.86e-06) = 5148.19241983, which means that
 the program can run more than 5000 times faster. 
 
 export OMP_NUM_THREADS=1
schronding@StormTrooper:~/repos/parallel_programming$ ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 3e-05 seconds
 Total Wall Time (Real Time): 3.2289e-05 seconds
   ↳ Parallel execution time: 2.9009e-05 seconds
   ↳ Serial execution time:   3.28e-06 seconds
 Analytic BS result: 5
 
 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 2.9e-05 seconds
 Total Wall Time (Real Time): 3.154e-05 seconds
   ↳ Parallel execution time: 2.801e-05 seconds
   ↳ Serial execution time:   3.53e-06 seconds
 Analytic BS result: 5
 
 */

 
 
 /* Why the program took less when is had just one thread? 
 
 DND: 
export OMP_NUM_THREADS=2
schronding@StormTrooper:~/repos/parallel_programming$ ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000122 seconds
 Total Wall Time (Real Time): 0.000125659 seconds
   ↳ Parallel execution time: 0.000121279 seconds
   ↳ Serial execution time:   4.38e-06 seconds
 Analytic BS result: 5
 
 export OMP_NUM_THREADS=2
schronding@StormTrooper:~/repos/parallel_programming$ ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000149 seconds
 Total Wall Time (Real Time): 0.000151208 seconds
   ↳ Parallel execution time: 0.000147088 seconds
   ↳ Serial execution time:   4.12e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000125 seconds
 Total Wall Time (Real Time): 0.000128088 seconds
   ↳ Parallel execution time: 0.000123978 seconds
   ↳ Serial execution time:   4.11e-06 seconds
 Analytic BS result: 5


 */

 /* 
 DND: 
 
 export OMP_NUM_THREADS=4
schronding@StormTrooper:~/repos/parallel_programming$ ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000263 seconds
 Total Wall Time (Real Time): 0.000269606 seconds
   ↳ Parallel execution time: 0.000265686 seconds
   ↳ Serial execution time:   3.92e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000479 seconds
 Total Wall Time (Real Time): 0.000273557 seconds
   ↳ Parallel execution time: 0.000269447 seconds
   ↳ Serial execution time:   4.11e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.000509 seconds
 Total Wall Time (Real Time): 0.000330546 seconds
   ↳ Parallel execution time: 0.000326076 seconds
   ↳ Serial execution time:   4.47e-06 seconds
 Analytic BS result: 5
 
 */

 /* 
 DND: 
export OMP_NUM_THREADS=8
 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.002497 seconds
 Total Wall Time (Real Time): 0.000591073 seconds
   ↳ Parallel execution time: 0.000586973 seconds
   ↳ Serial execution time:   4.1e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.001221 seconds
 Total Wall Time (Real Time): 0.000547003 seconds
   ↳ Parallel execution time: 0.000540263 seconds
   ↳ Serial execution time:   6.74e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.001355 seconds
 Total Wall Time (Real Time): 0.000581923 seconds
   ↳ Parallel execution time: 0.000577783 seconds
   ↳ Serial execution time:   4.14e-06 seconds
 Analytic BS result: 5
 */

 /* 
 DND: export OMP_NUM_THREADS=16
 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.008569 seconds
 Total Wall Time (Real Time): 0.0011987 seconds
    Parallel execution time: 0.00119413 seconds
    Serial execution time:   4.58e-06 seconds
 Analytic BS result: 5
 
 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.039865 seconds
 Total Wall Time (Real Time): 0.00342755 seconds
   ↳ Parallel execution time: 0.00342164 seconds
   ↳ Serial execution time:   5.91e-06 seconds
 Analytic BS result: 5

 ./final_proyect.bin 5 6 9 10 12 52
-----------------------------
 Parameter choices:
-----------------------------
 Spot: 5
 Strike: 6
 Time to expiry: 9
 sigma: 10
 mu: 12
Monte Carlo cycles: 52
 Mean: 0 Estimated Error: 0 std dev: 0
 True price should be in range: 0 to 0 with 0.95 confidence.
 Time to solution (CPU Time): 0.039865 seconds
 Total Wall Time (Real Time): 0.00342755 seconds
   ↳ Parallel execution time: 0.00342164 seconds
   ↳ Serial execution time:   5.91e-06 seconds
 Analytic BS result: 5

 
 */
