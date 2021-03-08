#include "parser.h"

//#define VERBOSE

using namespace std;

char* getCmdOption(char** begin, char** end, const string& option)
{
    char** itr = find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const string& option)
{
    return find(begin, end, option) != end;
}

bool VERBOSE = false;

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        cout << "No file(s) specified\n";
        return 1;
    }

    int pathsOffset = 1;

    if (cmdOptionExists(argv, argv+argc, "-v"))
    {
        VERBOSE = true;
        pathsOffset++;
    }

    string outputFilename = "output.csv";
    if (cmdOptionExists(argv, argv+argc, "-o"))
    {
        outputFilename = getCmdOption(argv, argv+argc, "-o");
        pathsOffset += 2;
    }

    vector<string> filePaths;
    filePaths.assign(argv + pathsOffset, argv + argc);
    vector<JobReport> jobReports;

    // parse files
    for (auto file : filePaths)
    {
        JobReport data;
        data.reportPath = file;
        if (data.processFile(file))
        {
            jobReports.push_back(data);
        }
    }

    if (VERBOSE)
    {
        cout << "--------------------" << endl;
        for (auto d : jobReports)
        {
            cout << "REPORT: [" << d.reportPath << "]" << endl;
            if (d.good())
            {
                d.print();
                d.printCSV();
            }
            cout << "--------------------" << endl;
        }
    }

    fstream outFile;
    outFile.open(outputFilename.c_str(), ios::out);
    if (!outFile.is_open())
    {
        cout << "failed: couldn't open output file" << endl;
        return 1;
    }

    outFile << jobReportCSVHeader << endl;
    for (auto d: jobReports)
    {
        outFile << d.getCSV() << endl;
    }

    outFile.close();

    return 0;
}
