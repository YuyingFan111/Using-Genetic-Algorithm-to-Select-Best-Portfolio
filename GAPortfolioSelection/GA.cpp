//
//  GA.cpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 6/29/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#include <vector>
#include "GA.hpp"

//------------------------------------Mutate---------------------------------------
//
//  Mutates a chromosome's bits dependent on the MUTATION_RATE
//-------------------------------------------------------------------------------------
void Mutate(Portfolio &p_, vector <Stock> total_stocks_)    //mutation is a very low chance
{
    for (unsigned int i = 0; i<p_.stocks.size(); i++)
    {
        //srand((int)time(NULL));
        if (RANDOM_NUM < MUTATION_RATE)
        {
            int random_ = rand()%SP500_NUM_OF_STOCKS;
            p_.stocks[i]=total_stocks_[random_];
        }
    }
    p_.calculateweights(p_.stocks);
    cout<<"the child:"<<p_<<endl;
    
    return;
}

//---------------------------------- Crossover ---------------------------------------
//
//  Dependent on the CROSSOVER_RATE this function selects a random point along the
//  lenghth of the chromosomes and swaps all the  bits after that point.
//------------------------------------------------------------------------------------
void Crossover(Portfolio &offspring1, Portfolio &offspring2)
{
    //dependent on the crossover rate
    //   if (RANDOM_NUM < CROSSOVER_RATE)
    {
        //create a random crossover point
        //srand((int)time(NULL));
        int crossover = (int)(RANDOM_NUM * CHROMO_LENGTH);
        
        vector <Stock> stocks1=offspring1.stocks;
        vector <Stock> stocks2=offspring2.stocks;
        vector <Stock> new_stock1;
        vector <Stock> new_stock2;
        for (int i=0; i<crossover; i++)
        {
            new_stock1.push_back(stocks1[i]);
            new_stock2.push_back(stocks2[i]);
        }
        for (int j=crossover; j<10; j++)
        {
            new_stock1.push_back(stocks2[j]);
            new_stock2.push_back(stocks1[j]);
        }
        Portfolio t1(new_stock1);
        Portfolio t2(new_stock2);
        //        t1.calculateweights(new_stock1);
        //        t2.calculateweights(new_stock2);
        //        cout<<"the first child1:"<<t1<<endl;
        //        cout<<"the first child2:"<<t2<<endl;
        offspring1 = t1; offspring2 = t2;
    }
}

//select the parents use the best 70 populations, not the worse 70

//--------------------------------Roulette-------------------------------------------
//
//  selects a chromosome from the population via roulette wheel selection
//------------------------------------------------------------------------------------
//string Roulette(int total_fitness, chromo_typ* Population)
Portfolio Roulette(int total_fitness, vector <Portfolio> & Population)
{
    //generate a random number between 0 & total fitness count
    //srand((int)time(NULL));
    float Slice = (float)(RANDOM_NUM * total_fitness);
    
    //go through the chromosones adding up the fitness so far
    float FitnessSoFar = 0.0f;
    
    for (int i = 0; i<Population.size(); i++)
    {
        FitnessSoFar += Population[i].fitness;
        
        //if the fitness so far > random number return the chromo at this point
        if (FitnessSoFar >= Slice)
            
            return Population[i];
    }
    return Population[0];
}

