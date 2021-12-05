/* 
 * File:   CD.h
 * Author: Hakim
 *
 * Last modified on Feb. 12, 2015
 */

#include <list>
#include <math.h>
// #include <fstream>
#include <time.h> 
#include <stdlib.h>
#include <string.h>	
#include <vector>
#include "ReportResult.h"

using std::ofstream;
// ==============Constant Numbers================
#define mypi 3.14159265358979
#define tol_error 1E-9
#define min_num_pnts_to_build_model 5000

// ==============Constant Functions================
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) < (Y) ? (Y) : (X))
#define MAX_DIM 300
#define cum_eig 0.995
// #define ranf()((double)rand() / (double)(RAND_MAX))
// ==============Variables List====================

double min_val[MAX_DIM], max_val[MAX_DIM];
double eigVectors[MAX_DIM][MAX_DIM];
double R_mean[MAX_DIM];
long R_count;
double Score[MAX_DIM], AvgScore, MaxScore, F_Score;
double ScCount;
int DIM, New_DIM;
double eigs;
long check_step = 1;
int change = 0;
double delta = 0.005;
double f;
double bin_width_factor = 2.0;


long Win_Size, num_resamp_pnts;


struct MultiDimData
{
	list <double> Data_List;
}MD[MAX_DIM];

struct H_Data{
	double count[2];
	double min, max;
	list <double> Ref_Data;
};

struct Histogram
{
	std::vector<H_Data> H;
}Hist[MAX_DIM];
// ===============Global Functions with Defination================

double STD(double tot_sum, double tot_sqsum, long int n)
{	
	if (n == 1)
	{
		return 0;
	}
	double sigma = pow(((tot_sqsum - (1.0 / (double) n) * tot_sum * tot_sum))/(double)(n - 1), 0.5);
	if ((sigma != sigma) || isinf(sigma))
		return 0;
	return sigma;
}
// ==============================================================

double Bandwidth_NR(double tot_sum, double tot_sqsum, long int n, double & sigma)
{	
	sigma = STD(tot_sum, tot_sqsum, n);
	if (sigma == 0)
		return 1;
	return 1.06 * sigma * pow(n, -0.2);
}




// ===============================================================
double Compute_Score(std::vector<H_Data> &H, int idx, long Ref_num_pnts, int DM)
{
	double S1, S2;
	double Score_i = 0;
	double R_epdf, T_epdf;
	R_epdf = (double)H[idx].count[0];
	T_epdf = (double)H[idx].count[1];
	if (DM == 1)
		Score_i = MIN(R_epdf, T_epdf);
	if (DM == 2)
	{
		S1 = 0.0; S2 = 0.0;
		R_epdf /= Ref_num_pnts;
		T_epdf /= Ref_num_pnts;
		if (R_epdf > 0.0 && T_epdf > 0.0)
		{
			S1 = T_epdf * log(T_epdf / R_epdf);
			S2 = R_epdf * log(R_epdf / T_epdf);
		}
		Score_i = MAX(S1,S2);
	}
	return Score_i;
}



// ===============================================================
void create_Hist(std::vector<H_Data> &H, std::list<double> Data, 
				double min_val, double max_val, int Len, int index)
{
	int i, idx;
	std::list<double>::iterator it;
	if (index == 0)
	{
		while (!H.empty())
		{
			H.pop_back();
		}
		H_Data HD;
		HD.count[0] = HD.count[1] = 0;

		for (int k = 0; k < Len; k++)
		{
			HD.min = min_val + k * (max_val - min_val) / Len;
			HD.max = min_val + (k + 1) * (max_val - min_val) / Len;		
			H.push_back(HD);
		}
	}
	for (it = Data.begin(); it != Data.end(); it ++)
	{
		// i = ceil((*it - min_val) / (max_val - min_val) * Len) - 1;
		// if (*it == min_val)
		// 	i = 0;
		for (i = 0; i < Len; i++)
			if (*it >= H[i].min && *it < H[i].max)
				{	idx = i;	break;}
		if (*it == min_val)
			idx = 0;
		if (idx >= 0 && idx < Len)
			H[idx].count[index]++;
	}
}
// ===============================================================
void update_Hist(std::vector<H_Data> &H, double point, double rem_pnt, 
				 double min_val, double max_val, int index, long Ref_num_pnts, int DM)
{	
	int Len = H.size();
	// int i = (ceil)((point - min_val) / (max_val - min_val) * Len) - 1;
	int i, idx;
	for (i = 0; i < Len; i++)
			if (point >= H[i].min && point < H[i].max)
				{	idx = i;	break;}
	if (point == min_val)
		idx = 0;
	if (idx >= 0 && idx < Len)
	{
		Score[index] -= Compute_Score(H, idx, Ref_num_pnts, DM);
		H[idx].count[1]++;
		Score[index] += Compute_Score(H, idx, Ref_num_pnts, DM);
	}
	for (i = 0; i < Len; i++)
			if (rem_pnt >= H[i].min && rem_pnt < H[i].max)
			{	idx = i;	break;}
	if (rem_pnt == min_val)
		idx = 0;
	if (idx >= 0 && idx < Len)
	{
		Score[index] -= Compute_Score(H, idx, Ref_num_pnts, DM);
		H[idx].count[1]--;
		Score[index] += Compute_Score(H, idx, Ref_num_pnts, DM);
	}
}
//*===============================================================
double Compare_Dist(std::vector<H_Data> H, double Bin_Width, 
					long Ref_num_pnts, int DM)
{
	double Score_i = 0.0;
	int S = H.size();
	
	for (int i = 0; i < S; i++)
	{
		Score_i += Compute_Score(H, i, Ref_num_pnts, DM);
	}
	// Thresh *= Bin_Width;
	
	// return 1 - Thresh;
	return Score_i;
}
//*===============================================================
