#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
using namespace std;
using namespace std::chrono;
//constants
#define A 3.2
#define B -1.2
#define C 0.02
#define D 0.13
#define h1 1e-3
#define h2 1e-5
double F(const double& x1, const double& x2)
{
	return A * x1 + B * x2 + exp(C * x1 * x1 + D * x2 * x2);
}
int main()
{
	double
		xmin = -5.0, xmax = 5.0, ymin = -5.0, ymax = 5.0;// D: |x| <=5, |y|<=5
	double x, y;
	double lmin[3] = { 0.0 };
	vector<double> h = { h1, h2 };
	auto start = steady_clock::now();
	for (size_t k = 0; k < h.size(); k++)
	{
		int
			ny = (int)(ymax - ymin) / h[k] + 1,
			nx = (int)(xmax - xmin) / h[k] + 1;
		x = xmin;
		y = ymin;
		lmin[0] = F(x, y);
		bool beginning = false;
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				if (beginning)
				{
					if (F(x, y) < lmin[0])
					{
						lmin[0] = F(x, y);
						lmin[1] = x;
						lmin[2] = y;
					}
				}
				y += h[k];
				beginning = true;
			}
			x += h[k];
		}
		if (lmin[1] - h[k] >= xmin) xmin = lmin[1] - h[k];
		if (lmin[1] + h[k] <= xmax) xmax = lmin[1] + h[k];
		if (lmin[2] - h[k] >= ymin) ymin = lmin[2] - h[k];
		if (lmin[2] + h[k] <= ymax) ymax = lmin[2] + h[k];


	}
	auto finish = steady_clock::now();
	cout << "F(x =" << setprecision(5) << fixed << lmin[1] << "," << " y = " << lmin[2] << ") = " << lmin[0]
		<< "  - is the minimum of the function \n";
	cout << "Elapsed time is: " << duration_cast<milliseconds>(finish - start).count() << " ms."<< endl;

	return 0;
}