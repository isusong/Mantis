#ifndef QMDIMASKEDITOR_H
#define QMDIMASKEDITOR_H

#include <QMdiSubWindow>
#include <QAction>
#include <QMessageBox>
#include "SettingsStore.h"
#include "../core/RangeImage.h"

class QToolBar;
class MaskEditorWidget;

class QMdiMaskEditor : public QMdiSubWindow
{
    Q_OBJECT
public:
    explicit QMdiMaskEditor(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void init(PRangeImage img);

    void setImg(PRangeImage img);

    bool isModified();

    bool onDestroyModified();

    static PRangeImage loadMt(QMdiMaskEditor *pthis);
    static PRangeImage import(QMdiMaskEditor *pthis, RangeImage::EImgType type);
    
signals:
    
protected slots:
    void slotDocOpen();
    void slotImportTip();
    void slotImportPlt();
    void slotDocSave();
    void slotDocSaveAs();
    void slotClean();
    void slotCursorDraw();
    void slotCursorErase();
    void slotMaskClear();
    void slotSettings();

protected:
    virtual void closeEvent(QCloseEvent *closeEvent);
    void makeConnections();
    bool saveImg(const QString &file);

protected:
    QToolBar *_tb;
    QAction *_actionDocOpen;
    QAction *_actionImportTip;
    QAction *_actionImportPlt;
    QAction *_actionDocSave;
    QAction *_actionDocSaveAs;
    QAction *_actionClean;
    QAction *_actionCursorDraw;
    QAction *_actionCursorErase;
    QAction *_actionMaskClear;
    QAction *_actionSettings;
    MaskEditorWidget *_maskEditor;

    QMessageBox::StandardButton _lastSaveResult;
    
};

#endif // QMDIMASKEDITOR_H
