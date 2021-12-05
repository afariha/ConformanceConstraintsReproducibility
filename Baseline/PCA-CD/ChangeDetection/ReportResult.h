#include <iostream>
#include <fstream>

using namespace std;

const int change_interval = 50000;   //  position of the changes
int counter;
int correct = 0, wrong = 0, late = 0;
int start = 0;

void Results(char* argv, int num_samples, int W, double ET)
{
	fstream infs;
	
	infs.open(argv, ios::in);
	if(!infs.good())
	{
		cout << "Cannot open change points file. ..\n";
		return ;
	}
	while(infs.good())
	{
		infs >> counter;
		if (!infs.good())
			break;
		if (counter % change_interval < 2*W) 
			{ 	correct++; 	}
		else if (counter / change_interval > start / change_interval) 
			{	late++;  }
		else {	wrong++; 	} 
		start = counter;
	}
	int missed = num_samples / change_interval - 1 - correct - late;
	cout << "Correct = " << correct << "\nLate = " << late << "\nWrong = " << 
		wrong << "\nMissed = " << missed << "\nRun time = " << ET << endl;
	infs.close();
}
