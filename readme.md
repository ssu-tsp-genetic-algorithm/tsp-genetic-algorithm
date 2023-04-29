# Genetic Algorithm을 통한 TSP 최적화

<aside>
💡 23/03/20 20:52

</aside>

## 목차

---

# 0. 과제 명세

---

- 생략

# 1. Genetic Algorithm

---

### 1-1. 정의

- 생물의 진화를 모방하여 최적해를 찾아내는 알고리즘
- 결정론(Detereministic) 알고리즘으로 좋은 해를 잘 구하지 못하는 문제
    - *Solution Space의 크기가 커서 Brute-Force로 시간내에 해결하지 못하는 문제*

### 1-2. 구성 요소

- `염색체 (Chromosome)`
    - 생물학적으로 유전자들이 뭉쳐져 포장된 것
    - GA에서의 염색체는 하나의 개체 값을 의미
        - *`순서 기반의 표현`과 `위치 기반의 표현`*
- `유전자 (Gene)`
    - 염색체의 요소로, 유전 정보를 나타냄
- `자손 (Offspring, Child)`
    - 부모 유전자로부터 생성된 자식 유전자. 이전 세대와 비슷한 유전 정보를 가짐
- `적합도 (Fitness)`
    - 어떤 염색체가 최적해에 얼마나 가까운지를 나타내는 고유한 값
    - 적합도를 통해 염색체의 개체 선별을 하게 됨

### 1-3. 알고리즘의 Flow

- 적자 생존의 법칙을 크게 관통
- Flow Chart
    
    ![Untitled](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled.png)
    
    ```
    1. 초기 집합 설정 (Initialization)
    2. 적합도 평가 (Evalutaion / Fitness Assignment)
    3. 부모 선택 (Selection)
    4. 교차 (Crossover)
    5. 변이 (Mutation)
    ```
    
- 세부 설명
    
    1. `초기 집합 설정 (Initialization)`
    
    - 문제를 해결하기 위한 무작위의 개체를 생성
    - 초기 개체가 우수한 해들로 이루어질 필요는 없다
    
    2. `적합도 평가 (Evaluation / Fitness Assignment)`
    
    - 생성된 개체의 적합도를 평가. 목적 함수를 사용하여 수행
    
    3. `부모 선택 (Selection)`
    
    - 생성된 개체 중에서 적합도가 높은 우수 개체를 선택
    - 토너먼트 선택, 룰렛 휠 선택, 순위 기반 선택 등의 다양한 방법 수행
    
    4. `교차 (Crossover)`
    
    - 부모 개체의 형질을 적절하게 섞어 새로운 개체를 생성
    
    5. `변이 (Mutation)`
    
    - 새로운 개체에 돌연변이를 적용. 일부 유전자를 무작위로 변경

# 2. 연산 구현

---

### 2-0. 사전 작업

- `**유전자형 결정**`
    - 유전자형(=유전자의 인코딩)은 다양한 형태가 존재
    - `스키마` : 염색체에서 나타날 수 있는 패턴으로 자리 수가 n일때, 2^n개의 스키마를 가짐
    - `이진수,  n진수 표현`
    - `실수 표현`
    - `위치 기반 표현`
    - `순서 기반 표현`
- `**초기 모집단 결정**`
    - 모집단은 염색체 간의 상호작용을 위해 매우 중요
    - 모집단의 크기에 따른 성능 차이로 인해, 적절한 조절이 필요

### 2-1. 선택 (Selection)

- 개체군으로부터 부모가 되어 교차를 수행할 염색체를 선택
    - 가급적이면 좋은 부모가 선택이 되도록 해야 함
- 종류
    1. `룰렛 휠(Roulette Wheel)` 
        - 염색체들의 적합도에 비례에서 개체군에서 다음 세대로 넘겨 줄 염색체를 선택
    2. `순위 선택 (Rank Selection)`
        - 염색체들의 적합도가 아닌 순위에 비례에서 선택
        - 개체군의 각 염색체에 대한 순위를 지정 후, 순위에 비례하여 선택 확률을 높게 지정
            
            *⇒ 모든 염색체가 선택될 기회를 제공하지만, 늦은 수렴 속도를 보임*
            
    3. `토너먼트 선택 (Tournament Selection)`
        - 두 개의 염색체를 임의로 선택한 후, 0과 1 사이의 난수를 발생
            
            → 이 값이 기준 `**t**`보다 작으면 두 염색체 중에서 적합도가 좋은 것을 선택
            
            → 그렇지 않으면 적합도가 낮은 것을 선택
            
    4. `엘리티즘 (Elitism)`
        - 가장 좋은 몇 개의 염색체를 아무런 변화 없이 새로운 개체군으로 그대로 복사
        - 교차와 변이 과정에서의 가장 좋은 염색체 소실 가능성을 방지
            
            *⇒ 급격한 성능 향상을 꾀할 수 있다*
            

### 2-2. 교차 (Crossover)

- 두 해의 특징을 부분 결합하여 하나의 새로운 특징을 생성
- 교차의 유형과 구현은 인코딩과 해결할 문제에 따라 달라짐
- **교차 유형**
    1. `단일점(Single Point) 교차`
        - 교차점 하나를 기준으로 부모 염색체를 붙여서 자손 염색체를 만듦
            
            ![[https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled%201.png)
            
            [https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)
            
    2. `두 점(Two Point) 교차`
        - 교차점 두개를 이용하는 방법
            
            ![[https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled%202.png)
            
            [https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)
            
    3. `정규(Uniform) 교차`
        - 염색체의 각 유전자 위치에 대해 난수를 발생시킴
        - 난수값과 미리 설정된 임계값과의 비교를 통해 첫번째와 두번째 부모 유전자를 섞음
            
            ![[https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled%203.png)
            
            [https://3catpapa.tistory.com/59](https://3catpapa.tistory.com/59)
            
    4. `산술(Arithmetic) 교차`
        - 두 부모 염색체에 특정 산술 연산을 적용하여 염색체를 생성하는 방법
            
            ![Untitled](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled%204.png)
            
    5. `간선 재조합` 
        - TSP 문제 해결을 위해 고안된 교차 연산, 부모해의 특성을 보존
            
            ```cpp
            1. 두 부모해를 분석, 각 도시에 연결된 인접 도시 목록을 만든다
            2. 한 도시는 최소 2개, 최대 4개까지의 인접 도시를 가질 수 있
            ```
            

### 2-3. 변이 (Mutation)

- 낮은 확률로 새로운 개체의 일부분의 유전자를 변경하는 연산
    - 자손 염색체가 부모 염색체에 없는 속성을 가지도록 유도
- ***탐색 공간에서 임의적인 이동을 유발하여 개체군 내의 다양성을 유지***
- **인코딩에 따른 연산 방식**
    1. `이진 인코딩의 변이` : 일부 비트를 변환
    2. `순열 인코딩의 변이` : 두 개의 숫자를 선택하고 위치를 바꿈
    3. `실수값 인코딩` : 선택된 유전자의 값에 작은 값을 더하거나 뺌
    4. `트리 인코딩의 변이` : 연산자 또는 숫자에 해당하는 노드를 변경
- **변이 구분**
    1. `**전형적 변이**` 
        - 각각의 유전자에 대해 [0, 1) 범위의 난수를 생성
        - 미리 정한 임의의 임계값 미만의 수가 나오면 해당 유전자를 임의로 변형
            
            *→ 그 이상의 수가 나오면? 그냥 둔다*
            
    2. `**비균등 변이**` 
        - 유전 알고리즘이 진행됨에 따라 변이의 강도를 점점 줄여간다.
            
            ![Untitled](Genetic%20Algorithm%E1%84%8B%E1%85%B3%E1%86%AF%20%E1%84%90%E1%85%A9%E1%86%BC%E1%84%92%E1%85%A1%E1%86%AB%20TSP%20%E1%84%8E%E1%85%AC%E1%84%8C%E1%85%A5%E1%86%A8%E1%84%92%E1%85%AA%209084d15ba4b04bc2950b8fdd5b97e94a/Untitled%205.png)
            
- **수선 (Repair)**
    - 교차와 변이 연산의 결과로 생긴 해가 적법해가 아닌 경우에 해결하는 방법
    - 수선 자체도 일종의 변이 효과를 초래 → ***보통은 변이 이후에 진행***

### 2-4. 유전 알고리즘의 주요 매개변수

| 매개 변수 | 설명 |
| --- | --- |
| 교차 확률 | 교차가 얼마나 자주 수행될 것인지?
엘리티즘이 필요하므로 일반적으로 80-95% 정도로 높게 설정 |
| 변이 확률 | 얼마나 자주 염색체의 일부가 변경될 것인가? (0.5~1.0%) |
| 개체군의 크기 | 한 세대 내에 얼마나 많은 염색체를 포함하고 있는지? |

# 3. 연습 예제 - 간단한 TSP

### 3-0. 기본 정보

- 문제
    
    ```
    2차원 V * V 크기의 인접행렬 map이 있다. (map[i][j] : i번째 도시부터 j번째 도시까지의 거리)
    
    유전 알고리즘을 통해, 0번째 도시에서 모든 도시를 방문하여 다시 0번째로 돌아오는 최단 경로를
    
    찾아라.
    ```
    
- `유전자`는 각 도시의 idx로 두고, `염색체`는 길이가 V인 도시 idx들의 조합으로 정의 *(순열 인코딩)*
- `*~~담금질 기법` 적용 → GeeksforGeeks 예제에 나오는 방식으로~~*
- 기본 데이터 타입
    
    ```cpp
    struct individual // ***{염색체와 적합도의 pair}***
    {
    		string chromosome; //염색체
    		int fitness;  //적합도
    };
    ```
    
- 그 외
    - 본 소스코드에서는 부모 선택과 교차 연산(`crossover`)은 제외되었다.
    - 정렬을 통해 fitness가 낮은 부모부터 자손 개체를 생성
        - *교차 연산과 개체 생성은 모두 돌연변이(`mutate`)연산으로 대체되었다*

### 3-1. 초기 집합 생성

- 문제에 대한 해답으로 사용될 수 있는 초기 집합을 생성
    
    ```cpp
    for(int i=0; i< POPULATION_SIZE; i++)
    {
    		temp.chromosome = createChromosome(); 
    		temp.fitness = calculateFitness(temp.chromosome);
    		population.push_back(temp);
    }
    ```
    
    ```cpp
    string createChromosome()
    {
    	string newChromosome = "0";
    
    	while(1)
    	{
    		if(newChromosome.size() == V)
    		{
    			newChromosome += newChromosome[0];
    			break;
    		}
    		int temp = getRandNum(1, V);
    		if(!getIsContain(newChromosome, (char)(temp + '0')))
    			newChromosome += (char)(temp + '0');
    	}
    	return newChromosome;
    }
    ```
    

### 3-2. 적합도 평가

- 염색체의 적합도는 이동 거리의 합으로 정의한다.
    
    ```cpp
    int calculateFitness(string chromosome)
    {
     	int map[V][V] = {{  0, 136,  32,  53, 162, 143, 180, 192,  83,  79},
    				 					 {136,   0,  75, 137,  17,  31,  56, 157, 178, 173},
    				 					 { 32,  75,   0, 104, 173, 178,  53,  81, 190,  98},
    				 					 { 53, 137, 104,   0, 137,  89, 101, 177,  72,  87},
    				 					 {162,  17, 173, 137,   0,  91, 127,  81,  20, 120},
    				 	 				 {143,  31, 178,  89,  91,   0,  87,  50,  78,  68},
    				 					 {180,  56,  53, 101, 127,  87,   0, 152,  81, 129},
    				 				 	 {192, 157,  81, 177,  81,  50, 152,   0,  20, 149},
    				 					 { 83, 178, 190,  72,  20,  78,  81,  20,   0,  74},
    				 					 { 79, 173,  98,  87, 120,  68, 129, 149,  74,   0}};
    
    	int fitness = 0;
    	for(int i=0; i<chromosome.size()-1; i++)
    	{
    		int from = chromosome[i] - '0'; 
    		int to = chromosome[i+1] - '0'; 
    		
    		if(map[from][to] == INT_MAX) return INT_MAX; //갈수 없는 길이라면?
    		fitness += map[from][to]; //갈 수 있다면 반환 값에 이동거리를 더함
    	}
    	return fitness; 
    }
    ```
    

### 3-3. 돌연변이 연산

- `1`이상 `V-1`이하의 서로 다른 2가지 인덱스를 뽑아 둘을 swap
    
    ```cpp
    string mutateGene(string chromosome)
    {
    	while(true)
    	{
    		int idxA = getRandNum(1, V);
    		int idxB = getRandNum(1, V);
    		if(idxA == idxB) continue; //만약 두 수가 같다면 CONTINUE
    		swap(chromosome[idxA], chromosome[idxB]); 
    		break;
    	}
    	return chromosome;
    }
    ```
    
- 메인 Loop 내 호출부
    
    ```cpp
    //=======Main Loop=============================
    	while(temperature > 10 && gen <= gen_thres)
    	{
    		sort(population.begin(), population.end(), lessThan);
    		vector<individual> newPopulation;
    
    		for(auto &p : population)
    		{
    			while(1)
    			{
    				string newGene = **mutateGene(p.chromosome); //돌연변이 연산**
    				individual newChromosome = {newGene, calculateFitness(newGene)};
    				...
    			}
    		}
    		...
    	}
    ```
    

### 3-4. 새로운 세대 생성

- `모의 담금질 알고리즘`을 적용하여 무시될 수 있는 염색체도 “낮은 확률로” 포함하도록 한다.
    
    *→ 보통은 유전 알고리즘과 비교가 되는 별도의 알고리즘인데, geeksforgeeks에서는 이를 적용을 시켰더라구요. 정확도를 높이는 역할 같은데, 어째서 이게 의도대로 동작하는지는 의문입니다.*
    
- 메인 Loop
    
    ```cpp
    //=======Main Loop=============================
    while(temperature > 10 && gen <= gen_thres)
    {
    	...
    	for(auto &p : population)
    	{
    		while(1)
    		{
    			... 돌연변이 연산 ...
    
    			if(newChromosome.fitness <= p.fitness)
    			{
    				newPopulation.push_back(newChromosome);
    				break;
    			}
    			else //모의 담금질 알고리즘
    			{
    				float prob = pow(2.7, -1 * ((float)(newChromosome.fitness
    												- p.fitness) / temperature));
    				if(prob > 0.5)
    				{
    					newPopulation.push_back(newChromosome);
    					break;
    				}
    			}
    		}
    	}
    
    	temperature = cooldown(temperature);
    	population = newPopulation;
    	
    	...
    	gen += 1;
    }
    ```
    

### 3-5. 풀 소스 코드

- main.cpp
    
    ```cpp
    #include <iostream>
    #include <algorithm>
    #include <utility>
    #include <limits.h>
    #include <time.h>
    #include "genetic.h"
    using namespace std;
    
    int map[V][V] = {{  0, 136,  32,  53, 162, 143, 180, 192,  83,  79},
     					 {136,   0,  75, 137,  17,  31,  56, 157, 178, 173},
     					 { 32,  75,   0, 104, 173, 178,  53,  81, 190,  98},
     					 { 53, 137, 104,   0, 137,  89, 101, 177,  72,  87},
     					 {162,  17, 173, 137,   0,  91, 127,  81,  20, 120},
     	 				 {143,  31, 178,  89,  91,   0,  87,  50,  78, 68},
     					 {180,  56,  53, 101, 127,  87,   0, 152,  81, 129},
     				 	 {192, 157,  81, 177,  81,  50, 152,   0,  20, 149},
     					 { 83, 178, 190,  72,  20,  78,  81,  20,   0,  74},
     					 { 79, 173,  98,  87, 120,  68, 129, 149,  74,  0}};
    
    int main()
    {
    	srand((unsigned)time(NULL));
    
    	int gen = 1; 
    	int gen_thres = 500;
    
    	vector<individual> population;
    	individual temp;
    
    	for(int i=0; i< POPULATION_SIZE; i++)
    	{
    		temp.chromosome = createChromosome();
    		temp.fitness = calculateFitness(temp.chromosome);
    		population.push_back(temp);
    	}
    
    	cout<<"\n======Initial Population======\n";
    	cout<<"CHROMO     FITNESS VALUE\n";
    	for(auto &p : population)
    		cout<<p.chromosome<<' '<<p.fitness<<'\n'; 
    	cout<<'\n';
    
    	bool bFound = false; 
    	int temperature = 100;
    
    	//=======Main Loop=============================
    	while(temperature > 10 && gen <= gen_thres)
    	{
    		cout<<"Current Temperature : "<<temperature<<'\n';
    		
    		sort(population.begin(), population.end(), lessThan);
    		vector<individual> newPopulation;
    
    		for(auto &p : population)
    		{
    			while(1)
    			{
    				string newGene = mutateGene(p.chromosome);
    				individual newChromosome = {newGene, calculateFitness(newGene)};
    
    				if(newChromosome.fitness <= p.fitness)
    				{
    					newPopulation.push_back(newChromosome);
    					break;
    				}
    				else
    				{
    					float prob = pow(2.7, -1 * ((float)(newChromosome.fitness
    													- p.fitness) / temperature));
    					if(prob > 0.5)
    					{
    						newPopulation.push_back(newChromosome);
    						break;
    					}
    				}
    			}
    		}
    
    		temperature = cooldown(temperature);
    		population = newPopulation;
    		
    		//cout<<"\n======Gen-"<<gen<<") Population======\n";
    		int average = 0, minVal = INT_MAX;
    		//cout<<"CHROMO     FITNESS VALUE\n";
    		for(auto &p : population)
    		{
    			//cout<<p.chromosome<<' '<<p.fitness<<'\n'; 
    			average += p.fitness;
    			minVal = min(minVal, p.fitness);
    		}
    		cout<<"-"<<gen<<"\'s AVERAGE = "<<average/POPULATION_SIZE<<", MIN : "<<minVal<<'\n';
    		//cout<<'\n';
    		gen += 1;
    	}
    }
    ```
    
- genetic.h
    
    ```cpp
    #pragma once
    #include <vector>
    #include <random>
    #include <cstdlib>
    using namespace std;
    
    #define V 10
    #define START 0
    
    #define POPULATION_SIZE 10
    
    struct individual
    {
    	string chromosome;
    	int fitness;
    };
    
    int getRandNum(int start, int end);
    
    bool getIsContain(string s, char ch);
    
    int cooldown(int temp);
    
    bool lessThan(struct individual t1, struct individual t2);
    
    int calculateFitness(string chromosome);
    
    string mutateGene(string chromosome);
    
    string createChromosome();
    ```
    
- genetic.cpp
    
    ```cpp
    #include "genetic.h"
    #include <limits.h>
    
    int getRandNum(int start, int end)
    {
    	int r = end - start;
    	int rnum = start + rand() % r;
    	return rnum;
    }
    
    bool getIsContain(string s, char ch)
    {
    	for(auto &c : s) if(c == ch) return true;
    	return false;
    }
    
    int cooldown(int temp)
    {
    	return (999 * temp) / 1000;
    }
    
    bool lessThan(struct individual t1, struct individual t2)
    {
    	return t1.fitness < t2.fitness;
    }
    
    int calculateFitness(string chromosome)
    {
     	int map[V][V] = {{  0, 136,  32,  53, 162, 143, 180, 192,  83,  79},
     					 {136,   0,  75, 137,  17,  31,  56, 157, 178, 173},
     					 { 32,  75,   0, 104, 173, 178,  53,  81, 190,  98},
     					 { 53, 137, 104,   0, 137,  89, 101, 177,  72,  87},
     					 {162,  17, 173, 137,   0,  91, 127,  81,  20, 120},
     	 				 {143,  31, 178,  89,  91,   0,  87,  50,  78, 68},
     					 {180,  56,  53, 101, 127,  87,   0, 152,  81, 129},
     				 	 {192, 157,  81, 177,  81,  50, 152,   0,  20, 149},
     					 { 83, 178, 190,  72,  20,  78,  81,  20,   0,  74},
     					 { 79, 173,  98,  87, 120,  68, 129, 149,  74,  0}};
    
    	int fitness = 0;
    	for(int i=0; i<chromosome.size()-1; i++)
    	{
    		int from = chromosome[i] - '0';
    		int to = chromosome[i+1] - '0';
    		
    		if(map[from][to] == INT_MAX) return INT_MAX;
    		fitness += map[from][to];
    	}
    	return fitness; 
    }
    
    string mutateGene(string chromosome)
    {
    	while(true)
    	{
    		int idxA = getRandNum(1, V);
    		int idxB = getRandNum(1, V);
    		if(idxA == idxB) continue;
    		swap(chromosome[idxA], chromosome[idxB]);
    		break;
    	}
    	return chromosome;
    }
    
    string createChromosome()
    {
    	string newChromosome = "0";
    
    	while(1)
    	{
    		if(newChromosome.size() == V)
    		{
    			newChromosome += newChromosome[0];
    			break;
    		}
    		int temp = getRandNum(1, V);
    		if(!getIsContain(newChromosome, (char)(temp + '0')))
    			newChromosome += (char)(temp + '0');
    	}
    	return newChromosome;
    }
    ```
    

# 4. 예제 - 일반 TSP 문제

---

### 4-0. 기본 정보

- 위에서 다뤘던 쉬운 TSP 문제
    - 인접 행렬 형태의 그래프
    - 염색체(Chromosome)는 도시의 나열 (경로)
- 

# +) 참고

---

[[ML101] #1. Machine Learning?](https://brunch.co.kr/@mnc/5)

[Traveling Salesman Problem using Genetic Algorithm - GeeksforGeeks](https://www.geeksforgeeks.org/traveling-salesman-problem-using-genetic-algorithm/)

[[최적화/전역 최적화] 유전 알고리즘 (Genetic Algorithm)](https://untitledtblog.tistory.com/110)

[제 15강 유전 알고리즘 ( 선택, 교차, 변이)](https://3catpapa.tistory.com/59)

[볼츠만 토너먼트 - Google Search](https://www.google.com/search?q=볼츠만+토너먼트&sxsrf=AJOqlzXzuSwHc0vIscrBoT5SSU5awLVXGQ:1679480893086&ei=PdgaZMn2BIGzoASegpPIDQ&oq=볼츠만+&gs_lcp=Cgxnd3Mtd2l6LXNlcnAQAxgBMgQIIxAnMgQIIxAnMgQIIxAnMgsIABCABBCxAxCDATIFCAAQgAQyBAgAEEMyBQgAEIAEMgUIABCABDIFCAAQgAQyBAgAEEM6CggAEEcQ1gQQsANKBAhBGABQogRY_Qtg-xZoAnABeACAAY8BiAGKA5IBAzAuM5gBAKABAcgBCsABAQ&sclient=gws-wiz-serp)

[https://koreascience.kr/article/CFKO199911921312203.pdf](https://koreascience.kr/article/CFKO199911921312203.pdf)

---
