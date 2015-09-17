#include "QMdiMaskEditor.h"
#include "MaskEditorWidget.h"
#include "DlgMaskEditorSettings.h"
#include "App.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include <QApplication>
#include "../core/UtlQt.h"

//=======================================================================
//=======================================================================
QMdiMaskEditor::QMdiMaskEditor(QWidget *parent, Qt::WindowFlags flags) :
    QMdiSubWindow(parent, flags),
    _tb(NULL),
    _actionDocOpen(NULL),
    _actionImportTip(NULL),
    _actionImportPlt(NULL),
    _actionDocSave(NULL),
    _actionDocSaveAs(NULL),
    _actionClean(NULL),
    _actionCursorDraw(NULL),
    _actionCursorErase(NULL),
    _actionMaskClear(NULL),
    _actionSettings(NULL),
    _maskEditor(NULL),
    _lastSaveResult(QMessageBox::No)
{
}


//=======================================================================
//=======================================================================
void QMdiMaskEditor::init(PRangeImage img)
{
    if (_tb != NULL) return; // already initialized

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Mask Editor");


    QWidget *widget = new QWidget(this);
    QVBoxLayout *vlayout = new QVBoxLayout(widget);
    widget->setLayout(vlayout);

    _tb = new QToolBar();
    vlayout->addWidget(_tb);

    _actionDocOpen = _tb->addAction(QIcon(":/controls/Icons/document-open.png"), "Open");
    _actionImportTip = _tb->addAction(QIcon(":/controls/Icons/import-tip.png"), "Import Tip");
    _actionImportPlt = _tb->addAction(QIcon(":/controls/Icons/import-plate.png"), "Import Plate");
    _actionImportKnf = _tb->addAction(QIcon(":/controls/Icons/import-knife.png"), "Import Knife");
    _actionImportBul = _tb->addAction(QIcon(":/controls/Icons/import-bullet.png"), "Import Bullet");
    _actionDocSave = _tb->addAction(QIcon(":/controls/Icons/document-save.png"), "Save");
    _actionDocSaveAs = _tb->addAction(QIcon(":/controls/Icons/document-save-as.png"), "Save As");
    _actionClean = _tb->addAction(QIcon(":/controls/Icons/edit-clear.png"), "Clean");
    _actionCursorDraw = _tb->addAction(QIcon(":/controls/Icons/draw-brush.png"), "Draw");
    _actionCursorErase = _tb->addAction(QIcon(":/controls/Icons/draw-eraser.png"), "Erase");
    _actionMaskClear = _tb->addAction(QIcon(":/controls/Icons/mask-clear.png"), "Clear Mask");
    _actionSettings = _tb->addAction(QIcon(":/controls/Icons/settings_36.png"), "Settings");

    _actionCursorDraw->setCheckable(true);
    _actionCursorErase->setCheckable(true);

    _maskEditor = new MaskEditorWidget();
    vlayout->addWidget(_maskEditor);

    _maskEditor->setImg(img);


    setWidget(widget);
    widget->show();

    slotCursorDraw();
    makeConnections();
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::makeConnections()
{
    bool result;
    result = connect(_actionDocOpen, SIGNAL(triggered()), this, SLOT(slotDocOpen()));
    result = connect(_actionImportTip, SIGNAL(triggered()), this, SLOT(slotImportTip()));
    result = connect(_actionImportPlt, SIGNAL(triggered()), this, SLOT(slotImportPlt()));
    result = connect(_actionImportKnf, SIGNAL(triggered()), this, SLOT(slotImportKnf()));
    result = connect(_actionImportBul, SIGNAL(triggered()), this, SLOT(slotImportBul()));
    result = connect(_actionDocSave, SIGNAL(triggered()), this, SLOT(slotDocSave()));
    result = connect(_actionDocSaveAs, SIGNAL(triggered()), this, SLOT(slotDocSaveAs()));
    result = connect(_actionClean, SIGNAL(triggered()), this, SLOT(slotClean()));
    result = connect(_actionCursorDraw, SIGNAL(triggered()), this, SLOT(slotCursorDraw()));
    result = connect(_actionCursorErase, SIGNAL(triggered()), this, SLOT(slotCursorErase()));
    result = connect(_actionMaskClear, SIGNAL(triggered()), this, SLOT(slotMaskClear()));
    result = connect(_actionSettings, SIGNAL(triggered()), this, SLOT(slotSettings()));

}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::closeEvent(QCloseEvent *closeEvent)
{
    if (!onDestroyModified())
    {
        closeEvent->ignore();
    }
    else
    {
        closeEvent->accept();
    }
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::setImg(PRangeImage img)
{
    _maskEditor->setImg(img);
}

//=======================================================================
//=======================================================================
bool QMdiMaskEditor::isModified()
{
    return _maskEditor->isModified();
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotDocOpen()
{
    PRangeImage img = loadMt(this);
    if (img.isNull()) return;

    setImg(img);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotImportTip()
{
    import(RangeImage::ImgType_Tip);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotImportPlt()
{
    import(RangeImage::ImgType_Plt);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotImportKnf()
{
    import(RangeImage::ImgType_Knf);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotImportBul()
{
    import(RangeImage::ImgType_Bul);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotDocSave()
{
    if (_maskEditor->getImg().isNull() || _maskEditor->getImg()->getFileName().size() <= 0)
    {
        slotDocSaveAs();
        return;
    }

    // make sure a clean file has an _c
    QString fname = _maskEditor->getImg()->getFileName();
    if (!fname.contains("_c."))
    {
        QString dir = UtlQt::filePath(fname);
        fname = UtlQt::fileName(fname);
        fname += "_c.mt";
        fname = UtlQt::pathCombine(dir, fname);
        _maskEditor->getImg()->setFileName(fname);
    }

    if (UtlQt::fileExists(_maskEditor->getImg()->getFileName()))
    {
        QMessageBox msgBox;
        msgBox.setText("File overwrite warning.");
        msgBox.setInformativeText("The file " + _maskEditor->getImg()->getFileName() + " already exists.\n\nDo you want to overwrite it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Cancel)
        {
            return;
        }
        if (ret == QMessageBox::No)
        {
            slotDocSaveAs();
            return;
        }
    }



    saveImg(_maskEditor->getImg()->getFileName());
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotDocSaveAs()
{
    if (_maskEditor->getImg().isNull())
    {
        QMessageBox::warning(App::mainWnd(), tr("Mask Editor"), tr("No image to save. Please open or import data first."));
        return;
    }

    QString startDir = _maskEditor->getImg()->getFileName(); // the file will be selected by the dialog, which is what we want
    if (startDir.size() <= 0)
    {
        startDir = App::settings()->inv().lastDirImport; // no file, maybe an import so you last import dir
    }

    QString fileName = QFileDialog::getSaveFileName(App::mainWnd(), tr("Save MT File"), startDir, tr("MT File (*.mt)"));
    if (fileName.size() <= 0)
    {
        _lastSaveResult = QMessageBox::Cancel;
        return; // canceled
    }

    saveImg(fileName);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotClean()
{
    //_maskEditor->clean();
    _maskEditor->cleanWithProgress();
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotCursorDraw()
{
    if (_maskEditor == NULL) return;
    _maskEditor->setViewTool(MaskEditorWidget::VT_Draw);
    _actionCursorDraw->setChecked(true);
    _actionCursorErase->setChecked(false);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotCursorErase()
{
    if (_maskEditor == NULL) return;
    _maskEditor->setViewTool(MaskEditorWidget::VT_Erase);
    _actionCursorErase->setChecked(true);
    _actionCursorDraw->setChecked(false);
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotMaskClear()
{
    _maskEditor->maskClear();
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::slotSettings()
{
    DlgMaskEditorSettings dlg;
    if (dlg.exec() == QDialog::Accepted)
    {
        if (_maskEditor == NULL) return;
        _maskEditor->refreshSettings();
    }

}

//=======================================================================
//=======================================================================
bool QMdiMaskEditor::saveImg(const QString &file)
{
    if (_maskEditor->getImg().isNull())
    {
        return false;
    }

    if (_maskEditor->saveFile(file))
    {
        _lastSaveResult = QMessageBox::Yes;
        emit onFileSaved(file);
        return true;
    }

    _lastSaveResult = QMessageBox::No;
    return false;
}

//=======================================================================
//=======================================================================
bool QMdiMaskEditor::onDestroyModified()
{
    if (!isModified())
    {
        // no modification continue with what you were doing
        return true;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "File has been modified", "The current file has been modified. Do you wish to save?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    if (reply == QMessageBox::Yes)
    {
        slotDocSave();
        if (_lastSaveResult == QMessageBox::Yes)
        {
            return true; // success
        }
        else
        {
            return false; // abort on cancel or failure
        }
    }
    else if (reply == QMessageBox::No)
    {
        return true; // don't save but continue with what you were doing
    }

    return false; // abort
}

//=======================================================================
//=======================================================================
PRangeImage QMdiMaskEditor::loadMt(QMdiMaskEditor *pthis)
{
    if (pthis != NULL)
    {
        if (!pthis->onDestroyModified())
        {
            return PRangeImage();
        }
    }

    //Get the filename.
    QString filename = QFileDialog::getOpenFileName(App::mainWnd(), tr("Open .mt file"), App::settings()->inv().lastDirClean, tr("Mantis File (*.mt)"));
    if (filename.isNull()) return PRangeImage(); //user canceled.

    //Update the last directory.
    QFileInfo fileInfo(filename);
    App::settings()->inv().lastDirClean = fileInfo.absolutePath();

     QApplication::setOverrideCursor(Qt::WaitCursor);


     PRangeImage rimg(new RangeImage(filename));
     if (rimg->isNull())
     {
         QApplication::restoreOverrideCursor();

         QMessageBox::warning(App::mainWnd(), tr("Error"), tr("File failed to open."));
         return PRangeImage();
     }

     //loadMaskEditor(rimg);

     QApplication::restoreOverrideCursor();

     return rimg;
}

//=======================================================================
//=======================================================================
void QMdiMaskEditor::import(RangeImage::EImgType type)
{
    PRangeImage img = import(this, type);
    if (img.isNull()) return;

    setImg(img);
    _maskEditor->setModified(true);
}

//=======================================================================
//=======================================================================
PRangeImage QMdiMaskEditor::import(QMdiMaskEditor *pthis, RangeImage::EImgType type)
{
    if (pthis != NULL)
    {
        if (!pthis->onDestroyModified())
        {
            return PRangeImage();
        }
    }

    QString openFileTitle, openTxTitle, openQltTitle;
    QString typeName = "Tip";

    switch (type)
    {
    case RangeImage::ImgType_Plt:
        typeName = "Plate";
        break;
    case RangeImage::ImgType_Knf:
        typeName = "Knife";
        break;
    case RangeImage::ImgType_Bul:
        typeName = "Bullet";
        break;
    }

    openFileTitle = QString(tr("Import AL3D %1 File")).arg(typeName);
    openTxTitle = QString(tr("Import AL3D %1 Texture")).arg(typeName);
    openQltTitle = QString(tr("Import AL3D %1 Quality Map")).arg(typeName);

    QString filenameAl3d = QFileDialog::getOpenFileName(App::mainWnd(), openFileTitle, App::settings()->inv().lastDirImport, tr("AL3D (*.al3d);;All files (*.*)" ));
        //tr("All files (*.*);;AL3D (*.al3d);;XYZM (*.xyzm)" ));
    if (filenameAl3d.isNull()) return PRangeImage(); //user canceled.

    //Update the last directory.
    QFileInfo fileInfo(filenameAl3d);
    App::settings()->inv().lastDirImport = fileInfo.absolutePath();

    // image extensions for texture and quality maps
    std::vector<QString> exts; exts.push_back("bmp"); exts.push_back("png");

    // find the texture
    QString filenameTx;
    for (size_t i=0; i<exts.size(); i++)
    {
        QString name("texture.");
        name += exts[i];
        filenameTx = UtlQt::pathCombine(fileInfo.absolutePath(), name);
        if (UtlQt::fileExists(filenameTx)) break;

        filenameTx = "";
    }

    if (filenameTx.size() <= 0)
    {

        filenameTx = QFileDialog::getOpenFileName(App::mainWnd(), openTxTitle, App::settings()->inv().lastDirImport,
            tr("Image Files (*.jpg *.jpeg *.png *.bmp *.tif *.tiff);;BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg *.jpeg);;All files (*.*)" ));

        if (filenameTx.isNull()) return PRangeImage(); //user canceled.
    }

    // find the qualitymap
    QString filenameQlt;
    for (size_t i=0; i<exts.size(); i++)
    {
        QString name("qualitymap.");
        name += exts[i];
        filenameQlt = UtlQt::pathCombine(fileInfo.absolutePath(), name);
        if (UtlQt::fileExists(filenameQlt)) break;

        filenameQlt = "";
    }

    if (filenameQlt.size() <= 0)
    {

        filenameQlt = QFileDialog::getOpenFileName(App::mainWnd(), openQltTitle, App::settings()->inv().lastDirImport,
            tr("Image Files (*.jpg *.jpeg *.png *.bmp *.tif *.tiff);;JPEG (*.jpg *.jpeg);;PNG (*.png);;All files (*.*)" ));

        if (filenameQlt.isNull())
        {
            filenameQlt = "";
        }
    }


    QApplication::setOverrideCursor(Qt::WaitCursor);
    RangeImage *pimg = RangeImage::importFromAl3d(filenameAl3d, filenameTx);
    if (pimg == NULL)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::warning(App::mainWnd(), tr("Error"), tr("File failed to import."));
        return PRangeImage();
    }

    PRangeImage rimg(pimg);
    rimg->setImgType(type);
    rimg->setFileName(""); // empty file name on import

    QString fpath = UtlQt::filePath(filenameAl3d);
    QString fname = UtlQt::fileName(filenameAl3d);
    fname += ".mt";
    fname = UtlQt::pathCombine(fpath, fname);
    rimg->setFileName(fname);

    rimg->setQualityMapFile(filenameQlt);

    QApplication::restoreOverrideCursor();

    return rimg;
}
