#include <opencv2/opencv.hpp>

//Convex Hull을 위한 Node
struct NodeCH
{
    Node node;
    int p;
    int q;
};

class KmeansGeneticSearch : public GeneticSearch{
private:
    //kmeans 클러스터링 클러스터 수
    int k;
    //각 도시 순서별 군집 정보를 저장한 배열
    cv::Mat clusteredLabel;
    //각 군집의 중심점을 저장한 배열
    cv::Mat centers;

public:
    KmeansGeneticSearch(const vector<Node> &newCities, int k);

    //클러스터링 결과를 노드별 id 순으로 반환
    void kmeansClustering(cv::Mat &clusteredLabel, cv::Mat &centers, vector<Node> cities, int k);

    //노드들을 openCV의 Mat으로 변환
    cv::Mat convertNodesToMat(const vector<Node> & nodes);

    //클러스터링 후 클러스터 내 Greedy 알고리즘을 통해 초기 모집단 설정
    void initPopulation(vector<Chromosome>& population);

    void initPopulationWithRandom(vector<Chromosome> &population);

    //클러스터링 후 클러스터 내 클러스터의 중심을 중심으로 Greedy 알고리즘을 통해 초기 모집단 설정
    void initPopulationWithMST(vector<Chromosome> &population);

    //각 클러스터의 중심위치 반환.
    vector<Node> getCenters();

    int getCenterNode(vector<Node>& group, int clusterNum);

    //클러스터링 후 클러스터 내 Convex Hull 알고리즘을 통해 초기 모집단 설정
    void initPopulationWithConvexHull(vector<Chromosome>& population);

    //클러스터 별 Convex Hull 알고리즘 실행결과 반환.
    vector<Node> createConvexHullRoute(int i, vector<vector<NodeCH>> & citiesGroup);

    static inline bool compNode(const NodeCH &a, const NodeCH &b);

    static inline int getCCwValue(const Node& a, const Node& b, const Node& c);
};

