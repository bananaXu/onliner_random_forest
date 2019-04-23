#ifndef DATA_H_
#define DATA_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <cmath>
using namespace std;

class Sample {
public:
    vector <double> FeatureVector;			// 特征向量 
    double Result;							// 结果值(本文为工人所获任务回报率)
    double Weight;							// 权重 
	int Id;									// id序号

    void disp() {
    	cout << "FeatureVector：";
    	for (int i = 0; i < FeatureVector.size(); i ++)
    	{
	    	cout << FeatureVector[i] << " ";
	    }
	    cout << endl;
	    cout << "Result：" << Result << endl;
	    cout << "Weight：" << Weight << endl;
    }
};

class DataSet {
public:
    vector <Sample> m_samples;
    int m_numSamples;						// 样本数量 
    int m_numFeatures;						// 特征数量 

    vector<double> m_minFeatRange;			// 最小特征 
    vector<double> m_maxFeatRange;			// 最大特征 

    void findFeatRange() {					// 寻找特征范围 
	    double minVal, maxVal;
	    for (int i = 0; i < m_numFeatures; i++) {
	        minVal = m_samples[0].FeatureVector[i];
	        maxVal = m_samples[0].FeatureVector[i];
	        for (int n = 1; n < m_numSamples; n++) {
	                minVal = min(minVal, m_samples[n].FeatureVector[i]);
	                maxVal = max(maxVal, m_samples[n].FeatureVector[i]);
	        }
	
	        m_minFeatRange.push_back(minVal);
	        m_maxFeatRange.push_back(maxVal);
	    }
	};

	// 从文件加载数据 
    void loadData(char *filename)	{
		ifstream fp(filename, ios::binary);
		
		fp >> m_numSamples;
		fp >> m_numFeatures;
		m_samples.clear();
		
		for (int i = 0; i < m_numSamples; i ++)
		{
			Sample sample;
			sample.FeatureVector.resize(m_numFeatures);
			for (int j = 0; j < m_numFeatures; j ++)
				fp >> sample.FeatureVector[j];
			fp >> sample.Result;
			sample.Weight = 1.0;
			sample.Id = i;
			m_samples.push_back(sample);
		}
		
		fp.close();
	    findFeatRange();
	};
};

#endif /* DATA_H_ */
