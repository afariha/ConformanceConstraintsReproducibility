/* 
 * File:   CD.cpp
 * Author: Hakim
 *
 * Created on Aug 4, 2014, 10:05 PM
 * Last modified on Feb. 12, 2015
 */

/*==============================================================*/
#include "stdlib.h"
#include "CD.h"
#include "pca.h"

using namespace std;


//*===============================================================


int main(int argc, char* argv[])
{
	fstream infs, CHfs, den;
	double rem_pnt;
	double R_sum[MAX_DIM], R_sqsum[MAX_DIM];
	int R_tot_num_pnts = 0, T_tot_num_pnts = 0;
	double sigma[MAX_DIM];
	double B_Width;
	long data_length, i;
	int DivMetric;
	
	double DP;
	double BNR[MAX_DIM];
	double p_mean_current, m_current,  M_current, PH_current, lambda;
	long start_point = 0;
	p_mean_current = 0; 
	m_current = 0;  
	M_current = 0; 
	PH_current = 0;
	
	// ============================================================
	// Initialize sum, sqsum variables
	for (i = 0; i < MAX_DIM; i++)
	{
		R_sum[i] = 0.0;
		R_sqsum[i] = 0.0;
	}

	/************************************/
	clock_t BeginSim, EndSim;	
	double elapTicks;
	double elapMilli;
	/************************************/
	std::list<double>::iterator it;
	
	if(argc != 9)
	{
		cout << "\n\n\n Usage (" << argv[0] << "): <Data File> <Window Size> <Threshold Factor> " 
			 "<Output Change File> <Data Dimension> <Delta>"
			 "<Divergence Metric [1/2]>" << endl;
		cout << "\n\n <Press Enter to exit>" << endl;
		getchar();
		return 2;
	}
	
	//Handle first argument  -- open the data file
	infs.open(argv[1], ios::in);
	if (!infs.good())
	{
		cout << "Error opening data file.." << endl;
		return 3;
	}

	
	//============Read the Window Size================
	Win_Size = atoi(argv[2]);
	f = atof(argv[3]);
	//========================================================
	//Handle third argument  -- open a file to store the density 
	CHfs.open(argv[4], ios::out);
	if (!CHfs.good())
	{
		cout << "Error opening output MAE file.." << endl;
		return 3;
	}
	
	//============Read the Dimension of the data================
	DIM = atoi(argv[5]);
	delta = atof(argv[6]);
	DivMetric = atoi(argv[7]);
	if (DivMetric != 1 && DivMetric != 2)
	{
		cout << "Unknown divergence metric ... \n";
		return 3;
	}
	stats::pca *mypca;
	mypca = new stats::pca(DIM);
	if (mypca == NULL)
	{
		cout << "Error when allocating space for pca ... exiting ... \n";
		return 3;
	}

	//  =========== Start Timer ==============
	BeginSim = clock();		//Get Starting Time
	data_length = 0;
	ScCount = 0;

	for (i = 0; i < DIM; i++)
		R_mean[i] = 0.0;
	// ============== Create the first reference model ==============
	while (infs.good() && R_tot_num_pnts < Win_Size)
	{
		vector<double> record(DIM);
		int idx = 0;
		for (auto value=record.begin(); value!=record.end(); ++value)
		{
		    infs >> *value;		    
			if(!infs.good())
				break;		
			R_mean[idx++] += *value;	
		}
		mypca->add_record(record);
		data_length ++;		R_tot_num_pnts ++; 
	}
	for (int idx = 0; idx < DIM; idx++)
	{
		R_mean[idx] /= (double) R_tot_num_pnts;
	}
	mypca->solve();
	eigs = 0;
	New_DIM = DIM;
	for (i = 0; i < DIM; i++)
	{
		eigs += mypca->get_eigenvalue(i);
		if (eigs > cum_eig)
		{
			New_DIM = i + 1;
			break;
		}
	}

	for (i = 0; i < New_DIM; i++)
	{
		// =============== Extract eigen vectors =====================
		vector<double> eigVec = mypca->get_eigenvector(i);
		for (int m = 0; m < DIM; m ++)
			eigVectors[m][i] = eigVec[m];
		// =============== Extract principal components =====================
		vector<double> princomp = mypca->get_principal(i);
		for (int k = 0; k < princomp.size(); k++)
		{
			DP = princomp[k];
			MD[i].Data_List.push_back(DP);
			if (k == 0)
				min_val[i] = max_val[i] = DP;
			else
			{
				min_val[i] = MIN(min_val[i], DP);
				max_val[i] = MAX(max_val[i], DP);
			}
			
			R_sum[i] += DP;
			R_sqsum[i] += DP * DP;
		}
	}
	for (i = 0; i < New_DIM; i++)
	{		
		BNR[i] = Bandwidth_NR(R_sum[i], R_sqsum[i], MIN(data_length, Win_Size), sigma[i]);		
		num_resamp_pnts = bin_width_factor * (max_val[i] - min_val[i]) / BNR[i];
		B_Width = (max_val[i] - min_val[i]) / (double) num_resamp_pnts;
		create_Hist(Hist[i].H, MD[i].Data_List, min_val[i], max_val[i], num_resamp_pnts, 0);
	}
	
	T_tot_num_pnts = 0;
	for (i = 0; i < DIM; i++)
	{
		while (!MD[i].Data_List.empty())
		{
			MD[i].Data_List.pop_front();
		}
	}
	
	/*delete mypca;
	mypca = new stats::pca(DIM);
	if (mypca == NULL)
	{
		cout << "Error when allocating space for pca ... exiting ... \n";
		return 3;
	}
	*/

	infs.close();
	infs.open(argv[1], ios::in);
	data_length = 0;
	while (infs.good())
	{
		if (T_tot_num_pnts < Win_Size)
		{
			while (infs.good() && (T_tot_num_pnts < Win_Size))
			{
				vector<double> record(DIM);
				int idx = 0;
				for (auto value=record.begin(); value!=record.end(); ++value)
				{
					infs >> *value;					    
					if(!infs.good())
						break;	
					*value = *value - R_mean[idx++];
				}
				vector<double> p_record(DIM);
				for (int m = 0; m < DIM; m++)
					p_record[m] = 0.0;
				for (int m = 0; m < DIM; m++)
					for (int k = 0; k < DIM; k++)
						p_record[m] += record[k] * eigVectors[k][m];
				for (int k = 0; k < DIM; k++)
				{
					DP = p_record[k];
					MD[k].Data_List.push_back(DP);
				}
				data_length ++;		T_tot_num_pnts ++; 
			}
			
			// mypca->solve();
			
			
			if (MD[0].Data_List.size() < Win_Size)
			{
				break;
			}
			for (i = 0; i < New_DIM; i++)
			{
				create_Hist(Hist[i].H, MD[i].Data_List, min_val[i], max_val[i], 
							num_resamp_pnts, 1);
				Score[i] = Compare_Dist(Hist[i].H, B_Width, R_tot_num_pnts, DivMetric);
			}
		}
	
		
		// ===================Compare the distributions============= 
		for (int k = 0; k < New_DIM; k++)
		{
			if (DivMetric == 1)
				F_Score = 1.0 - Score[k] / R_tot_num_pnts;	
			else
				F_Score = Score[k];
			if (k == 0)
			{ 	MaxScore = F_Score; }
			else
			{	
				MaxScore = MAX(MaxScore, F_Score);
			}				
		}
		// ======== Compute Tdynamic, the dynamic (PHT) threshold =============
		/* Commented out by Anna
		p_mean_current = (p_mean_current * (ScCount - start_point) + 
						 MaxScore) / (double)(ScCount - start_point + 1.0);
		m_current = m_current + p_mean_current - MaxScore + delta;
		if (fabs(m_current) > M_current)
            M_current = fabs(m_current);
        PH_current = - m_current;
        ScCount++;
        lambda = f * p_mean_current;
		*/
		
		// These lines are added by Anna so that change amount is reported perodically
		if(data_length % Win_Size == 0)
		{
			CHfs << MaxScore << endl;
			start_point = ScCount;
			p_mean_current = 0; 
			m_current = 0;  
			M_current = 0; 
			PH_current = 0;
		
			// **************************************************************
			// ====== Re-initialize the reference distribution parameters============
			
			
			for (int k = 0; k < DIM; k++)
			{
				while (!MD[k].Data_List.empty())
				{
					if (atoi(argv[8]) == 0)
						MD[k].Data_List.pop_back();
					else
						MD[k].Data_List.pop_front();
						
				}					
				//R_sum[k] = 0.0;
				//R_sqsum[k] = 0.0;
				//R_mean[k] = 0.0;
			}
			
			/*R_tot_num_pnts = 0;
			
			delete mypca;
			mypca = new stats::pca(DIM);
			if (mypca == NULL)
			{
				cout << "Error when allocating space for pca ... exiting ... \n";
				return 3;
			}

			while (infs.good() && R_tot_num_pnts < Win_Size)
			{
				vector<double> record(DIM);
				int idx = 0;
				for (auto value=record.begin(); value!=record.end(); ++value)
				{
				    infs >> *value;					    
					if(!infs.good())
						break;
					R_mean[idx++] += *value;			
				}
				mypca->add_record(record);
				data_length ++;		R_tot_num_pnts ++; 
			}
			for (int idx = 0; idx < DIM; idx++)				
				R_mean[idx] /= (double) R_tot_num_pnts;
			//mypca->solve();
			*/
			
			eigs = 0;
			for (i = 0; i < DIM; i++)
			{
				eigs += mypca->get_eigenvalue(i);
				if (eigs > cum_eig)
				{
					New_DIM = i + 1;						
					break;
				}
			}
			for (i = 0; i < New_DIM; i++)
			{
				// =============== Extract eigen vectors =====================
				vector<double> eigVec = mypca->get_eigenvector(i);
				for (int m = 0; m < DIM; m ++)
					eigVectors[m][i] = eigVec[m];
				// =============== Extract principal components =====================
				vector<double> princomp = mypca->get_principal(i);
				for (int k = 0; k < princomp.size(); k++)
				{
					DP = princomp[k];
					MD[i].Data_List.push_back(DP);
					if (k == 0)
						min_val[i] = max_val[i] = DP;
					else
					{
						min_val[i] = MIN(min_val[i], DP);
						max_val[i] = MAX(max_val[i], DP);
					}
					
					//R_sum[i] += DP;
					//R_sqsum[i] += DP * DP;
				}
			}
			if (MD[0].Data_List.size() < Win_Size)
			{
				break;
			}

 			// ======================== Re-generate the Ref model ========================
			for (i = 0; i < New_DIM; i++)
			{		
				BNR[i] = Bandwidth_NR(R_sum[i], R_sqsum[i], MIN(data_length, Win_Size), sigma[i]);		
				num_resamp_pnts = bin_width_factor * (max_val[i] - min_val[i]) / BNR[i];
				B_Width = (max_val[i] - min_val[i]) / (double) num_resamp_pnts;
				create_Hist(Hist[i].H, MD[i].Data_List, min_val[i], max_val[i], num_resamp_pnts, 0);				
			}

			/*delete mypca;
			mypca = new stats::pca(DIM);
			if (mypca == NULL)
			{
				cout << "Error when allocating space for pca ... exiting ... \n";
				return 3;
			}*/

			T_tot_num_pnts = 0;
			for (i = 0; i < DIM; i++)
			{
				while (!MD[i].Data_List.empty())
				{
					MD[i].Data_List.pop_front();
				}
			}				
			continue;
		}
		
		
		
		vector<double> n_record(DIM);
		int idx = 0;
		for (auto value=n_record.begin(); value!=n_record.end(); ++value)
		{
		    infs >> *value;
			if(!infs.good())
				break;	
			*value = *value - R_mean[idx++];		
		}
		// ========= Project the data point on the components ==========
		vector<double> p_record(DIM);
		for (int m = 0; m < DIM; m++)
			p_record[m] = 0.0;
		for (int m = 0; m < DIM; m++)
			for (int k = 0; k < DIM; k++)
				p_record[m] += n_record[k] * eigVectors[k][m];

		for (int k = 0; k < New_DIM; k++)
		{
			DP = p_record[k];
			rem_pnt = MD[k].Data_List.front();
			MD[k].Data_List.pop_front();
			// ====== Update the density of the test distribution
			update_Hist(Hist[k].H, DP, rem_pnt, min_val[k], max_val[k], k, R_tot_num_pnts, DivMetric);
			MD[k].Data_List.push_back(DP);			
		}	
		data_length++;
	}

	/******************************************/
	EndSim = clock();
	elapTicks = EndSim - BeginSim;
	elapMilli = (double) elapTicks / 1000;
	/******************************************/	
	CHfs.close();
	infs.close();
	Results(argv[4], data_length-1, Win_Size, elapMilli / 1000);
	return 0;
}

