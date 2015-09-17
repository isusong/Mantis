#ifndef THREADCLEANER_H
#define THREADCLEANER_H

#include "../core/ThreadWorker.h"

class MaskEditorWidget;

class ThreadCleaner : public ThreadWorker
{
    Q_OBJECT

public:
    ThreadCleaner(MaskEditorWidget *maskEditor);

protected:
    virtual bool onPreRunLoop();
    virtual void doWork();

protected:
    MaskEditorWidget *_maskEditor;
};

#endif // THREADCLEANER_H
