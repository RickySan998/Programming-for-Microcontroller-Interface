#include "stdio.h"

extern void distrBF(int T_arg, int* dij_arr, int* Dj_arr, int* n_res_arr);

// EE2024 Assignment 1, Sem 1, AY 2017/18
// (c) CK Tham, ECE NUS, 2017

#define T 4
#define N 5

int main(void)
{
	// Variable definitions
	int i;

	// Note: different initialisation list is needed for different T, N
	int dij[T][N]={{9,5,7,8,0},
	               {7,9,6,9,10},
	               {10,8,8,7,2},
				   {10,8,8,9,0}};

	int Dj[T][N]={{1,23,14,15,0},
	              {18,19,8,14,9},
	              {19,2,17,16,2},
				  {19,22,17,9,6}};

	int n_res[T][2]={{N,0},
	                 {0,0},
	                 {0,0},
					 {0,0}};

	// Call assembly language function
	distrBF(T,(int*)dij,(int*)Dj,(int*)n_res);

	// Print out results on console
	for ( i=0; i<T ; i++ )
		printf("%d %d\n", n_res[i][0], n_res[i][1]);

	// Enter an infinite loop, just incrementing a counter
	volatile static int loop = 0;
	while (1) {
		loop++;
	}
}
