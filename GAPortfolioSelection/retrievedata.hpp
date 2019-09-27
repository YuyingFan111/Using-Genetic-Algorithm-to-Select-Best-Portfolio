//
//  retrievedata.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 6/29/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef retrievedata_hpp
#define retrievedata_hpp

#include <iostream>
using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
int RetrieveMarketData(string url_request, Json::Value & root);

#endif /* retrievedata_hpp */
