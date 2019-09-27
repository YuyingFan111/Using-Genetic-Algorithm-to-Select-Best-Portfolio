//
//  Fundamental.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Fundamental_hpp
#define Fundamental_hpp

#include <iostream>
using namespace std;

class Fundamental
{
private:
    float P_E_ratio;
    float Div_yield;
    float beta;
    float high_52;
    float low_52;
    float MA_50;
    float MA_200;
public:
    Fundamental(float P_E_ratio_, float Div_yield_, float beta_, float high_52_, float low_52_, float MA_50_, float MA_200_) : P_E_ratio(P_E_ratio_), Div_yield(Div_yield_), beta(beta_), high_52(high_52_), low_52(low_52_), MA_50(MA_50_), MA_200(MA_200_) {}
    Fundamental() {}
    ~Fundamental() {}
    float get_pe()
    {
        return P_E_ratio;
    }
    float get_beta()
    {
        return beta;
    }
    float get_div_yield()
    {
        return Div_yield;
    }
    float get_ma()
    {
        return MA_50;
    }
    float get_ma200()
    {
        return MA_200;
    }
    friend ostream & operator << (ostream & out, const Fundamental & f)
    {
        out << "P/E Ratio:" << f.P_E_ratio << " Dividend Yield:" << f.Div_yield << " Beta:" << f.beta << " High 52 Weeks:" << f.high_52 << " Low 52 Weeks:" << f.low_52 << " MA 50 Days:" << f.MA_50 << " MA 200 Days:" << f.MA_200 <<endl;
        return out;
    }
};

#endif /* Fundamental_hpp */
