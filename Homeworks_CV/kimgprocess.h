#ifndef KIMGPROCESS_H
#define KIMGPROCESS_H

#include "kfc.h"
#include <qcustomplot.h>


class KImgProcess
{
private:
    QCustomPlot*    customPlot;

public:
    KImgProcess();
    ~KImgProcess();
    KImageColor SepiaTransform(KImageColor igColor, int pHue, double pSat);
    KImageColor ContrastTransform(KImageColor& igColor, int pMin=0, int pMax=255);
    void        ContrastTransform(KImageGray& igGray, int pMin=0, int pMax=255);
    void        DrawHistgram(QVector<double>& x, QVector<double>& y, char* name = "Plot");
};

#endif // KIMGPROCESS_H
