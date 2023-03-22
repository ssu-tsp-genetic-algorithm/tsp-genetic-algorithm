#pragma once
#include <vector>
#include <random>
#include <cstdlib>
using namespace std;

#define V 10
#define START 0

#define POPULATION_SIZE 10

struct individual
{
	string chromosome;
	int fitness;
};

int getRandNum(int start, int end);

bool getIsContain(string s, char ch);

int cooldown(int temp);

bool lessThan(struct individual t1, struct individual t2);

int calculateFitness(string chromosome);

string mutateGene(string chromosome);

string createChromosome();