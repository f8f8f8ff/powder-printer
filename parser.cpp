#include "parser.h"

using namespace std;

regex expr ("^(Path|Estimated Time|Estimated   Clear|Binder usage|Duration|Started|  Clear Binder useage|Error Code).*");

PrintData::PrintData()
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
}

void PrintData::processError()
{
    if (!failed)
    {
        errorTime = ((float)realTime/estTime)-1.0;
        errorBinder = ((float)realBinder/estBinder)-1.0;
    }
}

string PrintData::getCSV()
{
    string csv;
    stringstream stream;
    stream << date << ",";
    stream << path << ",";
    stream << estTime << ",";
    stream << realTime << ",";
    stream << errorTime << ",";
    stream << estBinder << ",";
    stream << realBinder << ",";
    stream << errorBinder << ",";
    stream << failed << ",";
    stream << errorCode;
    csv = stream.str();
    return csv;
}

bool PrintData::good()
{
    bool g = true;
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
    return g;
}

void PrintData::print()
{
    cout << "path: " << path << endl;
    cout << "date: " << date << endl;
    cout << "estTime: " << estTime << endl;
    cout << "realTime: " << realTime << endl;
    cout << "estBinder: " << estBinder << endl;
    cout << "realBinder: " << realBinder << endl;
    cout << "errorTime: " << errorTime << endl;
    cout << "errorBinder: " << errorBinder << endl;
    cout << "failed: " << ((failed) ? "true" : "false") << endl;
    cout << "errorCode: " << errorCode << endl;
    cout << endl;
}

void PrintData::printCSV()
{
    cout << "Date,Name,Estimated Time,Real Time,Time Error,Estimated Binder,Real Binder,Binder Error,Failed,Error Code" << endl;
    cout << getCSV() << endl;
}

void processLine(const string &line, PrintData &data)
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
        data.path = substr;
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
        data.estTime = minutes;
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
        data.realTime = minutes;
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
        data.estBinder = (int)binder;
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
        data.realBinder = (int)binder;
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
        stream >> data.date;
    }

    // search for "Clear Binder Useage" line as evidence of a completed print.
    else if (sm[1] == "  Clear Binder useage")
    {
        data.failed = false;
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
            data.failed = true;
            data.errorCode = errorCode;
        }
    }
}

// 0 error, 1 success
int processFile(const string &path, PrintData &data)
{
    cout << "Processing report \"" << path << "\"... " << endl;

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
        processLine(line, data);
        //getchar();
    }

    file.close();

    data.processError();

    if (data.good())
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
