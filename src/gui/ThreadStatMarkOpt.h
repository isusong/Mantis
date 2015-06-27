#ifndef THREADSTATMARKOPT_H
#define THREADSTATMARKOPT_H

#include "../core/ThreadWorker.h"
#include "../core/Profile.h"
#include "../core/VirtualTip.h"
#include "../core/StatInterface.h"
#include "../core/StatResults.h"

#include <QGLPixelBuffer>
#include <QGLWidget>

class ThreadStatMarkOpt : public ThreadWorker
{
    Q_OBJECT

public:

    struct TipSettings
    {
        int yawInc;
        int yawMin;
        int yawMax;
        int yawCur;
        PVirtualTip vt;
        PRangeImage tipImg;

        TipSettings(PRangeImage pTipImg=PRangeImage(), PVirtualTip virtualTip=PVirtualTip())
        {
            yawInc = 5;
            yawMin = 25;
            yawMax = 85;
            yawCur = yawMin;
            vt = virtualTip;
            tipImg = pTipImg;
        }

        TipSettings(PRangeImage pTipImg, int yawIncs, int yawMins, int yawMaxs, PVirtualTip virtualTip)
        {
            yawInc = yawIncs;
            yawMin = yawMins;
            yawMax = yawMaxs;
            yawCur = yawMin;
            vt = virtualTip;
            tipImg = pTipImg;
        }
    };

public:
    ThreadStatMarkOpt(const StatInterface::StatConfig &cfg, PProfile plate, PRangeImage tipImg, int yawInc=5, int yawMin=25, int yawMax=85);
    virtual ~ThreadStatMarkOpt();

    int getMaxYaw() { return _results->_resultMaxT.yaw; }
    const StatResult& getResultMaxT() { return _results->_resultMaxT; }
    PProfile   getProfileMaxT() { return _profileTipMax; }
    PStatResults getResults() { return _results; }

protected:

    virtual bool onPreRunLoop();
    virtual void doWork();
    QGLContext* getContext();

    bool statCompare(PProfile pro1, PProfile pro2);



protected:
    PProfile _profilePlate;
    PProfile _profileTipMax;
    TipSettings _ts;
    PStatInterface _stat;
    PStatResults _results;

    QGLContext *_context;
    std::tr1::shared_ptr<QGLPixelBuffer> _pbuffer; // Used to get OpenGL context
    std::tr1::shared_ptr<QGLWidget> _widget; // Used to get OpenGL context if no pbuffer

};

#endif // THREADSTATMARKOPT_H
