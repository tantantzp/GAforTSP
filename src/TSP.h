#ifndef TSP_H
#define TSP_H

#include "iostream"
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <random>
#include <fstream>
#include <time.h>


using namespace std;

static ifstream fin;
static ofstream fout;


class Node           //���нڵ���
{
public:
	double x,y;
	char name;
	void set(double a, double b, char c){ x = a, y = b, name = c;}
};

class Node_map     //���е�ͼ�࣬ ��¼�������
{
public:
	int N;
	Node* node;
    double** length_table;           //�������

	Node_map(int n);
	~Node_map();
	void init();
	void cacu_distance();
};

class Path        //·����
{
public:
	int N;
	int* route;           //·��
    double length;        //·������
	double fitness;       //��Ӧ��
	Node_map* node_map;

	Path(int n, Node_map* nm);
	~Path(){ delete []route; }

	void randon_route();
	double cacu_length();
	void change();
	void cross(Path* pair, double evolve_rate);
	void output();
    bool operator < ( Path& p );
	void operator =( Path& p);
};

class Group      //��Ⱥ��
{
public:
	int N;               //������
	int size;             //��Ⱥ��ģ
	double cross_rate;      //������
	double mutation_rate;          //������
    double evolve_rate;             //������,�������ܸ�������ĸ���
	Path** codes;              //��Ⱥ
	Path* best_code;           //���Ž�
	Node_map* node_map;

	Group(int n, Node_map* nmap, double cr, double mr, double er)
	{
	    N = n;
		node_map = nmap;
		size = 800 * N;    //��Ⱥ��С, �����������
		cross_rate = cr, mutation_rate = mr, evolve_rate = er;
		codes = new Path* [size];
		for(int i = 0; i < size; i++)
		    codes[i] = new Path(N, node_map);
		best_code = new Path(N, node_map);
	}
	~Group()
	{
	    delete best_code;
		for(int i = 0; i < size; i++)
		    delete codes[i];
		delete codes;
	}

	void randon_group();    //�����������Ⱥ
	void mix_group();       //����˳��
	void sort_fitness();    //����Ӧ������
	void cacu_fitness();
	void cross_over();      //���ս����ʽ��н���
	void mutation();        //���ձ����ʽ��н���
	void operator = (Group& g)
	{
	    for(int i = 0; i < size; i++)
		{
		    (*codes[i]) = (*g.codes[i]);
		}
		(*best_code) = (*g.best_code);
	}
};


class TSP                //�������࣬ �Ŵ��㷨���
{
public:
	int N;              //������
    Node_map* node_map;
	int iterate;      //��������
	double cross_rate;    //������
	double mutation_rate;    //������
    double evolve_rate;      //������
	Group* group;             //��Ⱥ
	Group* tmp_group;
	Path* best_code;          //���Ž�

	TSP(int n, double cr, double mr, double er)
	{
	    N = n;
		node_map = new Node_map(N);
		node_map->init();		
		iterate = 100;                 //��������
		cross_rate = cr, mutation_rate = mr, evolve_rate = er;
		group = new Group(N, node_map, cross_rate, mutation_rate, evolve_rate);
	    tmp_group = new Group(N, node_map, cross_rate, mutation_rate, evolve_rate);
	    best_code = new Path(N, node_map);
	}
	~TSP() 
	{
	    delete node_map;
		delete best_code;
		delete tmp_group;
        delete group;
	}

	void tsp();
	void genetic_evolve();
};



#include "TSP_implementation.h"

#endif