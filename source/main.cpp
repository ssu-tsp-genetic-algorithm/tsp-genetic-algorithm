#include <iostream>
#include <algorithm>
#include <utility>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "genetic.h"
using namespace std;

vector<Node> getDataFromTable(string fileLocation)
{
    ifstream inFile(fileLocation);
    vector<Node> retCities;

    if (inFile.good()) {
        string line;
        int i = 0;

        while (getline(inFile, line) && i < 1000) {
            stringstream ss(line);

            Node newNode;
            string token = "";
            for(int j=0; j<2; j++)
            {
                getline(ss, token, ',');
                if(i==0) token.erase(0, 3);

                double &target = (j == 0 ? newNode.y : newNode.x);
                target = std::stod(token);
                std::stod(token);
            }
            newNode.id = i++;
            retCities.push_back(newNode);
        }

        inFile.close();
    }
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