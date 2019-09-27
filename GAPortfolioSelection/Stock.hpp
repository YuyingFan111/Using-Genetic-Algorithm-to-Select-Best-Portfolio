//
//  Stock.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Stock_hpp
#define Stock_hpp

#include <iostream>
#include <vector>
#include <map>
#include "Fundamental.hpp"
#include "Trade.hpp"
using namespace std;

class Stock
{
private:
    string symbol;
    float prop;
    vector<Trade> trades;
    Fundamental fund;
    map<string,float> daily_returns;
    
public:
    static map<string,float> risk_free_rate;
    Stock(string symbol_) :symbol(symbol_)
    {}
    ~Stock() {}
    void setprop(float prop_)
    {
        prop=prop_;
    }
    float getprop()
    {
        return prop;
    }
    void setfund(const Fundamental & fund_)
    {
        fund=fund_;
    }
    Fundamental getfund()
    {
        //cout<<fund<<endl;
        return fund;
    }
    string getsymbol()
    {
        return symbol;
    }
    void addTrade(Trade aTrade)
    {
        trades.push_back(aTrade);
    }
    vector <Trade> getTrade()
    {
        return trades;
    }
    void calculate_returns ()
    {
        daily_returns.clear();
        for (vector<Trade>::iterator itr = trades.begin()+1; itr != trades.end(); itr++)
        {
            float temp = (*(itr)).Getprice()/(*(itr-1)).Getprice()-1;
            daily_returns[(*itr).Getdate()]=temp;
        }
    }
    map<string,float> get_returns()
    {
        return daily_returns;
    }
    float calculateweekly(string startdate, string enddate)
    {
        float temp1=0.0;
        float temp2=0.0;
        for (int j = 0;j<trades.size(); j++)
        {
            if (trades[j].Getdate()==startdate)
            {
                temp1=trades[j].Getprice();
            }
            if(trades[j].Getdate()==enddate)
            {
                temp2=trades[j].Getprice();
            }
        }
        return (temp2-temp1)/temp1;
    }
    friend ostream & operator << (ostream & out, const Stock & s)
    {
        out << "Symbol: " << s.symbol << endl;
        out << "Fundamental data: " << endl << s.fund << endl;
        for (vector<Trade>::const_iterator itr = s.trades.begin(); itr != s.trades.end(); itr++)
            out << *itr;
        return out;
    }
};




#endif /* Stock_hpp */
