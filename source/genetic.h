#pragma  once
#include <vector>
#include <random>
#include <cstdlib>
#include <unordered_map>
using namespace std;

struct Node
{
    double y;
    double x;
    int id;
    int areaId;
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

    //초기에 설정할 염색체가 없다면? 랜덤 생성
    void initPopulation(vector<Chromosome>& population);

    //초기에 설정할 염색체가 있다면? (두부, 클러스터링 등)
    void initPopulation(vector<Chromosome>& population, const Chromosome& targetChromosome);

    //population의 적합도를 평가하고 해당 값에 따라 오름차순으로 정렬
    void fitness(vector<Chromosome>& population);

    //순위 기반 부모 선택 연산
    void selectParents(vector<Chromosome>& population);

    //순서 crossover 연산
    Chromosome crossover(const Chromosome& p1, const Chromosome& p2);

    //entrance 함수
    bool mutate(vector<Node>& child);

    //단순 swap을 통한 mutate 연산
    bool swapMutate(vector<Node>& child);

    //reverse 연산을 통한 mutate 연산
    bool inverseMutate(vector<Node>& child);

private:
    //fitness 값을 비교하여 오름차순 조건을 반환
    static bool compChromosome(const Chromosome& c1, const Chromosome& c2);

private:
    //모집단 Size
    const int populationSize = 50;

    //crossover 연산의 최대 범위 비율 (백분율)
    double maxCrossoverRate = 50;

    //crossover 연산의 최대 범위 비율 (백분율)
    double maxMutateRate = 5;

    //담금질 기법
    const double coolingRate = 0.0001;

    //최대 generation 수
    const int genThres = 500000;

private:
    //도시 좌표 정보
    vector<Node> cities;

    //최소 fitnessValue
    double minFitnessValue = std::numeric_limits<double>::max();

    //최소 fitness를 가지는 염색체
    Chromosome minChromosome;

    //현재 Gen의 평균 fitnessValue
    double currFitnessAvgValue = 0.0f;

    //MST 기반의 Repair 연산을 위한것
    vector<pair<double, pair<int, int>>> distances;

public:
    //generation 한계 (최대 gen)
    int getGenerationThres(){ return genThres; }

    //최소 fitness 반환
    double getMinimumFitness(){ return minFitnessValue; }

    Chromosome& getMinimumChromosome() { return minChromosome; }

    //현재 Gen의 평균 Fitness
    double getCurrFitnessAvg(){ return currFitnessAvgValue; }

    //모집단 개수 반환
    int getPopulationSize() const { return populationSize; }

    //random 라이브러리 내 기능을 이용해 범위 내의 난수 반환
    int getRandomIntVal(int lo, int hi);

    //두 좌표간의 거리를 구함
    static double getDistance(const Node& a, const Node& b);
};