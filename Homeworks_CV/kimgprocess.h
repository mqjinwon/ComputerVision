#ifndef KIMGPROCESS_H
#define KIMGPROCESS_H

#include "kfc.h"
#include "vector"
#include "queue"
#include <qcustomplot.h>

using namespace std;

#define DEBUG 0

/* 0 : 검은색, 255 : 흰색
 * _BACKGROUND : 255(foreground), 0(background)
 * _FOREGROUND : 0(foreground), 255(background)
 */
#define		_BACKGROUND         0
#define		_FOREGROUND         1
typedef     vector<vector<int>*> _1DPIXEL;


class KImgProcess
{
private:
    QCustomPlot*    customPlot;
    // 우측부터 아래로 탐색
    int             eightNeighborRow[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    int             eightNeighborCol[8] = {1, 1, 0, -1, -1, -1, 0, 1};


public:
    KImgProcess();
    ~KImgProcess();

    KImageColor SepiaTransform(KImageColor igColor, int pHue, double pSat);

    KImageColor ContrastTransform(KImageColor& igColor, int pMin=0, int pMax=255);
    void        ContrastTransform(KImageGray& igGray, int pMin=0, int pMax=255);

    void        OtsuThreshold(KImageGray& igGray);

    KImageGray  Dilation(KImageGray& igGray, int mSize = 3, int nType = _BACKGROUND);
    KImageGray  Erosion(KImageGray& igGray, int mSize = 3, int nType = _BACKGROUND);
    KImageGray  Opening(KImageGray& igGray, int mSize = 3, int nType = _BACKGROUND);
    KImageGray  Closing(KImageGray& igGray, int mSize = 3, int nType = _BACKGROUND);

    _1DPIXEL    Labeling(KImageGray& igGray, int nType = _BACKGROUND);

    void        DrawHistgram(QVector<double>& x, QVector<double>& y, char* name = "Plot");

};

#endif // KIMGPROCESS_H
