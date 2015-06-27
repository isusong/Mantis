#include "DlgStatResults.h"
#include "ui_DlgStatResults.h"

//=======================================================================
//=======================================================================
DlgStatResults::DlgStatResults(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgStatResults)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(slotOnBtnClose()));
}

//=======================================================================
//=======================================================================
DlgStatResults::~DlgStatResults()
{
    delete ui;
}

//=======================================================================
//=======================================================================
void DlgStatResults::updateStats(StatResults *stats)
{
    // convert points
    QVector<QPointF> pts;
    if (!stats)
    {
        ui->statPlotResults->setPlotPts(pts);
        return;
    }

    for (unsigned int i=0; i<stats->_results.size(); i++)
    {
        StatResult res = stats->_results.at(i);
        float a = res.yaw;
        float t = res.t;
        if (res.markFailed || res.compareFailed)
        {
            t = 0;
        }

        pts.push_back(QPointF(a, t));
    }


    ui->statPlotResults->setPlotPts(pts);
}

//=======================================================================
//=======================================================================
void DlgStatResults::slotOnBtnClose()
{
    hide();
}
