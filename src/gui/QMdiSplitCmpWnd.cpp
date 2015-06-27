#include "QMdiSplitCmpWnd.h"
#include "../core/logger.h"
#include "SplitCmpViewCtrlsWidget.h"

//=======================================================================
//=======================================================================
QMdiSplitCmpWnd::QMdiSplitCmpWnd(SplitCmpViewCtrlsWidget *pvc, QWidget * parent, Qt::WindowFlags flags) :
    QMdiSubWindow(parent, flags),
    _pvc(pvc)
{
    _splitter = NULL;
    _imgViewer0 = NULL;
    _imgViewer1 = NULL;

    _guid = QUuid::createUuid();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::init()
{
    if (_splitter) return;

    setAttribute(Qt::WA_DeleteOnClose);

    _splitter = new QSplitter;
    _splitter->setHandleWidth(1);

    setWidget(_splitter);
    setWindowTitle("Split Comparison");
    //setProperty("guid", QUuid().toString());

    // DEBUG

    PRangeImage img;

    /*
    QString file = "D:/dev/mantis/mantis/data/P4B-45-2$3D/P4B-45-2.mt";
    img = PRangeImage(new RangeImage(file));
    if (img->isNull())
    {
        LogError("Failed to to load file: %s", file.toStdString().c_str());
        img.clear();
    }
    */


    // DEBUG END


    _imgViewer0 = new RangeImageViewer(img, false, false, -1, 500, 500, _splitter);
    _splitter->addWidget(_imgViewer0);
    _imgViewer0->getGraphicsWidget()->setWidgetLoc(GraphicsWidget2::Left);
    _imgViewer0->show();

    _imgViewer1 = new RangeImageViewer(PRangeImage(), false, false, -1, 500, 500, _splitter);
    _splitter->addWidget(_imgViewer1);
    _imgViewer1->getGraphicsWidget()->setWidgetLoc(GraphicsWidget2::Right);
    _imgViewer1->show();


    _splitter->show();

    makeConnections();
}

//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd::loadRangeImg(const QString &filename)
{
    if (!_imgViewer0 || !_imgViewer1) return false;

    int viewer = 0;
    if (_imgViewer1->getIsWindowSelected())
    {
        viewer = 1;
    }

    return loadRangeImg(filename, viewer);
}


//=======================================================================
//=======================================================================
bool QMdiSplitCmpWnd::loadRangeImg(const QString &filename, int viewer)
{
    if (!_imgViewer0 || !_imgViewer1) return false;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    PRangeImage rimg(new RangeImage(filename));
    if (rimg->isNull())
    {
        QApplication::restoreOverrideCursor();
        LogError("Failed to to load file: %s", filename.toStdString().c_str());
        return false;
    }

    if (viewer < 0) viewer = 0;
    if (viewer > 1) viewer = 1;

    if (viewer == 0)
    {
        _imgViewer0->setModel(rimg);
    }
    else
    {
        _imgViewer1->setModel(rimg);
    }

    QApplication::restoreOverrideCursor();
    return true;
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setRangeImg(PRangeImage rimg, int viewer)
{
    if (viewer == 0)
    {
        _imgViewer0->setModel(rimg);
    }
    else
    {
        _imgViewer1->setModel(rimg);
    }
}

//=======================================================================
//=======================================================================
int QMdiSplitCmpWnd::getSelectedView()
{
    if (_imgViewer1->getIsWindowSelected()) return 1;

    return 0;
}

//=======================================================================
//=======================================================================
GraphicsWidget2* QMdiSplitCmpWnd::getSelectedGraphics()
{
    if (getSelectedView() == 1)
    {
        return _imgViewer1->getGraphicsWidget();
    }
    else
    {
        return _imgViewer0->getGraphicsWidget();
    }
}

//=======================================================================
//=======================================================================
GraphicsWidget2* QMdiSplitCmpWnd::getGraphics(int iviewer)
{
    if (iviewer == 1)
    {
        return _imgViewer1->getGraphicsWidget();
    }
    else
    {
        return _imgViewer0->getGraphicsWidget();
    }
}

//=======================================================================
//=======================================================================
RangeImageRenderer* QMdiSplitCmpWnd::getSelectedRenderer()
{
    if (getSelectedView() == 1)
    {
        return _imgViewer1->getRenderer();
    }
    else
    {
        return _imgViewer0->getRenderer();
    }
}

//=======================================================================
//=======================================================================
RangeImageRenderer* QMdiSplitCmpWnd::getRenderer(int iviewer)
{
    if (iviewer == 1)
    {
        return _imgViewer1->getRenderer();
    }
    else
    {
        return _imgViewer0->getRenderer();
    }
}
//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::linkViews()
{
    int link = 1;
    int view = getSelectedView();
    if (view == 1) link = 0;

    GraphicsWidget2 *mg = getGraphics(view);
    GraphicsWidget2 *lg = getGraphics(link);
    RangeImageRenderer *mr = getRenderer(view);
    RangeImageRenderer *lr = getRenderer(link);

    lg->setRot(mg->getRot());
    lg->setTrans(mg->getTrans());

    lr->setLightInfo(mr->getLightInfo());
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setDefaultView(bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setRot(QVector3D(0,0,0));
        getGraphics(getSelectedView())->setTrans(QVector3D(0,0,0));
        return;
    }

    getGraphics(0)->setRot(QVector3D(0,0,0));
    getGraphics(0)->setTrans(QVector3D(0,0,0));
    getGraphics(1)->setRot(QVector3D(0,0,0));
    getGraphics(1)->setTrans(QVector3D(0,0,0));
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setYaw(double deg, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setYaw(deg);
        return;
    }

    getGraphics(0)->setYaw(deg);
    getGraphics(1)->setYaw(deg);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setPitch(double deg, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setPitch(deg);
        return;
    }

    getGraphics(0)->setPitch(deg);
    getGraphics(1)->setPitch(deg);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setPanH(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setTransX(v);
        return;
    }

    getGraphics(0)->setTransX(v);
    getGraphics(1)->setTransX(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setPanV(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setTransY(v);
        return;
    }

    getGraphics(0)->setTransY(v);
    getGraphics(1)->setTransY(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setZoom(double v, bool linkViews)
{
    if (!linkViews)
    {
        getGraphics(getSelectedView())->setTransZ(v);
        return;
    }


    getGraphics(0)->setTransZ(v);
    getGraphics(1)->setTransZ(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setDefaultLight(bool linkViews)
{
    RangeImageRenderer::LightInfo li;

    if (!linkViews)
    {
        getSelectedRenderer()->setLightInfo(li);
        return;
    }

    getRenderer(0)->setLightInfo(li);
    getRenderer(1)->setLightInfo(li);
}
//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setLightOrigin(const QVector3D& newOrigin, bool linkViews)
{
    if (!linkViews)
    {
        getSelectedRenderer()->setLightOrigin(newOrigin);
        return;
    }


    getRenderer(0)->setLightOrigin(newOrigin);
    getRenderer(1)->setLightOrigin(newOrigin);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setLightAmb(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        getSelectedRenderer()->setLightAmb(v);
        return;
    }


    getRenderer(0)->setLightAmb(v);
    getRenderer(1)->setLightAmb(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setLightDif(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        getSelectedRenderer()->setLightDif(v);
        return;
    }


    getRenderer(0)->setLightDif(v);
    getRenderer(1)->setLightDif(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setLightSpe(const QVector3D& v, bool linkViews)
{
    if (!linkViews)
    {
        getSelectedRenderer()->setLightSpe(v);
        return;
    }


    getRenderer(0)->setLightSpe(v);
    getRenderer(1)->setLightSpe(v);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::setLightShine(float s, bool linkViews)
{
    if (!linkViews)
    {
        getSelectedRenderer()->setLightShine(s);
        return;
    }


    getRenderer(0)->setLightShine(s);
    getRenderer(1)->setLightShine(s);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::closeEvent(QCloseEvent * closeEvent)
{
    emit onClosed();
    closeEvent->accept();
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::makeConnections()
{
    connect(_imgViewer0, SIGNAL(onLButtonDown(int, int)), this, SLOT(onLButtonDownViewer0(int, int)));
    connect(_imgViewer1, SIGNAL(onLButtonDown(int, int)), this, SLOT(onLButtonDownViewer1(int, int)));
    connect(_imgViewer0->getGraphicsWidget(), SIGNAL(onChangedTranslation(QVector3D)), this, SLOT(onTranslationViewer0(QVector3D)));
    connect(_imgViewer1->getGraphicsWidget(), SIGNAL(onChangedTranslation(QVector3D)), this, SLOT(onTranslationViewer1(QVector3D)));
    connect(_imgViewer0->getGraphicsWidget(), SIGNAL(onChangedRotation(QVector3D)), this, SLOT(onRotationViewer0(QVector3D)));
    connect(_imgViewer1->getGraphicsWidget(), SIGNAL(onChangedRotation(QVector3D)), this, SLOT(onRotationViewer1(QVector3D)));
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onLButtonDownViewer0(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    if (_imgViewer0->getIsWindowSelected()) return;

    _imgViewer0->setWindowSelected(true);
    _imgViewer1->setWindowSelected(false);
    //if (_pvc) _pvc->refreshGui(this);
}


//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onLButtonDownViewer1(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    if (_imgViewer1->getIsWindowSelected()) return;

    _imgViewer1->setWindowSelected(true);
    _imgViewer0->setWindowSelected(false);
    //if (_pvc) _pvc->refreshGui(this);
}


//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onTranslationViewer0(QVector3D trans)
{
    emit onChangedTranslation(0, trans);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onRotationViewer0(QVector3D rot)
{
    emit onChangedRotation(0, rot);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onTranslationViewer1(QVector3D trans)
{
    emit onChangedTranslation(1, trans);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::onRotationViewer1(QVector3D rot)
{
    emit onChangedRotation(1, rot);
}

//=======================================================================
//=======================================================================
void QMdiSplitCmpWnd::mousePressEvent(QMouseEvent *event)
{
    QMdiSubWindow::mousePressEvent(event);
}
