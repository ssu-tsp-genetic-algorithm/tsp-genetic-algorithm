#pragma once
#include "genetic.h"
#define INF 10000000
using namespace std;

class TreeRouteFinder
{
public:
    TreeRouteFinder();
    TreeRouteFinder(const vector<Node>& newCities);

public:
    //areaId의 오름차순 정렬 조건 반환
    static bool compAreaId(const Node& a, const Node& b);

    //영역에 대한 인접 리스트를 초기화
    void initAreaAdjList();

    //Pruning이 적용된 트리 서치 기법으로 각 영역의 최소에 가까운 경로를 탐색
    void findAreaMinimumRoute(const int areaId, int curr, vector<Node> currState, double currCost);

    //해당 영역의 id를 반환
    inline int getAreaId(const Node& a) const;

private:
    //한 변에 영역이 몇 개 있는지, 총 k^2 만큼의 영역
    const int areaSideCount = 5;

    //좌표 제한 (건들지마시오)
    const double coordThres = 100.0;

private:
    //도시 좌표 정보
    vector<Node> cities;

    //[id][idx] : id 영역의 idx번째 도시 (node.id가 저장)
    vector<vector<int> > citiesGroup;

    //[u][v] : t
    vector<vector<pair<int, int> > > adj;

    //중복 체크
    vector<bool> visited;

    //최소 cost
    vector<double> minCost;

    //찾은 최소의 경로
    vector<vector<Node>> minRoute;

public:
    //총 AreaCount
    int getTotalAreaCount() const { return areaSideCount * areaSideCount; }

    //각 영역의 시작 정점 id
    int getAreaStartIndex(int areaId) { return citiesGroup[areaId][0]; }

    //해당 영역의 최소 route 반환
    vector<Node> getMinRoute(int areaId) { return minRoute[areaId]; }

    //도시 배열 반환
    vector<Node>& getCities() { return cities; }
};
