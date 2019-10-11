#include <omp.h>
#include <stdio.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#define steps 100000

double f(double x)
{
	return 4 / (1 + x * x);
}

/*double integral_OMP(double a, double b) 
{
	int T = omp_get_num_procs(); //количество доступных потоков
	double res = 0;
	double dx = (b - a) / steps;
#pragma omp parallel
	{
		int t = omp_get_thread_num(); //индекс потока
		double r = 0;
		for (int i = 1; i < steps; i += T)
			r += f(dx*(i + 0.5) + a);
#pragma omp critical
		{
			res += r;
		}
	}
	return res * dx;
}*/

double integral_CPP(double a, double b)
{
	auto T = std::thread::hardware_concurrency(); //количество доступных потоков
	double dx = (b - a) / steps;
	std::mutex mtx;
	std::vector<std::thread> threads;
	double res = 0;
	double r = 0;
	for (unsigned t = 0; t < T; t++)
	{
		threads.emplace_back(std::thread([T, t, dx, &res, &mtx, a, &r]()
			{
				for (auto i = t; i < steps; i += T)
				{
					r += f(dx * (i + 0.5) + a);
				}
			}));
		mtx.lock();
		res += r;
		mtx.unlock();
	}
	return res;
}


/*int main(int argc, char**argv)
{
	double t0 = omp_get_wtime();
	double result_omp = integral_OMP(0, 1);
	double t1 = omp_get_wtime();
	printf("result: %g, time: %g.ms \n", result_omp, ((t1-t0)*1000));
	return 0;

}*/

int main(int argc, char** argv)
{
	double t0 = omp_get_wtime();
	double result_CPP = integral_CPP(0, 1);
	double t1 = omp_get_wtime();
	printf("result: %g, time: %g.ms \n", result_CPP, ((t1 - t0) * 1000));
	return 0;

}