# Requires Python version Python 3.9.5
## Execute the following commands

### Download and extract data
```
git lfs install
git lfs pull --include=data.zip
unzip data.zip
rm -rf __MACOSX/
```

### Install baselines
```
cd Baseline/PCA-CD/Libraries/
tar -xf libpca-1.2.11.tar.gz
tar -xf armadillo-4.200.0.tar.gz 
tar -xf lapack-3.5.0.tgz
cd libpca-1.2.11
sudo sh install.sh
brew install cmake
cd ../armadillo-4.200.0
cmake .
make
sudo make install
cmake .
make
sudo make install
cd ../../ChangeDetection/
sudo make
```

### Create virtual environment and install dependencies
#### Edit the second line below to point out to the location of `Python 3.9.5` in your machine
```
cd ../../..
virtualenv --python=/Library/Frameworks/Python.framework/Versions/3.9/bin/python3 venv
source ./venv/bin/activate
pip install ipykernel==6.6.0
sudo python -m ipykernel install --name=venv
pip install matplotlib==3.5.0
pip install scikit-learn==1.0.1
pip install jupyterlab==3.2.4
pip install runipy==0.1.5
pip install pdfkit==1.0.0
pip install -e DataInsights
```

# Generate plots and tables

```
runipy Figure_4.ipynb
runipy Figure_5.ipynb
python Figure_6_a.ipynb
python Figure_6_b.ipynb
runipy Figure_6_c.ipynb
runipy Figure_7.ipynb
runipy Figure_8.ipynb
```