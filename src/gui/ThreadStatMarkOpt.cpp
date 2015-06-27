#include "ThreadStatMarkOpt.h"
#include "../core/logger.h"

#define PBUFWIDTH 512
#define PBUFHEIGHT 512

//=======================================================================
//=======================================================================
ThreadStatMarkOpt::ThreadStatMarkOpt(const StatInterface::StatConfig &cfg, PProfile plate, PRangeImage tipImg, int yawInc, int yawMin, int yawMax) :
    _profilePlate(plate)
{
    _ts.yawMin = yawMin;
    _ts.yawMax = yawMax;
    _ts.yawInc = yawInc;
    _ts.yawCur = yawMin;
    _ts.tipImg = tipImg;

    _stat.reset(new StatInterface());
    _stat->setMaxShiftPercentage(cfg.maxShiftPercentage);
    _stat->setNumRandomPairs(cfg.numRandomPairs);
    _stat->setNumRigidPairs(cfg.numRigidPairs);
    _stat->setSearchWindow(cfg.searchWindow);
    _stat->setValidWindow(cfg.validWindow);
    _stat->setTSampleSize(cfg.tSampleSize);

    _results.reset(new StatResults());
}

//=======================================================================
//=======================================================================
ThreadStatMarkOpt::~ThreadStatMarkOpt()
{
}

//=======================================================================
//=======================================================================
bool ThreadStatMarkOpt::onPreRunLoop()
{
    if (!ThreadWorker::onPreRunLoop())
    {
        emit signalInitFailed(QString("Worker thread failed to initialize"));
    }

    if (_ts.tipImg.isNull())
    {
        emit signalInitFailed(QString("Tip Image not valid"));
    }

    _context = getContext();
    if (!_context)
    {
        emit signalInitFailed(QString("Failed to create an opengl context"));
        return false;
    }

    _results->_resultMaxT.clear();
    _results->_results.clear();
    _profileTipMax.reset();
    _ts.vt.reset(new VirtualTip(_ts.tipImg.data(), _context, this));

    int statSteps = 1;
    int profiles = (int)((float)(_ts.yawMax - _ts.yawMin) / (float)_ts.yawInc) + 1;
    int steps = (_ts.vt->getProgSteps() + statSteps) * profiles;
    steps += 2;
    progSetStepsTotal(steps);


    return true;
}

//=======================================================================
//=======================================================================
void ThreadStatMarkOpt::doWork()
{
    ThreadWorker::doWork();

    if (shouldStop()) return;
    if (!_ts.vt) return;
    if (_ts.yawCur > _ts.yawMax) return;

    if (_ts.yawCur == _ts.yawMin)
    {
        emit signalStart();
    }

    // find highet T1 value
    QString msg = QString("Calculating mark for angle: %1").arg(_ts.yawCur);
    progMsg(msg.toStdString().c_str());


    Profile *profile = _ts.vt->mark(0, _ts.yawCur, 0);
    if (profile)
    {
        progMsg("Calculating stats...");
        PProfile proTip(profile);
        if (statCompare(proTip, _profilePlate))
        {
            StatResult result(_ts.yawCur, _stat->getTValue(), _stat->getRValue());
            _results->_results.push_back(result);

            LogInfo("Stats for angle: %d, t: %f, r: %f", result.yaw, result.t, result.r);
            if (_profileTipMax == NULL || result.t > _results->_resultMaxT.t)
            {
                _profileTipMax = proTip;
                _results->_resultMaxT = result;
            }
        }
        else
        {
            _results->_results.push_back(StatResult(_ts.yawCur, false, true));
        }
    }
    else
    {
        _results->_results.push_back(StatResult(_ts.yawCur, true, false));
    }

    progStep();

    _ts.yawCur += _ts.yawInc;

    if (_ts.yawCur > _ts.yawMax)
    {
        if (_profileTipMax)
        {
            LogInfo("Stat Results For Max T: angle: %d, t: %.2f, r: %.2f", _results->_resultMaxT.yaw, _results->_resultMaxT.t, _results->_resultMaxT.r);
        }
        else
        {
            LogInfo("UnExpected: No valid stat results were computed, angle min: %d, max: %d, step: %d", _ts.yawMin, _ts.yawMax, _ts.yawInc);
        }

        progStep();
        progMsg("Finalizing...");

        //emit signalProgress(1);
        forceStop();
    }
}

//=======================================================================
//=======================================================================
bool ThreadStatMarkOpt::statCompare(PProfile pro1, PProfile pro2)
{
    try
    {
        _stat->compare(pro1.get(), pro2.get());

        return true;
    }
    catch (std::exception err)
    {
        LogError("UnExpected Exception in statistics compare: %s", err.what());
        return false;
    }
}


//=======================================================================
//=======================================================================
QGLContext* ThreadStatMarkOpt::getContext()
{
    // do we have a widget.. hopefully not
    if (_widget != NULL)
    {
        //Return the widget's context.
        return const_cast<QGLContext*>(_widget->context());
    }

    // do we need to create a pbuffer
    if (_pbuffer == NULL || !_pbuffer->isValid())
    {
        if (QGLPixelBuffer::hasOpenGLPbuffers())
        {
            _pbuffer.reset(new QGLPixelBuffer(PBUFWIDTH, PBUFHEIGHT));
        }
    }

    // do we have a pbuffer context
    if (_pbuffer && _pbuffer->isValid())
    {
        _pbuffer->makeCurrent();
        return const_cast<QGLContext*>(QGLContext::currentContext());
    }


    //If we made it here, make and use a widget.
    _widget.reset(new QGLWidget());
    _widget->isValid();
    _widget->makeCurrent();
    _widget->updateGL();
    _widget->show();
    _widget->setWindowTitle("Virtual Mark Window. DO NOT CLOSE");
    return const_cast<QGLContext*>(_widget->context());
}

