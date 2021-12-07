# Requires Python version Python 3.9.5
# Execute the following

# Make sure git-lfs is installed
sudo apt install git-lfs
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/

# Install baselines
sudo apt install cmake
cd Baseline/PCA-CD/Libraries/
tar -xf libpca-1.2.11.tar.gz
tar -xf armadillo-4.200.0.tar.gz 
tar -xf lapack-3.5.0.tgz
cd armadillo-4.200.0
cmake .
make
sudo make install
cmake .
make
sudo make install
cd ../libpca-1.2.11
sudo sh install.sh
cd ../../ChangeDetection/
sudo make

# Create virtual environment and install dependencies
cd ../../..
sudo apt install python3-virtualenv
virtualenv --python=/usr/bin/python3 venv
source ./venv/bin/activate
sudo python3 -m pip install ipykernel==6.6.0
sudo python3 -m ipykernel install --name=venv
sudo python3 -m pip install matplotlib==3.5.0
sudo python3 -m pip install scikit-learn==1.0.1
sudo python3 -m pip install jupyterlab==3.2.4
sudo python3 -m pip install runipy==0.1.5
sudo python3 -m pip install pdfkit==1.0.0
sudo python3 -m pip install -e DataInsights

# Generate plots and tables
sudo mkdir Plots
sudo python Figure_4.py
sudo python Figure_5.py
sudo python Figure_6_a.py
sudo python Figure_6_b.py
sudo python Figure_6_c.py
sudo python Figure_7.py
sudo runipy Figure_7.ipynb	# Keeping it only
sudo python Figure_8.py
