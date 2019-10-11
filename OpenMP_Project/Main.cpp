#include <omp.h>
#include <stdio.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#define steps 100000

double f(double x)
{
	return 4 / (1 + x * x);
}

double integral_OMP(double a, double b) 
{
	int T = omp_get_num_procs(); //количество доступных потоков
	double res = 0;
	double dx = (b - a) / steps;
#pragma omp_parallel
	{
		int t = omp_get_thread_num(); //индекс потока
		double r = 0;
		int i = 0;
		for (i; i < steps; i += T)
			r += f(dx*(i + 0.5) + a);
#pragma omp_critical
		{
			res += r;
		}
	}
	return res * dx;
}

/*double integral_CPP(double a, double b)
{
	auto T = std::thread::hardware_concurrency(); //количество доступных потоков
	double dx = (b - a) / steps;
	std::vector<std::thread> threads;
	double res = 0;
	for (unsigned t = 0; t < T; t++)
	{
		threads.emplace_back(std::thread([T, t, dx, &res, a]()
			{
				for (auto i = t; i < steps; i += T)
				{
					res += f(dx * (i + 0.5) + a);
				}
			}));
		for (auto& thr : threads)
		{
			thr.join();
			return res * dx;
		}

	}
}*/


int main(int argc, char**argv)
{
	unsigned t0 = omp_get_wtime();
	double result_omp = integral_OMP(0, 1);
	double t1 = omp_get_wtime();
	printf("result: %g, time: %g.ms \n", result_omp, ((t1-10)*1000));
	return 0;

}