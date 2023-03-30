#include <iostream>
#include <algorithm>
#include <utility>
#include <cstring>
#include <limits.h>
#include <time.h>
#include <cstdlib>
#include "genetic.h"
using namespace std;

vector<Node> getDataFromTable(string fileLocation)
{
    FILE* fp = fopen(fileLocation.c_str(), "r");
	char buffer[1024] = { 0 };
	char* tmp = (char*)calloc(128, sizeof(char));

	vector<Node> retCities;
	
	for (int i=-1; i<1000; i++) 
	{
		fgets(buffer, 1024, fp);

		Node newNode;
		newNode.id = i;

		tmp = strtok(buffer, ",");
		newNode.x = atof(tmp);
		tmp = strtok(NULL, ",");
		newNode.y = atof(tmp);

		if(i>=0) retCities.push_back(newNode);
	}
	fclose(fp);
    return retCities;
}

int main()
{
	srand((unsigned)time(NULL));
	vector<Node> cities = getDataFromTable("../2023_AI_TSP.csv");
	vector<Chromosome> population;

	GeneticSearch* tspSolver = new GeneticSearch(cities);

	tspSolver->initPopulation(population);

	tspSolver->fitness(population);
	for(int currGen = 0; currGen < tspSolver->getGenerationThres(); currGen++)
	{
		//부모 선택 & replace
		tspSolver->selectParents(population);

		//crossover, 상위 10개 idx와 랜덤한 idx
		for(int cIdx=0; cIdx<10; cIdx++) 
		{
			int tIdx = tspSolver->getRandomIntVal(cIdx+1, population.size()-1);
			Chromosome newChild = tspSolver->crossover(population[cIdx], population[tIdx]);

            //10% 확률의 mutate 연산
            if(tspSolver->getRandomIntVal(1, 100) >= 90)
                tspSolver->mutate(newChild.gene);
			population.push_back(newChild);
		}
		cout<<currGen+1<<" Gen - currAvg "<<tspSolver->getCurrFitnessAvg()<<" /  totalMin : "<<tspSolver->getMinimumFitness()<<'\n';
	}
	system("pause");

	delete tspSolver;
}