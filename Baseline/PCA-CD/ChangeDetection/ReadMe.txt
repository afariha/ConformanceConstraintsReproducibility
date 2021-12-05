The main folder contains the following folders:
1. ChangeDetection: the framework folder which contains the code.
2. Data: contains a sample dataset with 500000 data samples and 9 change points. Change points occur
every 50000 data samples.
3. Libraries: include the most important libraries required for computing the principal components.

The ChangeDetection folder contains:
1. CD.cpp: the main program.
2. CD.h: declaration for the global variables and the constant functions.
3. ReportResults.h: to determine the number of correctly detected changes, the late detections,
the false positives and the false negatives.
4. Makefile: to compile the framework.

How to use the code:
1. Extract the "libpca-1.2.11.tar.gz" file and read the instruction on how to install this library
which will require installing the other two libraries (and maybe more libraries) first.
2. Change the lines:
	INCS = -I"../../LIB/libpca/include" 
	LIBS = -L"../../LIB/libpca/build" -lpca -larmadillo
	in the Makefile to represent the folder where you installed the libpca and armadillo libraries.
3. Modify the constant "const int change_interval = 50000;" in the file "ReportResult.h" to specify
	the size of the batches. If the changes are not periodic (i.e. occur every x data samples),
	you may need to develop another algorithm to distinguish the correctly reported changes from
	the others.
4. In the bash prompt, type make
5. Run the program using the command
	"./CD ../Data/Meanshift.txt 10000 500 output.txt 2 0.005 1"
	The arguments are:
	a. The data file (../Data/Meanshift.txt)
	b. The window size (10000)
	c. The threshold factor (500)
	d. The output file which will include the exact points at which the changes were reported.
	e. The data dimension (2)
	f. Delta (0.005): the amount of the allowed change
	g. The divergence metric (1 to use the area metric; 2 to use the MKL metric) (see the paper).
 
