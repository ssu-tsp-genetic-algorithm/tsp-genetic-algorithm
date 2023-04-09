#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <ostream>
#include <fstream>
#include <sstream>
#include "treeRouteFinder.h"
#include "genetic.h"
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
            newNode.areaId = 0;
            retCities.push_back(newNode);
        }
        inFile.close();
    }
    else exit(-1);
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
    cout<<"write result on "<<filename<<"\n";
    outFile.close();
}

int main()
{
	srand((unsigned)time(NULL));
	vector<Node> cities = readDataFromCsv("../2023_AI_TSP.csv");
	vector<Chromosome> population;
    Chromosome initialChromosome;

    TreeRouteFinder* subRouteFinder = new TreeRouteFinder(cities);
	GeneticSearch* tspSolver = new GeneticSearch(subRouteFinder->getCities());


    /*---------Tufu 영역 기반의 Branch&Bound 알고리즘 -------------*/
    population.push_back(Chromosome());
    int areaId = tspSolver->getCities()[0].areaId; //시작 정점이 있는 영역부터 시작

    const int tufuOrder[25] = {8, 3, 4, 9, 14
                               , 13, 18, 19,24, 23
                               , 22, 17, 16,21,20
                               , 15, 10, 5, 0, 1
                               , 2, 7, 6, 11, 12};

    for(int idx = 0; idx< subRouteFinder->getTotalAreaCount(); idx++)
    {
        const int areaId = tufuOrder[idx];
        vector<Node> areaMinRoute; //영역당 최소 경로가 들어갈 벡터
        const int startIdx = subRouteFinder->getAreaStartIndex(areaId); //영역 내 시작 Idx

        subRouteFinder->findAreaMinimumRoute(areaId, startIdx, areaMinRoute, 0);
        areaMinRoute = subRouteFinder->getMinRoute(areaId);

        initialChromosome.gene.insert(initialChromosome.gene.end(), areaMinRoute.begin(), areaMinRoute.end());
    }
    initialChromosome.gene.push_back(cities[0]);

    //---------위에서 구한 모집단을 기반으로 GA 수행------------------
	tspSolver->initPopulation(population, initialChromosome);

	tspSolver->fitness(population);
	for(int currGen = 0; currGen < tspSolver->getGenerationThres(); currGen++)
	{
		//부모 선택 & replace
		tspSolver->selectParents(population);

		//crossover, 상위 25개 idx와 랜덤한 idx
		for(int cIdx=0; cIdx<tspSolver->getPopulationSize(); cIdx++)
		{
			int tIdx = tspSolver->getRandomIntVal(cIdx+1, population.size()-1);
			Chromosome newChild = tspSolver->crossover(population[cIdx], population[tIdx]);

            //25% 확률의 mutate 연산
            if(tspSolver->getRandomIntVal(1, 100) >= 70)
                tspSolver->mutate(population[cIdx].gene);
			population.push_back(newChild);
		}
		cout<<currGen+1<<" Gen - currAvg "<<tspSolver->getCurrFitnessAvg()<<" /  totalMin : "<<tspSolver->getMinimumFitness()<<'\n';
	}
    writeDataToCsv("../searchResult.csv", population[0]);
	system("pause");

	delete tspSolver;
}
