#include <stdio.h>
#include <thread>
#include <stdio.h>
#include <chrono>
#include <mutex>
#include <vector>

double dx(double x)
{
	return 4 / (1 + x * x);
}

double integral_CPP(double a, double b)
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
		for (auto&thr:threads)
		{
			thr.join();
			return res * dx;
		}

	}
}

int main(int argc, char** argv)
{
	double integral_CPP(double a, double b);
	return 0;

}