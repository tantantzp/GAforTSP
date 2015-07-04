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


class Node           //城市节点类
{
public:
	double x,y;
	char name;
	void set(double a, double b, char c){ x = a, y = b, name = c;}
};

class Node_map     //城市地图类， 记录距离矩阵
{
public:
	int N;
	Node* node;
    double** length_table;           //距离矩阵

	Node_map(int n);
	~Node_map();
	void init();
	void cacu_distance();
};

class Path        //路径类
{
public:
	int N;
	int* route;           //路径
    double length;        //路径长度
	double fitness;       //适应度
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

class Group      //种群类
{
public:
	int N;               //城市数
	int size;             //种群规模
	double cross_rate;      //交配率
	double mutation_rate;          //变异率
    double evolve_rate;             //进化率,决定接受更坏后代的概率
	Path** codes;              //种群
	Path* best_code;           //最优解
	Node_map* node_map;

	Group(int n, Node_map* nmap, double cr, double mr, double er)
	{
	    N = n;
		node_map = nmap;
		size = 800 * N;    //种群大小, 随城市数调整
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

	void randon_group();    //随机生成新种群
	void mix_group();       //打乱顺序
	void sort_fitness();    //按适应度排序
	void cacu_fitness();
	void cross_over();      //依照交配率进行交配
	void mutation();        //依照变异率进行交配
	void operator = (Group& g)
	{
	    for(int i = 0; i < size; i++)
		{
		    (*codes[i]) = (*g.codes[i]);
		}
		(*best_code) = (*g.best_code);
	}
};


class TSP                //旅行商类， 遗传算法解决
{
public:
	int N;              //城市数
    Node_map* node_map;
	int iterate;      //迭代次数
	double cross_rate;    //交配率
	double mutation_rate;    //变异率
    double evolve_rate;      //进化率
	Group* group;             //种群
	Group* tmp_group;
	Path* best_code;          //最优解

	TSP(int n, double cr, double mr, double er)
	{
	    N = n;
		node_map = new Node_map(N);
		node_map->init();		
		iterate = 100;                 //迭代次数
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