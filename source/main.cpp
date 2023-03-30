#include <iostream>
#include <algorithm>
#include <utility>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <time.h>
#include <cstdlib>
#include "genetic.h"
#include "fileIO.h"
using namespace std;

vector<Node> getDataFromTable(string fileLocation)
{
    ifstream fs(fileLocation.c_str());
    int count = 0;
    vector<Node> cities;

    if(!fs.is_open())
    {
        cout<<"csv open failed\n";
        exit(-1);
    }
	
    Node newNode = {0.0, 0.0, count++};
    while (fs >> newNode.x >> newNode.y) 
    {
        cities.push_back(newNode);
        newNode = {0.0, 0.0, count++};
    }
    fs.close();
    return cities;
}

int main()
{
	vector<Node> cities = getDataFromTable("../2023_AI_TSP.csv");
	vector<Chromosome> population;

	system("pause");

/**
	GeneticSearch* tspSolver = new GeneticSearch; //(cities)

	tspSolver->initPopulation(population);
	ㅊ
	tspSolver->fitness(population);
	for(int currGen = 0; currGen < tspSolver->getGenerationThres()
						 || tspSolver->getMinimumFitness(); currGen++)
	{
		//부모 선택 & replace
		tspSolver->selectParents(population);

		//crossover, 상위 10개 idx와 랜덤한 idx
		for(int cIdx=0; cIdx<10; cIdx++) 
		{
			int tIdx = tspSolver->getRandomIntVal(cIdx+1, cities.size());
			Chromosome newChild = tspSolver->crossover(population[cIdx], population[tIdx]);
			population.push_back(newChild);
		}

		//mutate, 랜덤한 확률로 진행
		
	}

	delete tspSolver;*/
}