#ifndef ONLINENODE_H_
#define ONLINENODE_H_

#include <vector>

#include "data.h"
#include "hyperparameters.h"
#include "randomtest.h"
#include "utilities.h"

using namespace std;

class OnlineNode {
public:
    OnlineNode(const Hyperparameters &hp, const int &numFeatures, const vector <double> &minFeatRange,
            const vector<double> &maxFeatRange, const int &depth, vector <int> sampleId,
			int counter, const DataSet &dataset): m_dataset(&dataset),
       	 	m_numFeatures(&numFeatures), m_depth(depth), m_isLeaf(true), m_counter(counter),
			m_hp(&hp), m_minFeatRange(&minFeatRange), m_maxFeatRange(&maxFeatRange), 
			m_sampleId(sampleId), m_leftChildNode(NULL), m_rightChildNode(NULL) {

		// 随机获取指定数量的特征
		vector <int> FeatureId;
		randPerm(numFeatures, hp.numRandFeat, FeatureId);
		
        // Creating random tests
        for (int i = 0; i < hp.numRandFeat; i++) {
            m_onlineTests.push_back(RandomTest(hp.numRandFeat, minFeatRange, FeatureId, maxFeatRange));
        }
    }

    ~OnlineNode() {
        if (!m_isLeaf) {
            delete m_leftChildNode;
            delete m_rightChildNode;
        }
    }

    void update(Sample &sample){
		m_counter ++;
		m_sampleId.push_back(sample.Id);
		
		if (m_isLeaf) {
			// 更新随机阈值
			for (int i = 0; i < m_hp->numRandFeat; i++) {
				m_onlineTests[i].update(sample);
			}

			// 更新平均值
			m_avgResultS = getAvg();

			// 是否分裂
			if (shouldISplit()) {
				m_isLeaf = false;

				// Find the best online test
				int minIndex = 0;
				double minScore, score; 
				minScore = (numeric_limits<double>::max)();
				for (int i = 0; i < m_hp->numRandFeat; i++) {
					score = m_onlineTests[i].score(*m_dataset);
					if (score < minScore) {
						minScore = score;
						minIndex = i;
					}
				}
				m_bestTest = m_onlineTests[minIndex];
				m_onlineTests.clear();

				cout << "--- Splitting node --- best score: " << minScore;
				cout << " by test number: " << minIndex << endl;

				// 分裂
				m_rightChildNode = new OnlineNode(*m_hp, *m_numFeatures, *m_minFeatRange, *m_maxFeatRange,
		 			m_depth+1, m_bestTest.leftChild, m_bestTest.leftChild.size(), *m_dataset);
				m_leftChildNode = new OnlineNode(*m_hp, *m_numFeatures, *m_minFeatRange, *m_maxFeatRange, 
					m_depth+1, m_bestTest.rightChild, m_bestTest.rightChild.size(), *m_dataset);
			}
		} else {
			if (sample.FeatureVector[m_bestTest.m_feature] > m_bestTest.m_threshold) {
				m_rightChildNode->update(sample);
			} else {
				m_leftChildNode->update(sample);
			}
		}
	}

	// 预测
    double eval(const Sample &sample) {
        if (m_isLeaf) {
			return m_avgResultS;
        } else {
			// 大于阈值在左子树寻找
            if (sample.FeatureVector[m_bestTest.m_feature] > m_bestTest.m_threshold) {
                return m_leftChildNode->eval(sample);
            } else {
                return m_rightChildNode->eval(sample);
            }
        }
    }

private:
	const DataSet *m_dataset;					// 根节点样本
	vector <int> m_sampleId;					// 当前节点所含样本编号

    int m_counter;								// 样本数量(工人数量)
    const int *m_numFeatures;					// 特征数量
    int m_depth;								// 节点深度
    bool m_isLeaf;								// 是否叶子节点
    const Hyperparameters *m_hp;				// 超参数
    const vector<double> *m_minFeatRange;		// 特征最小值
    const vector<double> *m_maxFeatRange;		// 特征最大值
	
    OnlineNode* m_leftChildNode;				// 左孩子节点
    OnlineNode* m_rightChildNode;				// 右孩子节点
	double m_avgResultS;						// 样本平均值(如果是叶子节点也是预测值)
	
    vector <RandomTest> m_onlineTests;			// 随机取多个阈值(为了选出最优分裂特征)
    RandomTest m_bestTest;						// 当前节点分裂效果最好的特征
	
	// 求本节点样本平均值
	double getAvg()
	{
		double sumResultS = 0.0;
		for (int i = 0; i < m_counter; i ++)
		{
			int t = m_sampleId[i];
			sumResultS += m_dataset->m_samples[t].Result;
		}
		return sumResultS/m_counter;
	}

    bool shouldISplit() {
		// 计算方差
        double D = 0.0;
        for (int i = 0; i < m_counter; i++) {
			// 从原始样本中取出当前节点每个样本结果值
			double res = m_dataset->m_samples[m_sampleId[i]].Result;
			D += (res-m_avgResultS)*(res-m_avgResultS)/m_counter;
			if (D > m_hp->DThreshold)
				break;
        }
		// 方差小于maxD不分裂，达到最大深度不分裂，没达到样本数量要求不分裂
		if (D <= m_hp->DThreshold || m_depth >= m_hp->maxDepth || m_counter < m_hp->counter)
			return false;
        return true;
    }

};

#endif /* ONLINENODE_H_ */
