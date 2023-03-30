#include "genetic.h"
#include <algorithm>

GeneticSearch::GeneticSearch() {}

GeneticSearch::GeneticSearch(const vector<Node>& newCities)
{
	cities = newCities;
}

void GeneticSearch::initPopulation(vector<Chromosome>& population)
{
	if(cities.size()==0) return; 

	random_device rd;
    mt19937 g(rd());

	population.clear();
	for(int i=0; i<populationSize; i++)
	{
		shuffle(cities.begin(), cities.end(), g);
		population.push_back({cities, 0.0f});
	}
}

void GeneticSearch::fitness(vector<Chromosome>& population)
{
	//모집단의 염색체 하나씩을 돌면서 fitness 계산
	for(auto &ch : population)
	{
		vector<Node> child = ch.gene;
		double fitnessSum = 0.0f;

		Node prev = child[0];
		for(int idx = 1; idx < child.size(); idx++) //총 경로 cost 게산
		{
			fitnessSum += getDistance(prev, child[idx]);
			prev = child[idx];
		}
		ch.fitnessVal = fitnessSum;
	}
	sort(population.begin(), population.end(), compChromosome);
}

void GeneticSearch::selectParents(vector<Chromosome>& population)
{
	//fitness에 따라 오름차순 정렬
	sort(population.begin(), population.end(), compChromosome); 

	//Elitism -> 상위 2개 집단을 고름
	population.erase(population.begin(), population.begin()+2);
}

vector<Chromosome> GeneticSearch::crossover(vector<Chromosome>& p1, vector<Chromosome>& p2)
{
	vector<Chromosome> newChildChromosome;

	int randIdx[2] = {getRandomIntVal(1, cities.size()-1)
					  , getRandomIntVal(1, cities.size()-1)};

	

	return newChildChromosome;
}

bool GeneticSearch::mutate(vector<Node>& child)
{
	int rIdxA, rIdxB, t; //random index, try count
	for(t=0; t<1e5; t++) 
	{
		rIdxA = getRandomIntVal(1, child.size()-1);
		rIdxB = getRandomIntVal(1, child.size()-1);
		if(rIdxA != rIdxB) break;
	}
	if(t == 1e5) return false; //1e5번의 try -> failed
	swap(child[rIdxA], child[rIdxB]);
	return true; //success
}

inline bool GeneticSearch::compChromosome(const Chromosome &c1, const Chromosome &c2)
{
  	return c1.fitnessVal < c2.fitnessVal;
}

inline double GeneticSearch::getDistance(const Node& a, const Node& b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

int GeneticSearch::getRandomIntVal(int lo, int hi)
{
	if(lo > hi) swap(lo, hi); 
	std::random_device rd;
  	std::mt19937 gen(rd());
  	std::uniform_int_distribution<int> dis(lo, hi);
  	return dis(gen);
}
	