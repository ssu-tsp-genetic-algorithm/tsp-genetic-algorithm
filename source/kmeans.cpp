#include "genetic.h"
#include "kmeans.h"

#include <algorithm>
#include <stack>
#include <opencv2/opencv.hpp>

using namespace std;

KmeansGeneticSearch::KmeansGeneticSearch(const vector<Node> &newCities, int k) : GeneticSearch(newCities), k(k) {}

/**
 * 노드들을 OpenCV의 Mat으로 변환
 * @param nodes
 * @return cv::Mat
 */
cv::Mat KmeansGeneticSearch::convertNodesToMat(const vector<Node> &nodes)
{
    //행: 도시의 개수(1000), 열: y x좌표(2)
    cv::Mat nodeMat(1000, 2, CV_32F);
    for(int i=0; i<1000; i++)
    {
        nodeMat.at<float>(i,0) = nodes[i].y;
        nodeMat.at<float>(i,1) = nodes[i].x;
    }
    return nodeMat;
}

/**
 * 클러스터링 진행
 * @param clusteredLabel //노드id에 따른 클러스터링 결과 저장
 * @param centers //클러스터링 결과 군집의 중심들
 * @param cities //노드
 * @param k //클러스터링 할 군집의 수
 */
void KmeansGeneticSearch::kmeansClustering(cv::Mat &clusteredLabel, cv::Mat &centers, vector<Node> cities, int k)
{
    if(cities.size()==0) return;

    //cv::Mat으로 변환
    const cv::Mat &mat = convertNodesToMat(cities);

    //클러스터링 알고리즘 수행 최대 반복횟수, 임계값 지정
    cv::TermCriteria termCriteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 0.1);

    //kmeans 클러스터링 실행
    cv::kmeans(mat, k, clusteredLabel, termCriteria,
               3, cv::KMEANS_PP_CENTERS, centers);
}

/**
 * 클러스터들의 중심 위치 반환.
 * @return
 */
vector<Node> KmeansGeneticSearch::getCenters(){
    vector<Node> clusterCenters(k);
    for(int i=0;i<k;i++)
    {
        clusterCenters[i].y = centers.at<float>(i, 0);
        clusterCenters[i].x = centers.at<float>(i, 1);
    }
    return clusterCenters;
}

/**
 * 클러스터링 후 각 군집별 랜덤하게 초기 유전자 형성
 * @param population
 */
void KmeansGeneticSearch::initPopulation(vector<Chromosome> &population)
{
    //클러스터링 실행
    kmeansClustering(clusteredLabel, centers, cities, k);

    //k개의 군집 생성
    vector<Node> group[k];
    for(int i=1; i<cities.size(); i++)
    {
        group[clusteredLabel.at<int>(i, 0)].push_back(cities[i]);
    }

    //군집 내 랜덤 생성
    random_device rd;
    mt19937 g(rd());

    population.clear();
    for(int i=0; i<populationSize; i++)
    {
        for(int j=0;j<3;j++)
            shuffle(group[j].begin(), group[j].end(), g);

        //첫 노드의 클러스터에 따라 초기 gene 형성
        vector<Node> tmp = {cities[0]};
        int firstClusterNum = clusteredLabel.at<int>(0,0);
        for(int i=firstClusterNum;i<k;i++)
            tmp.insert(tmp.end(), group[i].begin(), group[i].end());
        for(int i=0;i<firstClusterNum;i++)
            tmp.insert(tmp.end(), group[i].begin(), group[i].end());

        population.push_back({tmp, 0.0f});
    }
}

/**
 * 클러스터링 후 각 군집별 그리디 알고리즘을 통해 초기 유전자 형성
 * @param population
 */
void KmeansGeneticSearch::initPopulationWithGreedy(vector<Chromosome> &population)
{
    //[groupNum][idx] : groupNum 영역의 idx번째 도시 (node.id가 저장)
    vector<int> citiesGroup[k];
    //클러스터 내 인접행렬 [u][v] : t
    vector<vector<pair<double, int> > > adj;
    //클러스터 내 그리디 적용 시중복 체크
    vector<bool> visited(cities.size(), false);

    //클러스터링 실행
    kmeansClustering(clusteredLabel, centers, cities, k);

    //k개의 군집 생성
    vector<Node> group[k];
    for(int i=0; i<cities.size(); i++)
    {
        int groupNum = clusteredLabel.at<int>(i, 0);
        citiesGroup[groupNum].push_back(cities[i].id);
    }

    adj.resize(cities.size()+1);
    //클러스터 내 인접행렬 생성.
    for(int groupNum=0; groupNum<k; groupNum++)
    {
        vector<int>& currArea = citiesGroup[groupNum];

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
        }

    }

    vector<Node> route[k];

    //클러스터 별 그리디알고리즘 실행
    for(int i=0;i<k;i++){
        vector<int>& currGroup = citiesGroup[i];
        route[i].push_back(cities[citiesGroup[i][0]]);  //경로의 시작 지점을 그룹의 첫 노드로 설정.
        int curr = currGroup[0];
        visited[curr] = true;
        while(route[i].size() < currGroup.size())
        {
            int next = -1;
             //연결된 노드 중에서 가장 짧은 거리를 가진 노드를 다음 노드로 선택
            for(auto& edge : adj[curr])
            {
                int node = edge.second;
                if(!visited[node])
                {
                    next = node;
                    break;
                }
            }

            // 다음 노드를 현재 노드로 업데이트하고 경로에 추가
            curr = next;
            route[i].push_back(cities[curr]);
            visited[curr] = true;
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    vector<Node> tmp;

    //첫 노드의 클러스터 번호
    int firstClusterNum = clusteredLabel.at<int>(0,0);
    //초기 gene을 구성할 클러스터의 순서
    vector<int> clustersOrder(k);

    // 클러스터의 순서 랜덤 배열
    for(int i=0; i<=k; i++) clustersOrder[i] = i;
    swap(clustersOrder[0], clustersOrder[firstClusterNum]);

    Chromosome initialChromosome;

    for(int i=0; i<populationSize; i++)
    {
        std::shuffle(clustersOrder.begin()+1, clustersOrder.end(),g);
        for(int o=0;o<k;o++)
            tmp.insert(tmp.end(), route[clustersOrder[o]].begin(), route[clustersOrder[o]].end());
        initialChromosome.gene = tmp;
        population.push_back(initialChromosome);
    }
}

/**
 * 클러스터링 후 각 군집별 Convex Hull 알고리즘을 통해 초기 유전자 형성
 * @param population
 */
void KmeansGeneticSearch::initPopulationWithConvexHull(vector<Chromosome>& population)
{
    //[groupNum][idx] : groupNum 영역의 idx번째 도시 (node.id가 저장)
    vector<vector<NodeCH>> citiesGroup(k);
    //클러스터 내 인접행렬 [u][v] : t
    vector<vector<pair<double, int> > > adj;
    //중복 체크
    vector<bool> visited(cities.size(), false);
    Chromosome initialChromosome;

    //클러스터링 실행
    kmeansClustering(clusteredLabel, centers, cities, k);

    //k개의 군집 생성
    vector<Node> group[k];
    for(int i=0; i<cities.size(); i++)
    {
        int groupNum = clusteredLabel.at<int>(i, 0);
        citiesGroup[groupNum].push_back({cities[i], 0, 0});
    }

    adj.resize(cities.size()+1);

    //Convex Hull 알고리즘 실행
    for(int i=0; i<k; i++)
    {
        vector<Node> convexHull = createConvexHullRoute(i, citiesGroup);

        if(i==0) //시작 정점인 경우
        {
            const Node& stNode = cities[0];
            int stIdx = find_if(convexHull.begin(), convexHull.end(), [stNode](Node& n){
                return n.y == stNode.y && n.x == stNode.x; }) - convexHull.begin();
            convexHull.insert(convexHull.end(), convexHull.begin(), convexHull.begin()+stIdx);
            convexHull.erase(convexHull.begin(), convexHull.begin()+stIdx);
        }

        initialChromosome.gene.insert(initialChromosome.gene.end(), convexHull.begin(), convexHull.end());
    }

    population.clear();
    for(int i=0; i<populationSize; i++)
        population.push_back(initialChromosome);
}

/**
 * 클러스터 별 Convex Hull 알고리즘 실행
 * @param groupId
 * @param citiesGroup
 * @return vector<Node> Convex Hull 알고리즘 실행 결과
 */
vector<Node> KmeansGeneticSearch::createConvexHullRoute(int groupId, vector<vector<NodeCH>> & citiesGroup)
{
    vector<NodeCH>& currArea = citiesGroup[groupId];
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
        ret.push_back(citiesGroup[groupId][stk.top()].node);
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

bool KmeansGeneticSearch::compNode(const NodeCH &a, const NodeCH &b)
{
    if(a.node.y != b.node.y) return a.node.y < b.node.y;
    return a.node.x < b.node.x;
}

int KmeansGeneticSearch::getCCwValue(const Node& a, const Node& b, const Node& c)
{
    double result = (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
    return result == 0 ? 0 : (result > 0 ? 1 : -1);
}