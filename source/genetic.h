#pragma once
#include <vector>
#include <random>
#include <cstdlib>
using namespace std;

struct Node
{
	double y;
	double x;
} typedef Gene;

struct Chromosome
{
	vector<Gene> gene;
	double fitnessVal;
};

class GeneticSearch
{
public:
	GeneticSearch();

	GeneticSearch(const vector<Node>& newCities);

	void initPopulation(vector<Chromosome>& population);

	//population의 적합도를 평가하고 해당 값에 따라 오름차순으로 정렬
	void fitness(vector<Chromosome>& population);

	//Elitism 기반의 부모 선택 연산
	void selectParents(vector<Chromosome>& population);

	//두 점 crossover 연산
	vector<Chromosome> crossover(vector<Chromosome>& p1, vector<Chromosome>& p2);

	//단순 swap을 통한 mutate 연산
	bool mutate(vector<Node>& child);

public:
	int getGenerationThres(){ return genThres; }

private:
	//두 좌표간의 거리를 구함
	inline double getDistance(const Node& a, const Node& b);

	//fitness 값을 비교하여 오름차순 조건을 반환
	inline bool compChromosome(const Chromosome& c1, const Chromosome& c2);

	//random 라이브러리 내 기능을 이용해 범위 내의 난수 반환
	int getRandomIntVal(int lo, int hi);
	
private:
	//도시 좌표 정보
	vector<Node> cities;

	//모집단 Size
	const int populationSize = 10;
	
	//시작 노드 Idx
	const int startIdx = 0;
	
	//최대 generation 수
	const int genThres = 100;
};