// homework4.cpp : �������̨Ӧ�ó������ڵ㡣
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
	int N;       //���������
	fin>>N;
	TSP tsp(N, 0.95, 0.6, 0.65);     //�����������࣬������ʾ������, ������, ������
	tsp.tsp();              //��ʼ����
	
	system("pause");
	return 0;
}
