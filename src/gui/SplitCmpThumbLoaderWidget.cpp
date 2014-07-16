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
#include "../core/logger.h"
#include "../core/utlqt.h"
#include "QListWidgetItemEx.h"

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
    FileItemList fileItems;

    findFiles(dirPath, &fileItems);

    if (!fileItems.size())
    {
        LogTrace("Failed to find any project files");
        return false;
    }

    // add all items to the list
    ui->listWidgetThumbs->clear();

    for (unsigned int i=0; i<fileItems.size(); i++)
    {
        PFileItem item = fileItems[i];
        QListWidgetItem *listItem = new QListWidgetItemEx(QIcon(item->fullPathIcon), item->folderName, item->fullPathMt);
        //listItem->setData(Qt::UserRole + 1, QVariant(item->fullPathMt));
        ui->listWidgetThumbs->addItem(listItem);
    }


    return true;
}

//=======================================================================
//=======================================================================
void SplitCmpThumbLoaderWidget::findFiles(const QString &dirPath, FileItemList *plist)
{
    QDir dir(dirPath);
    QStringList iconFiles = dir.entryList(QStringList("icon.png"));
    QStringList mtFiles = dir.entryList(QStringList("*.mt"));

    // validate, should be a single icon.png and singe .mt file in each project folder
    if (iconFiles.size() > 1)
    {
       LogTrace("UnExpected: multiple icon.png files found in folder: %s", dirPath.toStdString().c_str());
    }

    if (mtFiles.size() > 1)
    {
        LogTrace("UnExpected: multiple mt files found in folder: %s", dirPath.toStdString().c_str());
    }

    if (mtFiles.size() > 0 && iconFiles.size() <= 0)
    {
        LogTrace("UnExpected: no icon.png found in folder: %s", dirPath.toStdString().c_str());
    }

    if (mtFiles.size() > 0)
    {
        // should only be a single project file, but add them all anyway
        for (int i=0; i<mtFiles.size(); i++)
        {
            PFileItem item(new FileItem);
            item->folderName = dir.dirName();
            item->fullPathMt = UtlQt::pathCombine(dir.absolutePath(), mtFiles[i]);
            if (iconFiles.size())
            {
                item->fullPathIcon = UtlQt::pathCombine(dir.absolutePath(), iconFiles[0]);
            }
            else
            {
                item->fullPathIcon = ":/general/Icons/questionmark.png";
            }

            plist->push_back(item);
        }
    }



    QString curDir = QDir::cleanPath(dirPath);

    // find any sub directories and recurse
    QFileInfoList subDirs = dir.entryInfoList(QDir::AllDirs);
    for (int i=0; i<subDirs.size(); i++)
    {
        if (!subDirs[i].isDir()) continue;

        QString name = subDirs[i].fileName();
        if (name == "." || name == "..")
        {
            continue;
        }

        QString subDir = subDirs[i].absoluteFilePath();
        if (QDir::cleanPath(subDir) == curDir)
        {
            continue;
        }

        findFiles(subDir, plist);
    }
}


