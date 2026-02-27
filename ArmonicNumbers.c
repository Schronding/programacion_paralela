#include <stdio.h>

#define N 1000000000

int main ( void )
{
long int i;
double sum = 0;

#pragma omp parallel for reduction(+:sum)
for (i=N; i>=1; i--)
sum += 1.0/i;

printf("H_%d = %.15lf\n", N, sum);
return 0;
}