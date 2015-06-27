#ifndef STATRESULTS_H
#define STATRESULTS_H

#include <QString>

struct StatResult
{
    int yaw;
    double t;
    double r;
    bool markFailed;
    bool compareFailed;

    StatResult(int iyaw, bool bMarkFailed, bool bCompareFailed)
    {
        yaw = iyaw;
        t = -999999999;
        r = -999999999;
        markFailed = bMarkFailed;
        compareFailed = bCompareFailed;
    }

    StatResult(int iyaw=0, double dt=-999999999, double dr=-999999999, bool bMarkFailed=false, bool bCompareFailed=false)
    {
        yaw = iyaw;
        t = dt;
        r = dr;
        markFailed = bMarkFailed;
        compareFailed = bCompareFailed;
    }

    void clear()
    {
        yaw = 0;
        t = -999999999;
        r = -999999999;
        markFailed = false;
        compareFailed = false;
    }
};

class StatResults
{
public:
    StatResults() {}

public:
    std::vector<StatResult> _results;
    StatResult _resultMaxT;
};

typedef std::tr1::shared_ptr<StatResults> PStatResults;

#endif
