#include "qmdisplitcmpwnd2.h"
#include "../core/logger.h"
#include "GuiSettings.h"
#include "../core/UtlMtFiles.h"
#include "../core/UtlQt.h"

//=======================================================================
//=======================================================================
QMdiSplitCmpWnd2::QMdiSplitCmpWnd2(QWidget * parent, Qt::WindowFlags flags) :
    QMdiSubWindow(parent, flags),
    _imgViewer(NULL),
    _plots(NULL),
    _markModeOn(false),
    _allowProfileUpd(true)
{
    _statPlots[0] = NULL;
    _statPlots[1] = NULL;
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::init()
{
    if (_imgViewer) return; // already initialized

    setAttribute(Qt::WA_DeleteOnClose);

    setWindowTitle("Split Comparison");

    // need a smaller plot window when stacking vertically
    QSize szHintPlot(358, 268);

    // 2 plots
    _statPlots[0] = new StatPlot(szHintPlot, szHintPlot);
    _statPlots[1] = new StatPlot(szHintPlot, szHintPlot);
    _statPlots[0]->setSearchPen(GuiSettings::colorSearchBoxLeft());
    _statPlots[1]->setSearchPen(GuiSettings::colorSearchBoxRight());
    _statPlots[0]->setAxisColor(GuiSettings::colorSelectWinL());
    _statPlots[1]->setAxisColor(GuiSettings::colorSelectWinR());

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addStretch(0);
    vlayout->setSizeConstraint(QLayout::SetFixedSize);
    vlayout->addWidget(_statPlots[0]);
    vlayout->addWidget(_statPlots[1]);

    _plots = new QWidget;
    _plots->setLayout(vlayout);

    // model viewer and model file labels
    _imgViewer = new RangeImageViewer(PRangeImage(), true, false, -1, 500, 500, this);
    _labelFileL = new QLabel();
    _labelFileR = new QLabel();
    _labelFileL->setAlignment(Qt::AlignHCenter);
    _labelFileR->setAlignment(Qt::AlignHCenter);

    // set up the labels horizontally
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins ( 0, 0, 0, 0 );
    hlayout->setMargin(0);
    hlayout->addWidget(_labelFileL);
    hlayout->addWidget(_labelFileR);
    QWidget *labels = new QWidget;
    labels->setLayout(hlayout);

    // stack labels on top of viewer
    QVBoxLayout *layoutviewer = new QVBoxLayout();
    layoutviewer->setContentsMargins ( 0, 0, 0, 0 );
    layoutviewer->setSpacing(4);
    layoutviewer->addWidget(labels);
    layoutviewer->addWidget(_imgViewer);
    QWidget *widgetViewer = new QWidget();
    widgetViewer->setLayout(layoutviewer);


    //Assemble widget.
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(_plots);
    splitter->addWidget(widgetViewer);

    splitter->show();
    setWidget(splitter);

    makeConnections();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::makeConnections()
{
    bool result = false;
    result = connect(_imgViewer, SIGNAL(onCmpSliderReleased()), this, SLOT(slotCmpSliderReleased()));
    result = connect(getGraphics(), SIGNAL(onSplitStatSelectionUpdated()), this, SLOT(slotSplitStatSelectionUpdated()));
    result = connect(getGraphics(), SIGNAL(onChangedTranslationMouse(int, const QVector3D&)), this, SLOT(slotChangedTranslationMouse(int, const QVector3D&)));
    result = connect(getGraphics(), SIGNAL(onChangedRotationMouse(int, const QVector3D&)), this, SLOT(slotChangedRotationMouse(int, const QVector3D&)));
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::markAvailable(RangeImageRenderer **tip, RangeImageRenderer **plate)
{
    int tips = 0;
    int plates = 0;

    if (tip) *tip = NULL;

    for (int i=0; i<2; i++)
    {
        RangeImageRenderer *r = getRenderer(i);
        if (!r) continue;

        if (r->getIsTip())
        {
            tips++;

            if (tip) *tip = r;
        }
        else
        {
            plates++;

            if (plate) *plate = r;
        }
    }

   if (tips == 1 && plates == 1) return true;
   // if (tips == 1) return true; // for quicker debugging if you don't need the plate

    return false;
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::getMarkMode()
{
    return _markModeOn;
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::setMarkMode(bool marked)
{
    bool ret = false; // true if we find at least one tip

    _markModeOn = marked;
    for (int i=0; i<2; i++)
    {
        RangeImageRenderer *r = getRenderer(i);
        if (!r) continue;

        if (r->getIsTip())
        {
            r->setDrawMark(_markModeOn);
            ret = true;
        }
    }

    return ret;
}

/*
//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::mark()
{
    //if (!markAvailable()) return false;
    //_markModeOn = true;
    //updateProfiles(); // create the profile which will update and draw the tip

    return setMark(_markModeOn);
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::unmark()
{
    //if (!markAvailable()) return false;
    //if (!_markModeOn) return false;

    //_markModeOn = false;
    //updateProfiles(); // create the profile which will update and draw the tip

    return setMark(_markModeOn);
}
*/

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::haveTip()
{
    for (int i=0; i<2; i++)
    {
        RangeImageRenderer *r = getRenderer(i);
        if (!r) continue;

        if (r->getIsTip())
        {
            return true;
        }
    }

    return false;
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::updateProfilesAndStats()
{
    if (!_allowProfileUpd) return;

    updateProfiles();
    emit updateStatsRT();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::slotCmpSliderReleased()
{
    updateProfilesAndStats();
}

//=======================================================================
// called from GraphicsWidget2::runSplitStatsSelection,
//  which happens everytime the slider is moved, but in mark mode, its way to
//  slow to calculate the tip profile
//
//=======================================================================
void QMdiSplitCmpWnd2::slotSplitStatSelectionUpdated()
{
    /*
    bool markMode = getMarkMode();
    if (markMode) return;
    */
    if (haveTip()) return;

    updateProfiles();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setProfile(int iviewer, PProfile pro)
{
    RangeImageRenderer *r = getRenderer(iviewer);
    if (!r)
    {
        return; // unexpected
    }

    if (r->getIsTip())
    {
        float degx = getGraphics()->getPitch(iviewer);
        float degy = getGraphics()->getYaw(iviewer);
        float degz = getGraphics()->getRoll(iviewer);
        _proInfo[iviewer].tipAng = QVector3D(degx, degy, degz);
    }
    else
    {
        _proInfo[iviewer].plateCol = r->getProfilePlateCol();
    }


    _proInfo[iviewer].dataType = r->getImgType();
    _proInfo[iviewer].profile = pro;

    _statPlots[iviewer]->setProfile(getProfile(iviewer));
    setProfileAxisText(iviewer);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::updateProfiles()
{
    if (!_allowProfileUpd) return;

    bool tips = haveTip();

    // make mode takes a while to process
    // non-mark is quick and updates while moving the slider so the cursor changing is a bit counter productive
    if (tips) // tip profile is slow to calculate
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    for (int i=0; i<2; i++)
    {
        getProfile(i).reset();
        RangeImageRenderer *r = getRenderer(i);
        if (!r) continue;

        PProfile p;
        if (r->getIsTip())
        {
            float degx = getGraphics()->getPitch(i);
            float degy = getGraphics()->getYaw(i);
            float degz = getGraphics()->getRoll(i);
            p = r->getProfileTip(degx, degy, degz);
        }
        else
        {
            Profile *pro = r->getProfile();
            if (pro) p.reset(pro);
        }

        setProfile(i, p);
    }

    /*
    for (int i=0; i<2; i++)
    {
        getProfile(i).reset();
        RangeImageRenderer *r = getRenderer(i);
        if (!r) continue;

        PProfile p;
        if (r->getIsTip())
        {
            float degx = getGraphics()->getPitch(i);
            float degy = getGraphics()->getYaw(i);
            float degz = getGraphics()->getRoll(i);
            p = r->getProfileTip(degx, degy, degz);
            //r->setDrawMark(markMode);

            _proInfo[i].isTip = true;
            _proInfo[i].tipAng = QVector3D(degx, degy, degz);
        }
        else
        {
            Profile *pro = r->getProfile();
            if (pro) p.reset(pro);

            _proInfo[i].isTip = false;
            _proInfo[i].plateCol = r->getProfilePlateCol();
        }

        _proInfo[i].profile = p;
    }

    _plot0->setProfile(getProfile(0));
    _plot1->setProfile(getProfile(1));
    setProfileAxisText(0);
    setProfileAxisText(1);
    */

    emit onStatsProfileUpdated(getProfile(0), getProfile(1));

    if (tips)
    {
        QApplication::restoreOverrideCursor();
    }
}

//=======================================================================
//=======================================================================
PProfile& QMdiSplitCmpWnd2::getProfile(int num)
{
    if (num == 0)
    {
        return _proInfo[0].profile;
    }

    return _proInfo[1].profile;
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setProfileAxisText(int viewer)
{
    StatPlot *plot = _statPlots[viewer];
    ProInfo *info = &_proInfo[viewer];

    QString strx;
    if (info->filename.length() <= 0)
    {
        strx = tr("x");
    }
    else if (info->dataType == RangeImage::ImgType_Tip)
    {
        strx = QString(tr("x - tip angle %1,%2,%3")).arg(QString::number(info->tipAng.y(), 'f', 0 ),
                                                   QString::number(info->tipAng.x(), 'f', 0 ),
                                                   QString::number(info->tipAng.z(), 'f', 0 ));
    }
    else if (info->dataType == RangeImage::ImgType_Knf)
    {
        strx = QString(tr("x - knife column %1")).arg(QString::number(info->plateCol));
    }
    else if (info->dataType == RangeImage::ImgType_Bul)
    {
        strx = QString(tr("x - bullet column %1")).arg(QString::number(info->plateCol));
    }
    else
    {
        strx = QString(tr("x - plate column %1")).arg(QString::number(info->plateCol));
    }

    plot->setAxisTextX(strx);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::slotSetSearchWindow1(int loc, int width, int dataLen)
{
    //LogTrace("set seach window 1 - loc: %d, width: %d", loc, width);
    if (getGraphics() && getGraphics()->getModelData(0) && getGraphics()->getModelData(0)->model.get() != NULL)
    {
        LogTrace("QMdiSplitCmpWnd2::slotSetSearchWindow1 - loc: %d, width: %d", loc, width);

        //getGraphics()->getModelData(0)->model->logInfo();
        getGraphics()->getModelData(0)->model->setSearchBox(loc, width, dataLen, true);
    }


    if (!getProfile(0).get()) return;
    _statPlots[0]->setSearchWindow(loc, width);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::slotSetSearchWindow2(int loc, int width, int dataLen)
{
    //LogTrace("set seach window 2 - loc: %d, width: %d", loc, width);
    if (getGraphics() && getGraphics()->getModelData(1) && getGraphics()->getModelData(1)->model.get() != NULL)
    {
        LogTrace("QMdiSplitCmpWnd2::slotSetSearchWindow2 - loc: %d, width: %d", loc, width);

        //getGraphics()->getModelData(1)->model->logInfo();
        getGraphics()->getModelData(1)->model->setSearchBox(loc, width, dataLen, true);
    }

    if (!getProfile(1).get()) return;
    _statPlots[1]->setSearchWindow(loc, width);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::slotChangedTranslationMouse(int viewer, const QVector3D &v)
{
    emit onChangedTranslationMouse(viewer, v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::slotChangedRotationMouse(int viewer, const QVector3D &v)
{
    emit onChangedRotationMouse(viewer, v);
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::loadRangeImg(const QString &filename)
{
    if (!_imgViewer) return false;

    return loadRangeImg(filename, _imgViewer->getGraphicsWidget()->getSelectedWindow());
}


//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd2::loadRangeImg(const QString &filename, int viewer)
{
    if (!_imgViewer) return false;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    PRangeImage rimg(new RangeImage(filename));
    if (rimg->isNull())
    {
        QApplication::restoreOverrideCursor();
        LogError("Failed to to load file: %s", filename.toStdString().c_str());
        return false;
    }

    // new import system should tag image type, this is for legacy tip and plate files, created before the import feature.
    if (rimg->isUnkType())
    {
        bool isTip = UtlMtFiles::isTipFile(filename);
        if (isTip)
        {
            rimg->setImgType(RangeImage::ImgType_Tip);
        }
        else
        {
            rimg->setImgType(RangeImage::ImgType_Plt);
        }
    }

    setRangeImg(rimg, viewer);

    QApplication::restoreOverrideCursor();
    return true;
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setRangeImg(PRangeImage rimg, int viewer)
{
    bool allowUpdPrev = _allowProfileUpd;
    _allowProfileUpd = false;

    if (viewer < 0) viewer = 0;
    if (viewer > 1) viewer = 1;

    QString fname = "";
    if (!rimg.isNull())
    {
        fname = UtlQt::fileName(rimg->getFileName());
    }

    if (viewer == ViewLeft)
    {
        _labelFileL->setText(fname);
    }
    else
    {
        _labelFileR->setText(fname);
    }

    _proInfo[viewer].filename = fname;
    _proInfo[viewer].fullpath = rimg->getFileName();
    _proInfo[viewer].dataType = rimg->getImgType();
    _proInfo[viewer].tipAng = QVector3D(0,0,0);
    _proInfo[viewer].plateCol = 0;
    _proInfo[viewer].profile.reset();

    //unmark(); // turn mark off if there is one

    _imgViewer->setModel(rimg, viewer);


    // set the splitter back to the center
    _imgViewer->setSliderPos(.5f);

    _allowProfileUpd = allowUpdPrev;
    updateProfilesAndStats(); // this will update the stats
}

//=======================================================================
//=======================================================================
int QMdiSplitCmpWnd2::getSelectedView()
{
    return getGraphics()->getSelectedWindow();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::toggleSelectedView()
{
    getGraphics()->toggleSelectedWindow();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::showPlots(bool show)
{
    if (!_plots) return;
    if (show)
    {
        _plots->show();
    }
    else
    {
        _plots->hide();
    }
}

//=======================================================================
//=======================================================================
RangeImageRenderer* QMdiSplitCmpWnd2::getSelectedRenderer()
{
    return dynamic_cast<RangeImageRenderer*>(getGraphics()->getModelDataSel()->model.get());
}

//=======================================================================
//=======================================================================
RangeImageRenderer* QMdiSplitCmpWnd2::getRenderer(int iviewer)
{
    return dynamic_cast<RangeImageRenderer*>(getGraphics()->getModelData(iviewer)->model.get());
}

//=======================================================================
//=======================================================================
PRangeImage QMdiSplitCmpWnd2::getModel(int iviewer)
{
    RangeImageRenderer *ir = getRenderer(iviewer);
    if (!ir) return PRangeImage();

    return ir->getModel();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::linkViews(bool on)
{
    if (!getGraphics()) return;
    getGraphics()->setViewLinkOn(on);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::enableViewLink(GraphicsWidget2::LinkView type)
{
    if (!getGraphics()) return;
    getGraphics()->enableViewLink(type);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::disableViewLink(GraphicsWidget2::LinkView type)
{
    if (!getGraphics()) return;
    getGraphics()->disableViewLink(type);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setDefaultView()
{
    if (!getGraphics()) return;
   getGraphics()->setDefaultView();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setYaw(double deg, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setYaw(deg, getSelectedView());
        return;
    }

    getGraphics()->setLinkedYaw(deg);
    /*
    getGraphics()->setYaw(deg, 0);
    getGraphics()->setYaw(deg, 1);
    */
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setPitch(double deg, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setPitch(deg, getSelectedView());
        return;
    }

    getGraphics()->setLinkedPitch(deg);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setRoll(double deg, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setRoll(deg, getSelectedView());
        return;
    }

    getGraphics()->setLinkedRoll(deg);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setPanH(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setTransX(v, getSelectedView());
        return;
    }

    getGraphics()->setLinkedTransX(v);
    /*
    getGraphics()->setTransX(v, 0);
    getGraphics()->setTransX(v, 1);
    */
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setPanV(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setTransY(v, getSelectedView());
        return;
    }

    getGraphics()->setLinkedTransY(v);
    /*
    getGraphics()->setTransY(v, 0);
    getGraphics()->setTransY(v, 1);
    */
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setZoom(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics()->setTransZ(v, getSelectedView());
        return;
    }

    getGraphics()->setLinkedTransZ(v);

    /*
    getGraphics()->setTransZ(v, 0);
    getGraphics()->setTransZ(v, 1);
    */
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setDefaultLight(bool linkViews)
{
    RangeImageRenderer::LightInfo li;

    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightInfo(li);
        return;
    }

    if (getRenderer(0)) getRenderer(0)->setLightInfo(li);
    if (getRenderer(1)) getRenderer(1)->setLightInfo(li);
}
//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setLightOrigin(const QVector3D& newOrigin, bool linkViews)
{
    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightOrigin(newOrigin);
        return;
    }


    if (getRenderer(0)) getRenderer(0)->setLightOrigin(newOrigin);
    if (getRenderer(1)) getRenderer(1)->setLightOrigin(newOrigin);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setLightAmb(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightAmb(v);
        return;
    }


    if (getRenderer(0))  getRenderer(0)->setLightAmb(v);
    if (getRenderer(1)) getRenderer(1)->setLightAmb(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setLightDif(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightDif(v);
        return;
    }


    if (getRenderer(0)) getRenderer(0)->setLightDif(v);
    if (getRenderer(1)) getRenderer(1)->setLightDif(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setLightSpe(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightSpe(v);
        return;
    }


    if (getRenderer(0)) getRenderer(0)->setLightSpe(v);
    if (getRenderer(1)) getRenderer(1)->setLightSpe(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::setLightShine(float s, bool linkViews)
{
    if (!linkViews)
    {
        if (getSelectedRenderer()) getSelectedRenderer()->setLightShine(s);
        return;
    }


    if (getRenderer(0)) getRenderer(0)->setLightShine(s);
    if (getRenderer(1)) getRenderer(1)->setLightShine(s);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd2::closeEvent(QCloseEvent * closeEvent)
{
    emit onClosed();
    closeEvent->accept();
}
