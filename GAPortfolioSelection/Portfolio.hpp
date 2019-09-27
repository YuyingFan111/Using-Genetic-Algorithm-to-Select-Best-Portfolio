//
//  Portfolio.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Portfolio_hpp
#define Portfolio_hpp

#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include "Fundamental.hpp"
#include "Trade.hpp"
#include "Stock.hpp"
#include "Utilities.hpp"
using namespace std;

class Portfolio
{
public:
    vector <Stock> stocks;
    vector <float> weights;
    map<string,float> p_returns;
    float average_return;
    float p_sigma;
    float Sharpe;
    float p_pe;
    float p_beta;
    float p_div;
    float ma_50;
    float ma_200;
    float fitness;
public:
    Portfolio() {}
    Portfolio(vector <Stock> stocks_): stocks(stocks_)
    {}
    ~ Portfolio() {}
    vector <float> getweights()
    {
        return weights;
    }
    vector <Stock> getstocks()
    {
        return stocks;
    }
    void setweights(vector<int> random_number_, vector <float> ori_weights_)
    {
        vector <float> temp;
        for (int i=0; i<random_number_.size(); i++)
        {
            temp.push_back(ori_weights_[random_number_[i]]);
        }
        float total=0.0;
        for (int j=0; j<temp.size(); j++)
        {
            total=total+temp[j];
        }
        weights.clear();
        for (int m=0; m<temp.size(); m++)
        {
            weights.push_back(temp[m]/total);
        }
    }
    void calculateweights(vector <Stock> stocks_)
    {
        float sum=0.0;
        for (int i=0; i<stocks_.size(); i++)
        {
            sum=sum+stocks_[i].getprop();
        }
        weights.clear();
        for (int j=0; j<stocks_.size(); j++)
        {
            weights.push_back(stocks_[j].getprop()/sum);
        }
    }
    void Assignfitness()
    {
        set_portfolio_returns();
        set_portfolio_sigma();
        set_P_E();
        set_sharpe();
        set_p_beta();
        set_p_div();
        set_ma();
        float pe_score=0.0;
        if (p_pe<30.0&&p_pe>10)
            pe_score=5;
        else if (p_pe>=30)
            pe_score=-5;
        //fitness=100.0*(0.6*Sharpe+0.2*p_beta+0.2*p_div);
        //fitness=60*Sharpe+40*p_beta+20*p_div;
        //better: fitness=60*Sharpe+20*p_beta+30*p_div+8*pe_score
        //fitness=60*Sharpe+20*p_beta+30*p_div+8*pe_score+6*ma_50/ma_200;
        fitness=60*Sharpe+30*p_beta+30*p_div+8*pe_score+10*ma_50/ma_200;
        cout<<"fitness:"<<fitness<<endl;
    }
    void set_sharpe()
    {
        Map daily_rf_map;
        daily_rf_map.clear();
        for (Map::iterator itr=stocks[0].risk_free_rate.begin(); itr!=stocks[0].risk_free_rate.end(); itr++)
        {
            daily_rf_map[itr->first]=pow(1+itr->second, 1/252.0)-1;
        }
        Map excess_return=(p_returns-daily_rf_map);
        Map sharpe_ratio_map=excess_return/(p_sigma/sqrt(252));
        Sharpe= calculate_average(sharpe_ratio_map);
        cout<<"Sharpe:"<<Sharpe<<endl;
    }
    void set_P_E()
    {
        p_pe=0.0;
        for (int i=0; i<stocks.size(); i++)
        {
            p_pe=p_pe+stocks[i].getfund().get_pe()*weights[i];
        }
        cout<<"P_E:"<<p_pe<<endl;
    }
    void set_p_beta()
    {
        p_beta=0.0;
        for (int i=0; i<stocks.size(); i++)
        {
            p_beta=p_beta+stocks[i].getfund().get_beta()*weights[i];
        }
        cout<<"beta:"<<p_beta<<endl;
    }
    void set_p_div()
    {
        p_div=0.0;
        for (int i=0; i<stocks.size(); i++)
        {
            p_div=p_div+stocks[i].getfund().get_div_yield()*weights[i];
        }
        cout<<"div:"<<p_div<<endl;
    }
    void set_ma()
    {
        ma_50=0.0;
        ma_200=0.0;
        for (int i=0; i<stocks.size(); i++)
        {
            ma_50=ma_50+stocks[i].getfund().get_ma()*weights[i];
            ma_200=ma_200+stocks[i].getfund().get_ma200()*weights[i];
        }
        cout<<"MA:"<<ma_50<<endl;
        cout<<"MA200:"<<ma_200<<endl;
    }
    void set_portfolio_returns()
    {
        p_returns.clear();
        for (int i=0; i<stocks.size(); i++)
        {
            map<string,float> individual_returns=stocks[i].get_returns();
            for (map<string,float>::iterator itr=individual_returns.begin(); itr!=individual_returns.end();itr++)
            {
                p_returns[itr->first]=p_returns[itr->first]+itr->second*weights[i];
            }
        }
        average_return=calculate_average(p_returns);
    }
    void set_portfolio_sigma()
    {
        float average=calculate_average(p_returns);
        map<string,float> dif2;
        for (map<string,float>::iterator itr=p_returns.begin(); itr!=p_returns.end();itr++)
        {
            dif2[itr->first]=(itr->second-average)*(itr->second-average);
        }
        p_sigma=sqrt(calculate_average(dif2));
        cout<<"sigma:"<<p_sigma<<endl;
    }
    float calculateweekreturn(string startday, string endday)
    {
        float weeklyreturn;
        float sum1=0.0;
        float sum2=0.0;
        float temp1=0.0;
        float temp2=0.0;
        for (int i=0; i<stocks.size(); i++)
        {
            for (int j = 0;j<stocks[i].getTrade().size(); j++)
            {
                if (stocks[i].getTrade()[j].Getdate()==startday)
                {
                    temp1=stocks[i].getTrade()[j].Getprice();
                    sum1=sum1+temp1*weights[i];
                }
                if(stocks[i].getTrade()[j].Getdate()==endday)
                {
                    temp2=stocks[i].getTrade()[j].Getprice();
                    sum2=sum2+temp2*weights[i];
                }
            }
        }
        weeklyreturn=(sum2-sum1)/sum1;
        return weeklyreturn;
    }
    friend ostream & operator << (ostream & out, Portfolio & p)
    {
        for (vector <Stock>::iterator itr = p.stocks.begin(); itr != p.stocks.end(); itr++)
            out<< "Symbols: "<<itr->getsymbol()<<"  ";
        for (vector <float>::iterator itr2 = p.weights.begin(); itr2!= p.weights.end(); itr2++)
            out<< "Weights: "<<*itr2<<"  ";
        cout<<endl;
        return out;
    }
};


#endif /* Portfolio_hpp */
