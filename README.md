# Requires Python version Python 3.9.5

## If using Windows, do the following to prepare first

- Install windows subsystem for linux using `wsl --install`
- Download and install ubuntu from this link: https://ubuntu.com/wsl
- Open the Ubuntu terminal and prepare the system with the following commands:

```
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt install unzip
sudo apt-get update
sudo apt-get install -y build-essential
sudo apt-get install python3-pip
sudo apt-get install texlive-full
```

## Execute the following commands

### Download and extract data using git-lfs (or just simply download the data.zip from the repo and extract it manually). Commands that would likely work for Linux are in parentheses.
```
brew install git-lfs	(sudo apt-get install git-lfs)
git lfs install
cd ConformaceConstraintsReproducibility
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/
```

### Install baselines
```
brew install cmake	(sudo apt install cmake)
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
```

### Create virtual environment and install dependencies
#### Edit the third line below to point out to the location of `Python 3.9.5` in your machine
```
cd ../../..
brew install virtualenv (sudo apt-get install virtualenv)
virtualenv --python=/Library/Frameworks/Python.framework/Versions/3.9/bin/python3 venv (virtualenv --python=/usr/bin/python3 venv)
source ./venv/bin/activate 
pip install matplotlib==3.5.0
pip install scikit-learn==1.0.1
pip install -e DataInsights
```

# Generate plots and tables

```
sudo mkdir Plots
sudo ./venv/bin/python Figure_4.py
sudo ./venv/bin/python Figure_5.py
sudo ./venv/bin/python Figure_6_a.py
sudo ./venv/bin/python Figure_6_b.py
sudo ./venv/bin/python Figure_6_c.py
sudo ./venv/bin/python Figure_7.py
sudo ./venv/bin/python Figure_8.py
```
