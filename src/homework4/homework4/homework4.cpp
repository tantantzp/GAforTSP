// homework4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TSP.h"
#include <ctime>
#include <random>
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc == 3)
	{
		fin.open(argv[1]);
		fout.open(argv[2]);	
	}
	else
	{
		cout<<"homework4.exe input.txt output.txt";
		return 1;
	} 

	srand((unsigned int)time(0));
	int N;       //输入城市数
	fin>>N;
	TSP tsp(N, 0.95, 0.6, 0.65);     //创建旅行商类，参数表示交配率, 变异率, 进化率
	tsp.tsp();              //开始搜索
	
	system("pause");
	return 0;
}
