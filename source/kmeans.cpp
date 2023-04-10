#include "genetic.h"
#include "kmeans.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
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
 * KmeansGeneticSearch 클러스터링 진행
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
 * Kmeans클러스터링 후 각 군집별 랜덤하게 구성 후 이어붙임
 * @param population
 * @param cities
 * @param populationSize //최대 초기 부모 유전자 수
 */
void KmeansGeneticSearch::initPopulationWithKmeansRandom(vector<Chromosome> &population)
{
    //각 도시 순서별 군집 정보를 저장한 배열
    cv::Mat clusteredLabel;
    //각 군집의 중심점을 저장한 배열
    cv::Mat centers;

    //클러스터링 실행
    kmeansClustering(clusteredLabel, centers, cities, k);

    //3개의 군집 생성
    vector<Node> group[3];
    for(int i=1; i<cities.size(); i++)
    {
        if(clusteredLabel.at<int>(i, 0) == 0)
            group[0].push_back(cities[i]);
        else if(clusteredLabel.at<int>(i, 0) == 1)
            group[1].push_back(cities[i]);
        else
            group[2].push_back(cities[i]);
    }

    //군집 내 랜덤 생성
    random_device rd;
    mt19937 g(rd());

    population.clear();
    for(int i=0; i<populationSize; i++)
    {
        for(int j=0;j<3;j++)
        {
            shuffle(group[j].begin(), group[j].end(), g);
        }

        vector<Node> tmp = {cities[0]};

        if(clusteredLabel.at<int>(0,0)==0)
        {
            tmp.insert(tmp.end(), group[0].begin(), group[0].end());
            tmp.insert(tmp.end(), group[1].begin(),group[1].end());
            tmp.insert(tmp.end(), group[2].begin(),group[2].end());
        }
        else if(clusteredLabel.at<int>(0,0)==1)
        {
            tmp.insert(tmp.end(), group[1].begin(), group[1].end());
            tmp.insert(tmp.end(), group[2].begin(),group[2].end());
            tmp.insert(tmp.end(), group[0].begin(),group[0].end());
        }
        else
        {
            tmp.insert(tmp.end(), group[2].begin(), group[2].end());
            tmp.insert(tmp.end(), group[0].begin(),group[0].end());
            tmp.insert(tmp.end(), group[1].begin(),group[1].end());
        }

        population.push_back({tmp, 0.0f});
    }
}