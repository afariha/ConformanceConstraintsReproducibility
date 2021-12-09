#!/bin/sh

# Requires Python version Python 3.9.5
# Execute the following

# Change this to where your Python 3.9.5 is.
PYTHON=/Library/Frameworks/Python.framework/Versions/3.9/bin/python3

# ref: https://askubuntu.com/questions/425754/how-do-i-run-a-sudo-command-inside-a-script
if [ $(id -u) != 0 ]; then
    echo "Please run this script as root."
    exit 1
fi

if [ $SUDO_USER ]; then
    real_user=$SUDO_USER
else
    real_user=$(whoami)
fi

# Download and data
sudo -u $real_user brew install git-lfs
sudo -u $real_user git lfs install
sudo -u $real_user git lfs pull --include=data.zip
sudo -u $real_user unzip data.zip
sudo -u $real_user find . -name "__MACOSX" -type d  # Recursively delete __MACOSX directories.

# Install texlive
sudo -u $real_user brew install texlive

# Install baselines
sudo -u $real_user brew install cmake
cd Baseline/PCA-CD/Libraries/
sudo -u $real_user tar -xf libpca-1.2.11.tar.gz
sudo -u $real_user tar -xf armadillo-4.200.0.tar.gz
cd armadillo-4.200.0
sudo -u $real_user cmake .
make install
sudo -u $real_user cmake .
make install
cd ../libpca-1.2.11
sh install.sh
cd ../../ChangeDetection/
make

# Create virtual environment and install dependencies
cd ../../..

# ref: https://stackoverflow.com/questions/592620/how-can-i-check-if-a-program-exists-from-a-bash-script
if ! command -v virtualenv &> /dev/null
then
    sudo -u $real_user $PYTHON -m venv venv
else
    sudo -u $real_user virtualenv --python=$PYTHON venv
fi
sudo -s -u $real_user ./venv/bin/pip install matplotlib==3.5.0 scikit-learn==1.0.1
sudo -s -u $real_user ./venv/bin/pip install -e DataInsights

# Generate plots and tables
sudo -u $real_user mkdir Plots
./venv/bin/python Figure_4.py
./venv/bin/python Figure_5.py
./venv/bin/python Figure_6_a.py
./venv/bin/python Figure_6_b.py
./venv/bin/python Figure_6_c.py
./venv/bin/python Figure_7.py
./venv/bin/python Figure_8.py
