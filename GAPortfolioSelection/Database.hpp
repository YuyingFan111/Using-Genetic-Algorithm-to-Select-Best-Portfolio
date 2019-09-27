//
//  Database.hpp
//  GAPortfolioSelection
//
//  Created by YUYING FAN on 6/29/19.
//  Copyright © 2019 NYU. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <iostream>
#include "Stock.hpp"
using namespace std;

int OpenDatabase(const char * name, sqlite3 * & db);
void CloseDatabase(sqlite3 *db);
int DropTable(const char * sql_drop_table, sqlite3 *db);
int CreateTable(const char *sql_create_table, sqlite3 *db);
int InsertTable(const char *sql_insert, sqlite3 *db);
int DisplayTable(const char *sql_select, sqlite3 *db);
int SelectValue(const char *sql_select, sqlite3 *db, vector<float> &fundinfo, int count);
int SelectTrade(const char *sql_select, sqlite3 *db, vector <vector <string> > &priceinfo);
int SelectRiskFree(const char *sql_select, sqlite3 *db, vector <vector <string> > &priceinfo);
int GetWeights(const char *sql_select, sqlite3 *db, vector<float> &weights);
int GetSymbols(const char *sql_select, sqlite3 *db, vector<string> &symbols);
int PopulateFundamentalTable(const Json::Value & root, string symbol, sqlite3 *db);
int PopulateRiskFreeTable(const Json::Value & root, sqlite3 *db);
int PopulateStockTable(const Json::Value & root, string symbol, sqlite3 *db);
int PopulateSP500Table(const Json::Value & root, sqlite3 *db, vector <string> &symbols);
int PopulateNewSP500Table(sqlite3 *db, vector <string> &symbols, vector<string> &weights);


#endif /* Database_hpp */
