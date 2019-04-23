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
		// Ϊ���ɭ���½�numTrees�Żع���
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

	// �õ��������������ɭ��
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
				// OOB�ж� 
            	// m_trees[i]->eval(sample);
			}
        }
    }

	// ѵ�����ɭ��
    virtual void train(DataSet &dataset) {
        vector<int> randIndex;
        int sampRatio = dataset.m_numSamples / 10;
        for (int n = 0; n < m_hp->numEpochs; n ++) {					// ������ѵ��numEpochs��
            randPerm(dataset.m_numSamples, randIndex);					// �����������˳��
            for (int i = 0; i < dataset.m_numSamples; i++) {
                update(dataset.m_samples[randIndex[i]]);
                if (i%sampRatio == 0) {
                    cout << "--- Online Random Forest training --- Epoch: " << n + 1 << " --- ";
                    cout << (10 * i) / sampRatio << "%" << endl;
                }
            }
        }
    }

	// �Ե�����������Ԥ��
    virtual double eval(Sample &sample) {
    	double sum = 0.0;
		for (int i = 0; i < m_hp->numTrees; i ++)
			sum += m_trees[i]->eval(sample);
		return sum/m_hp->numTrees;
    }

	// �����ݼ����в���
    virtual vector<double> test(DataSet &dataset) {
    	vector <double> res(dataset.m_numSamples);
    	for (int i = 0; i < dataset.m_numSamples; i ++)
    	{
    		res[i] = eval(dataset.m_samples[i]);
    		cout << "ʵ��ֵ��" << dataset.m_samples[i].Result << "   Ԥ��ֵ��" << res[i]<< endl;
   		} 
    	return res;
    }

	// ���������ݼ���������Ԥ��(Ԥ��ͬʱ�����������ɭ��)
    virtual vector<double> trainAndTest(DataSet &dataset) {
    	vector <double> res(dataset.m_numSamples);
        vector<int> randIndex;
        int sampRatio = dataset.m_numSamples / 10;
        for (int n = 0; n < m_hp->numEpochs; n ++) {					// ������ѵ��numEpochs��
            randPerm(dataset.m_numSamples, randIndex);					// �����������˳��
            for (int i = 0; i < dataset.m_numSamples; i++) {
                update(dataset.m_samples[randIndex[i]]);
                if (i%sampRatio == 0) {
                    cout << "--- Online Random Forest training --- Epoch: " << n + 1 << " --- ";
                    cout << (10 * i) / sampRatio << "%" << endl;
                }
            }
            
            res = test(dataset);
            for (int i = 0; i < dataset.m_numSamples; i ++)
            	cout << "ʵ��ֵ��" << dataset.m_samples[i].Result << "   Ԥ��ֵ��" << res[i]<< endl;
        }
        return res;
    }

private:
    const Hyperparameters *m_hp;		// ������ 

    vector<OnlineTree*> m_trees;		// ���ļ��� 
};

#endif /* ONLINERF_H_ */
