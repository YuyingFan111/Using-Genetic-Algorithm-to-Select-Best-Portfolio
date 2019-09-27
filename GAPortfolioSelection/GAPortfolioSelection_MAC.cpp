#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <sqlite3.h>
#include <math.h>

#include "json/json.h"
#include "curl/curl.h"
#include "Database.hpp"
#include "retrievedata.hpp"
#include "Fundamental.hpp"
#include "Trade.hpp"
#include "Stock.hpp"
#include "Utilities.hpp"
#include "Portfolio.hpp"
#include "GA.hpp"

using namespace std;



int risk_frees(sqlite3 *db, string stock_select_, Map &risk_free_rate_)
{
    vector <vector <string>> temp;
    if (SelectRiskFree(stock_select_.c_str(), db, temp) == -1)
        return -1;
    for (int i=0; i<temp.size(); i++)
    {
        string date_=temp[i][0];
        float adjusted_close_=(float)stof(temp[i][5]);
        risk_free_rate_[date_]=adjusted_close_;
    }
    return 0;
}

int stock_prices(sqlite3 *db, string stock_select_, Stock & aStock_)
{
    vector <vector <string>> temp;
    if (SelectTrade(stock_select_.c_str(), db, temp) == -1)
        return -1;
    for (int i=0; i<temp.size(); i++)
    {
        string date_=temp[i][0];
        float open_=(float)stof(temp[i][1]);
        float high_=(float)stof(temp[i][2]);
        float low_=(float)stof(temp[i][3]);
        float close_=(float)stof(temp[i][4]);
        float adjusted_close_=(float)stof(temp[i][5]);
        int volume_=stoi(temp[i][6]);
        
        Trade aTrade(date_,open_,high_,low_,close_,adjusted_close_,volume_);
        aStock_.addTrade(aTrade);
    }
    return 0;
}

int stock_fundamentals(sqlite3 *db, string fundamental_select_, Stock & aStock_, int count)
{
    vector<float> temp;
    if (SelectValue(fundamental_select_.c_str(), db, temp, count) == -1)
        return -1;
    Fundamental fund(temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6]);
    aStock_.setfund(fund);
    return 0;
}

float calculateSPYnum(string startdate, Stock SPYstock)
{
    float num=0.1;
    for (vector<Trade>::iterator itr = SPYstock.getTrade().begin(); itr != SPYstock.getTrade().end(); itr++)
    {
        if ((*itr).Getdate()==startdate)
        {
            num=1000000/(*itr).Getprice();
        }
    }
    return num;
}

int main(void)

{
    char answer;
    bool continue_menu = true;
    
    cout<<"**********************************************************\n";
    cout<<"1 - Store SP500 and stocks price data into database.\n";
    cout<<"2 - Store fundamental data into database.\n";
    cout<<"3 - Store risk free rate into database.\n";
    cout<<"4 - GA selection.\n";
    cout<<"5 - get the weights from csv.\n";
    cout<<"6 - SPY returns for 2019. \n";
    cout<<"7 - backtesting for best portfolio 2019. \n";
    cout<<"8 - prob testing. \n";
    cout<<"0 - Exit the program.\n";
    cout<<"Enter your choice and press return: ";
    
    while(continue_menu != false)
    {
        cin >> answer;
        switch (answer)
        {
            case '1':
            {
                cout<<"1 - Store SP500 and stocks price data into database.\n";
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                
                std::string new_sp500_drop_table = "DROP TABLE IF EXISTS NEWSP500;";
                if (DropTable(new_sp500_drop_table.c_str(), stockDB) == -1)
                    return -1;
                
                string new_sp500_create_table = "CREATE TABLE NEWSP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL, weight CHAR(20) NOT NULL, shares CHAR(20) NOT NULL);";
                
                if (CreateTable(new_sp500_create_table.c_str(), stockDB) == -1)
                    return -1;
                
                vector <string> new_symbols;
                vector <string> weights;
                if (PopulateNewSP500Table(stockDB, new_symbols, weights) == -1)
                    return -1;
                
                string new_sp500_select_table = "SELECT * FROM NEWSP500;";
                if (DisplayTable(new_sp500_select_table.c_str(), stockDB) == -1)
                    return -1;
                
                new_symbols.push_back("SPY");
                
                for (int i=0; i<new_symbols.size(); i++)
                {
                    string stockDB_symbol = new_symbols[i];
                    if (stockDB_symbol=="BRK.B")
                        stockDB_symbol="BRK_B";
                    if (stockDB_symbol=="BF.B")
                        stockDB_symbol="BF_B";
                    if (stockDB_symbol=="LUK"||stockDB_symbol=="MON"||stockDB_symbol=="MSI")
                        continue;
                    if (stockDB_symbol=="CASH_USD")
                        stockDB_symbol="TMUS";
                    
                    std::string stockDB_drop_table = "DROP TABLE IF EXISTS STOCK_" + stockDB_symbol + ";";
                    if (DropTable(stockDB_drop_table.c_str(), stockDB) == -1)
                        return -1;
                    
                    string stockDB_create_table = "CREATE TABLE STOCK_" + stockDB_symbol
                    + "(id INT PRIMARY KEY NOT NULL,"
                    + "symbol CHAR(20) NOT NULL,"
                    + "date CHAR(20) NOT NULL,"
                    + "open REAL NOT NULL,"
                    + "high REAL NOT NULL,"
                    + "low REAL NOT NULL,"
                    + "close REAL NOT NULL,"
                    + "adjusted_close REAL NOT NULL,"
                    + "volume INT NOT NULL);";
                    
                    if (CreateTable(stockDB_create_table.c_str(), stockDB) == -1)
                        return -1;
                    
                    if (stockDB_symbol=="BRK_B")
                        stockDB_symbol="BRK-B";
                    if (stockDB_symbol=="BF_B")
                        stockDB_symbol="BF-B";
                    //https://eodhistoricaldata.com/api/eod/AGN.US?from=2019-07-01&to=2019-07-31&api_token=5ba84ea974ab42.45160048&period=d&fmt=json
                    string stock_url_common = "https://eodhistoricaldata.com/api/eod/";
                    string stock_start_date = "2008-01-01";
                    string stock_end_date = "2019-07-31";
                    string api_token = "5ba84ea974ab42.45160048";
                    string stockDB_data_request = stock_url_common + stockDB_symbol + ".US?" +
                    "from=" + stock_start_date + "&to=" + stock_end_date + "&api_token=" + api_token + "&period=d&fmt=json";
                    
                    Json::Value stockDB_root;   // will contains the root value after parsing.
                    if (RetrieveMarketData(stockDB_data_request, stockDB_root) == -1)
                        return -1;
                    if (stockDB_symbol=="BRK-B")
                        stockDB_symbol="BRK_B";
                    if (stockDB_symbol=="BF-B")
                        stockDB_symbol="BF_B";
                    if (PopulateStockTable(stockDB_root, stockDB_symbol, stockDB) == -1)
                        return -1;
                    
                    string stockDB_select_table = "SELECT * FROM STOCK_" + stockDB_symbol + ";";
                    if (DisplayTable(stockDB_select_table.c_str(), stockDB) == -1)
                        return -1;
                }
                
                // Close Database
                CloseDatabase(stockDB);
                break;
            }
            case '2':
            {
                cout<<"2 - Store fundamental data into database.\n";
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                
                std::string new_sp500_drop_table = "DROP TABLE IF EXISTS NEWSP500;";
                if (DropTable(new_sp500_drop_table.c_str(), stockDB) == -1)
                    return -1;
                
                string new_sp500_create_table = "CREATE TABLE NEWSP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL, weight CHAR(20) NOT NULL, shares CHAR(20) NOT NULL);";
                
                if (CreateTable(new_sp500_create_table.c_str(), stockDB) == -1)
                    return -1;
                
                vector <string> new_symbols;
                vector <string> weights;
                if (PopulateNewSP500Table(stockDB, new_symbols, weights) == -1)
                    return -1;
                
                string new_sp500_select_table = "SELECT * FROM NEWSP500;";
                if (DisplayTable(new_sp500_select_table.c_str(), stockDB) == -1)
                    return -1;
                
                new_symbols.push_back("SPY");
                
                std::string fundamentals_drop_table = "DROP TABLE IF EXISTS FUNDAMENTALS;";
                if (DropTable(fundamentals_drop_table.c_str(), stockDB) == -1)
                    return -1;
                
                string fundamentals_create_table = "CREATE TABLE FUNDAMENTALS (symbol CHAR(20) NOT NULL, P_E_ratio REAL NOT NULL, Div_yield REAL NOT NULL, beta REAL NOT NULL, high_52 REAL NOT NULL, low_52 REAL NOT NULL, MA_50 REAL NOT NULL, MA_200 REAL NOT NULL);";
                
                if (CreateTable(fundamentals_create_table.c_str(), stockDB) == -1)
                    return -1;
                
                for (int i=0; i<new_symbols.size(); i++)
                {
                    string fundamentals_symbol = new_symbols[i];
                    if (fundamentals_symbol=="BRK.B")
                        fundamentals_symbol="BRK_B";
                    if (fundamentals_symbol=="BF.B")
                        fundamentals_symbol="BF_B";
                    
                    std::string fundamentals_drop_table = "DROP TABLE IF EXISTS FUNDAMENTALS_" + fundamentals_symbol + ";";
                    if (DropTable(fundamentals_drop_table.c_str(), stockDB) == -1)
                        return -1;
                    
                    if (fundamentals_symbol=="BRK_B")
                        fundamentals_symbol="BRK-B";
                    if (fundamentals_symbol=="BF_B")
                        fundamentals_symbol="BF-B";
                    
                    //https://eodhistoricaldata.com/api/fundamentals/AAPL.US?api_token=5ba84ea974ab42.45160048
                    string fundamentals_url_common = "https://eodhistoricaldata.com/api/fundamentals/";
                    string api_token = "5ba84ea974ab42.45160048";
                    string fundamentals_data_request = fundamentals_url_common + fundamentals_symbol + ".US?" +
                    "api_token=" + api_token;
                    
                    Json::Value stock_fund_root;   // will contains the root value after parsing.
                    if (RetrieveMarketData(fundamentals_data_request, stock_fund_root) == -1)
                        return -1;
                    
                    if (fundamentals_symbol=="BRK-B")
                        fundamentals_symbol="BRK_B";
                    if (fundamentals_symbol=="BF-B")
                        fundamentals_symbol="BF_B";
                    
                    //int count = 0;
                    if (PopulateFundamentalTable(stock_fund_root, fundamentals_symbol, stockDB) == -1)
                        return -1;
                    
                }
                string fundamentals_select_table = "SELECT * FROM FUNDAMENTALS;";
                if (DisplayTable(fundamentals_select_table.c_str(), stockDB) == -1)
                    return -1;
                
                // Close Database
                CloseDatabase(stockDB);
                
                break;
            }
            case '3':
            {
                cout<<"3 - Store risk free rate into database.\n";
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                
                std::string RiskFree_drop_table = "DROP TABLE IF EXISTS RISKFREE;";
                if (DropTable(RiskFree_drop_table.c_str(), stockDB) == -1)
                    return -1;
                
                string RiskFree_create_table = "CREATE TABLE RISKFREE (id INT PRIMARY KEY NOT NULL, date CHAR(20) NOT NULL, open REAL NOT NULL, high REAL NOT NULL, low REAL NOT NULL, close REAL NOT NULL, adjusted_close REAL NOT NULL, volume INT NOT NULL);";
                
                if (CreateTable(RiskFree_create_table.c_str(), stockDB) == -1)
                    return -1;
                
                string RiskFree_data_request = "https://eodhistoricaldata.com/api/eod/TNX.INDX?from=2008-01-01&to=2019-07-10&api_token=5ba84ea974ab42.45160048&period=d&fmt=json";
                
                Json::Value sp500_root;   // will contains the root value after parsing.
                if (RetrieveMarketData(RiskFree_data_request, sp500_root) == -1)
                    return -1;
                if (PopulateRiskFreeTable(sp500_root, stockDB) == -1)
                    return -1;
                
                string RiskFree_select_table = "SELECT * FROM RISKFREE;";
                if (DisplayTable(RiskFree_select_table.c_str(), stockDB) == -1)
                    return -1;
                
                if (risk_frees(stockDB, RiskFree_select_table, Stock::risk_free_rate) == -1)
                    return -1;
                
                Stock astock("AAPL");
                for (Map::iterator itr=astock.risk_free_rate.begin();itr!=astock.risk_free_rate.end();itr++)
                {
                    cout<<itr->second<<endl;
                }
                break;
            }
            case '4':
            {
                cout<<"4 - GA selection.\n";
                vector <Stock> total_stocks;
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                vector <string> symbols;
                string a = "SELECT * FROM NEWSP500;";
                if (GetSymbols(a.c_str(), stockDB, symbols)==-1)
                    return -1;
                
                int count=1;
                cout<<symbols.size()<<endl;
                for (vector<string>::iterator itr=symbols.begin(); itr!=symbols.end(); itr++)
                {
                    if (*itr=="BRK.B")
                        *itr="BRK_B";
                    if (*itr=="BF.B")
                        *itr="BF_B";
                    if (*itr=="CASH_USD")
                        *itr="TMUS";
                    if (*itr=="DOW"||*itr=="CTVA"||*itr=="AMCR"||*itr=="FOXA"||*itr=="CPRI")
                    {
                        count++;
                        continue;
                    }
                    Stock aStock(*itr);
                    string c = "SELECT * FROM FUNDAMENTALS;";
                    if (stock_fundamentals(stockDB, c, aStock, count)==-1)
                        return -1;
                    string stock_select = "SELECT * FROM STOCK_" + *itr + " WHERE strftime(\'%Y-%m-%d\', date) between \"2008-01-01\" and \"2018-12-31\" and open > 0 and adjusted_close > 0;";
                    if (stock_prices(stockDB, stock_select, aStock)==-1)
                        return -1;
                    aStock.calculate_returns();
                    cout<<*itr<<"!!!!!!!!!!!!!!"<<endl;
                    total_stocks.push_back(aStock);
                    count++;
                }
                
                vector<float> ori_weights;
                string b = "SELECT * FROM NEWSP500;";
                if (GetWeights(b.c_str(), stockDB, ori_weights)==-1)
                    return -1;
                
                int te=0;
                for (int i=0; i<ori_weights.size(); i++)
                {
                    if (symbols[i]=="DOW"||symbols[i]=="CTVA"||symbols[i]=="AMCR"||symbols[i]=="FOXA"||symbols[i]=="CPRI")
                    {
                        te=te+1;
                        i=i+1;
                    }
                    else
                        total_stocks[i-te].setprop(ori_weights[i]);
                }
                
                CloseDatabase(stockDB);
                
                vector <Portfolio> Population(POP_SIZE);
                for (int i = 0; i<POP_SIZE; i++)
                {
                    //first generation
                    vector <int> random_number_;
                    Population[i].stocks = GetRandomStocks(10, total_stocks, random_number_);
                    Population[i].setweights(random_number_,ori_weights);
                    Population[i].fitness = 0.0f;
                    cout<<Population[i];
                }
                
                int GenerationsRequiredToFindASolution = 0;
                int countchange = 0;
                bool bFound = false;
                bool bChange;
                float MaxFitness = 0.0f;
                Portfolio BestP;
                
                //enter the main GA loop
                while (!bFound)  //find the fitness for the population by adding up
                {
                    //this is used during roulette wheel sampling
                    float TotalFitness = 0.0f;
                    bChange = false;
                    
                    // test and update the fitness of every chromosome in the population
                    for (int i = 0; i<POP_SIZE; i++)
                    {
                        Population[i].Assignfitness();
                        if (Population[i].fitness > MaxFitness)
                        {
                            MaxFitness = Population[i].fitness;
                            BestP = Population[i];
                            bChange = true;
                            countchange = 0;
                        }
                    }
                    
                    if (bChange == false)
                        countchange++;
                    
                    if (countchange == 10)
                    {
                        cout << "\nSolution found in " << GenerationsRequiredToFindASolution << " generations!" << endl;
                        cout << "Result: ";
                        cout<< BestP;
                        cout << "Fitness: "<<MaxFitness<<endl;
                        bFound = true;
                        break;
                    }
                    
                    //Sort all the chromosomes in a population in ascending order
                    std::sort(Population.begin(),Population.end(),less_than_fitness());
                    cout<<"sort completed!"<<"^^^^^^^^^"<<endl;
                    
                    //define some temporary storage for the new population we are about to create
                    vector<Portfolio> temp(POP_SIZE*CROSSOVER_RATE);
                    
                    int cPop = 0;
                    
                    vector <Portfolio> goodPopulation;
                    for (int i=0; i<POP_SIZE*CROSSOVER_RATE; i++)
                    {
                        goodPopulation.push_back(Population[POP_SIZE-i-1]);
                    }
                    for (int i=0; i<goodPopulation.size(); i++)
                    {
                        TotalFitness += goodPopulation[i].fitness;
                    }
                    cout<<"Prepared!"<<endl;
                    
                    //loop until we have created POP_SIZE new chromosomes
                    while (cPop < POP_SIZE*CROSSOVER_RATE)
                    {
                        // we are going to create the new population by grabbing members of the old population
                        // two at a time via roulette wheel selection.
                        Portfolio offspring1 = Roulette(TotalFitness, goodPopulation);
                        Portfolio offspring2 = Roulette(TotalFitness, goodPopulation);
                        
                        while (offspring1.average_return==offspring2.average_return)
                        {
                            offspring1 = Roulette(TotalFitness, goodPopulation);
                            offspring2 = Roulette(TotalFitness, goodPopulation);
                        }
                        
                        //make sure not duplicate
                        cout<<"will crossover!"<<endl;
                        
                        //add crossover dependent on the crossover rate
                        Crossover(offspring1, offspring2);
                        cout<<"Crossover!"<<endl;
                        
                        //now mutate dependent on the mutation rate
                        Mutate(offspring1, total_stocks);
                        Mutate(offspring2, total_stocks);
                        cout<<"Mutate!"<<endl;
                        
                        //add these offspring to the new population. (assigning zero as their
                        //fitness scores)
                        offspring1.fitness=0.0f;
                        offspring2.fitness=0.0f;
                        vector <Stock> o1(offspring1.getstocks());
                        vector <Stock> o2(offspring2.getstocks());
                        checkduplicate(o1, total_stocks);
                        checkduplicate(o2, total_stocks);
                        cout<<"change"<<endl;
                        offspring1=Portfolio(o1);
                        offspring2=Portfolio(o2);
                        offspring1.calculateweights(o1);
                        offspring2.calculateweights(o2);
                        temp[cPop++] = offspring1;
                        temp[cPop++] = offspring2;
                        
                    }//end loop
                    
                    //copy temp population into main population array
                    for (int i = 0; i<POP_SIZE*CROSSOVER_RATE; i++)
                    {
                        Population[i] = temp[i];  //use children to replace the old chromoson
                    }
                    
                    ++GenerationsRequiredToFindASolution;
                    cout<<"generation: "<<GenerationsRequiredToFindASolution<<endl;
                    
                    // exit app if no solution found within the maximum allowable number
                    // of generations
                    if (GenerationsRequiredToFindASolution > MAX_ALLOWABLE_GENERATIONS)  // if more than 400, quit
                    {
                        cout << "The best portfolio is:";
                        cout<<Population[99]<<endl;
                        cout<<"Max fitness:"<<MaxFitness<<endl;
                        
                        const char * stockDB_name = "Stocks.db";
                        sqlite3 * stockDB = NULL;
                        if (OpenDatabase(stockDB_name, stockDB) == -1)
                            return -1;
                        std::string portfolio_drop_table = "DROP TABLE IF EXISTS BestPortfolio;";
                        if (DropTable(portfolio_drop_table.c_str(), stockDB) == -1)
                            return -1;
                        
                        string portfolio_create_table = "CREATE TABLE BestPortfolio (id INT PRIMARY KEY NOT NULL, stockname CHAR(20) NOT NULL, weight REAL NOT NULL);";
                        
                        if (CreateTable(portfolio_create_table.c_str(), stockDB) == -1)
                            return -1;
                        int countstock=0;
                        for (int p=0; p<Population[99].getstocks().size(); p++)
                        {
                            countstock++;
                            char portfolio_insert_table[512];
                            sprintf(portfolio_insert_table, "INSERT INTO BestPortfolio (id, stockname, weight) VALUES(%d, \"%s\", %f)", countstock, Population[99].getstocks()[p].getsymbol().c_str(), Population[99].weights[p]);
                            if (InsertTable(portfolio_insert_table, stockDB) == -1)
                                return -1;
                        }
                        
                        bFound = true;
                    }
                }
                break;
            }
            case '5':
            {
                cout<<"5 - get the weights from csv.\n";
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                
                std::string new_sp500_drop_table = "DROP TABLE IF EXISTS NEWSP500;";
                if (DropTable(new_sp500_drop_table.c_str(), stockDB) == -1)
                    return -1;
                
                string new_sp500_create_table = "CREATE TABLE NEWSP500 (id INT PRIMARY KEY NOT NULL, symbol CHAR(20) NOT NULL, name CHAR(20) NOT NULL, sector CHAR(20) NOT NULL, weight CHAR(20) NOT NULL, shares CHAR(20) NOT NULL);";
                
                if (CreateTable(new_sp500_create_table.c_str(), stockDB) == -1)
                    return -1;
                
                vector <string> new_symbols;
                vector <string> weights;
                if (PopulateNewSP500Table(stockDB, new_symbols, weights) == -1)
                    return -1;
                
                string new_sp500_select_table = "SELECT * FROM NEWSP500;";
                if (DisplayTable(new_sp500_select_table.c_str(), stockDB) == -1)
                    return -1;
                break;
            }
            case '6':
            {
                cout<<"6 - SPY returns for 2019. \n";
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                Stock SPY("SPY");
                string c = "SELECT * FROM FUNDAMENTALS;";
                if (stock_fundamentals(stockDB, c, SPY, 506)==-1)
                    return -1;
                string stock_select = "SELECT * FROM STOCK_SPY WHERE strftime(\'%Y-%m-%d\', date) between \"2019-01-01\" and \"2019-07-31\" and open > 0 and adjusted_close > 0;";
                if (stock_prices(stockDB, stock_select, SPY)==-1)
                    return -1;
                SPY.calculate_returns();
                vector <string> starts={"2019-01-07", "2019-01-14", "2019-01-22", "2019-01-28"};
                vector <string> ends={"2019-01-11", "2019-01-18", "2019-01-25", "2019-02-01"};
                map<string, float> result3;
                for (int m=0; m<starts.size(); m++)
                {
                    result3[starts[m]]=SPY.calculateweekly(starts[m], ends[m]);
                    cout<<result3[starts[m]]<<endl;
                }
                cout<<"first month SPY:"<<calculatemothly(result3)<<endl;
                
                vector <string> starts2={"2019-02-04", "2019-02-11", "2019-02-19", "2019-02-25"};
                vector <string> ends2={"2019-02-08", "2019-02-15", "2019-02-22", "2019-03-01"};
                map<string, float> result4;
                for (int m=0; m<starts2.size(); m++)
                {
                    result4[starts2[m]]=SPY.calculateweekly(starts2[m], ends2[m]);
                    cout<<result4[starts2[m]]<<endl;
                }
                cout<<"second month SPY:"<<calculatemothly(result4)<<endl;
                
                vector <string> starts3={"2019-03-04", "2019-03-11", "2019-03-18", "2019-03-25"};
                vector <string> ends3={"2019-03-08", "2019-03-15", "2019-03-22", "2019-03-29"};
                map<string, float> result5;
                for (int m=0; m<starts3.size(); m++)
                {
                    result5[starts3[m]]=SPY.calculateweekly(starts3[m], ends3[m]);
                    cout<<result5[starts3[m]]<<endl;
                }
                cout<<"third month SPY:"<<calculatemothly(result5)<<endl;
                
                vector <string> starts4={"2019-04-01", "2019-04-08", "2019-04-15", "2019-04-22"};
                vector <string> ends4={"2019-04-05", "2019-04-12", "2019-04-18", "2019-04-26"};
                map<string, float> result6;
                for (int m=0; m<starts4.size(); m++)
                {
                    result6[starts4[m]]=SPY.calculateweekly(starts4[m], ends4[m]);
                    cout<<result6[starts4[m]]<<endl;
                }
                cout<<"fourth month SPY:"<<calculatemothly(result6)<<endl;
                
                vector <string> starts5={"2019-04-29", "2019-05-06", "2019-05-13", "2019-05-20", "2019-05-28"};
                vector <string> ends5={"2019-05-03", "2019-05-10", "2019-05-17", "2019-05-24", "2019-05-31"};
                map<string, float> result7;
                for (int m=0; m<starts5.size(); m++)
                {
                    result7[starts5[m]]=SPY.calculateweekly(starts5[m], ends5[m]);
                    cout<<result7[starts5[m]]<<endl;
                }
                cout<<"fifth month SPY:"<<calculatemothly(result7)<<endl;
                
                vector <string> starts6={"2019-06-03", "2019-06-10", "2019-06-17", "2019-06-24"};
                vector <string> ends6={"2019-06-07", "2019-06-14", "2019-06-21", "2019-06-28"};
                map<string, float> result8;
                for (int m=0; m<starts6.size(); m++)
                {
                    result8[starts6[m]]=SPY.calculateweekly(starts6[m], ends6[m]);
                    cout<<result8[starts6[m]]<<endl;
                }
                cout<<"sixth month SPY:"<<calculatemothly(result8)<<endl;
                
                cout<<"six months average SPY:"<<calculatemothly(result3)+calculatemothly(result4)+calculatemothly(result5)+calculatemothly(result6)+calculatemothly(result7)+calculatemothly(result8)<<endl;
                
                vector <string> starts7={"2019-07-01", "2019-07-08", "2019-07-15", "2019-07-22"};
                vector <string> ends7={"2019-07-05", "2019-07-12", "2019-07-19", "2019-07-26"};
                map<string, float> result9;
                for (int m=0; m<starts7.size(); m++)
                {
                    result9[starts7[m]]=SPY.calculateweekly(starts7[m], ends7[m]);
                    cout<<result9[starts7[m]]<<endl;
                }
                cout<<"senventh month SPY:"<<calculatemothly(result9)<<endl;
                break;
            }
            case '7':
            {
                cout<<"7 - backtesting for best portfolio 2019. \n";
                vector <Stock> total_stocks;
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                vector <string> symbols;
                string a = "SELECT * FROM NEWSP500;";
                if (GetSymbols(a.c_str(), stockDB, symbols)==-1)
                    return -1;
                
                int count=1;
                cout<<symbols.size()<<endl;
                for (vector<string>::iterator itr=symbols.begin(); itr!=symbols.end(); itr++)
                {
                    if (*itr=="BRK.B")
                        *itr="BRK_B";
                    if (*itr=="BF.B")
                        *itr="BF_B";
                    if (*itr=="CASH_USD")
                        *itr="TMUS";
                    if (*itr=="DOW"||*itr=="CTVA"||*itr=="AMCR"||*itr=="FOXA"||*itr=="CPRI")
                    {
                        count++;
                        continue;
                    }
                    Stock aStock(*itr);
                    string c = "SELECT * FROM FUNDAMENTALS;";
                    if (stock_fundamentals(stockDB, c, aStock, count)==-1)
                        return -1;
                    string stock_select = "SELECT * FROM STOCK_" + *itr + " WHERE strftime(\'%Y-%m-%d\', date) between \"2019-01-01\" and \"2019-07-31\" and open > 0 and adjusted_close > 0;";
                    if (stock_prices(stockDB, stock_select, aStock)==-1)
                        return -1;
                    aStock.calculate_returns();
                    cout<<*itr<<"!!!!!!!!!!!!!!"<<endl;
                    total_stocks.push_back(aStock);
                    count++;
                }
                
                vector<float> ori_weights;
                string b = "SELECT * FROM NEWSP500;";
                if (GetWeights(b.c_str(), stockDB, ori_weights)==-1)
                    return -1;
                
                int te=0;
                for (int i=0; i<ori_weights.size(); i++)
                {
                    if (symbols[i]=="DOW"||symbols[i]=="CTVA"||symbols[i]=="AMCR"||symbols[i]=="FOXA"||symbols[i]=="CPRI")
                    {
                        te=te+1;
                        i=i+1;
                    }
                    else
                        total_stocks[i-te].setprop(ori_weights[i]);
                }
                
                CloseDatabase(stockDB);
                
                vector <Stock> teststock;
                for (int i=0; i<total_stocks.size(); i++)
                {
                    if (total_stocks[i].getsymbol()=="AGN")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="APTV")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="DLTR")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="FANG")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="FLT")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="HCA")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="MKTX")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="ROST")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="TDG")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="ULTA")
                        teststock.push_back(total_stocks[i]);
                    //Symbols: AGN  Symbols: APTV  Symbols: DLTR  Symbols: FANG  Symbols: FLT  Symbols: HCA  Symbols: MKTX  Symbols: ROST  Symbols: TDG  Symbols: ULTA  Weights: 0.188385  Weights: 0.0746544  Weights: 0.0967906  Weights: 0.0619982  Weights: 0.0937704  Weights: 0.135621  Weights: 0.0492382  Weights: 0.140426  Weights: 0.0877814  Weights: 0.0713341
//                    if (total_stocks[i].getsymbol()=="ABBV")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="AGN")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="AVGO")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="CTAS")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="DG")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="DHI")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="FLT")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="ORLY")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="SHW")
//                        teststock.push_back(total_stocks[i]);
//                    if (total_stocks[i].getsymbol()=="STZ")
//                        teststock.push_back(total_stocks[i]);
                }
                
                //ma 155.775
                //150.539  237.214
                //137.966
                //pe 145.281   216.104
                //pe 141.072   195.264
                //beta 137.38
                //123.324
                //123.617
                //112.89
                //new fitness:142.919  209.699
                //fitness:108.886
                //fitness:113.97
                Portfolio testp(teststock);
                testp.calculateweights(teststock);
                testp.Assignfitness();
                
                vector <string> starts={"2019-01-07", "2019-01-14", "2019-01-22", "2019-01-28"};
                vector <string> ends={"2019-01-11", "2019-01-18", "2019-01-25", "2019-02-01"};
                map<string, float> result1;
                for (int m=0; m<starts.size(); m++)
                {
                    result1[starts[m]]=(testp).calculateweekreturn(starts[m], ends[m]);
                    cout<<result1[starts[m]]<<endl;
                }
                float first=calculatemothly(result1);
                cout<<"first month:"<<first<<endl;
                
                vector <string> starts2={"2019-02-04", "2019-02-11", "2019-02-19", "2019-02-25"};
                vector <string> ends2={"2019-02-08", "2019-02-15", "2019-02-22", "2019-03-01"};
                map<string, float> result2;
                for (int m=0; m<starts2.size(); m++)
                {
                    result2[starts2[m]]=(testp).calculateweekreturn(starts2[m], ends2[m]);
                    cout<<result2[starts2[m]]<<endl;
                }
                float second=calculatemothly(result2);
                cout<<"second month:"<<second<<endl;
                
                vector <string> starts3={"2019-03-04", "2019-03-11", "2019-03-18", "2019-03-25"};
                vector <string> ends3={"2019-03-08", "2019-03-15", "2019-03-22", "2019-03-29"};
                map<string, float> result3;
                for (int m=0; m<starts3.size(); m++)
                {
                    result3[starts3[m]]=(testp).calculateweekreturn(starts3[m], ends3[m]);
                    cout<<result3[starts3[m]]<<endl;
                }
                float third=calculatemothly(result3);
                cout<<"third month:"<<third<<endl;
                
                vector <string> starts4={"2019-04-01", "2019-04-08", "2019-04-15", "2019-04-22"};
                vector <string> ends4={"2019-04-05", "2019-04-12", "2019-04-18", "2019-04-26"};
                map<string, float> result4;
                for (int m=0; m<starts4.size(); m++)
                {
                    result4[starts4[m]]=(testp).calculateweekreturn(starts4[m], ends4[m]);
                    cout<<result4[starts4[m]]<<endl;
                }
                float fourth=calculatemothly(result4);
                cout<<"fourth month:"<<fourth<<endl;
                
                vector <string> starts5={"2019-04-29", "2019-05-06", "2019-05-13", "2019-05-20", "2019-05-28"};
                vector <string> ends5={"2019-05-03", "2019-05-10", "2019-05-17", "2019-05-24", "2019-05-31"};
                map<string, float> result5;
                for (int m=0; m<starts5.size(); m++)
                {
                    result5[starts5[m]]=(testp).calculateweekreturn(starts5[m], ends5[m]);
                    cout<<result5[starts5[m]]<<endl;
                }
                float fifth=calculatemothly(result5);
                cout<<"fifth month:"<<fifth<<endl;
                
                vector <string> starts6={"2019-06-03", "2019-06-10", "2019-06-17", "2019-06-24"};
                vector <string> ends6={"2019-06-07", "2019-06-14", "2019-06-21", "2019-06-28"};
                map<string, float> result6;
                for (int m=0; m<starts6.size(); m++)
                {
                    result6[starts6[m]]=(testp).calculateweekreturn(starts6[m], ends6[m]);
                    cout<<result6[starts6[m]]<<endl;
                }
                float sixth=calculatemothly(result6);
                cout<<"sixth month:"<<sixth<<endl;
                
                cout<<"six months average:"<<first+second+third+fourth+fifth+sixth<<endl;
                
                vector <string> starts7={"2019-07-01", "2019-07-08", "2019-07-15", "2019-07-22"};
                vector <string> ends7={"2019-07-05", "2019-07-12", "2019-07-19", "2019-07-26"};
                map<string, float> result7;
                for (int m=0; m<starts7.size(); m++)
                {
                    result7[starts7[m]]=(testp).calculateweekreturn(starts7[m], ends7[m]);
                    cout<<result7[starts7[m]]<<endl;
                }
                cout<<"senventh month:"<<calculatemothly(result7)<<endl;
                break;
            }
            case '8':
            {
                cout<<"8 - prob testing. \n";
                vector <Stock> total_stocks;
                const char * stockDB_name = "Stocks.db";
                sqlite3 * stockDB = NULL;
                if (OpenDatabase(stockDB_name, stockDB) == -1)
                    return -1;
                vector <string> symbols;
                string a = "SELECT * FROM NEWSP500;";
                if (GetSymbols(a.c_str(), stockDB, symbols)==-1)
                    return -1;
                
                int count=1;
                cout<<symbols.size()<<endl;
                for (vector<string>::iterator itr=symbols.begin(); itr!=symbols.end(); itr++)
                {
                    if (*itr=="BRK.B")
                        *itr="BRK_B";
                    if (*itr=="BF.B")
                        *itr="BF_B";
                    if (*itr=="CASH_USD")
                        *itr="TMUS";
                    if (*itr=="DOW"||*itr=="CTVA"||*itr=="AMCR"||*itr=="FOXA"||*itr=="CPRI")
                    {
                        count++;
                        continue;
                    }
                    Stock aStock(*itr);
                    string c = "SELECT * FROM FUNDAMENTALS;";
                    if (stock_fundamentals(stockDB, c, aStock, count)==-1)
                        return -1;
                    string stock_select = "SELECT * FROM STOCK_" + *itr + " WHERE strftime(\'%Y-%m-%d\', date) between \"2019-01-01\" and \"2019-07-31\" and open > 0 and adjusted_close > 0;";
                    if (stock_prices(stockDB, stock_select, aStock)==-1)
                        return -1;
                    aStock.calculate_returns();
                    cout<<*itr<<"!!!!!!!!!!!!!!"<<endl;
                    total_stocks.push_back(aStock);
                    count++;
                }
                
                vector<float> ori_weights;
                string b = "SELECT * FROM NEWSP500;";
                if (GetWeights(b.c_str(), stockDB, ori_weights)==-1)
                    return -1;
                
                int te=0;
                for (int i=0; i<ori_weights.size(); i++)
                {
                    if (symbols[i]=="DOW"||symbols[i]=="CTVA"||symbols[i]=="AMCR"||symbols[i]=="FOXA"||symbols[i]=="CPRI")
                    {
                        te=te+1;
                        i=i+1;
                    }
                    else
                        total_stocks[i-te].setprop(ori_weights[i]);
                }
                
                Stock SPY("SPY");
                string c = "SELECT * FROM FUNDAMENTALS;";
                if (stock_fundamentals(stockDB, c, SPY, 506)==-1)
                    return -1;
                string stock_select = "SELECT * FROM STOCK_SPY WHERE strftime(\'%Y-%m-%d\', date) between \"2019-07-01\" and \"2019-07-31\" and open > 0 and adjusted_close > 0;";
                if (stock_prices(stockDB, stock_select, SPY)==-1)
                    return -1;
                SPY.calculate_returns();
                
                CloseDatabase(stockDB);
                
                vector <Stock> teststock;
                for (int i=0; i<total_stocks.size(); i++)
                {
                    if (total_stocks[i].getsymbol()=="AGN")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="APTV")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="DLTR")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="FANG")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="FLT")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="HCA")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="MKTX")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="ROST")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="TDG")
                        teststock.push_back(total_stocks[i]);
                    if (total_stocks[i].getsymbol()=="ULTA")
                        teststock.push_back(total_stocks[i]);
                }
                //165.035
                //new 161.5
                //145.689
                //147.344
                cout<<total_stocks[30].getsymbol()<<endl;
                Portfolio testp(teststock);
                testp.calculateweights(teststock);
                testp.Assignfitness();
                
                vector <string> starts7={"2019-07-01", "2019-07-08", "2019-07-15", "2019-07-22"};
                vector <string> ends7={"2019-07-05", "2019-07-12", "2019-07-19", "2019-07-26"};
                map<string, float> result7;
                for (int m=0; m<starts7.size(); m++)
                {
                    result7[starts7[m]]=testp.calculateweekreturn(starts7[m], ends7[m]);
                    cout<<result7[starts7[m]]<<endl;
                }
                cout<<"senventh month:"<<calculatemothly(result7)<<endl;
                
                map<string, float> result9;
                for (int m=0; m<starts7.size(); m++)
                {
                    result9[starts7[m]]=SPY.calculateweekly(starts7[m], ends7[m]);
                    cout<<result9[starts7[m]]<<endl;
                }
                cout<<"senventh month SPY:"<<calculatemothly(result9)<<endl;
                break;
            }
            case '0':
            {
                cout<<"0 - Exit the program.\n";
                continue_menu = false;
                exit(1);
            }
            default:
            {
                cout<<"Invalid input!\n";
                break;
            }
        }
    }
}
