//
//  Utilities.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Utilities_hpp
#define Utilities_hpp

#include <iostream>
#include <vector>
#include <algorithm>    // std::sort
#include <map>
#include <math.h>
#include "Stock.hpp"
#define NUM_OF_STOCKS 500
#define SP500_NUM_OF_STOCKS 500
using namespace std;


typedef vector <float> Vector;
typedef vector <Vector> Matrix;
typedef map<string,float> Map;

struct less_than_symbol
{
    inline bool operator() (Stock & s1, Stock & s2)
    {
        return (s1.getsymbol() < s2.getsymbol());
    }
};

float calculate_expectation(vector <float> v);
Vector operator + (const Vector &a, const Vector &b);
Vector operator - (const Vector &a, const Vector &b);
Vector operator / (const Vector &a, const float &b);
Map operator - (Map &a, Map &b);
Map operator / (Map &a, const float &b);
float calculate_average(map<string,float> mymap);
vector <Stock> GetRandomStocks(int l, vector <Stock> total_stocks_, vector <int> & random_number);
void checkduplicate(vector <Stock> &s, vector <Stock> total_stocks_);
float calculatemothly(map<string, float> weekly);


#endif /* Utilities_hpp */
