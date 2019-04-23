#ifndef ONLINERF_H_
#define ONLINERF_H_

#include "regression.h"
#include "data.h"
#include "hyperparameters.h"
#include "onlinetree.h"
#include "utilities.h"

class OnlineRF: public Regressor {
public:
    OnlineRF(const Hyperparameters &hp, const int &numFeatures, const vector<double> &minFeatRange,
            const vector<double> &maxFeatRange, const DataSet &dataset_tr) : m_hp(&hp) {
        OnlineTree *tree;
		// 为随机森林新建numTrees颗回归树
        for (int i = 0; i < hp.numTrees; i++) {
            tree = new OnlineTree(hp, numFeatures, minFeatRange, maxFeatRange, dataset_tr);
            m_trees.push_back(tree);
        }
    }

    ~OnlineRF() {
        for (int i = 0; i < m_hp->numTrees; i++) {
            delete m_trees[i];
        }
    }

	// 用单个样本更新随机森林
    virtual void update(Sample &sample) {
        for (int i = 0; i < m_hp->numTrees; i++) {
            int numTries = poisson(1.0);
            if (numTries) 
			{
                for (int n = 0; n < numTries; n++)
                    m_trees[i]->update(sample);
            } 
			else
			{
				// OOB判断 
            	// m_trees[i]->eval(sample);
			}
        }
    }

	// 训练随机森林
    virtual void train(DataSet &dataset) {
        vector<int> randIndex;
        int sampRatio = dataset.m_numSamples / 10;
        for (int n = 0; n < m_hp->numEpochs; n ++) {					// 对样本训练numEpochs次
            randPerm(dataset.m_numSamples, randIndex);					// 随机样本输入顺序
            for (int i = 0; i < dataset.m_numSamples; i++) {
                update(dataset.m_samples[randIndex[i]]);
                if (i%sampRatio == 0) {
                    cout << "--- Online Random Forest training --- Epoch: " << n + 1 << " --- ";
                    cout << (10 * i) / sampRatio << "%" << endl;
                }
            }
        }
    }

	// 对单个样本进行预测
    virtual double eval(Sample &sample) {
    	double sum = 0.0;
		for (int i = 0; i < m_hp->numTrees; i ++)
			sum += m_trees[i]->eval(sample);
		return sum/m_hp->numTrees;
    }

	// 对数据集进行测试
    virtual vector<double> test(DataSet &dataset) {
    	vector <double> res(dataset.m_numSamples);
    	for (int i = 0; i < dataset.m_numSamples; i ++)
    	{
    		res[i] = eval(dataset.m_samples[i]);
    		cout << "实际值：" << dataset.m_samples[i].Result << "   预测值：" << res[i]<< endl;
   		} 
    	return res;
    }

	// 对整个数据集进行在线预测(预测同时更新在线随机森林)
    virtual vector<double> trainAndTest(DataSet &dataset) {
    	vector <double> res(dataset.m_numSamples);
        vector<int> randIndex;
        int sampRatio = dataset.m_numSamples / 10;
        for (int n = 0; n < m_hp->numEpochs; n ++) {					// 对样本训练numEpochs次
            randPerm(dataset.m_numSamples, randIndex);					// 随机样本输入顺序
            for (int i = 0; i < dataset.m_numSamples; i++) {
                update(dataset.m_samples[randIndex[i]]);
                if (i%sampRatio == 0) {
                    cout << "--- Online Random Forest training --- Epoch: " << n + 1 << " --- ";
                    cout << (10 * i) / sampRatio << "%" << endl;
                }
            }
            
            res = test(dataset);
            for (int i = 0; i < dataset.m_numSamples; i ++)
            	cout << "实际值：" << dataset.m_samples[i].Result << "   预测值：" << res[i]<< endl;
        }
        return res;
    }

private:
    const Hyperparameters *m_hp;		// 超参数 

    vector<OnlineTree*> m_trees;		// 树的集合 
};

#endif /* ONLINERF_H_ */
