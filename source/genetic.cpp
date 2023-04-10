#include "genetic.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <cstdlib>

GeneticSearch::GeneticSearch() {}

GeneticSearch::GeneticSearch(const vector<Node>& newCities)
{
    cities = newCities;
}

void GeneticSearch::initPopulation(vector<Chromosome>& population)
{
    if(population[0].gene.size()==0) return;
    random_device rd;
    mt19937 g(rd());

    population.clear();
    for(int i=0; i<populationSize; i++)
    {
        vector<Node> temp = population[0].gene;
        shuffle(temp.begin()+1, temp.end(), g);
        population.push_back({temp, 0.0f});
    }
}

void GeneticSearch::initPopulation(vector<Chromosome>& population, const Chromosome& targetChromosome)
{
    if(targetChromosome.gene.size()==0) return;

    population.clear();
    for(int i=0; i<populationSize; i++)
        population.push_back(targetChromosome);
}

void GeneticSearch::fitness(vector<Chromosome>& population)
{
    //모집단의 염색체 하나씩을 돌면서 fitness 계산
    for(auto &ch : population)
    {
        vector<Node> child = ch.gene;
        double fitnessSum = 0.0f;

        Node prev = child[0];
        for(int idx = 0; idx < child.size(); idx++) //총 경로 cost 게산
        {
            fitnessSum += getDistance(child[idx], child[(idx+1) % child.size()]);
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

    //순위 기반 선택 -> populationSize 만큼의 상위 집단을 고름
    if(population.size() >= populationSize)
        population.erase(population.begin()+populationSize, population.end());
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
        hiIdx = getRandomIntVal(loIdx + 1, min((int)cities.size()-1, loIdx + maxCrossoverLength));
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
    int idx = 0;

    for(int i=0; i < p2.gene.size(); i++)
    {
        auto& gene = p2.gene[i];
        int target = gene.id;
        if(visited[target] && idx) continue;
        while(newChild.gene[idx].id != 0) idx = (hiIdx + 1) % cities.size(); //교차 영역은 건너뜀

        newChild.gene[idx] = gene;
        visited[gene.id] = true;

        idx = idx + 1; //circular idx
    }
    return newChild;
}

bool GeneticSearch::mutate(vector<Node>& child)
{
    int rIdxA, rIdxB; //random index
    const int maxMutateLength = cities.size() * maxMutateRate / 100;
    rIdxA = getRandomIntVal(1, child.size()-2);
    rIdxB = getRandomIntVal(rIdxA, min((int)child.size()-1, rIdxA + maxMutateLength));

    if(rIdxA==rIdxB)return false;

    reverse(child.begin()+rIdxA, child.begin()+rIdxB);
    return true; //success
}

inline double GeneticSearch::getDistance(const Node& a, const Node& b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool GeneticSearch::compChromosome(const Chromosome &c1, const Chromosome &c2)
{
    return c1.fitnessVal < c2.fitnessVal;
}

bool GeneticSearch::compCoord(const Node &a, const Node &b)
{
    return pair<int, int>(a.y, a.x) < pair<int, int>(b.y, b.x);
}

int GeneticSearch::getRandomIntVal(int lo, int hi)
{
    if(lo > hi) swap(lo, hi);
    //std::random_device rd;
    //std::mt19937 gen(rd());
    //std::uniform_int_distribution<int> dis(lo, hi);
    //return dis(std::default_random_engine());
    return (rand() % (hi + 1 - lo) + lo);
}