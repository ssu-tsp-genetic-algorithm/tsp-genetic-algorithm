#include "treeRouteFinder.h"
#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

TreeRouteFinder::TreeRouteFinder(){}

TreeRouteFinder::TreeRouteFinder(const vector<Node> &newCities)
{
    citiesGroup.resize(getTotalAreaCount());
    cities = newCities;

    adj.resize(cities.size()+1);
    minRoute.resize(getTotalAreaCount());

    //도시를 돌며 정점의 영역 ID와 영역 분류 리스트를 초기화
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
    for(int i=1; i<currArea.size(); i++) //u - v 모든 쌍을 시도하여 기준점과의 상대 위치를 초기화
    {
        currArea[i].p = currArea[i].node.y - currArea[0].node.y;
        currArea[i].q = currArea[i].node.x - currArea[0].node.x;
    }

    NodeCH st = currArea[0];
    sort(currArea.begin()+1, currArea.end(), [&st](const NodeCH& a, const NodeCH& b) { //ccw 값에 따라 반시계 방향으로 정렬
        int ccwVal = getCCwValue(st.node, a.node, b.node);
        if(ccwVal == 0) return GeneticSearch::getDistance(st.node, a.node) < GeneticSearch::getDistance(st.node, b.node);
        return ccwVal > 0;
    });

    stack<int> stk; //초기 정점들을 push
    stk.push(0);
    stk.push(1);

    for(int next = 2; next < currArea.size(); next++) //나머지 간선들을 돌며 검사
    {
        while(stk.size() >= 2)
        {
            int first, second;
            first = stk.top(); stk.pop();
            second = stk.top();

            //볼록 껍질 조건에 만족하면 push
            //그렇지 않다면 다시 loop를 돌며 stack의 정점을 하나 뺸다
            if(getCCwValue(currArea[second].node, currArea[first].node, currArea[next].node) > 0)
            {
                stk.push(first);
                break;
            }
        }
        stk.push(next);
    }

    //stack에서 하나씩 빼면서 convexHull 배열을 완성한다
    vector<Node> convexHull;
    while(!stk.empty())
    {
        convexHull.push_back(citiesGroup[areaId][stk.top()].node);
        visited[stk.top()] = true; //Insertion 알고리즘을 구현할때에 사용
        stk.pop();
    }
    return convexHull;
}

vector<Node> TreeRouteFinder::createConvexHullRoute(const int& areaId)
{
    vector<Node> convexHull = createInitialConvexHull(areaId);
    vector<NodeCH>& currArea = citiesGroup[areaId];

    vector<pair<int, int> > insertDataList; //{k, idx}쌍

    while(convexHull.size() != currArea.size()) //convexHull에 영역 내 모든 정점이 들어갈때까지 반복
    {
        for (int i = 0; i < currArea.size(); i++) // 영역내 convex Hull에 포함되지 않은 정점들을 모두 탐색
        {
            if (visited[i]) continue; //포함된건 pass
            const Node &targetNode = currArea[i].node;
            double minLength = 1e6f;
            pair<int, int> insertData;

            for (int j = 0; j < convexHull.size(); j++) //convex Hull내 정점들을 순회하며
            {
                const Node &p = convexHull[j];
                const Node &q = convexHull[(j + 1) % convexHull.size()];

                //간선을 끊고 targetNode를 사이에 끼웠을때의 cost 값을 구해서
                double dist = GeneticSearch::getDistance(p, targetNode)
                              + GeneticSearch::getDistance(targetNode, q)
                              - GeneticSearch::getDistance(p, q);

                //최소인 데이터만 남겨둠
                if (dist < minLength)
                {
                    insertData = {i, j};
                    minLength = dist;
                }
            }
            //후보 리스트에 포함
            insertDataList.push_back(insertData);
        }

        double minLength = 1e6f;
        int insertPos = -1, insertTargetIdx = -1;

        //후보 리스트를 돌며 가장 해당 간선에 가까운 정점 k를 찾는다.
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
        if (insertPos != -1) //가장 가까운 간선을 convex Hull에 포함시킨다.
        {
            visited[insertTargetIdx] = true;
            convexHull.insert(convexHull.begin() + insertPos + 1, currArea[insertTargetIdx].node);
        }
        insertDataList.clear(); //후보는 초기화
    }
    return convexHull;
}

int TreeRouteFinder::getAreaId(const Node &a) const
{
    return 0; //전체 영역으로 지정
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