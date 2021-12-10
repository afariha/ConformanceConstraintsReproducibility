#!/bin/sh

PYTHON=$(which python3)

set -x

if [ $(id -u) != 0 ]; then
    echo "Please run this script as root."
    exit 1
fi

wget https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh -O - | bash
apt-get update
apt install -y unzip
apt-get install -y build-essential
apt-get install -y python3-pip
apt-get install -y texlive-full
apt-get install -y git-lfs
sudo -u $SUDO_USER git lfs install
sudo -u $SUDO_USER git lfs pull --include=data.zip
sudo -u $SUDO_USER unzip data.zip
sudo -u $SUDO_USER rm -rf __MACOSX/
apt install -y cmake
apt-get install -y liblapack-dev
cd Baseline/PCA-CD/Libraries/
sudo -u $SUDO_USER tar -xf libpca-1.2.11.tar.gz
sudo -u $SUDO_USER tar -xf armadillo-4.200.0.tar.gz
cd armadillo-4.200.0
cmake .
make install
cd ../libpca-1.2.11
sh install.sh
cd ../../ChangeDetection/
make
ldconfig
apt-get install -y libarmadillo-dev
cd ../../..
apt-get install -y virtualenv

if ! command -v virtualenv &> /dev/null
then
    sudo -u $SUDO_USER virtualenv --python=/usr/bin/python3 venv
else
    sudo -u $SUDO_USER $PYTHON -m venv venv
fi

sudo -u $SUDO_USER ./venv/bin/pip install matplotlib==3.5.0 scikit-learn==1.0.1
sudo -u $SUDO_USER ./venv/bin/pip install -e DataInsights

sudo -u $SUDO_USER mkdir Plots
./venv/bin/python Figure_4.py
./venv/bin/python Figure_5.py
./venv/bin/python Figure_6_a.py
./venv/bin/python Figure_6_b.py
./venv/bin/python Figure_6_c.py
./venv/bin/python Figure_7.py
./venv/bin/python Figure_8.py
