#include "treeRouteFinder.h"
#include <iostream>
#include <algorithm>

TreeRouteFinder::TreeRouteFinder(){}

TreeRouteFinder::TreeRouteFinder(const vector<Node> &newCities)
{
    citiesGroup.resize(getTotalAreaCount());
    cities = newCities;
    cities.pop_back();
    adj.resize(cities.size()+1);
    visited = vector<bool>(cities.size(), false);

    for(auto& node : cities)
    {
        if(node.areaId != 0) break;
        node.areaId = getAreaId(node);
        citiesGroup[node.areaId].push_back(node.id);
    }
    initAreaAdjList();
}

bool TreeRouteFinder::compAreaId(const Node &a, const Node &b) { return a.areaId < b.areaId; }

void TreeRouteFinder::initAreaAdjList()
{
    for(int areaId = 0; areaId < getTotalAreaCount(); areaId++)
    {
        vector<int>& currArea = citiesGroup[areaId];
        for(int i=0; i<currArea.size(); i++)
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
        }

        double initialCost = 0.0;
        vector<Node> initialRoute;

        initialRoute.push_back(cities[currArea[0]]);
        for(int i=1; i<currArea.size(); i++)
        {
            initialCost += GeneticSearch::getDistance(cities[currArea[i-1]], cities[currArea[i]]);
            initialRoute.push_back(cities[currArea[i]]);
        }
        minCost = initialCost;
        minRoute = initialRoute;
    }
}

void TreeRouteFinder::findAreaMinimumRoute(const int areaId, int curr, vector<Node> currState, double currCost)
{
    if(currState.size() == citiesGroup[areaId].size() //모두 방문한 상태에서
        && minCost > currCost)   //탐색 cost가 여태껏 찾은 cost보다 작다면 갱신
    {
        minCost = currCost;
        minRoute = currState;
        cout<<"Curr Cost : "<<currCost<<'\n';
        return;
    }

    int tryCount = 0;
    if(currState.empty()) visited[curr] = true;

    cout<<areaId<<' '<<minCost<<' '<<currCost<<'\n';

    for(auto &nextInfo : adj[curr])
    {
        int next = nextInfo.second;
        int cost = nextInfo.first;

        if(visited[next]) continue;
        if(currCost + cost > minCost) break;

        //cout<<curr<<"-"<<next<<"\n";

        currState.push_back(cities[next]);
        currCost += cost;
        visited[next] = true;

        findAreaMinimumRoute(areaId, next, currState, currCost);

        currState.pop_back();
        currCost -= cost;
        visited[next] = false;
    }
}

int TreeRouteFinder::getAreaId(const Node &a) const
{
    const int areaLen = (int)coordThres / areaSideCount;
    return (int)(a.y / areaLen) * areaSideCount + (int)(a.x / areaLen);
}
