#include "SettingsStore.h"
#include <QSettings>
#include <QDockWidget>

//=======================================================================
//=======================================================================
void SettingsStore::loadAll()
{
    loadInvestigator();
    loadMarkOpt();
    loadMaskEditor();
}

//=======================================================================
//=======================================================================
void SettingsStore::loadInvestigator()
{
    loadInvestigator(&_inv);
}

//=======================================================================
//=======================================================================
void SettingsStore::loadMarkOpt()
{
    loadMarkOpt(&_mark);
}

//=======================================================================
//=======================================================================
void SettingsStore::loadMaskEditor()
{
    loadMaskEditor(&_mask);
}

//=======================================================================
//=======================================================================
void SettingsStore::saveAll()
{
    saveInvestigator();
    saveMarkOpt();
    saveMaskEditor();
}

//=======================================================================
//=======================================================================
void SettingsStore::saveInvestigator()
{
    saveInvestigator(_inv);
}

//=======================================================================
//=======================================================================
void SettingsStore::saveMarkOpt()
{
    saveMarkOpt(_mark);
}

//=======================================================================
//=======================================================================
void SettingsStore::saveMaskEditor()
{
    saveMaskEditor(_mask);
}

//=======================================================================
//=======================================================================
void SettingsStore::saveInvestigator(const InvSettings &inv)
{
    QSettings settings;
    settings.beginGroup("investigator");
    settings.setValue("lastDir",inv.lastDir);
    settings.setValue("lastDirClean",inv.lastDirClean);
    settings.setValue("lastDirImport",inv.lastDirImport);
    settings.setValue("lastDirSet", inv.lastDirSet);
    settings.setValue("autoUpdateStatsRT", inv.autoUpdateStatsRT);
    settings.setValue("showStatPlots", inv.showStatPlots);
    settings.setValue("showStartupDlg", inv.showStartupDlg);
    settings.setValue("startupMode", inv.startupMode);
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::loadInvestigator(InvSettings *inv)
{
    QSettings settings;
    settings.beginGroup("investigator");
    inv->lastDir = settings.value("lastDir", inv->lastDir).toString();
    inv->lastDirClean = settings.value("lastDirClean", inv->lastDirClean).toString();
    inv->lastDirImport = settings.value("lastDirImport", inv->lastDirImport).toString();
    inv->lastDirSet = settings.value("lastDirSet", inv->lastDirSet).toBool();
    inv->autoUpdateStatsRT = settings.value("autoUpdateStatsRT", inv->autoUpdateStatsRT).toBool();
    inv->showStatPlots = settings.value("showStatPlots", inv->showStatPlots).toBool();
    inv->showStartupDlg = settings.value("showStartupDlg", inv->showStartupDlg).toBool();
    inv->startupMode = settings.value("startupMode", inv->startupMode).toInt();
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::saveDocStates(QMainWindow *pWin, QDockWidget *doc)
{
    if (!doc) return;
    if (!doc->isVisible()) return;

    QSettings settings;
    settings.beginGroup(doc->windowTitle());
    settings.setValue("pos", doc->pos());
    settings.setValue("floating", doc->isFloating());
    settings.setValue("docWidgetArea", pWin->dockWidgetArea(doc));
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::loadDocStates(QMainWindow *pWin, QDockWidget *doc)
{
    if (!doc) return;

    QSettings settings;
    settings.beginGroup(doc->windowTitle());

    bool floating = settings.value("floating", doc->isFloating()).toBool();
    doc->setFloating(floating);

    if (floating)
    {
        QPoint pt = settings.value("pos", doc->pos()).toPoint();
        doc->move(pt);
    }
    else
    {
        Qt::DockWidgetArea docArea = (Qt::DockWidgetArea)settings.value("docWidgetArea", Qt::NoDockWidgetArea).toInt();
        pWin->addDockWidget(docArea, doc);
    }
}

//=======================================================================
//=======================================================================
void SettingsStore::saveMarkOpt(const MarkOptSettings &mark)
{
    QSettings settings;
    settings.beginGroup("markopt");
    settings.setValue("yawMin", mark.yawMin);
    settings.setValue("yawMax", mark.yawMax);
    settings.setValue("yawInc", mark.yawInc);
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::loadMarkOpt(MarkOptSettings *mark)
{
    QSettings settings;
    settings.beginGroup("markopt");
    mark->yawMin = settings.value("yawMin", mark->yawMin).toInt();
    mark->yawMax = settings.value("yawMax", mark->yawMax).toInt();
    mark->yawInc = settings.value("yawInc", mark->yawInc).toInt();
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::saveMaskEditor(const MaskEditorSettings &s)
{
    QSettings settings;
    settings.beginGroup("maskeditor");
    settings.setValue("szBrush", s.szBrush);
    settings.endGroup();
}

//=======================================================================
//=======================================================================
void SettingsStore::loadMaskEditor(MaskEditorSettings *s)
{
    QSettings settings;
    settings.beginGroup("maskeditor");
    s->szBrush = settings.value("szBrush", s->szBrush).toFloat();
    settings.endGroup();
}
