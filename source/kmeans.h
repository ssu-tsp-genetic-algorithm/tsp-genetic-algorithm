#include <opencv2/opencv.hpp>

class KmeansGeneticSearch : public GeneticSearch{
private:
    //kmeans 클러스터링 군집 수
    int k;

public:
    KmeansGeneticSearch(const vector<Node> &newCities, int k);

    //클러스터링 결과를 노드별 id 순으로 반환
    void kmeansClustering(cv::Mat &clusteredLabel, cv::Mat &centers, vector<Node> cities, int k);

    //노드들을 openCV의 Mat으로 변환
    cv::Mat convertNodesToMat(const vector<Node> & nodes);

    //클러스터링 기반으로 초기 모집단 랜덤 설정
    void initPopulationWithKmeansRandom(vector<Chromosome>& population);
};

