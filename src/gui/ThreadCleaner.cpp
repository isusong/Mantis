#include "ThreadCleaner.h"
#include "MaskEditorWidget.h"
#include "../core/CleaningCode/Clean.h"
#include "../core/logger.h"

//=======================================================================
//=======================================================================
ThreadCleaner::ThreadCleaner(MaskEditorWidget *maskEditor) :
    _maskEditor(maskEditor)
{
}

//=======================================================================
//=======================================================================
bool ThreadCleaner::onPreRunLoop()
{
    if (!ThreadWorker::onPreRunLoop())
    {
        emit signalInitFailed(QString("Worker thread failed to initialize"));
    }

    if (!_maskEditor) return false;
    if (_maskEditor->getImg().isNull()) return false;

    int progCount = _maskEditor->computeProgCount();
    progSetStepsTotal(progCount);

    return true;
}

//=======================================================================
//=======================================================================
void ThreadCleaner::doWork()
{
    ThreadWorker::doWork();

    if (!_maskEditor) return;
    if (_maskEditor->getImg().isNull()) return;

    if (shouldStop()) return;

    DlgClean *options = _maskEditor->getCleanOptions();

    //int progCount = computeProgCount(*_dlgClean.get());

    // save the mask to use the current modified mask data during cleaning
    _maskEditor->saveMask(this);

    //Run connected components to avoid floating data islands.
    Clean cleaner;
    RangeImage *imgNew;
    PRangeImage rngImg = _maskEditor->getImgCopy(); // lets create a copy to avoid threading issue3s

    if (options->getDataIslands())
    {
        imgNew = cleaner.connectedComponents(rngImg.data(), this);
        imgNew->setFileName(rngImg->getFileName());
        rngImg = PRangeImage(imgNew);
    }

    if (shouldStop()) return;

    if (options->getThresholdRun())
    {
        QSharedPointer<QImage> qmap = options->getQualityMap();
        int txmin = options->getThresholdTx();
        int qlmax = options->getThresholdQlt();

        int pixTurnedOff = cleaner.thresholdMask(rngImg.data(), qmap.data(), qlmax, txmin, this);
        LogTrace("Pixels masked by threshold run: %d", pixTurnedOff);
    }

    if (shouldStop()) return;

    if (options->getTipSpike())
    {
        LogTrace("Removing spikes and filling holes... ");
        // This redoes the conncomps if necessary.

        imgNew = cleaner.spikeRemovalHoleFilling(rngImg.data(), this);
        imgNew->setFileName(rngImg->getFileName());
        rngImg = PRangeImage(imgNew);
    }

    if (shouldStop()) return;

    if (options->getTipCoordSys() && rngImg->isTip())
    {
        LogTrace("Computing a tip coordinate system.");
        imgNew = cleaner.coordinateSystem4Tip(rngImg.data(), this);
        imgNew->setFileName(rngImg->getFileName());
        rngImg = PRangeImage(imgNew);
    }

    if (shouldStop()) return;

    if (options->getPltDetrend())
    {
        LogTrace("Detrending....");
        imgNew = cleaner.detrend(rngImg.data(), this);
        imgNew->setFileName(rngImg->getFileName());
        rngImg = PRangeImage(imgNew);
    }

    if (shouldStop()) return;

    if (options->getPltCoordSys() && !rngImg->isTip())
    {
        LogTrace("Computing a centroid coordinate system.");
        imgNew = cleaner.coordinateSystem2Centroid(rngImg.data(), this);
        imgNew->setFileName(rngImg->getFileName());
        rngImg = PRangeImage(imgNew);
    }


    // reset the image to get everything up to date
    _maskEditor->setImg(rngImg, this);
    _maskEditor->setModified(true); // data has been modified

    forceStop();
}
