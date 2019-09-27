//
//  GA.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 6/29/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef GA_hpp
#define GA_hpp

#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include "Portfolio.hpp"

#define CROSSOVER_RATE            0.7
#define MUTATION_RATE             0.03
#define POP_SIZE                  100           //must be an even number
#define CHROMO_LENGTH             10
#define GENE_LENGTH               1
#define MAX_ALLOWABLE_GENERATIONS   40
#define RANDOM_NUM    ((float)rand()/RAND_MAX)
using namespace std;


struct less_than_fitness
{
    inline bool operator() (const Portfolio & chromo1, const Portfolio & chromo2)
    {
        return (chromo1.fitness < chromo2.fitness);
    }
};
//function class, only one function, every time call this class, call this function


/////////////////////////////////prototypes/////////////////////////////////////////////////////

Portfolio Roulette(int total_fitness, vector <Portfolio> & Population);
void Mutate(Portfolio &p_, vector <Stock> total_stocks_);
void Crossover(Portfolio &offspring1, Portfolio &offspring2);

#endif /* GA_hpp */
