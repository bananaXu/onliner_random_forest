#include <iostream>
#include <vector>
#include <windows.h> 

#include "worker.h"
#include "task.h"
#include "onlinerf.h" 
#include "Hyperparameters.h"
#include "data.h"
#include "regression.h"

using namespace std;
HANDLE m_hEventStart = CreateEvent(NULL, FALSE,FALSE, NULL);		// 算法开始事件
Hyperparameters hp;
DataSet dataset_tr;	

//void transform(vector <WORKER> &worker, int T)
//{
//	int n = worker.size();
//	DataSet dataset;
//	dataset.m_samples.resize(n);
//	dataset.m_numSamples = n;
//	dataset.m_numFeatures = hp.Features;
//	for (int i = 0; i < n; i ++)
//	{
//		Sample sample;
//		sample.FeatureVector.resize(hp.Features);
//		sample.Result = 0.0;
//		sample.Weight = 1.0;
//		sample.Id = ;
//		sample.FeatureVector[0] = ; 
//		sample.FeatureVector[1] = worker[i].x; 
//		sample.FeatureVector[2] = worker[i].y; 
//		sample.FeatureVector[3] = T; 
//		sample.FeatureVector[4] = worker[i].w; 
//		sample.FeatureVector[5] = worker[i].s; 
//		sample.FeatureVector[6] = ; 
//		sample.FeatureVector[7] = ; 
//		sample.FeatureVector[8] = ; 
//	}
//}

void matching(vector <WORKER> &matchWorker, vector <TASK> &matchTask)
{
	OnlineRF ORF(hp, dataset_tr.m_numFeatures, dataset_tr.m_minFeatRange, dataset_tr.m_maxFeatRange, dataset_tr);
	ORF.train(dataset_tr);
	ORF.test(dataset_tr); 
}

DWORD WINAPI forecastAndMatch(LPVOID lpParameter)						// 预测及匹配 
{
	vector <WORKER> worker;
	vector <TASK> task;
	// 重定向输入流为in.txt，将测试工人和任务数据一次性导入 
	ifstream fp(hp.testData, ios::binary);
	int workerNum, taskNum;
	fp >> workerNum >> taskNum;
	worker.resize(workerNum);
	task.resize(taskNum);
	worker.resize(workerNum);
	// 输入所有工人和任务 
	for (int i = 0; i < workerNum; i ++)
	{
		int x, y, r, t, s, w;
		fp >> x >> y >> r >> t >> s >> w;
		worker[i] = WORKER(x, y, r, t, s, w);
	}
	
	for (int i = 0; i < taskNum; i ++)
	{
		int x, y, r, st, end, f; 
		fp >> x >> y >> r >> st >> end >> f;
		task[i] = TASK(x, y, r, st, end, f); 
	}
	
	sort(worker.begin(), worker.end());					// 将工人按出现时间排序 
	sort(task.begin(), task.end());						// 将任务按出现时间排序 
	fp.close();
	
	// 每过时间T进行一次匹配 
	int i = 0, j = 0, t = 0;							// i定位工人，j定位任务，t代表时间段 
	vector <WORKER> matchWorker;						// 每轮进行匹配的工人 
	vector <TASK> matchTask;							// 每轮进行匹配的任务 
	
	while (i < workerNum && j < taskNum)
	{
		WaitForSingleObject(m_hEventStart, INFINITE);	// 等待算法开始事件被触发
		
		while (i < workerNum && worker[i].t == t) 		// 取出t时刻出现的工人 
		{
			matchWorker.push_back(worker[i]);
			i ++;
		}
		while (j < taskNum && task[j].st == t)			// 取出t时刻出现的任务 
		{
			matchTask.push_back(task[j]);
			j ++;
		}
		
		cout << "第" << t << "轮匹配开始：" << endl;
		srand(time(0));
		matching(matchWorker, matchTask);				// 进行匹配 
		cout << "第" << t << "轮匹配结束！" << endl << endl;
		
		t ++;											// 轮数加一 
	}
	CloseHandle(m_hEventStart);							// 关闭事件 
	cout << "匹配已完成！" << endl;
	hp.flag = true;
	// 关闭输入流 
	return 0L;
}

DWORD WINAPI setTimer(LPVOID lpParameter)			// 定时器，每隔TIMER时间发一个信号开始匹配 
{
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, NULL);
	while (!hp.flag)
	{
		WaitForSingleObject(m_hMutex, INFINITE);
		SetEvent(m_hEventStart);
		Sleep(hp.TIMER);
		CloseHandle(m_hMutex);
	} 
	return 0L;
}

int main()
{
	cout << "在线随机森林训练中！" << endl;
	// 初始化在线随机森林 
	dataset_tr.loadData("train.txt");		// 加载训练数据集 
	// dataset_tr.m_samples[0].disp();		// 测试一下输出 

	// 创建一个线程每一段时间发出一个信号
	HANDLE setTimerThread = CreateThread(NULL, 0, setTimer, NULL, 0, NULL);
	// 创建一个线程进行在线随机森林预测 
	HANDLE forecastThread = CreateThread(NULL, 0, forecastAndMatch, NULL, 0, NULL);
	
	// 等待子线程结束 
	WaitForSingleObject(forecastThread, INFINITE);
	WaitForSingleObject(setTimerThread, INFINITE);
	
	// 关闭所有句柄 
	CloseHandle(forecastThread);
	CloseHandle(setTimerThread);
	
	return 0;
}
