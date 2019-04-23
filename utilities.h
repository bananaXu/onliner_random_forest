#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
using namespace std;

//! Returns a random number in [0, 1]
double randDouble() {
    return rand() / (RAND_MAX + 1.0);
}

//! Returns a random number in [min, max]
double randomFromRange(const double &minRange, const double &maxRange) {
	double x = randDouble();
    return minRange + (maxRange - minRange)*x;
}

int randomFromRange(const int &minRange, const int &maxRange) {
	if (minRange == minRange)
		return minRange;
    return minRange + rand()%(maxRange - minRange);
}

//! Random permutations
void randPerm(const int &inNum, vector<int> &outVect){
    outVect.resize(inNum);
    int randIndex, tempIndex;
    for (int nFeat = 0; nFeat < inNum; nFeat++) {
        outVect[nFeat] = nFeat;
    }
    for (register int nFeat = 0; nFeat < inNum; nFeat++) {
        randIndex = (int) floor(((double) inNum - nFeat) * randDouble()) + nFeat;
        if (randIndex == inNum) {
            randIndex--;
        }
        tempIndex = outVect[nFeat];
        outVect[nFeat] = outVect[randIndex];
        outVect[randIndex] = tempIndex;
    }
}

void randPerm(const int &inNum, const int inPart, vector<int> &outVect) {
    outVect.resize(inNum);
    int randIndex, tempIndex;
    for (int nFeat = 0; nFeat < inNum; nFeat++) {
        outVect[nFeat] = nFeat;
    }
    for (register int nFeat = 0; nFeat < inPart; nFeat++) {
        randIndex = (int) floor(((double) inNum - nFeat) * randDouble()) + nFeat;
        if (randIndex == inNum) {
            randIndex--;
        }
        tempIndex = outVect[nFeat];
        outVect[nFeat] = outVect[randIndex];
        outVect[randIndex] = tempIndex;
    }

    outVect.erase(outVect.begin() + inPart, outVect.end());
}

//! Poisson sampling
int poisson(double A) {
    int k = 0;
    int maxK = 10;
    while (1) {
        double U_k = randDouble();
        A *= U_k;
        if (k > maxK || A < exp(-1.0)) {
            break;
        }
        k++;
    }
    return k;
}

#endif /* UTILITIES_H_ */
