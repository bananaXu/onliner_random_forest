#ifndef ONLINETREE_H_
#define ONLINETREE_H_

#include <cstdlib>
#include <iostream>

#include "regression.h"
#include "data.h"
#include "hyperparameters.h"
#include "onlinenode.h"

using namespace std;

class OnlineTree: public Regressor {
public:
	OnlineTree(){};
    OnlineTree(const Hyperparameters &hp, const int &numFeatures, const vector<double> &minFeatRange, const vector<double> &maxFeatRange,
			const DataSet &dataset_tr) :
        m_hp(&hp), m_rootNode(NULL), m_dataset(&dataset_tr) {
       	vector <int> m_sampleId();
        m_rootNode = new OnlineNode(hp, numFeatures, minFeatRange, maxFeatRange, 0, vector <int>(), 0, dataset_tr);
    }

    ~OnlineTree() {
        delete m_rootNode;
    }

	// 用单个样本更新决策树
    virtual void update(Sample &sample) {
        m_rootNode->update(sample);
    }

	// 训练决策树
    virtual void train(DataSet &dataset) {
        vector<int> randIndex;
        int sampRatio = m_dataset->m_numSamples / 10;
		// 训练numEpochs次
        for (int n = 0; n < m_hp->numEpochs; n++) {
			// 随机样本输入顺序
            randPerm(dataset.m_numSamples, randIndex);
            for (int i = 0; i < m_dataset->m_numSamples; i++) {
				// 用该样本更新决策树
                update(dataset.m_samples[randIndex[i]]);
                if (i%sampRatio == 0) {
                    cout << "--- Online Random Tree training --- Epoch: " << n + 1 << " --- ";
                    cout << (10 * i) / sampRatio << "%" << endl;
                }
            }
        }
    }

	// 预测
    virtual double eval(Sample &sample) {
        return m_rootNode->eval(sample);
    }

	// 测试
    virtual vector <double> test(DataSet &dataset) {
        vector<double> res(dataset.m_numSamples);
        for (int i = 0; i < dataset.m_numSamples; i ++)
        	res[i] = m_rootNode->eval(m_dataset->m_samples[i]);
        return res;
    }

	// 边预测边训练
    virtual vector <double> trainAndTest(DataSet &dataset_tr) {
		vector <double> results;
        return results;
    }

private:
    const Hyperparameters *m_hp;				// 超参数 
	const DataSet *m_dataset; 
    OnlineNode* m_rootNode;
};

#endif /* ONLINETREE_H_ */
