#ifndef WORKER_H_
#define WORKER_H_

#include <iostream>
using namespace std;

class WORKER
{
public:
	WORKER() {};
	WORKER(int x, int y, int r, int t, int s, int w)
		:x(x), y(y), r(r), t(t), s(s), w(w), wt(0) {}; 
	void output() 
	{
		cout << x << " " << y << " " << r << " " 
			 << t << " " << s << " " << w << endl;
	}
	bool operator < (const WORKER &x) const 
	{
		return t < x.t;
	}
	int x;						// 工人x坐标 
	int y;						// 工人y坐标 
	int r;						// 工人接受任务半径 
	int t;						// 工人进入平台时间 
	int s;						// 工人效率 
	int w;						// 气温 
	int wt;						// 已等待时间 
};

#endif /* WORKER_H_ */
