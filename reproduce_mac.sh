# Requires Python version Python 3.9.5
# Execute the following

# Download and data
brew install git-lfs
git lfs install
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/

# Install texlive
brew install texlive

# Install baselines
brew install cmake
cd Baseline/PCA-CD/Libraries/
tar -xf libpca-1.2.11.tar.gz
tar -xf armadillo-4.200.0.tar.gz 
cd armadillo-4.200.0
cmake .
sudo make install
cmake .
sudo make install
cd ../libpca-1.2.11
sudo sh install.sh
cd ../../ChangeDetection/
sudo make


# Create virtual environment and install dependencies
cd ../../..

# Change the line below to point out to the location where python3 is installed
virtualenv --python=/Library/Frameworks/Python.framework/Versions/3.9/bin/python3 venv
source ./venv/bin/activate
sudo ./venv/bin/pip install matplotlib==3.5.0
sudo ./venv/bin/pip install scikit-learn==1.0.1
sudo ./venv/bin/pip install -e DataInsights

# Generate plots and tables
sudo mkdir Plots
sudo ./venv/bin/python Figure_4.py
sudo ./venv/bin/python Figure_5.py
sudo ./venv/bin/python Figure_6_a.py
sudo ./venv/bin/python Figure_6_b.py
sudo ./venv/bin/python Figure_6_c.py
sudo ./venv/bin/python Figure_7.py
sudo ./venv/bin/python Figure_8.py

