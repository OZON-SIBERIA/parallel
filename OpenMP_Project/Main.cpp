#include <omp.h>
#include <stdio.h>
#include <thread>
#include <stdio.h>
#include <chrono>

double dx(double x)
{
	return 4 / (1 + x * x);
}

double integral_OMP(double a, double b) 
{
	int T = omp_get_num_procs(); //количество доступных потоков
	double res = 0;
#pragma omp_parallel
	{
		int t = omp_get_thread_num(); //индекс потока
		double r = 0;
		for (int i; i < steps; i += T)
			r += f(dx(i + 0.5) + a);
#pragma omp_critical
		{
			res += r;
		}
	}
	return res * dx;
}

int main(int argc, char**argv)
{
	unsigned t0 = omp_get_wtime();
	double result_omp = integral_OMP(0, 1);
	double t1 = omp_get_wtime();
	printf("result: %g, time: %g.ms \h", result_omp, ((t1-10)*1000));
	return 0;

}