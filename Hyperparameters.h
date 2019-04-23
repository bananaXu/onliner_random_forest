#ifndef HYPERPARAMETERS_H_
#define HYPERPARAMETERS_H_

#include <string>
using namespace std;

class Hyperparameters
{
public:
	Hyperparameters():Features(9), maxDepth(20), TIMER(1000), flag(false), trainData("train.txt"), 
				testData("test.txt"), outData("out.txt"), numTrees(20),  numEpochs(5), DThreshold(0.5),
				counter(10), numRandFeat(5) {};
	bool flag; 				// 样本是否测试完
    int Features;			// 特征数量
	int numRandFeat;		// 每次抽取的特征数量
    int TIMER;				// 定时时长
    int maxDepth;			// 最大深度
    
    int numTrees;			// 树的数量
    int numEpochs;			// 训练次数 
	int counter;			// 样本数量阈值(超过阈值叶子节点才能分裂)
	double DThreshold;		// 样本方差阈值(超过阈值叶子节点才能分裂)
    // Data
    char *trainData;
	char *testData;
	char *worker;
	char *task;

    // Output
    string outData;
};

#endif /* HYPERPARAMETERS_H_ */
