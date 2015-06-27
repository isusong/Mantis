#include "IProgress.h"

//=======================================================================
//=======================================================================
IProgress::IProgress() :
    _progStepsTotal(1),
    _progStepsCur(0)
{
}

//=======================================================================
//=======================================================================
void IProgress::progStep(int step)
{
    _progStepsCur += step;
    if (_progStepsCur > _progStepsTotal)
    {
        _progStepsCur = _progStepsTotal;
    }

    progOnUpdated();
}

//=======================================================================
//=======================================================================
void IProgress::progSetStepsTotal(int steps, bool resetStepsCur)
{
    _progStepsTotal = steps;
    if (resetStepsCur)
    {
        _progStepsCur = 0;
    }
}
