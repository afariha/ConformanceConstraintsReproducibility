#!/bin/sh

# Requires Python version Python 3.9.5

PYTHON=$(which python3)
set -x

if [ $(id -u) != 0 ]; then
    echo "Please run this script as root."
    exit 1
fi

# Download and data
sudo -u $SUDO_USER brew install git-lfs
sudo -u $SUDO_USER git lfs install
sudo -u $SUDO_USER git lfs pull --include=data.zip
sudo -u $SUDO_USER unzip data.zip
sudo -u $SUDO_USER rm -rf __MACOSX

# Install texlive
sudo -u $SUDO_USER brew install texlive

# Install baselines
sudo -u $SUDO_USER brew install cmake
cd Baseline/PCA-CD/Libraries/
sudo -u $SUDO_USER tar -xf libpca-1.2.11.tar.gz
sudo -u $SUDO_USER tar -xf armadillo-4.200.0.tar.gz
cd armadillo-4.200.0
sudo -u $SUDO_USER cmake .
make install
sudo -u $SUDO_USER cmake .
make install
cd ../libpca-1.2.11
sh install.sh
cd ../../ChangeDetection/
make

# We don't want to fail from this point onward.
set -e

# Create virtual environment and install dependencies
cd ../../..

if ! command -v virtualenv &> /dev/null
then
    sudo -u $SUDO_USER $PYTHON -m venv venv
else
    sudo -u $SUDO_USER virtualenv --python=$PYTHON venv
fi
sudo -s -u $SUDO_USER ./venv/bin/pip install matplotlib==3.5.0 scikit-learn==1.0.1
sudo -s -u $SUDO_USER ./venv/bin/pip install -e DataInsights

# Generate plots and tables
sudo -u $SUDO_USER mkdir -p Plots
./venv/bin/python Figure_4.py
./venv/bin/python Figure_5.py
./venv/bin/python Figure_6_a.py
./venv/bin/python Figure_6_b.py
./venv/bin/python Figure_6_c.py
./venv/bin/python Figure_7.py
./venv/bin/python Figure_8.py
