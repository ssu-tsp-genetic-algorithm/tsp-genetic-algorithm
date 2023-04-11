#include "genetic.h"
#include "kmeans.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#define MAX_BRANCH_COUNT 3

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
    cout<<clusterCenters[0].x<<endl;
    return clusterCenters;
}

/**
 * Kmeans클러스터링 후 각 군집별 랜덤하게 구성 후 이어붙임
 * @param population
 * @param cities
 * @param populationSize //최대 초기 부모 유전자 수
 */
void KmeansGeneticSearch::initPopulation(vector<Chromosome> &population)
{
    //[groupNum][idx] : groupNum 영역의 idx번째 도시 (node.id가 저장)
    vector<int> citiesGroup[k];
    //[u][v] : t
    vector<vector<pair<double, int> > > adj;
    //중복 체크
    vector<bool> visited(cities.size(), false);
    //찾은 최소의 경로
    vector<vector<Node>> minRoute;


    //클러스터링 실행
    kmeansClustering(clusteredLabel, centers, cities, k);

    //k개의 군집 생성
    vector<Node> group[k];
    for(int i=1; i<cities.size(); i++)
    {
        group[clusteredLabel.at<int>(i)].push_back(cities[i]);
    }

    population.clear();

    //군집 내 랜덤 생성
    random_device rd;
    mt19937 g(rd());

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

void KmeansGeneticSearch::initPopulationWithGreedy(vector<Chromosome> &population)
{
    //[groupNum][idx] : groupNum 영역의 idx번째 도시 (node.id가 저장)
    vector<int> citiesGroup[k];
    //[u][v] : t
    vector<vector<pair<double, int> > > adj;
    //중복 체크
    vector<bool> visited(cities.size(), false);
    //찾은 최소의 경로
    vector<vector<Node>> minRoute;


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
    //인접행렬 생성.
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

    vector<Node> tmp;
    int firstClusterNum = clusteredLabel.at<int>(0,0);
    for(int i=firstClusterNum;i<k;i++)
        tmp.insert(tmp.end(), route[i].begin(), route[i].end());
    for(int i=0;i<firstClusterNum;i++)
        tmp.insert(tmp.end(), route[i].begin(), route[i].end());


    Chromosome initialChromosome;
    initialChromosome.gene = tmp;

    for(int i=0; i<populationSize; i++)
        population.push_back(initialChromosome);
}
