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