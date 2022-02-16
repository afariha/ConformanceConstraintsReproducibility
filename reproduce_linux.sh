curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt-get update
sudo apt install -y unzip
sudo apt-get install -y build-essential
sudo apt-get install -y python3-pip
sudo apt-get install -y texlive-full
sudo apt-get install -y git-lfs
git lfs install
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/
sudo apt install -y cmake
sudo apt-get install -y liblapack-dev
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
sudo ldconfig
sudo apt-get install -y libarmadillo-dev
cd ../../..
sudo apt-get install -y virtualenv
virtualenv --python=/usr/bin/python3 venv
source ./venv/bin/activate 
sudo ./venv/bin/pip install matplotlib==3.5.0
sudo ./venv/bin/pip install scikit-learn==1.0.1
sudo ./venv/bin/pip install -e DataInsights
sudo mkdir Plots
sudo ./venv/bin/python Figure_4.py
sudo ./venv/bin/python Figure_5.py
sudo ./venv/bin/python Figure_6_a.py
sudo ./venv/bin/python Figure_6_b.py
sudo ./venv/bin/python Figure_6_c.py
sudo ./venv/bin/python Figure_7.py
sudo ./venv/bin/python Figure_8.py
cd Paper
pdflatex paper
bibtex paper
pdflatex paper
pdflatex paper



