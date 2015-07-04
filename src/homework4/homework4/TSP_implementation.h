#ifndef TSP_IMPLEMENTATION_H
#define TSP_IMPLEMENTATION_H

#include "iostream"
#include <vector>
#include <algorithm>
#include <ctime>
#include <random>
#include "TSP.h"
#include <time.h>

using namespace std;

clock_t start_time, finish_time;

Node_map::Node_map(int n):N(n)
	{
		node = new Node[N];
		length_table = new double*[N];
		for(int i=0; i<N; i++)
		{
		    length_table[i] = new double[N];
		}
	}
Node_map::~Node_map()
	{
	    delete []node;
		for(int i=0; i<N; i++)
		    delete [] length_table[i];
		delete length_table;
	}

void Node_map::init()          //读入数据， 城市地图初始化
{
    for(int i=0; i<N; i++)
	{
	    char temp;
		double tempx, tempy;
		fin>>temp>>tempx>>tempy;
		node[i].set(tempx, tempy, temp);
	}
	cacu_distance();
}

void Node_map::cacu_distance()    //计算距离矩阵
{
    for(int i = 0; i< N; i++)
	{
		length_table[i][i] = (double)INT_MAX;
	    for(int j = i+1; j < N; j++)
		{
			length_table[i][j] = length_table[j][i] = sqrt(
				(node[i].x - node[j].x) * (node[i].x - node[j].x) +
				(node[i].y - node[j].y) * (node[i].y - node[j].y) );
		}
	}
}

//////////////////Path////////////////////////

void Path::cross(Path* pair, double evlove_rate)          //交配，常规交配法
{
	int place = rand() % N;
	
	Path* child1 = new Path(N, node_map);
	Path* child2 = new Path(N, node_map);
	(*child1) = (*this);
	(*child2) = (*pair);

    int* flag = new int[N];

	for(int i = 0; i < N; i++) flag[i] = 0;
	for(int i = 0; i < place; i++ )                //标记place前出现的基因
		flag[route[i]] = 1;
	int j = place;
	for(int i = 0; i < N; i++)                  //在pair中按顺序选择在原序列place前未出现的基因
	{
	    if(! flag[pair -> route[i]])
			child1 -> route[j++] = pair -> route[i];
	}

	for(int i = 0; i < N; i++) flag[i] = 0;
	for(int i = 0; i < place; i++ )
		flag[pair -> route[i]] = 1;
	j = place;
	for(int i = 0; i < N; i++)
	{
	    if(! flag[route[i]])
			child2 -> route[j++] = route[i];
	}
	child1 -> cacu_length();
	child2 -> cacu_length();
	if(child1 -> length < length)
	    (*this) = (*child1);
	else
	{
	    int tmp = rand() % 100;
		if(tmp > 100 * evlove_rate)           //依据进化率来决定是否取舍后代
			(*this) = (*child1);
	}
	if(child2 -> length < pair -> length)
	    (*pair) = (*child2);
	else
	{
	    int tmp = rand() % 100;
		if(tmp > 100 * evlove_rate)
			(*pair) = (*child2);
	}
	
	delete child1;
	delete child2;
	delete flag;
}

Path::Path(int n, Node_map* nm)
{
	N = n;
	node_map = nm;
	route = new int[N];
	for(int i = 0; i < N; i++)
	{
		route[i] = i;
	}
	length = (double)INT_MAX;
	fitness = 0;
}

void Path::randon_route()              //产生随机路径
{
	random_shuffle(route,route + N);
	cacu_length();
}

double Path::cacu_length()    //计算路径长度
{
	int j = 0;
	length = 0;
	for(j = 1; j < N; j++)
	{
	    length += node_map->length_table[route[j -1]][route[j]];
	}
	length += node_map->length_table[route[N-1]][route[0]];
	return length;
}

void Path::change()           //变异
{
    int i = rand() % N;
	int j = rand() % N;
	if(i > j)
	{
	    int t = i;
		i = j;
		j = t;
	}
	else if(i == j)
	{
	    return;
	}

	int choose = rand() % 3;      //随机选择一种变异方式

	if(choose == 0)         //交换一位
	{
	    int t = route[i];
		route[i] = route[j];
		route[j] = t;
	}
	else if(choose == 1)          //颠倒一段
	{
	    reverse(route + i, route + j);
	}
	else                      //移位
	{
	    if(j = N - 1)    return;
		rotate(route + i, route + j, route + j + 1);
	}

	cacu_length();
}
bool Path::operator < ( Path& p )     //按适应度比较
{
	return fitness < p.fitness;
}
void Path::operator =( Path& p)
{
	for(int i=0;i < N; i++)
	{
		route[i] = p.route[i];
	}
    length = p.length;
	fitness = p.fitness;
}

void Path::output()
{
    for(int i=0; i<N ;i++)
	{
	    cout<<node_map->node[route[i]].name<<' ';
	}
	cout<<length<<endl;
}

/////////////Group///////////////////////

void Group::randon_group()         //产生随机种群
{
    for(int i = 0; i < size; i++)
        codes[i]->randon_route();
}

void Group::mix_group()    //打乱种群
{
	random_shuffle(codes, codes + size);
}

bool compare_code(Path* a, Path* b) 
{
	if((*a).fitness > (*b).fitness)
		return true;
	return false;
}

void Group::sort_fitness()    //按照适应度排序
{
    sort(codes, codes + size, compare_code);
}

void Group::cacu_fitness()      //适应度函数计算, 采用线性拉伸
{
	double max_fit = 0;
	double sum = 0;
    for(int i = 0; i < size; i++)
	{
		codes[i]->fitness = (1 / codes[i]->length) * 2000;     //路径取倒数再放大
		sum += codes[i] -> fitness;
		if(codes[i] -> fitness > max_fit) max_fit = codes[i] -> fitness;
	}
	double a, b;
	double avg = sum / size;
	a = ((100 - 1) * avg) / ( max_fit - avg);              //线性扩展法
	b = avg * ((max_fit - 100 * avg) / (max_fit - avg));
	for(int i = 0; i < size; i++)
	{
	    codes[i] -> fitness = a * codes[i] -> fitness + b;
	}
	
}

void Group::cross_over()      //种群内部交配
{
	int num = size * cross_rate;
	if(num & 1)  --num;
	for(int i = 0; i < num; i += 2)
	{
		codes[i]->cross(codes[i+1], evolve_rate);
	}
}

void Group::mutation()        //种群变异
{
	int num = size * mutation_rate;
	int t = 0;
	Path* tmp = new Path(N, node_map);
	for(int i = 0 ;i < num; i++)
	{	
	    t = rand() % size;
        (*tmp) = (*codes[t]);
		codes[t]->change();
		if(codes[t] -> length > tmp -> length)
		{
		    int temp = rand() % 100;
			if(temp > 100 * evolve_rate)   //根据进化率决定新变异个体的取舍
			    (*codes[t]) = (*tmp);
		}
	}
	delete tmp;
}

///////////////////TSP///////////////////

void TSP::tsp()                  //采用多个种群, 防止陷入局部最优，城市数小于20时采用N / 4个种群，大于20时将控制在2分钟结束
{
	if(N <= 20)
	{
	    int t = N / 4;   
		for(int i = 0; i < t; i++)
		{
			genetic_evolve();
		}
       // best_code-> output();
		for(int i=0; i<N ;i++)
		{
			fout<<best_code->node_map->node[best_code->route[i]].name<<' ';
		}
		fout<<best_code->length<<endl;

	}
	else
	{
        start_time = clock();
		while(true)
		{
		    finish_time = clock();
			double totaltime = (double)(finish_time - start_time) / CLOCKS_PER_SEC;
	        if((totaltime <120)) 
			{
			    genetic_evolve();
			}
			else break;
		}
		for(int i=0; i<N ;i++)
		{
			fout<<best_code->node_map->node[best_code->route[i]].name<<' ';
		}
		fout<<best_code->length<<endl;
	}
}

void TSP::genetic_evolve()    //遗传进化算法
{
	group->randon_group();
    double minLength;
    double* possibility = new double[group -> size];

    int it = 0;
	while(it <= iterate)         //iterate 控制进化次数
	{
        group->cacu_fitness();
		group->sort_fitness();

		if( group -> codes[0] -> length < best_code ->length)   //更新最优解
		{
			(*best_code) = (*group -> codes[0]);
			//best_code -> output();
		}

		if(it == iterate)
		{
	        //best_code-> output();
			delete []possibility;
			return;
		}
		
		double sum = 0;
		for(int i = 0; i < group -> size; i++) sum += group->codes[i]->fitness;
		for(int i = 0; i < group -> size; i++) 
			possibility[i] = group->codes[i]->fitness / sum;

		for(int i = 0; i < group -> size; i++)             //依照选取概率轮盘赌选取N个个体作为群体
		{
		    double r = (rand()%100) / 100;
			double temp = 0;
			for(int j = 0; j < group -> size; j++)
			{
				temp += possibility[j];
				if(temp >= r)
				{
					(*tmp_group->codes[i]) = (*group->codes[j]);
				    break;
				}
			}
		}

		tmp_group->cross_over();    //令种群交配
		tmp_group->mutation();     //令种群变异
		(*group) = (*tmp_group);   //更新先由种群
		it++;
	}
}


#endif