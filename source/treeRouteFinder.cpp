#include "treeRouteFinder.h"
#include <iostream>
#include <vector>
#include <algorithm>
#define MAX_BRANCH_COUNT 3

TreeRouteFinder::TreeRouteFinder(){}

TreeRouteFinder::TreeRouteFinder(const vector<Node> &newCities)
{
    citiesGroup.resize(getTotalAreaCount());
    cities = newCities;

    adj.resize(cities.size()+1);
    visited = vector<bool>(cities.size(), false);
    minCost = vector<double>(getTotalAreaCount(), INF);
    minRoute.resize(getTotalAreaCount());

    for(auto& node : cities)
    {
        if(node.areaId != 0) break;
        node.areaId = getAreaId(node);
        citiesGroup[node.areaId].push_back(node.id);
    }
    initAreaAdjList(); //영역 마다의 인접 리스트 시작
}

bool TreeRouteFinder::compAreaId(const Node &a, const Node &b) { return a.areaId < b.areaId; }

void TreeRouteFinder::initAreaAdjList()
{
    //영역 단위로 인접 리스트 구상
    for(int areaId = 0; areaId < getTotalAreaCount(); areaId++)
    {
        vector<int>& currArea = citiesGroup[areaId];
        for(int i=0; i<currArea.size(); i++) //u - v 모든 쌍을 시도
        {
            int u = currArea[i];
            for (int j = i + 1; j < currArea.size(); j++)
            {
                int v = currArea[j];
                const double dist = GeneticSearch::getDistance(cities[i], cities[j]);
                adj[u].push_back({dist, v});
                adj[v].push_back({dist, u});
            }
            sort(adj[u].begin(), adj[u].end()); //dist 순으로 정렬
            adj[u].erase(adj[u].begin() + MAX_BRANCH_COUNT, adj[u].end()); //MAX_BRANCH_COUNT만 남겨두고 지워
        }

        double initialCost = 0.0;
        vector<Node> initialRoute;

        for(int i=0; i<currArea.size(); i++) //초기 minCost, minRoute 지정
        {
            if(i>0) initialCost += GeneticSearch::getDistance(cities[currArea[i-1]], cities[currArea[i]]);
            initialRoute.push_back(cities[currArea[i]]);
        }
        minCost[areaId] = initialCost;
        minRoute[areaId] = initialRoute;
    }
}

void TreeRouteFinder::findAreaMinimumRoute(const int areaId, int curr, vector<Node> currState, double currCost)
{
    if(currState.size() == citiesGroup[areaId].size())   //탐색 cost가 여태껏 찾은 cost보다 작다면 갱신
    {
        if(minCost[areaId] <= currCost) return;
        minCost[areaId] = currCost;
        minRoute[areaId] = currState;
        cout<<"Curr Cost : "<<currCost<<'\n';
        return;
    }

    int tryCount = 0;
    if(currState.empty()) visited[curr] = true; //시작 정점 체크

    for(auto &nextInfo : adj[curr])
    {
        const int next = nextInfo.second;
        const int cost = nextInfo.first;

        if(visited[next]) continue;
        if(currCost + cost > minCost[areaId]) break; //Pruning : 현재까지 찾은 minCost보다 크다면 가지를 침

        currState.push_back(cities[next]);
        currCost += cost;
        visited[next] = true;

        findAreaMinimumRoute(areaId, next, currState, currCost); //다음 sub-tree로 나아감

        currState.pop_back(); //BackTrack!!
        currCost -= cost;
        visited[next] = false;
    }
}

int TreeRouteFinder::getAreaId(const Node &a) const
{
    const int areaLen = (int)coordThres / areaSideCount;  
    return (int)(a.y / areaLen) * areaSideCount + (int)(a.x / areaLen);
}