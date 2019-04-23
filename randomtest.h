#ifndef RANDOMTEST_H_
#define RANDOMTEST_H_

#include "data.h"
#include "utilities.h"

class RandomTest {
public:
	RandomTest(){};
    RandomTest(const int &numFeatures, const vector<double> &minFeatRange,
	const vector <int> FeatureId, const vector<double> &maxFeatRange) {
		// 随机选取一个特征
		m_feature = FeatureId[randomFromRange(0, (int)(numFeatures+1))];	
		// 从该随机特征最大值和最小值之间选取一个随机值
		m_threshold = randomFromRange(minFeatRange[m_feature], maxFeatRange[m_feature]);
    }
	
	void updateStats(Sample &sample, const bool &decision) {
		if (decision)
			leftChild.push_back(sample.Id);
		else
			rightChild.push_back(sample.Id);
	}

	// 用单个样本更新test
    void update(Sample &sample) {
        updateStats(sample, eval(sample));
    }

	// 评估样本应该放在左边还是右边(大于阈值放左边)
    bool eval(Sample &sample) {
        return sample.FeatureVector[m_feature] > m_threshold;
    }

	// 计算左右子集的方差和
	double score(const DataSet &dataset) {
		const DataSet *p_dataset = &dataset;
		double Dl, Dr;
		int sizeL = leftChild.size(), sizeR = rightChild.size();
		// 全部分到一个集合，返回double型最大值
		if (!sizeL || !sizeR)
			return (numeric_limits<double>::max)();
		
		// 计算左右子集总和
		double suml = 0.0, sumr = 0.0;
		for (int i = 0; i < sizeL; i ++) {
			suml += p_dataset->m_samples[leftChild[i]].Result;
		}
		for (int i = 0; i < sizeR; i ++) {
			sumr += p_dataset->m_samples[rightChild[i]].Result;
		}
		
		// 计算左右子集平均值
		double leftAvgReslut = suml/sizeL;
		double rightAvgReslut = sumr/sizeR;
		
		// 计算左右子集方差
		for (int i = 0; i < sizeL; i ++) {
			double result = p_dataset->m_samples[leftChild[i]].Result;
			Dl = (result-leftAvgReslut)*(result-leftAvgReslut)/sizeL;
		}
        for (int i = 0; i < sizeR; i ++) {
			double result = p_dataset->m_samples[rightChild[i]].Result;
			Dr = (result-rightAvgReslut)*(result-rightAvgReslut)/sizeR;
        }
		
		return Dl+Dr;
    }

	vector <int> leftChild;					// 左子集
	vector <int> rightChild;				// 右子集
	int m_feature;							// 所选特征
	double m_threshold;						// 阈值

};

#endif /* RANDOMTEST_H_ */
