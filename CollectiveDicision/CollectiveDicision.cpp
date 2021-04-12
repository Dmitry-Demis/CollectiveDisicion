// CollectiveDicision.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "mpi.h"
#include <vector>
#include <iomanip>
using namespace std;

//constants
#define A 3.2
#define B -1.2
#define C 0.02
#define D 0.13
#define h1 1e-1
#define h2 1e-3
#define comm MPI_COMM_WORLD

double F(const double& x1, const double& x2)
{
	return A * x1 + B * x2 + exp(C * x1 * x1 + D * x2 * x2);
}
void doDebug(int rank)
{
	if (!rank)
	{
		system("pause");
	}
	MPI_Barrier(comm);
}
int main(int argc, char** argv)
{
	int rankP, sizeP;
	if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
	{
		return 1;
	}
	if (MPI_Comm_size(MPI_COMM_WORLD, &sizeP) != MPI_SUCCESS)
	{
		MPI_Finalize();
		return 2;
	}

	if (MPI_Comm_rank(MPI_COMM_WORLD, &rankP) != MPI_SUCCESS)
	{
		MPI_Finalize();
		return 3;
	}
	const
		int
		sizeX = 2, // column count
		sizeY = 5; // row count
	double
		xmin = -5.0, xmax = 5.0, ymin = -5.0, ymax = 5.0;// D: |x| <=5, |y|<=5
 // beginning 
	double xLmin, xLmax, yLmin, yLmax, dx, dy;
	double lmin[3] = { 0.0 };
	int msgtag = 10;
	MPI_Status status;
	vector<double> h = { h1, h2 };
	double minimum[3] = { 0 };
	//doDebug(rankP);
	double timeBeginning, timeEnding;
	timeBeginning = MPI_Wtime();
	for (size_t k = 0; k < h.size(); k++)
	{
		// size of X and Y for each process
		dx = (xmax - xmin) / sizeX; // 10/2 = 5
		dy = (ymax - ymin) / sizeY; // 10/5 = 2
		if (rankP & 1) // odd
		{
			xLmin = xmin + dx; 
			xLmax = xmax;
		}
		else
		{
			xLmin = xmin;
			xLmax = xmin + dx;
		}
		int level = rankP / 2 + 1;
		yLmin = dy * ((double)level - 1) + ymin; 
		yLmax = dy * level + ymin;

		//We'll know how many points inside an x and a y with h[i];
		int
			ny = (int)(yLmax - yLmin) / h[k] + 1, 
			nx = (int)(xLmax - xLmin) / h[k] + 1;
		lmin[0] = F(xLmin, yLmin);
		bool beginning = false; 
		//Checking out the whole area on a local minimum of a function
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				if (beginning)
				{
					if (F(xLmin, yLmin)<lmin[0])
					{
						lmin[0] = F(xLmin, yLmin);
						lmin[1] = xLmin;
						lmin[2] = yLmin;
					}
				}
				yLmin += h[k];
				beginning = true;
			}
			xLmin += h[k];
		}
		
		if (!rankP)
		{
			for (int i = 0; i < 3; i++)
			{
				minimum[i] = lmin[i];
			}
			for (int i = 1; i < sizeP; i++)
			{
				MPI_Recv(lmin, 3, MPI_DOUBLE, i, msgtag, comm, &status);
				if (lmin[0]<minimum[0])
				{
					for (int i = 0; i < 3; i++)
					{
						minimum[i] = lmin[i];
					}					
				}
			}
			if (minimum[1] - h[k] >= xmin) xmin = minimum[1] - h[k];
			if (minimum[1] + h[k] <= xmax) xmax = minimum[1] + h[k];
			if (minimum[2] - h[k] >= ymin) ymin = minimum[2] - h[k];
			if (minimum[2] + h[k] <= ymax) ymax = minimum[2] + h[k];
		}
		else
		{
			MPI_Send(lmin, 3, MPI_DOUBLE, 0, msgtag, comm);
		}
		MPI_Barrier(comm);
	}
	timeEnding = MPI_Wtime();
	if (!rankP)
	{
		cout << "F(x =" << setprecision(5) << fixed << minimum[1] << "," << " y = " << minimum[2] << ") = " << minimum[0]
			<< "  - is the minimum of the function \n" ;
		cout << "Time elapsed: " <<timeEnding - timeBeginning << " s." << endl;
	}

	MPI_Finalize();
	return 0;
}

