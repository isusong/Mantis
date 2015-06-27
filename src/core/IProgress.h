#ifndef IPROGRESS_H
#define IPROGRESS_H

#include "qglobal.h"

class IProgress
{
public:
    IProgress();

    virtual bool progCancel() { return false; }
    virtual void progStep(int step=1);
    virtual void progSetStepsTotal(int steps, bool resetStepsCur = true);
    virtual void progMsg(const char *msg) { Q_UNUSED(msg); }
    virtual float progPercent() { return (float)_progStepsCur / (float)_progStepsTotal; }

protected:
    virtual void progOnUpdated() {}

protected:
    int _progStepsTotal;
    int _progStepsCur;
};

#endif // IPROGRESS_H
