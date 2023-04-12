#include "treeRouteFinder.h"
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#define MAX_BRANCH_COUNT 3

TreeRouteFinder::TreeRouteFinder(){}

TreeRouteFinder::TreeRouteFinder(const vector<Node> &newCities)
{
    citiesGroup.resize(getTotalAreaCount());
    cities = newCities;

    adj.resize(cities.size()+1);
    minRoute.resize(getTotalAreaCount());

    for(auto& node : cities)
    {
        if(node.areaId != 0) break;
        node.areaId = getAreaId(node);
        citiesGroup[node.areaId].push_back({cities[node.id], 0, 0});
    }
}

bool TreeRouteFinder::compAreaId(const Node &a, const Node &b) { return a.areaId < b.areaId; }

vector<Node> TreeRouteFinder::createConvexHullRoute(const int& areaId)
{
    vector<NodeCH>& currArea = citiesGroup[areaId];
    vector<bool> visited(currArea.size(), false);

    sort(currArea.begin(), currArea.end(), compNode);
    for(int i=1; i<currArea.size(); i++) //u - v 모든 쌍을 시도
    {
        currArea[i].p = currArea[i].node.y - currArea[0].node.y;
        currArea[i].q = currArea[i].node.x - currArea[0].node.x;
    }

    NodeCH st = currArea[0];
    sort(currArea.begin()+1, currArea.end(), [&st](const NodeCH& a, const NodeCH& b) {
        int ccwVal = getCCwValue(st.node, a.node, b.node);
        if(ccwVal == 0) return GeneticSearch::getDistance(st.node, a.node) < GeneticSearch::getDistance(st.node, b.node);
        return ccwVal > 0;
    });

    stack<int> stk;
    stk.push(0);
    stk.push(1);

    for(int next = 2; next < currArea.size(); next++)
    {
        while(stk.size() >= 2)
        {
            int first, second;
            first = stk.top(); stk.pop();
            second = stk.top();

            if(getCCwValue(currArea[second].node, currArea[first].node, currArea[next].node) > 0)
            {
                stk.push(first);
                break;
            }
        }
        stk.push(next);
    }

    vector<Node> ret;
    while(!stk.empty())
    {
        ret.push_back(citiesGroup[areaId][stk.top()].node);
        visited[stk.top()] = true;
        stk.pop();
    }

    for(int i=0; i<currArea.size(); i++)
    {
        if(visited[i]) continue;
        Node& targetNode = currArea[i].node;

        double minLength = 1e6f;
        vector<int> insertPosCandidate;
        for(int j=0; j<ret.size(); j++)
        {
            const Node& p = ret[j];
            const Node& q = ret[(j+1) % ret.size()];

            double dist = GeneticSearch::getDistance(targetNode, p)
                        + GeneticSearch::getDistance(targetNode, q)
                        - GeneticSearch::getDistance(p, q);

            if(dist <= minLength)
            {
                if(dist == minLength) insertPosCandidate.push_back(j);
                else insertPosCandidate = {j};
                minLength = dist;
            }
        }

        minLength = 1e6f;
        int insertPos = -1;

        for(auto &candidate : insertPosCandidate)
        {
            const Node& p = ret[candidate];
            const Node& q = ret[(candidate+1) % ret.size()];

            double dist = (GeneticSearch::getDistance(p, targetNode)
                          + GeneticSearch::getDistance(targetNode, q))
                          / GeneticSearch::getDistance(p, q);

            if(dist < minLength)
            {
                dist = minLength;
                insertPos = candidate;
            }
        }

        if(insertPos != -1)
            ret.insert(ret.begin() + insertPos, targetNode);
    }

    return ret;
}

int TreeRouteFinder::getAreaId(const Node &a) const
{
    const int areaLen = (int)coordThres / areaSideCount;  
    return (int)(a.y / areaLen) * areaSideCount + (int)(a.x / areaLen);
}

int TreeRouteFinder::getCCwValue(const Node& a, const Node& b, const Node& c)
{
    double result = (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    return result == 0 ? 0 : (result > 0 ? 1 : -1);
}

bool TreeRouteFinder::compNode(const NodeCH &a, const NodeCH &b)
{
    if(a.node.y != b.node.y) return a.node.y < b.node.y;
    return a.node.x < b.node.x;
}