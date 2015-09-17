#ifndef QMDISPLITCMPWND2_H
#define QMDISPLITCMPWND2_H

#include <QMdiSubWindow>
#include "RangeImageViewer.h"
#include "qwt-plots/StatPlot.h"

class QMdiSplitCmpWnd2 : public QMdiSubWindow
{
   Q_OBJECT

public:
    enum EView
    {
        ViewLeft = 0,
        ViewRight = 1
    };

    struct ProInfo
    {
        QString filename;
        QString fullpath;
        int dataType;
        QVector3D tipAng;
        int plateCol;
        PProfile profile;

        ProInfo()
        {
            dataType = RangeImage::ImgType_Unk;
            tipAng = QVector3D(0,0,0);
            plateCol = 0;
        }
    };

public:
    QMdiSplitCmpWnd2(QWidget * parent, Qt::WindowFlags flags = 0);

    void init();

    bool markAvailable(RangeImageRenderer **tip=NULL, RangeImageRenderer **plate=NULL);
    bool getMarkMode();
    bool setMarkMode(bool marked);
    //bool mark();
    //bool unmark();
    bool haveTip();

    void updateProfilesAndStats();
    void updateProfiles();
    void setProfile(int iviewer, PProfile pro);

    bool loadRangeImg(const QString &filename);
    bool loadRangeImg(const QString &filename, int viewer);
    void setRangeImg(PRangeImage img, int viewer);

    RangeImageViewer* getViewer() { return _imgViewer; }
    GraphicsWidget2* getGraphics() { if (!_imgViewer) return NULL; return _imgViewer->getGraphicsWidget(); }
    int getSelectedView();
    RangeImageRenderer* getSelectedRenderer();
    RangeImageRenderer* getRenderer(int iviewer=ViewLeft);
    PRangeImage getModel(int iviewer=ViewLeft);
    void toggleSelectedView();

    void showPlots(bool show);

    void linkViews(bool on);
    void enableViewLink(GraphicsWidget2::LinkView type);
    void disableViewLink(GraphicsWidget2::LinkView type);
    void setDefaultView();
    void setYaw(double deg, bool linkViews);
    void setPitch(double deg, bool linkViews);
    void setRoll(double deg, bool linkViews);
    void setPanH(double v, bool linkViews);
    void setPanV(double v, bool linkViews);
    void setZoom(double v, bool linkViews);

    void setDefaultLight(bool linkViews);
    void setLightOrigin(const QVector3D& newOrigin, bool linkViews);
    void setLightAmb(const QVector3D& v, bool linkViews);
    void setLightDif(const QVector3D& v, bool linkViews);
    void setLightSpe(const QVector3D& v, bool linkViews);
    void setLightShine(float s, bool linkViews);


public slots:
    void slotCmpSliderReleased();
    void slotSplitStatSelectionUpdated();
    void slotSetSearchWindow1(int loc, int width, int dataLen);
    void slotSetSearchWindow2(int loc, int width, int dataLen);
    void slotChangedTranslationMouse(int viewer, const QVector3D &v);
    void slotChangedRotationMouse(int viewer, const QVector3D &v);

signals:
    void onClosed();
    void onStatsProfileUpdated(PProfile p1, PProfile p2);
    void updateStatsRT();
    void onChangedTranslationMouse(int viewer, const QVector3D &v);
    void onChangedRotationMouse(int viewer, const QVector3D &v);

protected:
    virtual void closeEvent(QCloseEvent *closeEvent);

    void makeConnections();

    PProfile& getProfile(int num=0);
    void setProfileAxisText(int viewer);

protected:
    RangeImageViewer* _imgViewer;
    StatPlot* _statPlots[2];
    QWidget *_plots;
    QLabel *_labelFileL;
    QLabel *_labelFileR;

    //Data objects.
    //PProfile _profile0; // virtual mark for model 1
    //PProfile _profile1; // virtual mark for model 2
    ProInfo _proInfo[2];

    bool _markModeOn;

    bool _allowProfileUpd;
};

#endif // QMDISPLITCMPWND2_H
