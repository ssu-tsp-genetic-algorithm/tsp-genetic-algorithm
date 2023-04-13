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

vector<Node> TreeRouteFinder::createInitialConvexHull(const int& areaId)
{
    vector<NodeCH>& currArea = citiesGroup[areaId];
    visited = vector<bool>(currArea.size(), false);

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

    vector<Node> convexHull;
    while(!stk.empty())
    {
        convexHull.push_back(citiesGroup[areaId][stk.top()].node);
        visited[stk.top()] = true;
        stk.pop();
    }
    return convexHull;
}

vector<Node> TreeRouteFinder::createConvexHullRoute(const int& areaId)
{
    vector<Node> convexHull = createInitialConvexHull(areaId);
    vector<NodeCH>& currArea = citiesGroup[areaId];

    vector<pair<int, int> > insertDataList;

    cout<<currArea.size()<<'\n';
    while(convexHull.size() != currArea.size())
    {
        for (int i = 0; i < currArea.size(); i++)
        {
            if (visited[i]) continue;
            const Node &targetNode = currArea[i].node;
            double minLength = 1e6f;

            pair<int, int> insertData;
            for (int j = 0; j < convexHull.size(); j++)
            {
                const Node &p = convexHull[j];
                const Node &q = convexHull[(j + 1) % convexHull.size()];

                double dist = GeneticSearch::getDistance(p, targetNode)
                              + GeneticSearch::getDistance(targetNode, q)
                              - GeneticSearch::getDistance(p, q);

                if (dist < minLength)
                {
                    insertData = {i, j};
                    minLength = dist;
                }
            }
            insertDataList.push_back(insertData);
        }

        double minLength = 1e6f;
        int insertPos = -1, insertTargetIdx = -1;

        for (auto &c: insertDataList)
        {
            if (visited[c.first]) continue;
            const Node &targetNode = currArea[c.first].node;

            const Node &a = convexHull[c.second];
            const Node &b = convexHull[(c.second + 1) % convexHull.size()];

            double dist = (GeneticSearch::getDistance(a, targetNode)
                           + GeneticSearch::getDistance(targetNode, b))
                           / GeneticSearch::getDistance(a, b);

            if (dist < minLength)
            {
                insertPos = c.second;
                insertTargetIdx = c.first;
                minLength = dist;
            }
        }
        if (insertPos != -1)
        {
            visited[insertTargetIdx] = true;
            convexHull.insert(convexHull.begin() + insertPos + 1, currArea[insertTargetIdx].node);
        }
        insertDataList.clear();
    }
    return convexHull;
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