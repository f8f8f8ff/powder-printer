#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>

using namespace std;

const string jobReportCSVHeader =
    "Report Path,Date,Name,Estimated Time,Real Time,Time Error,Estimated Binder,Real Binder,Binder Error,Failed,Error Code,Printer IP,Printer Model";

class JobReport
{
public:
    JobReport();

    void processError();
    string getCSV();
    bool good();
    void print();
    void printCSV();

    void processLine(const string &line);
    int processFile(const string &path);

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
    string printerIp;
    string printerModel;
};

