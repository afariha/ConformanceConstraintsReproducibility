##############################################################################
# Python/NumPy/Sklearn implementation of SPLL Change Detection Algorithm (Kuncheva, 2013)
# Based on Matlab code from https://github.com/LucyKuncheva/Change-detection
# This code was translated from Matlab to Python with minimal changes. Code is distributed under
# original Matlab code license (GPL-3.0)
# Author: Haidar Khan
# Date: 10/23/18
# Description: Implements the change point detection algorithm proposed by (Kuncheva, 2013) using a
# window based approach. 
# Usage: SPLL(W1,W2,K) tests if a change occurs between data windows W1 and W2 with K-Means clustering 
# parameter K

import numpy as np
from sklearn.cluster import KMeans
from scipy.stats import chi2
EPS = np.finfo(float).eps

def SPLL(W1, W2, K=3):
    n1 = W1.shape[1]
    n2 = W2.shape[1]

    assert n1==n2, "The number of features must be the same for W1 and W2"
    n = n1
    s1 = log_LL(W1,W2,K)
    s2 = log_LL(W2,W1,K)

    st = max(s1,s2)

    pst = min(chi2.cdf(st,n), 1-chi2.cdf(st,n))
    Change = float(pst < 0.05)
    return Change, pst, st

def log_LL(W1,W2,K):
    M1, n = W1.shape
    M2, _ = W2.shape

    kmeans = KMeans(n_clusters=K).fit(W1)

    labels = kmeans.predict(W1)
    means = kmeans.cluster_centers_

    SC = np.zeros((n*n, K))
    ClassPriors = np.zeros((K,1))
    for k in range(K):
        ClassIndex = np.nonzero(labels==k)[0]
        ClassPriors[k] = len(ClassIndex)
        if len(ClassIndex) <= 1:
            sc = np.zeros((n,n))
        else:
            sc = np.cov(W1[labels==k,:].T)
        SC[:,k] = sc.flatten()

    ClassCount = ClassPriors
    ClassPriors = ClassPriors/M1

    scov = np.sum(SC*np.tile(ClassPriors, (1,n*n)).T,1) 

    # Assuming the same full covariance
    # Pooled (weighted by the priors)
    scov = np.reshape(scov, (n,n))
    z = np.diag(scov)

    indexvarzero = z < EPS # define eps #identify features with very low variability
    if sum(indexvarzero) == 0:  #if no such features
        invscov = np.linalg.inv(scov)  # invert the covariance matrix
    else:   # otherwise
        z[indexvarzero] = np.min(z[not indexvarzero])   # set their variance to the minimum variance of other features
        invscov = np.diag(1/z) # invert the diagonal matrix only

    # Calculate the MATCH from Window 2
    LogLikelihoodTerm = np.zeros((M2))
    for j in range(M2):
        xx = W2[j,:]
        DistanceToMeans = np.zeros((K))
        for ii in range(K):
            if ClassCount[ii] > 0:
                DistanceToMeans[ii] = np.dot(np.dot(means[ii,:] - xx , invscov), means[ii,:]-xx)
            else:
                DistanceToMeans[ii] = np.inf
        LogLikelihoodTerm[j] = np.min(DistanceToMeans)
        # scale
    st = np.mean(LogLikelihoodTerm)
    return st