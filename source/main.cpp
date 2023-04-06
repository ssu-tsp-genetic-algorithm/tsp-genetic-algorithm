#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <ostream>
#include <fstream>
#include <sstream>
#include "genetic.h"
#include "kmeans.h"
using namespace std;

vector<Node> readDataFromCsv(string fileLocation)
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
                if(i+j==0) token.erase(0, 3);

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

// CSV 파일로 출력하는 함수
void writeDataToCsv(const std::string& filename, Chromosome& bestChromosome)
{
    ofstream outFile(filename);
    if (!outFile.is_open()) return;

    outFile << bestChromosome.fitnessVal<<", 0, 0, 0"<< endl;
    for(int i=1; i<bestChromosome.gene.size(); i++)
    {
        outFile << bestChromosome.gene[i-1].y <<", " << bestChromosome.gene[i-1].x<<", ";
        outFile << bestChromosome.gene[i].y <<", " << bestChromosome.gene[i].x<<endl;
    }
    outFile.close();
    std::cout << "CSV file has been written: " << filename << std::endl;
}

int main()
{
	srand((unsigned)time(NULL));
	vector<Node> cities = readDataFromCsv("../2023_AI_TSP.csv");
	vector<Chromosome> population;

	KmeansGeneticSearch* tspSolver = new KmeansGeneticSearch(cities, 3);

	tspSolver->initPopulationWithKmeansRandom(population);
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

            //25% 확률의 mutate 연산
            if(tspSolver->getRandomIntVal(1, 100) >= 75)
                tspSolver->mutate(newChild.gene);
			population.push_back(newChild);
		}
		cout<<currGen+1<<" Gen - currAvg "<<tspSolver->getCurrFitnessAvg()<<" /  totalMin : "<<tspSolver->getMinimumFitness()<<'\n';
	}
    writeDataToCsv("../searchResult.csv", population[0]);
	system("pause");

	delete tspSolver;
}
