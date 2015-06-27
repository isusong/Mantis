#ifndef SETTINGSSTORE_H
#define SETTINGSSTORE_H

#include <QString>
#include <QDir>
#include <QMainWindow>

class SettingsStore
{
public:

    enum StartUpMode
    {
        StartUp_MaskEditor = 1,
        StartUp_SplitCmp = 2,
    };

    struct InvSettings
    {
        QString lastDir;
        QString lastDirClean;
        QString lastDirImport;
        bool lastDirSet;
        bool autoUpdateStatsRT;
        bool showStatPlots;

        bool showStartupDlg;
        int startupMode;

        InvSettings()
        {
            lastDir = QDir::currentPath();
            lastDirClean = QDir::currentPath();
            lastDirImport = QDir::currentPath();
            lastDirSet = false;
            autoUpdateStatsRT = true;
            showStatPlots = false;

            showStartupDlg = true;
            startupMode = StartUp_MaskEditor;
        }
    };

    struct MarkOptSettings
    {
        int yawMin;
        int yawMax;
        int yawInc;

        MarkOptSettings(int iYawMin=25, int iYawMax=85, int iYawInc=5)
        {
            yawMin = iYawMin;
            yawMax = iYawMax;
            yawInc = iYawInc;
        }
    };

    struct MaskEditorSettings
    {
        float szBrush;

        MaskEditorSettings()
        {
           szBrush = 1.0f;
        }
    };


public:

    SettingsStore() {}

    void loadAll();
    void loadInvestigator();
    void loadMarkOpt();
    void loadMaskEditor();
    static void loadDocStates(QMainWindow *pWin, QDockWidget *doc);

    void saveAll();
    void saveInvestigator();
    void saveMarkOpt();
    void saveMaskEditor();
    static void saveDocStates(QMainWindow *pWin, QDockWidget *doc);

    InvSettings& inv() { return _inv; }
    MarkOptSettings& mark() { return _mark; }
    MaskEditorSettings& mask() { return _mask; }

protected:
    void saveInvestigator(const InvSettings &inv);
    void loadInvestigator(InvSettings *inv);

    void saveMarkOpt(const MarkOptSettings &mark);
    void loadMarkOpt(MarkOptSettings *mark);

    void saveMaskEditor(const MaskEditorSettings &s);
    void loadMaskEditor(MaskEditorSettings *s);

protected:
    InvSettings _inv;
    MarkOptSettings _mark;
    MaskEditorSettings _mask;
};

#endif // SETTINGSSTORE_H
