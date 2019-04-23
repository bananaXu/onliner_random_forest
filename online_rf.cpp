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
HANDLE m_hEventStart = CreateEvent(NULL, FALSE,FALSE, NULL);		// �㷨��ʼ�¼�
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

DWORD WINAPI forecastAndMatch(LPVOID lpParameter)						// Ԥ�⼰ƥ�� 
{
	vector <WORKER> worker;
	vector <TASK> task;
	// �ض���������Ϊin.txt�������Թ��˺���������һ���Ե��� 
	ifstream fp(hp.testData, ios::binary);
	int workerNum, taskNum;
	fp >> workerNum >> taskNum;
	worker.resize(workerNum);
	task.resize(taskNum);
	worker.resize(workerNum);
	// �������й��˺����� 
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
	
	sort(worker.begin(), worker.end());					// �����˰�����ʱ������ 
	sort(task.begin(), task.end());						// �����񰴳���ʱ������ 
	fp.close();
	
	// ÿ��ʱ��T����һ��ƥ�� 
	int i = 0, j = 0, t = 0;							// i��λ���ˣ�j��λ����t����ʱ��� 
	vector <WORKER> matchWorker;						// ÿ�ֽ���ƥ��Ĺ��� 
	vector <TASK> matchTask;							// ÿ�ֽ���ƥ������� 
	
	while (i < workerNum && j < taskNum)
	{
		WaitForSingleObject(m_hEventStart, INFINITE);	// �ȴ��㷨��ʼ�¼�������
		
		while (i < workerNum && worker[i].t == t) 		// ȡ��tʱ�̳��ֵĹ��� 
		{
			matchWorker.push_back(worker[i]);
			i ++;
		}
		while (j < taskNum && task[j].st == t)			// ȡ��tʱ�̳��ֵ����� 
		{
			matchTask.push_back(task[j]);
			j ++;
		}
		
		cout << "��" << t << "��ƥ�俪ʼ��" << endl;
		srand(time(0));
		matching(matchWorker, matchTask);				// ����ƥ�� 
		cout << "��" << t << "��ƥ�������" << endl << endl;
		
		t ++;											// ������һ 
	}
	CloseHandle(m_hEventStart);							// �ر��¼� 
	cout << "ƥ������ɣ�" << endl;
	hp.flag = true;
	// �ر������� 
	return 0L;
}

DWORD WINAPI setTimer(LPVOID lpParameter)			// ��ʱ����ÿ��TIMERʱ�䷢һ���źſ�ʼƥ�� 
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
	cout << "�������ɭ��ѵ���У�" << endl;
	// ��ʼ���������ɭ�� 
	dataset_tr.loadData("train.txt");		// ����ѵ�����ݼ� 
	// dataset_tr.m_samples[0].disp();		// ����һ����� 

	// ����һ���߳�ÿһ��ʱ�䷢��һ���ź�
	HANDLE setTimerThread = CreateThread(NULL, 0, setTimer, NULL, 0, NULL);
	// ����һ���߳̽����������ɭ��Ԥ�� 
	HANDLE forecastThread = CreateThread(NULL, 0, forecastAndMatch, NULL, 0, NULL);
	
	// �ȴ����߳̽��� 
	WaitForSingleObject(forecastThread, INFINITE);
	WaitForSingleObject(setTimerThread, INFINITE);
	
	// �ر����о�� 
	CloseHandle(forecastThread);
	CloseHandle(setTimerThread);
	
	return 0;
}
