# Requires Python version Python 3.9.5

# Make sure git-lfs is installed
sudo apt-get install git-lfs
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

# Change the line below to point out to the location where python3 is installed
virtualenv --python=/usr/bin/python3 venv
source ./venv/bin/activate
pip install matplotlib==3.5.0
pip install scikit-learn==1.0.1
pip install -e DataInsights

# Generate plots and tables
sudo mkdir Plots
sudo ./venv/bin/python Figure_4.py
sudo ./venv/bin/python Figure_5.py
sudo ./venv/bin/python Figure_6_a.py
sudo ./venv/bin/python Figure_6_b.py
sudo ./venv/bin/python Figure_6_c.py
sudo ./venv/bin/python Figure_7.py
sudo ./venv/bin/python Figure_8.py
