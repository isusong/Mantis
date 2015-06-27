#include "ThreadWorker.h"
#include "logger.h"

//=======================================================================
//=======================================================================
ThreadWorker::ThreadWorker() :
    _stop(false),
    _running(false)
{

}

//=======================================================================
//=======================================================================
bool ThreadWorker::progCancel()
{
    if (_stop || !_running)
    {
        return true;
    }

    return false;
}

//=======================================================================
//=======================================================================
void ThreadWorker::progMsg(const char *msg)
{
    emit signalMsg(QString(msg));
}

//=======================================================================
//=======================================================================
void ThreadWorker::progOnUpdated()
{
    float per = (float)_progStepsCur/(float)_progStepsTotal;
    emit signalProgress(per);
}

//=======================================================================
//=======================================================================
void ThreadWorker::slotCancel()
{
    forceStop();
}

//=======================================================================
//=======================================================================
void ThreadWorker::startThread()
{
    // must set running flag here,
    // if someone calls isRunning immediatly after calling start
    // run may not have started and is running will be false, because the thread is initializing still
    _running = true;

    QThread::start();
}

//=======================================================================
//=======================================================================
void ThreadWorker::run()
{
    _running = true;

    if (!onPreRunLoop())
    {
        LogError("ThreadWorker::run - onPreRunLoop failed.. exiting thread");
        _running = false;
        _stop = false;
        return;
    }

    while (!_stop)
    {
        // exec(); // for event handeling
        doWork();
    }

    _running = false;
    _stop = false;

    emit signalEnd();
}
