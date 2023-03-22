#include "genetic.h"
#include <limits.h>

int getRandNum(int start, int end)
{
	int r = end - start;
	int rnum = start + rand() % r;
	return rnum;
}

bool getIsContain(string s, char ch)
{
	for(auto &c : s) if(c == ch) return true;
	return false;
}

int cooldown(int temp)
{
	return (999 * temp) / 1000;
}

bool lessThan(struct individual t1, struct individual t2)
{
	return t1.fitness < t2.fitness;
}

int calculateFitness(string chromosome)
{
 	int map[V][V] = {{  0, 136,  32,  53, 162, 143, 180, 192,  83,  79},
 					 {136,   0,  75, 137,  17,  31,  56, 157, 178, 173},
 					 { 32,  75,   0, 104, 173, 178,  53,  81, 190,  98},
 					 { 53, 137, 104,   0, 137,  89, 101, 177,  72,  87},
 					 {162,  17, 173, 137,   0,  91, 127,  81,  20, 120},
 	 				 {143,  31, 178,  89,  91,   0,  87,  50,  78, 68},
 					 {180,  56,  53, 101, 127,  87,   0, 152,  81, 129},
 				 	 {192, 157,  81, 177,  81,  50, 152,   0,  20, 149},
 					 { 83, 178, 190,  72,  20,  78,  81,  20,   0,  74},
 					 { 79, 173,  98,  87, 120,  68, 129, 149,  74,  0}};

	int fitness = 0;
	for(int i=0; i<chromosome.size()-1; i++)
	{
		int from = chromosome[i] - '0';
		int to = chromosome[i+1] - '0';
		
		if(map[from][to] == INT_MAX) return INT_MAX;
		fitness += map[from][to];
	}
	return fitness; 
}

string mutateGene(string chromosome)
{
	while(true)
	{
		int idxA = getRandNum(1, V);
		int idxB = getRandNum(1, V);
		if(idxA == idxB) continue;
		swap(chromosome[idxA], chromosome[idxB]);
		break;
	}
	return chromosome;
}

string createChromosome()
{
	string newChromosome = "0";

	while(1)
	{
		if(newChromosome.size() == V)
		{
			newChromosome += newChromosome[0];
			break;
		}
		int temp = getRandNum(1, V);
		if(!getIsContain(newChromosome, (char)(temp + '0')))
			newChromosome += (char)(temp + '0');
	}
	return newChromosome;
}