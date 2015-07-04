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

void Node_map::init()          //�������ݣ� ���е�ͼ��ʼ��
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

void Node_map::cacu_distance()    //����������
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

void Path::cross(Path* pair, double evlove_rate)          //���䣬���潻�䷨
{
	int place = rand() % N;
	
	Path* child1 = new Path(N, node_map);
	Path* child2 = new Path(N, node_map);
	(*child1) = (*this);
	(*child2) = (*pair);

    int* flag = new int[N];

	for(int i = 0; i < N; i++) flag[i] = 0;
	for(int i = 0; i < place; i++ )                //���placeǰ���ֵĻ���
		flag[route[i]] = 1;
	int j = place;
	for(int i = 0; i < N; i++)                  //��pair�а�˳��ѡ����ԭ����placeǰδ���ֵĻ���
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
		if(tmp > 100 * evlove_rate)           //���ݽ������������Ƿ�ȡ����
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

void Path::randon_route()              //�������·��
{
	random_shuffle(route,route + N);
	cacu_length();
}

double Path::cacu_length()    //����·������
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

void Path::change()           //����
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

	int choose = rand() % 3;      //���ѡ��һ�ֱ��췽ʽ

	if(choose == 0)         //����һλ
	{
	    int t = route[i];
		route[i] = route[j];
		route[j] = t;
	}
	else if(choose == 1)          //�ߵ�һ��
	{
	    reverse(route + i, route + j);
	}
	else                      //��λ
	{
	    if(j = N - 1)    return;
		rotate(route + i, route + j, route + j + 1);
	}

	cacu_length();
}
bool Path::operator < ( Path& p )     //����Ӧ�ȱȽ�
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

void Group::randon_group()         //���������Ⱥ
{
    for(int i = 0; i < size; i++)
        codes[i]->randon_route();
}

void Group::mix_group()    //������Ⱥ
{
	random_shuffle(codes, codes + size);
}

bool compare_code(Path* a, Path* b) 
{
	if((*a).fitness > (*b).fitness)
		return true;
	return false;
}

void Group::sort_fitness()    //������Ӧ������
{
    sort(codes, codes + size, compare_code);
}

void Group::cacu_fitness()      //��Ӧ�Ⱥ�������, ������������
{
	double max_fit = 0;
	double sum = 0;
    for(int i = 0; i < size; i++)
	{
		codes[i]->fitness = (1 / codes[i]->length) * 2000;     //·��ȡ�����ٷŴ�
		sum += codes[i] -> fitness;
		if(codes[i] -> fitness > max_fit) max_fit = codes[i] -> fitness;
	}
	double a, b;
	double avg = sum / size;
	a = ((100 - 1) * avg) / ( max_fit - avg);              //������չ��
	b = avg * ((max_fit - 100 * avg) / (max_fit - avg));
	for(int i = 0; i < size; i++)
	{
	    codes[i] -> fitness = a * codes[i] -> fitness + b;
	}
	
}

void Group::cross_over()      //��Ⱥ�ڲ�����
{
	int num = size * cross_rate;
	if(num & 1)  --num;
	for(int i = 0; i < num; i += 2)
	{
		codes[i]->cross(codes[i+1], evolve_rate);
	}
}

void Group::mutation()        //��Ⱥ����
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
			if(temp > 100 * evolve_rate)   //���ݽ����ʾ����±�������ȡ��
			    (*codes[t]) = (*tmp);
		}
	}
	delete tmp;
}

///////////////////TSP///////////////////

void TSP::tsp()                  //���ö����Ⱥ, ��ֹ����ֲ����ţ�������С��20ʱ����N / 4����Ⱥ������20ʱ��������2���ӽ���
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

void TSP::genetic_evolve()    //�Ŵ������㷨
{
	group->randon_group();
    double minLength;
    double* possibility = new double[group -> size];

    int it = 0;
	while(it <= iterate)         //iterate ���ƽ�������
	{
        group->cacu_fitness();
		group->sort_fitness();

		if( group -> codes[0] -> length < best_code ->length)   //�������Ž�
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

		for(int i = 0; i < group -> size; i++)             //����ѡȡ�������̶�ѡȡN��������ΪȺ��
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

		tmp_group->cross_over();    //����Ⱥ����
		tmp_group->mutation();     //����Ⱥ����
		(*group) = (*tmp_group);   //����������Ⱥ
		it++;
	}
}


#endif