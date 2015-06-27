#ifndef DLGLIGHTING_H
#define DLGLIGHTING_H

#include <QDialog>
#include <QVector3D>

class Investigator;

namespace Ui {
class DlgLighting;
}

class DlgLighting : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgLighting(Investigator *p, QWidget *parent = 0);
    ~DlgLighting();

public slots:
    void onClose();
    void onTextEditedLightDirX(const QString &text);
    void onTextEditedLightDirY(const QString &text);
    void onTextEditedLightDirZ(const QString &text);
    void onTextEditedLightShine(const QString &text);
    void onClickLightAmb();
    void onClickLightDif();
    void onClickLightSpe();
    void onClickLightDef();

protected:
    void makeConnections();

    bool getViewsLinked();
    void setLightingDefault(bool updateRenderer);
    QVector3D getColor(const QColor &c);
    QColor getColor(const QVector3D &v);

private:
    Ui::DlgLighting *ui;
    Investigator *_pi;
};

#endif // DLGLIGHTING_H
