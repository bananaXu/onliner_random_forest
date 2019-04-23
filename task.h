#ifndef TASK_H_
#define TASK_H_

#include <iostream>
using namespace std;

class TASK
{
public:
	TASK() {};
	TASK(int x, int y, int r, int st, int end, int f)
		:x(x), y(y), r(r), st(st), end(end), f(f), wt(0) {}; 
	void output() 
	{
		cout << x << " " << y << " " << r << " " 
			 << st << " " << end << " " << f << endl;
	}
	bool operator < (const TASK &x) const 
	{
		return st < x.st;
	}
	int x;						// 任务x坐标 
	int y;						// 任务y坐标 
	int r;						// 任务能被接受半径 
	int st;						// 任务进入平台时间 
	int end;					// 任务结束时间 
	int f;						// 任务回报 
	int wt;						// 已等待时间 
};

#endif /* TASK_H_ */
