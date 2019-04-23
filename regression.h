#ifndef REGRESSION_H_
#define REGRESSION_H_

#include <vector>
#include "data.h"

using namespace std;

class Regressor {
public:
    virtual void update(Sample &sample) = 0;
    virtual void train(DataSet &dataset) = 0;
    virtual double eval(Sample &sample) = 0;
    virtual vector<double> test(DataSet & dataset) = 0;
    virtual vector<double> trainAndTest(DataSet &dataset_tr) = 0;
};

#endif /* REGRESSION_H_ */
