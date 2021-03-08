#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class PrintData
{
public:
    PrintData();

    void processError();
    string getCSV();
    bool good();
    void print();
    void printCSV();

    string reportPath;
    string path;
    string date;
    int estTime;
    int realTime;
    float errorTime;
    int estBinder;
    int realBinder;
    float errorBinder;
    int errorCode;
    bool failed;
};

void processLine(const string &line, PrintData &data);
int processFile(const string &path, PrintData &data);
