//
//  retrievedata.cpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 6/29/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <sqlite3.h>

#include "json/json.h"
#include "curl/curl.h"
#include "retrievedata.hpp"

using namespace std;

//writing call back function for storing fetched values in memory
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int RetrieveMarketData(string url_request, Json::Value & root)
{
    std::string readBuffer;
    
    //global initiliation of curl before calling a function
    curl_global_init(CURL_GLOBAL_ALL);
    
    //creating session handle
    CURL * myHandle;
    
    // Weíll store the result of CURLís webpage retrieval, for simple error checking.
    CURLcode result;
    
    // notice the lack of major error-checking, for brevity
    myHandle = curl_easy_init();
    
    curl_easy_setopt(myHandle, CURLOPT_URL, url_request.c_str());
    //curl_easy_setopt(myHandle, CURLOPT_URL, "https://eodhistoricaldata.com/api/eod/AAPL.US?from=2018-01-05&to=2019-02-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json");
    
    //adding a user agent
    curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows; U; Windows NT 6.1; rv:2.2) Gecko/20110201");
    curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(myHandle, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(myHandle, CURLOPT_VERBOSE, 1);
    
    // send all data to this function
    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteCallback);
    
    // we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, &readBuffer);
    
    //perform a blocking file transfer
    result = curl_easy_perform(myHandle);
    
    // check for errors
    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
    }
    else {
        cout << readBuffer << endl;
        //
        // Now, our chunk.memory points to a memory block that is chunk.size
        // bytes big and contains the remote file.
        //
        // Do something nice with it!
        //
        
        // https://github.com/open-source-parsers/jsoncpp
        // Using JsonCpp in your project
        // Amalgamated source
        // https ://github.com/open-source-parsers/jsoncpp/wiki/Amalgamated
        
        //json parsing
        Json::CharReaderBuilder builder;
        Json::CharReader * reader = builder.newCharReader();
        string errors;
        
        bool parsingSuccessful = reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.size(), &root, &errors);
        if (not parsingSuccessful)
        {
            // Report failures and their locations
            // in the document.
            cout << "Failed to parse JSON" << std::endl
            << readBuffer
            << errors << endl;
            system("pause");
            return -1;
        }
        std::cout << "\nSucess parsing json\n" << root << endl;
        
    }
    
    //End a libcurl easy handle.This function must be the last function to call for an easy session
    curl_easy_cleanup(myHandle);
    return 0;
}

