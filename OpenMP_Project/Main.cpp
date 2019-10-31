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

union elem_t
{
	char padding[64];
	double value;
};

double integral_OMP(double a, double b) 
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
}

double integral_CPP(double a, double b)
{
	auto T = std::thread::hardware_concurrency(); //количество доступных потоков
	double dx = (b - a) / steps;
	std::mutex mtx;
	std::vector<std::thread> threads;
	double res = 0;
	for (unsigned t = 0; t < T; t++)
	{
		threads.emplace_back(std::thread([T, t, dx, &res, &mtx, a]()
			{
				double r = 0;
				for (auto i = t; i < steps; i += T)
				{
					r += f(dx * (i + 0.5) + a);
				}
				mtx.lock();
				res += r*dx;
				mtx.unlock();
			}));
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
	return res;
}

double integral_ARR(double a, double b)
{
	auto T = std::thread::hardware_concurrency(); //количество доступных потоков
	double dx = (b - a) / steps;
	std::mutex mtx;
	std::vector<std::thread> threads;
	double res = 0;
	for (unsigned t = 0; t < T; t++)
	{
		threads.emplace_back(std::thread([T, t, dx, &res, &mtx, a]()
			{
				double r = 0;
				for (auto i = t; i < steps; i += T)
				{
					r += f(dx * (i + 0.5) + a);
				}
				mtx.lock();
				res += r * dx;
				mtx.unlock();
			}));
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
	return res;
}


double integral_Padding(double a, double b)
{
	double result = 0;
	unsigned P = omp_get_num_procs();
	double dx = (b - a) / steps;
	elem_t* pResults = (elem_t*)malloc(P*sizeof(elem_t));
	unsigned T;
#pragma omp parallel
	{
		unsigned t = omp_get_thread_num();
#pragma omp single
		{
			T = omp_get_num_threads();
		}
		pResults[t].value = 0;
		for (unsigned i = t; i < steps; i += T)
		{
			pResults[t].value += f(dx * (i + 0.5) + a);
		}
	}
		for (unsigned i = 0; i < T; i ++)
		{
			result += pResults[i].value;
		}
	return result * dx;
}

double Integral_Reduction(double a, double b)
{
	double dx = (b - a) / steps;
	double results = 0;
#pragma omp parallel for reduction(+:results)

	for (int i = 0; i < steps; ++i)
		results += f(dx * (i + 0.5) + a);

	return results * dx;
}

int first()
{
	double t0 = omp_get_wtime();
	double result_OMP = integral_OMP(0, 1);
	double t1 = omp_get_wtime();
	int T = omp_get_num_procs();
	printf("OMP result: %g, time: %g.ms %d threads \n", result_OMP, ((t1 - t0) * 1000), T);
	return 1;
}


int second()
{
	double t0 = omp_get_wtime();
	double result_CPP = integral_CPP(0, 1);
	double t1 = omp_get_wtime();
	auto T = std::thread::hardware_concurrency();
	printf("CPP result: %g, time: %g.ms %d threads \n", result_CPP, ((t1 - t0) * 1000), T);
	return 2;
}

int third()
{
	double t0 = omp_get_wtime();
	double result_ARR = integral_ARR(0, 1);
	double t1 = omp_get_wtime();
	auto T = std::thread::hardware_concurrency();
	printf("ARR result: %g, time: %g.ms %d threads \n", result_ARR, ((t1 - t0) * 1000), T);
	return 3;
}

int fourth()
{
	double t0 = omp_get_wtime();
	double result_Padding = integral_Padding(0, 1);
	double t1 = omp_get_wtime();
	auto T = std::thread::hardware_concurrency();
	printf("Padding result: %g, time: %g.ms %d threads \n", result_Padding, ((t1 - t0) * 1000), T);
	return 4;
}

int fithe()
{
	double t0 = omp_get_wtime();
	double result_Reduction = Integral_Reduction(0, 1);
	double t1 = omp_get_wtime();
	auto T = std::thread::hardware_concurrency();
	printf("Reduction result: %g, time: %g.ms %d threads \n", result_Reduction, ((t1 - t0) * 1000), T);
	return 5;
}

struct run_result
{
	double resulting_value;
	double time_ms;
};

std::vector<run_result> run_experiment(double(*Integral) (double a, double b, double (*pFunc) (double)));

std::iostream& operator<<(std)

run_result run_experiment_threads(std::size_t cThreadCount, double(*Integral) (double a, double c, double (*pFunc) (double)))
{
	long double t1;
	run_result result;
	omp_get_num_threads(int(cThreadCount));
	t1 = omp_get_wtime();
	result.resulting_value = Integral(0, 1, func);
	result.time_ms = (omp_get_wtime() - t1) / 1000);
	return result;
}

std::vector<run_result> run_experiment(double(*Integral) (double a, double b, double (*pFunc) (double)))
{
	std::vector<run_result> results;
	auto cParallelism = std::size_t(omp_get_num_procs());
	results.reserve(cParallelism);
	for (std::size_t i = 0; i < cParallelism;)
		results.emplace_back(run_experiment_threads(i++, Integral));
	return results;
}

int main(int argc, char** argv)
{
	first();
	second();
	third();
	fourth();
	fithe();

	return 0;

}