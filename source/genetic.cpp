#include "genetic.h"
#include <algorithm>

GeneticSearch::GeneticSearch() {}

GeneticSearch::GeneticSearch(const vector<Node>& newCities)
{
	cities = newCities;

	int idx = 0; //도시 id 초기화
	for(auto &node : cities)
		node.id = idx++;
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
		fitnessSum += getDistance({0.0, 0.0}, prev); //원점 ~ child[0] 거리도 포함
		for(int idx = 1; idx < child.size(); idx++) //총 경로 cost 게산
		{
			fitnessSum += getDistance(prev, child[idx]);
			prev = child[idx];
		}
		ch.fitnessVal = fitnessSum;
		minFitnessValue = min(minFitnessValue, ch.fitnessVal);
	}
}

void GeneticSearch::selectParents(vector<Chromosome>& population)
{
	//fitness에 따라 오름차순 정렬
	fitness(population);
	sort(population.begin(), population.end(), compChromosome); 

	//순위 기반 선택 -> 상위 20개 집단을 고름
	population.erase(population.begin()+21, population.end());
}

Chromosome GeneticSearch::crossover(const Chromosome& p1, const Chromosome& p2)
{
	//-----idx 2개 랜덤 pick-------------
	int loIdx=0, hiIdx=0;
	const int maxCrossoverLength = maxCrossoverRate * cities.size() / 100;
	while(loIdx == hiIdx)
	{
		loIdx = getRandomIntVal(0, cities.size()-1);
		hiIdx = getRandomIntVal(0, cities.size()-1);
		if(loIdx > hiIdx) swap(loIdx, hiIdx); //lo는 hi보다 항상 작다.
		if(abs(loIdx-hiIdx) > maxCrossoverLength) hiIdx = loIdx + maxCrossoverLength; //cross 최대 범위를 넘지 않도록 
	}
	
	//-----본격 crossover --------------
	vector<bool> visited(cities.size(), false); //중복 체크 
	Chromosome newChild; 
	newChild.gene = vector<Gene>(cities.size());

	//교차 영역을 지정
	for(int idx = loIdx; idx <= hiIdx; idx++)
	{
		newChild.gene[idx] = p1.gene[idx];
		visited[p1.gene[idx].id] = true; //중복 체크
	}

	//p2에서 나머지를 땡겨옴
	int idx = (loIdx == 0 ? hiIdx + 1 : 0);
	for(auto &gene : p2.gene)
	{
		if(visited[gene.id]) continue;
		if(newChild.gene[idx].id != 0) idx = (hiIdx + 1) % cities.size(); //교차 영역은 건너뜀

		newChild.gene[idx] = gene;
		visited[gene.id] = true;
		
		idx = (idx + 1) % cities.size(); //circular idx
	}
	
	return newChild;
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

bool GeneticSearch::compChromosome(const Chromosome &c1, const Chromosome &c2)
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
	