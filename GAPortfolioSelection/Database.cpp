//
//  Database.cpp
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
#include "Database.hpp"


using namespace std;

int OpenDatabase(const char * name, sqlite3 * & db)
{
    int rc = 0;
    //char *error = NULL;
    // Open Database
    cout << "Opening database: " << name << endl;
    rc = sqlite3_open(name, &db);
    if (rc)
    {
        cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        system("pause");
        return -1;
    }
    cout << "Opened database: " << name << endl;
    return 0;
    
}

void CloseDatabase(sqlite3 *db)
{
    cout << "Closing a database ..." << endl;
    sqlite3_close(db);
    cout << "Closed a database" << endl << endl;
}

int DropTable(const char * sql_drop_table, sqlite3 *db)
{
    // Drop the table if exists
    if (sqlite3_exec(db, sql_drop_table, 0, 0, 0) != SQLITE_OK) { // or == -- same effect
        std::cout << "SQLite can't drop sessions table" << std::endl;
        sqlite3_close(db);
        system("pause");
        return -1;
    }
    return 0;
}

int CreateTable(const char *sql_create_table, sqlite3 *db)
{
    int rc = 0;
    char *error = NULL;
    // Create the table
    cout << "Creating a table..." << endl;
    rc = sqlite3_exec(db, sql_create_table, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    cout << "Created a table." << endl;
    return 0;
}

int InsertTable(const char *sql_insert, sqlite3 *db)
{
    int rc = 0;
    char *error = NULL;
    // Execute SQL
    cout << "Inserting a value into a table ..." << endl;
    rc = sqlite3_exec(db, sql_insert, NULL, NULL, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    cout << "Inserted a value into the table." << endl;
    return 0;
}

int DisplayTable(const char *sql_select, sqlite3 *db)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
    {
        for (int colCtr = 0; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
        }
        
        // End Line
        cout << endl;
        
        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}

int SelectValue(const char *sql_select, sqlite3 *db, vector<float> &fundinfo, int count)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = count; rowCtr < count+1; ++rowCtr)
    {
        for (int colCtr = 1; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
            float output = atof(results[cellPosition]);
            fundinfo.push_back(output);
        }
        
        // End Line
        cout << endl;
        
        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}


int SelectTrade(const char *sql_select, sqlite3 *db, vector <vector <string> > &priceinfo)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
    {
        vector<string> temp_price;
        for (int colCtr = 2; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            string keai = string(results[cellPosition]);
            temp_price.push_back(keai);
            
        }
        
        priceinfo.push_back(temp_price);
        
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}

int SelectRiskFree(const char *sql_select, sqlite3 *db, vector <vector <string> > &priceinfo)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
    {
        vector<string> temp_price;
        for (int colCtr = 1; colCtr < columns; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            string keai = string(results[cellPosition]);
            temp_price.push_back(keai);
            
        }
        
        priceinfo.push_back(temp_price);
        
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}

int GetWeights(const char *sql_select, sqlite3 *db, vector<float> &weights)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
    {
        for (int colCtr = 4; colCtr < 5; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
            string output = string(results[cellPosition]);
            float output2 = (float)stof(output);
            weights.push_back(output2);
        }
        
        // End Line
        cout << endl;
        
        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}


int GetSymbols(const char *sql_select, sqlite3 *db, vector<string> &symbols)
{
    int rc = 0;
    char *error = NULL;
    
    // Display MyTable
    cout << "Retrieving values in a table ..." << endl;
    char **results = NULL;
    int rows, columns;
    // A result table is memory data structure created by the sqlite3_get_table() interface.
    // A result table records the complete query results from one or more queries.
    sqlite3_get_table(db, sql_select, &results, &rows, &columns, &error);
    if (rc)
    {
        cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
        sqlite3_free(error);
        system("pause");
        return -1;
    }
    
    // Display Table
    for (int rowCtr = 1; rowCtr <= rows; ++rowCtr)
    {
        for (int colCtr = 1; colCtr < 2; ++colCtr)
        {
            // Determine Cell Position
            int cellPosition = (rowCtr * columns) + colCtr;
            
            // Display Cell Value
            cout.width(12);
            cout.setf(ios::left);
            cout << results[cellPosition] << " ";
            string output = string(results[cellPosition]);
            symbols.push_back(output);
        }
        
        // End Line
        cout << endl;
        
        // Display Separator For Header
        if (0 == rowCtr)
        {
            for (int colCtr = 0; colCtr < columns; ++colCtr)
            {
                cout.width(12);
                cout.setf(ios::left);
                cout << "~~~~~~~~~~~~ ";
            }
            cout << endl;
        }
    }
    // This function properly releases the value array returned by sqlite3_get_table()
    sqlite3_free_table(results);
    return 0;
}

int PopulateFundamentalTable(const Json::Value & root, string symbol, sqlite3 *db)
{
    float P_E_ratio = 0, Div_yield = 0, beta = 0, high_52 = 0, low_52 = 0, MA_50 = 0, MA_200 = 0;
    Stock myStock(symbol);
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        cout << *itr << endl;
        if (itr.key().asString() == "Highlights")
            for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
            {
                if (inner.key().asString() == "PERatio")
                {
                    if (*inner)
                        P_E_ratio = (float)atof(inner->asCString());
                }
                else if (inner.key().asString() == "DividendYield")
                {
                    if (*inner)
                        Div_yield = (float)atof(inner->asCString());
                }
            }
        else if(itr.key().asString() == "Technicals")
            for (Json::Value::const_iterator inner2 = (*itr).begin(); inner2 != (*itr).end(); inner2++)
            {
                if (inner2.key().asString() == "Beta")
                {
                    if (*inner2)
                        beta = (float)atof(inner2->asCString());
                }
                else if (inner2.key().asString() == "52WeekHigh")
                {
                    if (*inner2)
                        high_52 = (float)atof(inner2->asCString());
                }
                else if (inner2.key().asString() == "52WeekLow")
                {
                    if (*inner2)
                        low_52 = (float)atof(inner2->asCString());
                }
                else if (inner2.key().asString() == "50DayMA")
                {
                    if (*inner2)
                        MA_50 = (float)atof(inner2->asCString());
                }
                else if (inner2.key().asString() == "200DayMA")
                {
                    if (*inner2)
                        MA_200 = (float)atof(inner2->asCString());
                }
            }
    }
    Fundamental aFund(P_E_ratio, Div_yield, beta, high_52, low_52, MA_50, MA_200);
    myStock.setfund(aFund);
    
    // Execute SQL
    //P_E_ratio = 0, Div_yield = 0, beta = 0, high_52 = 0, low_52 = 0, MA_50 = 0, MA_200 = 0;
    char fundamentals_insert_table[512];
    sprintf(fundamentals_insert_table, "INSERT INTO FUNDAMENTALS (symbol, P_E_ratio, Div_yield, beta, high_52, low_52, MA_50, MA_200) VALUES(\"%s\", %f, %f, %f, %f, %f, %f, %f)", symbol.c_str(), P_E_ratio, Div_yield, beta, high_52, low_52, MA_50, MA_200);
    if (InsertTable(fundamentals_insert_table, db) == -1)
        return -1;
    
    cout << myStock << endl;
    return 0;
}

int PopulateRiskFreeTable(const Json::Value & root, sqlite3 *db)
{
    string date;
    float open = 0, high = 0, low = 0, close = 0, adjusted_close = 0;
    int volume = 0;
    int count = 0;
    
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        cout << *itr << endl;
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;
            
            if (inner.key().asString() == "adjusted_close")
                adjusted_close = inner->asFloat();
            else if (inner.key().asString() == "close")
                close = inner->asFloat();
            else if (inner.key() == "date")
                date = inner->asString();
            else if (inner.key().asString() == "high")
                high = inner->asFloat();
            else if (inner.key().asString() == "low")
                low = inner->asFloat();
            else if (inner.key() == "open")
                open = inner->asFloat();
            else if (inner.key().asString() == "volume")
                volume = inner->asInt();
            else
            {
                cout << "Invalid json field" << endl;
                return -1;
            }
        }
        count++;
        
        // Execute SQL
        char stockDB_insert_table[512];
        sprintf(stockDB_insert_table, "INSERT INTO RISKFREE (id, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", %f, %f, %f, %f, %f, %d)", count, date.c_str(), open, high, low, close, adjusted_close, volume);
        if (InsertTable(stockDB_insert_table, db) == -1)
            return -1;
    }
    return 0;
}

int PopulateStockTable(const Json::Value & root, string symbol, sqlite3 *db)
{
    string date;
    float open = 0, high = 0, low = 0, close = 0, adjusted_close = 0;
    unsigned int volume = 0;
    Stock myStock(symbol);
    int count = 0;
    
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        cout << *itr << endl;
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;
            
            if (inner.key().asString() == "adjusted_close")
                adjusted_close = inner->asFloat();
            else if (inner.key().asString() == "close")
                close = inner->asFloat();
            else if (inner.key() == "date")
                date = inner->asString();
            else if (inner.key().asString() == "high")
                high = inner->asFloat();
            else if (inner.key().asString() == "low")
                low = inner->asFloat();
            else if (inner.key() == "open")
                open = inner->asFloat();
            else if (inner.key().asString() == "volume")
                volume = inner->asUInt();
            else
            {
                cout << "Invalid json field" << endl;
                return -1;
            }
        }
        Trade aTrade(date, open, high, low, close, adjusted_close, volume);
        myStock.addTrade(aTrade);
        count++;
        
        // Execute SQL
        char stockDB_insert_table[512];
        sprintf(stockDB_insert_table, "INSERT INTO STOCK_%s (id, symbol, date, open, high, low, close, adjusted_close, volume) VALUES(%d, \"%s\", \"%s\", %f, %f, %f, %f, %f, %d)", +symbol.c_str(), count, symbol.c_str(), date.c_str(), open, high, low, close, adjusted_close, volume);
        if (InsertTable(stockDB_insert_table, db) == -1)
            return -1;
    }
    cout << myStock;
    return 0;
}

int PopulateSP500Table(const Json::Value & root, sqlite3 *db, vector <string> &symbols)
{
    int count = 0;
    string name, symbol, sector;
    
    for (Json::Value::const_iterator itr = root.begin(); itr != root.end(); itr++)
    {
        cout << *itr << endl;
        for (Json::Value::const_iterator inner = (*itr).begin(); inner != (*itr).end(); inner++)
        {
            //cout << inner.key() << ": " << *inner << endl;
            
            if (inner.key().asString() == "Name")
                name = inner->asString();
            else if (inner.key().asString() == "Sector")
                sector = inner->asString();
            else if (inner.key() == "Symbol")
                symbol = inner->asString();
            else
            {
                cout << "Invalid json field" << endl;
                system("pause");
                return -1;
            }
        }
        symbols.push_back(symbol);
        count++;
        
        // Execute SQL
        char sp500_insert_table[512];
        sprintf(sp500_insert_table, "INSERT INTO SP500 (id, symbol, name, sector) VALUES(%d, \"%s\", \"%s\", \"%s\")", count, symbol.c_str(), name.c_str(), sector.c_str());
        if (InsertTable(sp500_insert_table, db) == -1)
            return -1;
    }
    return 0;
}

int PopulateNewSP500Table(sqlite3 *db, vector <string> &symbols, vector<string> &weights)
{
    fstream fin;
    fin.open("new_SP500.csv", ios::in);
    string title, name, symbol, weight, sector, shares;
    getline(fin, title);
    cout<<title<<endl;
    int count=0;
    while (fin.good())
    {
        getline(fin, name, ',');
        cout<<name<<endl;
        getline(fin, symbol, ',');
        cout<<symbol<<endl;
        getline(fin, weight, ',');
        cout<<weight<<endl;
        getline(fin, sector, ',');
        cout<<sector<<endl;
        getline(fin, shares);
        cout<<shares<<endl;
        count++;
        symbols.push_back(symbol);
        weights.push_back(weight);
        
        // Execute SQL
        char new_sp500_insert_table[512];
        sprintf(new_sp500_insert_table, "INSERT INTO NEWSP500 (id, symbol, name, sector, weight, shares) VALUES(%d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\")", count, symbol.c_str(), name.c_str(), sector.c_str(), weight.c_str(), shares.c_str());
        if (InsertTable(new_sp500_insert_table, db) == -1)
            return -1;
    }
    return 0;
}

