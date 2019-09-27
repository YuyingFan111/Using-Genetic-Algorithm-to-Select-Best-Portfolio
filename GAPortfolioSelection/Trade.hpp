//
//  Trade.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 7/11/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Trade_hpp
#define Trade_hpp

#include <iostream>
using namespace std;

class Trade
{
private:
    string date;
    float open;
    float high;
    float low;
    float close;
    float adjusted_close;
    int volume;
public:
    Trade(string date_, float open_, float high_, float low_, float close_, float adjusted_close_, int volume_) :
    date(date_), open(open_), high(high_), low(low_), close(close_), adjusted_close(adjusted_close_), volume(volume_)
    {}
    ~Trade() {}
    float Getprice()
    {
        return adjusted_close;
    }
    string Getdate()
    {
        return date;
    }
    friend ostream & operator << (ostream & out, const Trade & t)
    {
        out << "Date: " << t.date << " Open: " << t.open << " High: " << t.high << " Low: " << t.low << " Close: " << t.close << " Adjusted_Close: " << t.adjusted_close << " Volume: " << t.volume << endl;
        return out;
    }
};


#endif /* Trade_hpp */
