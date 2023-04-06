#include "genetic.h"
#include <algorithm>
#include <cstdlib>

GeneticSearch::GeneticSearch() {}

GeneticSearch::GeneticSearch(const vector<Node>& newCities)
{
	cities = newCities;
    cities.push_back(cities[0]); //돌아오도록

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
        vector<Node> temp = cities;
        shuffle(temp.begin()+1, temp.end()-1, g);
        population.push_back({temp, 0.0f});
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
        currFitnessAvgValue += fitnessSum;
		minFitnessValue = min(minFitnessValue, ch.fitnessVal);
	}
    currFitnessAvgValue /= population.size();
}

void GeneticSearch::selectParents(vector<Chromosome>& population)
{
	//fitness에 따라 오름차순 정렬
	fitness(population);

	sort(population.begin(), population.end(), compChromosome); 

	//순위 기반 선택 -> 상위 20개 집단을 고름
	if(population.size() >= 20)
		population.erase(population.begin()+21, population.end());
}

Chromosome GeneticSearch::crossover(const Chromosome& p1, const Chromosome& p2)
{
	//cout<<"crossover #1\n";

	//-----idx 2개 랜덤 pick-------------
	int loIdx=0, hiIdx=0;
	const int maxCrossoverLength = cities.size() * maxCrossoverRate / 100;
	while(loIdx == hiIdx)
	{
		loIdx = getRandomIntVal(0, cities.size() - 2);
		hiIdx = getRandomIntVal(loIdx+1, loIdx + maxCrossoverLength);
        hiIdx %= (cities.size()-1);

        if(loIdx > hiIdx) swap(loIdx, hiIdx);
        if(hiIdx - loIdx > maxCrossoverLength) hiIdx = loIdx + maxCrossoverLength;
        hiIdx = (hiIdx == cities.size()-1 ?  hiIdx - 1 : hiIdx); //돌아가는 부분은 제외
	}
	
	//cout<<"crossover #2\n";
	//-----본격 crossover --------------
	vector<bool> visited(cities.size() + 10, false); //중복 체크
	Chromosome newChild; 
	newChild.gene = vector<Gene>(cities.size());

	//교차 영역을 지정
	for(int cIdx = loIdx; cIdx <= hiIdx; cIdx++)
	{
		newChild.gene[cIdx] = p1.gene[cIdx];
		visited[p1.gene[cIdx].id] = true; //중복 체크
	}

	//cout<<"crossover #3\n";
	//p2에서 나머지를 땡겨옴
	int idx = (loIdx == 0 ? hiIdx + 1 : 0);
	for(auto &gene : p2.gene)
	{
        int target = gene.id;
		if(visited[target]) continue;
		while(newChild.gene[idx].id != 0) idx = (hiIdx + 1) % cities.size(); //교차 영역은 건너뜀

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
		rIdxA = getRandomIntVal(1, child.size()-2);
		rIdxB = getRandomIntVal(1, child.size()-2);
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

bool GeneticSearch::compCoord(const Node &a, const Node &b)
{
    return pair<int, int>(a.y, a.x) < pair<int, int>(b.y, b.x);
}

inline double GeneticSearch::getDistance(const Node& a, const Node& b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

int GeneticSearch::getRandomIntVal(int lo, int hi)
{
	if(lo > hi) swap(lo, hi); 
	//std::random_device rd;
  	//std::mt19937 gen(rd());
  	//std::uniform_int_distribution<int> dis(lo, hi);
	//return //dis(gen);
	return (rand() % (hi + 1 - lo) + lo);
}
	