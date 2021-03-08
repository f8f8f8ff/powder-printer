#include "parser.h"

using namespace std;

regex expr ("^(Path|Estimated Time|Estimated   Clear|Binder usage|Duration|Started|  Clear Binder useage|Error Code|Printer|Model).*");

JobReport::JobReport()
{
    reportPath = "";
    path = "";
    date = "";
    estTime = 0;
    realTime = 0;
    errorTime = 0.0;
    estBinder = 0;
    realBinder = 0;
    errorBinder = 0.0;
    errorCode = 0;
    failed = true;
    printerIp = "";
    printerModel = "";
    volume = 0.0;
}

void JobReport::processError()
{
    if (!failed)
    {
        errorTime = ((float)realTime/estTime)-1.0;
        errorBinder = ((float)realBinder/estBinder)-1.0;
    }
}

string JobReport::getCSV()
{
    string csv;
    stringstream stream;
    stream << reportPath << ",";
    stream << date << ",";
    stream << path << ",";
    stream << volume << ",";
    stream << estTime << ",";
    stream << realTime << ",";
    stream << errorTime << ",";
    stream << estBinder << ",";
    stream << realBinder << ",";
    stream << errorBinder << ",";
    stream << failed << ",";
    stream << errorCode << ",";
    stream << printerIp << ",";
    stream << printerModel;
    csv = stream.str();
    return csv;
}

bool JobReport::good()
{
    bool g = true;
    if (reportPath == "")
    {
        cout << "Bad data: no reportPath" << endl;
        g = false;
    }
    if (date == "")
    {
        cout << "Bad data: no date" << endl;
        g = false;
    }
    if (path == "")
    {
        cout << "Bad data: no path" << endl;
        g = false;
    }
    if (estTime == 0)
    {
        cout << "Bad data: no estTime" << endl;
        g = false;
    }
    if (realTime == 0)
    {
        cout << "Bad data: no realTime" << endl;
        g = false;
    }
    if (estBinder == 0)
    {
        cout << "Bad data: no estBinder" << endl;
        g = false;
    }
    if (realBinder == 0)
    {
        cout << "Bad data: no realBinder" << endl;
        g = false;
    }
    if (printerIp == "")
    {
        cout << "Bad data: no printerIp" << endl;
        g = false;
    }
    if (printerModel == "")
    {
        cout << "Bad data: no printerModel" << endl;
        g = false;
    }
    if (volume == 0.0)
    {
        cout << "Bad data: no volume" << endl;
        g = false;
    }
    return g;
}

void JobReport::print()
{
    cout << "reportPath: " << reportPath << endl;
    cout << "date: " << date << endl;
    cout << "path: " << path << endl;
    cout << "volume: " << volume << endl;
    cout << "estTime: " << estTime << endl;
    cout << "realTime: " << realTime << endl;
    cout << "estBinder: " << estBinder << endl;
    cout << "realBinder: " << realBinder << endl;
    cout << "errorTime: " << errorTime << endl;
    cout << "errorBinder: " << errorBinder << endl;
    cout << "failed: " << ((failed) ? "true" : "false") << endl;
    cout << "errorCode: " << errorCode << endl;
    cout << "printerIp: " << printerIp << endl;
    cout << "printerModel: " << printerModel << endl;
    cout << endl;
}

void JobReport::printCSV()
{
    cout << jobReportCSVHeader << endl;
    cout << getCSV() << endl;
}

void JobReport::processLine(const string &line)
{
    smatch sm;
    regex_match(line, sm, expr);

    if (sm.size() == 0)
        return;
    /*
    else
    {
        cout << sm.size() << " matches: ";
        for (auto m : sm)
            cout << "[" << m << "] ";
        cout << endl;
    }
    */

    // get path
    if (sm[1] == "Path")
    {
        stringstream stream(line);
        string substr;
        while (stream.good())
        {
            getline(stream, substr, '\\');
        }
        path = substr;
        //cout << "path: " << path << endl;
    }

    // get estTime in minutes
    else if (sm[1] == "Estimated Time")
    {
        stringstream stream(line);
        string substr;
        vector<int> values;
        int minutes;
        while (getline(stream, substr, ' '))
        {
            int v;
            if (stringstream(substr) >> v)
                values.push_back(v);
        }
        if (values.size() == 2)
        {
            minutes = values[0]*60 + values[1];
        }
        else
        {
            minutes = values[0];
        }
        estTime = minutes;
        //cout << "estTime: " << estTime << endl;
    }

    // get realTime
    else if (sm[1] == "Duration")
    {
        stringstream stream(line);
        string substr;
        vector<int> values;
        int minutes;
        while (getline(stream, substr, ' '))
        {
            int v;
            if (stringstream(substr) >> v)
                values.push_back(v);
        }
        minutes = values[0]*60 + values[1];
        realTime = minutes;
        //cout << "realTime: " << realTime << endl;
    }

    // get estBinder
    else if (sm[1] == "Estimated   Clear")
    {
        stringstream stream(line);
        string substr;
        float binder;
        while (getline(stream, substr, ' '))
        {
            if(stringstream(substr) >> binder)
                break;
        }
        estBinder = (int)binder;
        //cout << "estBinder: " << estBinder << endl;
    }

    // get realBinder
    else if (sm[1] == "Binder usage")
    {
        stringstream stream(line);
        string substr;
        float binder;
        while (getline(stream, substr, ' '))
        {
            if(stringstream(substr) >> binder)
                break;
        }
        realBinder = (int)binder;
        //cout << "realBinder: " << realBinder << endl;
    }

    //get date
    else if (sm[1] == "Started")
    {
        stringstream stream(line);
        string substr;
        int datei;
        while (getline(stream, substr, ' '))
        {
            if (stringstream(substr) >> datei)
                break;
        }
        int d = datei % 100;
        int m = (datei / 100) % 100;
        int y = datei / 10000;
        stringstream().swap(stream);
        stream.fill('0');
        stream << y << "/" << setw(2) << m << "/" << setw(2) << d;
        date = stream.str();
    }

    // search for "Clear Binder Useage" line as evidence of a completed print.
    else if (sm[1] == "  Clear Binder useage")
    {
        failed = false;
    }

    // search for Error Code
    else if (sm[1] == "Error Code")
    {
        stringstream stream(line);
        string substr;
        int errorCode=0;
        while (getline(stream, substr, ' '))
        {
            if (stringstream(substr) >> errorCode)
                break;
        }
        if (errorCode != 0)
        {
            failed = true;
            errorCode = errorCode;
        }
    }

    // get printer IP and model
    else if (sm[1] == "Printer")
    {
        stringstream stream(line);
        string substr;
        vector<string> split;
        while (getline(stream, substr, ':'))
        {
            split.push_back(substr);
        }
        if (split.size() == 3)
        {
            for (int i=0; i<split.size(); ++i)
            {
                split[i].erase(split[i].begin());
                split[i].erase(split[i].end()-1);
            }
            printerIp = split[1];
            printerModel = split[2];
        }
    }

    // get volume
    else if (sm[1] == "Model")
    {
        stringstream stream(line);
        string substr;
        float v;
        while (getline(stream, substr, '\t'))
        {
            stringstream(substr) >> v;
        }
        volume += v / 0.061024; // convert to cu cm from cu in
    }
}

// 0 error, 1 success
int JobReport::processFile(const string &path)
{
    cout << "Processing report \"" << path << "\"... " << endl;

    stringstream stream(path);
    while (getline(stream,reportPath,'/')) { }

    fstream file;
    file.open(path.c_str(), ios::in);
    if (!file.is_open())
    {
        cout << "failed: couldn't open file" << endl;
        return 0;
    }

    string line;
    int lineNum = 0;

    if (getline(file, line))
    {
        lineNum++;
        auto found = line.find("3DPrint");
        if (found == string::npos)
        {
            cout << "failed: not a report" << endl;
            return 0;
        }
    }
    else 
    {
        cout << "failed: empty file" << endl;
        return 0;
    }

    while (getline(file, line))
    {
        lineNum++;
        // remove \r character if present
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
        //cout << "line " << lineNum << ": " << line << endl;
        processLine(line);
        //getchar();
    }

    file.close();

    processError();

    if (good())
    {
        cout << "Success." << endl;
        return 1;
    }
    else
    {
        cout << "Failed." << endl;
        return 0;
    }
}
