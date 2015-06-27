#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include "IProgress.h"

class ThreadWorker : public QThread, public IProgress
{
    Q_OBJECT
public:
    ThreadWorker();

    void startThread(); // todo: allow caller to set priority
    void forceStop() { _stop = true; }
    bool isRunning() { return _running; }

    virtual bool progCancel();
    virtual void progMsg(const char *msg);

public slots:
    void slotCancel(); // executes in the original thread, which is fine

signals:
    void signalInitFailed(QString reason);
    void signalStart();
    void signalProgress(float percent);
    void signalMsg(QString msg);
    void signalEnd();

protected:

    virtual void progOnUpdated();

    virtual void run();

    virtual bool onPreRunLoop() { return true; }
    virtual void doWork() {};

    bool shouldStop() { return _stop; }

protected:
    bool _stop;
    bool _running;


};

#endif
