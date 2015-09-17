/*
 * Copyright 2008-2014 Iowa State University
 *
 * This file is part of Mantis.
 *
 * This computer software was prepared by The Ames
 * Laboratory, hereinafter the Contractor, under
 * Interagency Agreement number 2009-DN-R-119 between
 * the National Institute of Justice (NIJ) and the
 * Department of Energy (DOE). All rights in the computer
 * software are reserved by NIJ/DOE on behalf of the
 * United States Government and the Contractor as provided
 * in its Contract, DE-AC02-07CH11358.  You are authorized
 * to use this computer software for Governmental purposes
 * but it is not to be released or distributed to the public.
 * NEITHER THE GOVERNMENT NOR THE CONTRACTOR MAKES ANY WARRANTY,
 * EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE
 * OF THIS SOFTWARE.
 *
 * This notice including this sentence
 * must appear on any copies of this computer software.
 *
 * Author: Brian Bailey (bbailey@code-hammer.com)
 */

#include "SplitCmpThumbLoaderWidget.h"
#include "ui_SplitCmpThumbLoaderWidget.h"
#include <QDir>
#include <QMouseEvent>
#include "../core/logger.h"
#include "../core/utlmtfiles.h"
#include "../core/RangeImage.h"
#include "QListWidgetItemEx.h"
#include "GuiSettings.h"
#include "../core/UtlQt.h"

//=======================================================================
//=======================================================================
SplitCmpThumbLoaderWidget::SplitCmpThumbLoaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SplitCmpThumbLoaderWidget)
{
    ui->setupUi(this);

    setMinimumSize(900, 220);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    ui->listWidgetThumbs->setFlow(QListWidget::LeftToRight);
    ui->listWidgetThumbs->setWrapping(false);
    ui->listWidgetThumbs->setViewMode(QListView::IconMode);
    ui->listWidgetThumbs->setIconSize(QSize(150,150));

    ui->listWidgetThumbs->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    ui->listWidgetThumbs->setMinimumSize(900, 200);

    setContextMenuPolicy(Qt::CustomContextMenu);
}

//=======================================================================
//=======================================================================
SplitCmpThumbLoaderWidget::~SplitCmpThumbLoaderWidget()
{
    delete ui;
}

//=======================================================================
//=======================================================================
QListWidget* SplitCmpThumbLoaderWidget::getListWidgetThumbs()
{
    return ui->listWidgetThumbs;
}

//=======================================================================
//=======================================================================
bool SplitCmpThumbLoaderWidget::setProjectFolder(const QString &dirPath)
{
    UtlMtFiles::FileItemList fileItems;

    LogTrace("Attempting to set splitcmp project folder to: %s", dirPath.toStdString().c_str());
    UtlMtFiles::findFiles(dirPath, &fileItems);

    if (!fileItems.size())
    {
        LogTrace("Failed to find any project files");
        return false;
    }

    _dirPath = dirPath;
    _dirPath.replace('\\', '/');

    // add all items to the list
    ui->listWidgetThumbs->clear();

    for (unsigned int i=0; i<fileItems.size(); i++)
    {
        UtlMtFiles::PFileItem item = fileItems[i];
        loadItem(item->fileName, item->fullPathMt);
        /*
        // load the icon
        QListWidgetItem *listItem = NULL;
        RangeImage ri(item->fullPathMt, true);
        if (ri.isIconValid())
        {
            QImage icon = ri.getIcon();
            QPixmap pmap = QPixmap::fromImage(icon);
            QString ssfile("D:/dev/mantis_dev/mantis/data/test/");
            ssfile += item->fileName;
            ssfile += ".png";


            ri.getIcon().save(ssfile);
            listItem = new QListWidgetItemEx(QIcon(pmap), item->fileName, item->fullPathMt);
        }
        else
        {
            listItem = new QListWidgetItemEx(QIcon(":/general/Icons/questionmark.png"), item->fileName, item->fullPathMt);
        }


        listItem->setBackgroundColor(GuiSettings::colorListWidgetBg());
        ui->listWidgetThumbs->addItem(listItem);
        */
    }


    return true;
}


//=======================================================================
//=======================================================================
void SplitCmpThumbLoaderWidget::loadItem(const QString &fileName, const QString &fullpathMt)
{
    // load the icon
    QListWidgetItem *listItem = NULL;
    RangeImage ri(fullpathMt, true);
    if (ri.isIconValid())
    {
        QImage icon = ri.getIcon();
        QPixmap pmap = QPixmap::fromImage(icon);
        QString ssfile("D:/dev/mantis_dev/mantis/data/test/");
        ssfile += fileName;
        ssfile += ".png";


        ri.getIcon().save(ssfile);
        listItem = new QListWidgetItemEx(QIcon(pmap), fileName, fullpathMt);
    }
    else
    {
        listItem = new QListWidgetItemEx(QIcon(":/general/Icons/questionmark.png"), fileName, fullpathMt);
    }


    listItem->setBackgroundColor(GuiSettings::colorListWidgetBg());
    ui->listWidgetThumbs->addItem(listItem);
}

//=======================================================================
//=======================================================================
bool SplitCmpThumbLoaderWidget::validateInsert(const QString &filepath)
{
    // is it in the project path
    QString dirpath = _dirPath.toLower();
    QString fpath = filepath.toLower();
    fpath.replace('\\', '/');
    if (!fpath.contains(dirpath))
    {
        return false;
    }

    // do we already have the item
    for (int i=0; i<ui->listWidgetThumbs->count(); i++)
    {
        QListWidgetItemEx *item = (QListWidgetItemEx *)ui->listWidgetThumbs->item(i);
        if (!item) continue;

        QString file = item->getFile();
        if (!QString::compare(fpath, file, Qt::CaseInsensitive))
        {
            return false; // already loaded
        }
    }

    // ok add it
    QString fileName = UtlQt::fileNameWithExt(filepath);
    loadItem(fileName, filepath);
    return true;
}


