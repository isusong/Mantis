#ifndef APP_H
#define APP_H

class QMainWindow;
class SettingsStore;

class App
{
public:
    static void mainWnd(QMainWindow *w);
    static QMainWindow* mainWnd();

    static void settings(SettingsStore *s);
    static SettingsStore* settings();


protected:
    static QMainWindow *_mainWnd;
    static SettingsStore *_settings;
};

#endif // APP_H
