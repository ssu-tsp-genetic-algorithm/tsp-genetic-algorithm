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

void writeDataToCsv(const string& filename, const vector<double>& fitnessPer100)
{
    ofstream outFile(filename);
    if (!outFile.is_open()) return;

    for(int i=0; i<fitnessPer100.size(); i++)
        outFile << fitnessPer100[i]<<"\n";

    cout<<"write fitness on "<<filename<<"\n";
    outFile.close();
}

// CSV 파일로 최소 경로를 출력하는 함수
void writeDataToCsv(const string& filename, Chromosome& bestChromosome)
{
    ofstream outFile(filename);
    if (!outFile.is_open()) return;

    outFile << bestChromosome.fitnessVal<<", 0, 0, 0"<< endl;
    const int geneLen = bestChromosome.gene.size();
    for(int i=0; i < geneLen; i++)
    {
        outFile << bestChromosome.gene[i].y <<", " << bestChromosome.gene[i].x<<", ";
        outFile << bestChromosome.gene[(i+1)%geneLen].y <<", " << bestChromosome.gene[(i+1)%geneLen].x<<endl;
    }
    cout<<"write result on "<<filename<<"\n";
    outFile.close();
}

int main()
{
	srand((unsigned)time(NULL));
	vector<Node> cities = readDataFromCsv("../2023_AI_TSP.csv");
	vector<Chromosome> population;
    vector<double> fitnessPer100;

    Chromosome initialChromosome;

    TreeRouteFinder* subRouteFinder = new TreeRouteFinder(cities);
	GeneticSearch* tspSolver = new GeneticSearch(subRouteFinder->getCities());

    //---------Tufu 영역 기반의 Convex-Hull 알고리즘 -------------
    const int tufuOrder[25] = {8,  3,  4,  9, 14
                            , 13, 18, 19, 24, 23
                            , 22, 17, 16, 21, 20
                            , 15, 10, 5,   0,  1
                            ,  2, 7,  6,  11, 12};

    for(int i=0; i<25; i++)
    {
        vector<Node> convexHull = subRouteFinder->createConvexHullRoute(tufuOrder[i]);

        if(i==0) //시작 정점이라면?
        {
            const Node& stNode = cities[0];
            int stIdx = find_if(convexHull.begin(), convexHull.end(), [stNode](Node& n){
                return n.y == stNode.y && n.x == stNode.x; }) - convexHull.begin();
            convexHull.insert(convexHull.end(), convexHull.begin(), convexHull.begin()+stIdx);
            convexHull.erase(convexHull.begin(), convexHull.begin()+stIdx);
        }
        initialChromosome.gene.insert(initialChromosome.gene.end(), convexHull.begin(), convexHull.end());
    }

    //---------위에서 구한 모집단을 기반으로 GA 수행------------------

	tspSolver->initPopulation(population, initialChromosome);

	tspSolver->fitness(population);

	for(int currGen = 0; currGen < tspSolver->getGenerationThres(); currGen++)
	{
		//부모 선택 & replace
		tspSolver->selectParents(population);

        //------------ 교차 연산  ---------------------------------
		for(int cIdx=0; cIdx<tspSolver->getPopulationSize(); cIdx++)
		{
			int tIdx = tspSolver->getRandomIntVal(cIdx+1, population.size()-1);
			Chromosome newChild = tspSolver->crossover(population[cIdx], population[tIdx]);

            //20% 확률의 mutate 연산
            if(tspSolver->getRandomIntVal(1, 100) <= 25)
                tspSolver->mutate(population[cIdx].gene);
			population.push_back(newChild);
		}
		cout<<currGen+1<<" Gen - currAvg "<<tspSolver->getCurrFitnessAvg()<<" /  totalMin : "<<tspSolver->getMinimumFitness()<<'\n';
        tspSolver->updateOperationRate(); //유사담금질 기법

        //------------csv에 쓰기 ---------------------------------
        if((currGen+1) % 50000 == 0)
        {
            string filename = "../output/searchResult_" + to_string(currGen+1) + "_gen.csv";
            writeDataToCsv(filename, population[0]);
        }
        if((currGen+1) % 100 == 0)
            fitnessPer100.push_back(population[0].fitnessVal);
	}
    writeDataToCsv("../output/bestRoute.csv", tspSolver->getMinimumChromosome());
    writeDataToCsv("../output/fitnessChange.csv", fitnessPer100);
    system("pause");

	delete tspSolver;
}