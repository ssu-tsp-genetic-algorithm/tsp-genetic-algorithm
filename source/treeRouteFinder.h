#pragma once
#include "genetic.h"
#define INF 10000000
using namespace std;

//Convex Hull을 위한 Node
struct NodeCH
{
    Node node;
    int p; //기준 정점과의 상대위치 표현
    int q;
};

class TreeRouteFinder
{
public:
    TreeRouteFinder();
    TreeRouteFinder(const vector<Node>& newCities);

public:
    //areaId의 오름차순 정렬 조건 반환
    static bool compAreaId(const Node& a, const Node& b);

    //초기 볼록껍질 생성
    vector<Node> createInitialConvexHull(const int& areaId);

    //볼록껍질을 활용해 TSP의 해를 생성
    vector<Node> createConvexHullRoute(const int& areaId);

    //해당 영역의 id를 반환
    inline int getAreaId(const Node& a) const;

private:
    //한 변에 영역이 몇 개 있는지, 총 k^2 만큼의 영역
    const int areaSideCount = 5;

    //좌표 제한 (건들지마시오)
    const double coordThres = 100.0;

    //CCW값을 구한다.
    static inline int getCCwValue(const Node& a, const Node& b, const Node& c);

    //Graham's Scan을 위해 특정 조건에 맞게 Node를 비교한다.
    static inline bool compNode(const NodeCH& a, const NodeCH& b);


private:
    //도시 좌표 정보
    vector<Node> cities;

    //[id][idx] : id 영역의 idx번째 도시 (node.id가 저장)
    vector<vector<NodeCH> > citiesGroup;

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

    //해당 영역의 최소 route 반환
    vector<Node> getMinRoute(int areaId) { return minRoute[areaId]; }

    //도시 배열 반환
    vector<Node>& getCities() { return cities; }
};
