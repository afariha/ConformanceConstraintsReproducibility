# Reproduction Steps
- Requires Python version Python 3.9.5 (Python 3.7 and 3.8 should also work, but we recommend 3.9.5)

### (Step 1) Initial setup
- If using MAC, install homebrew from here: https://brew.sh/
- If using Windows, do the following to prepare first
  - From Power Shell (run as Administrator), execute `wsl --install`. Once done, reboot.
  - Download and install ubuntu from this link: https://ubuntu.com/wsl

### (Step 2) Clone this repository and cd to it
```
git clone https://github.com/afariha/ConformanceConstraintsReproducibility.git
cd ConformanceConstraintsReproducibility
```

### (Step 3) Reproduce (Automatic)
- Windows/Linux: Open the Ubuntu terminal (bash) and run `sudo sh ./reproduce_linux.sh`.
- MAC: Open terminal (bash) and run `sudo sh ./reproduce_mac.sh`.

### (Step 3) Reproduce (Manual steps)

- (Step 3-A) Prepare the system: Only needed for Windows WSL or Linux (Ubuntu)
```
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt-get update
sudo apt install -y unzip
sudo apt-get install -y build-essential
sudo apt-get install -y python3-pip
```

- Note: For what follows, if you are using Linux or Ubuntu WSL over Windows: whenever present, use the command in parentheses. If using MAC, ignore anything in parentheses.

- (Step 3-B) Install texlive
```
brew install texlive (sudo apt-get install -y texlive-full)
```

- (Step 3-C) Download and extract data using git-lfs (or just simply download the data.zip from the repo and extract it manually).
```
brew install git-lfs	(sudo apt-get install -y git-lfs)
git lfs install
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/
```

- (Step 3-D) Install baselines and related depenedencies
```
brew install cmake	(sudo apt install -y cmake)
(sudo apt-get install -y liblapack-dev)
cd Baseline/PCA-CD/Libraries/
tar -xf libpca-1.2.11.tar.gz
tar -xf armadillo-4.200.0.tar.gz 
cd armadillo-4.200.0
cmake .
sudo make install
cd ../libpca-1.2.11
sudo sh install.sh
cd ../../ChangeDetection/
sudo make
(sudo ldconfig)
(sudo apt-get install -y libarmadillo-dev)
```

- (Step 3-E) Create a virtual environment and install dependencies
  - Note: Edit the third line below to point out to the location of `Python 3.9.5` in your machine.
```
cd ../../..
brew install virtualenv (sudo apt-get install -y virtualenv)
virtualenv --python=/Library/Frameworks/Python.framework/Versions/3.9/bin/python3 venv (virtualenv --python=/usr/bin/python3 venv)
source ./venv/bin/activate 
sudo ./venv/bin/pip install matplotlib==3.5.0
sudo ./venv/bin/pip install scikit-learn==1.0.1
sudo ./venv/bin/pip install -e DataInsights
```

- (Step 3-F) Generate plots and tables

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

## What to expect?
You should see 7 files within the directory `Plots`. They should match the corresponding ones in the [paper](https://dl.acm.org/doi/abs/10.1145/3448016.3452795).


## Reproducing the paper
### Execute the following commands

```
cd Paper
pdflatex paper
bibtex paper
pdflatex paper
pdflatex paper
```

You should see the paper reproduced as `paper.pdf` within the directory `Paper`.
