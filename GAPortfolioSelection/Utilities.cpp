//
//  Utilities.cpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#include "Utilities.hpp"
#include <algorithm>    // std::sort
#include <vector>

float calculate_expectation(vector <float> v)
{
    float sum=0;
    for (int i=0; i<v.size(); i++)
    {
        sum=sum+v[i];
    }
    return sum/v.size();
}

//vector calculation overloading
Vector operator + (const Vector &a, const Vector &b)
{
    unsigned long size_a=a.size();
    unsigned long size_b=b.size();
    unsigned long size;
    if (size_a<size_b)
        size=size_a;
    else
        size=size_b;
    Vector result(size);
    for (unsigned long i=size-1; i>=0; i--)
    {
        result[i]=a[i]+b[i];
    }
    return result;
}

Vector operator - (const Vector &a, const Vector &b)
{
    unsigned long size_a=a.size();
    unsigned long size_b=b.size();
    unsigned long size;
    if (size_a<size_b)
        size=size_a;
    else
        size=size_b;
    Vector result(size);
    for (unsigned long i=size-1; i>=0; i--)
    {
        result[i]=a[i]-b[i];
    }
    return result;
}

Vector operator / (const Vector &a, const float &b)
{
    unsigned long size=a.size();
    Vector result(size);
    for (unsigned long i=0; i<size; i++)
    {
        result[i]=a[i]/b;
    }
    return result;
}


Map operator - (Map &a, Map &b)
{
    Map result;
    for (Map::iterator itr=a.begin(); itr!=a.end();itr++)
    {
        result[itr->first]=a[itr->first]-b[itr->first];
    }
    return result;
}

Map operator / (Map &a, const float &b)
{
    Map result;
    for (Map::iterator itr=a.begin(); itr!=a.end();itr++)
    {
        result[itr->first]=a[itr->first]/b;
    }
    return result;
}

float calculate_average(map<string,float> mymap)
{
    float temp=0;
    int count=0;
    for (map<string,float>::iterator itr=mymap.begin(); itr!=mymap.end();itr++)
    {
        temp=temp+itr->second;
        count++;
    }
    return temp/count;
}

vector <Stock> GetRandomStocks(int l, vector <Stock> total_stocks_, vector <int> & random_number)
{
    vector <Stock> temp;
    srand((int)time(NULL));
    //srand(1);
    random_number.push_back(rand()%SP500_NUM_OF_STOCKS);
    for (int j=1;j<l;j++)
    {
        int a=rand()%SP500_NUM_OF_STOCKS;
        bool flag=true;
        while (flag)
        {
            if (std::find(random_number.begin(), random_number.end(), a)!=random_number.end())
            {
                a=rand()%SP500_NUM_OF_STOCKS;
            }
            else
                flag=false;
            
        }
        random_number.push_back(a);
    }
    for (int i=0; i<random_number.size(); i++)
    {
        temp.push_back(total_stocks_[random_number[i]]);
    }
    return temp;
}

void checkduplicate(vector <Stock> &s, vector <Stock> total_stocks_)
{
    bool flag=true;
    while(flag)
    {
        flag=false;
        std::sort(s.begin(), s.end(),less_than_symbol());
        for (int i=1; i<s.size(); i++)
        {
            if (s[i].getsymbol()==s[i-1].getsymbol())
            {
                vector <int> random_number_;
                vector <Stock> add_in = GetRandomStocks(1, total_stocks_, random_number_);
                s.reserve(s.size()+add_in.size()-1);
                s.erase(s.begin()+i-1);
                s.insert(s.end(), add_in.begin(), add_in.end());
                flag=true;
            }
        }
    }
}

float calculatemothly(map<string, float> weekly)
{
    float sum=0.0;
    for (map<string, float> ::iterator itr=weekly.begin(); itr!=weekly.end(); itr++)
    {
        sum=sum+itr->second;
    }
    return sum;
}
