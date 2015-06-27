#ifndef THREADMTFILEUPDATE_H
#define THREADMTFILEUPDATE_H

#include "ThreadWorker.h"

class ThreadMtFileUpdate : public ThreadWorker
{
public:
    ThreadMtFileUpdate();

protected:
    virtual void doWork();

};

#endif
