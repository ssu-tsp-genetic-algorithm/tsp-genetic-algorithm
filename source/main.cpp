#include <iostream>
#include <algorithm>
#include <utility>
#include <limits.h>
#include <time.h>
#include "genetic.h"
#include "fileIO.h"
using namespace std;

int main()
{
	//vector<vector<Node> > cities = getDataFromTable();
	vector<Chromosome> population;

	GeneticSearch* tspSolver = new GeneticSearch; //(cities)

	tspSolver->initPopulation(population);
	
	tspSolver->fitness(population);
	for(int currGen = 0; currGen < tspSolver->getGenerationThres(); currGen++)
	{
		tspSolver->selectParents(population);

		
	}

	delete tspSolver;
}