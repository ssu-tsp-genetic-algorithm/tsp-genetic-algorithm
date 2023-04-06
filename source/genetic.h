#include <vector>
#include <random>
#include <cstdlib>
using namespace std;

struct Node
{
	double y;
	double x;
	int id;
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

	//순위 기반 부모 선택 연산
	void selectParents(vector<Chromosome>& population);

	//순서 crossover 연산
	Chromosome crossover(const Chromosome& p1, const Chromosome& p2);

	//단순 swap을 통한 mutate 연산
	bool mutate(vector<Node>& child);

private:
	//두 좌표간의 거리를 구함
	inline double getDistance(const Node& a, const Node& b);

	//fitness 값을 비교하여 오름차순 조건을 반환
	static bool compChromosome(const Chromosome& c1, const Chromosome& c2);

    //좌표 오른차순 정렬 조건
    static bool compCoord(const Node& a, const Node& b);
	
private:

    //crossover 연산의 최대 범위 비율 (백분율)
	const int maxCrossoverRate = 15;

	//시작 노드 Idx
	const int startIdx = 0;
	
	//최대 generation 수
	const int genThres = 100000;

	//최소 fitnessValue
	double minFitnessValue = std::numeric_limits<double>::max();

    //현재 Gen의 평균 fitnessValue
    double currFitnessAvgValue = 0.0f;

protected:
    //도시 좌표 정보
    vector<Node> cities;
    //모집단 Size
    const int populationSize = 30;

public:
	//generation 한계 (최대 gen)
	int getGenerationThres(){ return genThres; }

	//최소 fitness 반환
	double getMinimumFitness(){ return minFitnessValue; }

    //현재 Gen의 평균 Fitness
    double getCurrFitnessAvg(){ return currFitnessAvgValue; }

	//random 라이브러리 내 기능을 이용해 범위 내의 난수 반환
	int getRandomIntVal(int lo, int hi);
};