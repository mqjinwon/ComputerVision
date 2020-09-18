#include "kimgprocess.h"

KImgProcess::KImgProcess()
{

}

KImgProcess::~KImgProcess()
{
    delete customPlot;
}


KImageColor KImgProcess::SepiaTransform(KImageColor igColor, int pHue, double pSat)
{
    KArray<KHSV> igHSV;

    igColor.RGBtoHSV(igHSV);

    for(int nRow=0; nRow < igHSV.Row(); nRow++)
        for(int nCol=0; nCol < igHSV.Col(); nCol++){
            igHSV[nRow][nCol].h = pHue;
            igHSV[nRow][nCol].s = pSat;
        }

    return igColor.FromHSV(igHSV);

}

KImageColor KImgProcess::ContrastTransform(KImageColor &igColor, int pMin, int pMax)
{
    KImageGray igGray = igColor.ColorToGray();

    // HSV 변환
    KArray<KHSV> igHSV;
    igColor.RGBtoHSV(igHSV);

    // histogram 구하기
    KHisto hTmp;
    hTmp.Histogram(igGray);

    // 최소의 픽셀값, 최대의 픽셀값 찾기
    int _Min = 0, _Max = 0;
    for(int i=0; i < hTmp.Size(); i++){
        if(hTmp[i] != 0){
            _Min = i;
            break;
        }
    }
    for(int i=hTmp.Size()-1; i >= 0; i--){
        if(hTmp[i] != 0){
            _Max = i;
            break;
        }
    }

    for(int col=0; col < igHSV.Col(); col++){
        for(int row=0; row < igHSV.Row(); row++){

            int pValue = igGray[row][col];

            if(pValue < _Min)   igGray[row][col] = pMin;
            else if(pValue > _Max)  igGray[row][col] = pMax;
            else{
                igHSV[row][col].v = ((pMax-pMin) / (double)(_Max-_Min) * (pValue-_Min) + pMin);
            }
        }
    }

    return igColor.FromHSV(igHSV);
}

void KImgProcess::ContrastTransform(KImageGray &igGray, int pMin, int pMax)
{
    // histogram 구하기
    KHisto hTmp;
    hTmp.Histogram(igGray);

    // 최소의 픽셀값, 최대의 픽셀값 찾기
    int _Min = 0, _Max = 0;
    for(int i=0; i < hTmp.Size(); i++){
        if(hTmp[i] != 0){
            _Min = i;
            break;
        }
    }
    for(int i=hTmp.Size()-1; i >= 0; i--){
        if(hTmp[i] != 0){
            _Max = i;
            break;
        }
    }

    for(int col=0; col < igGray.Col(); col++){
        for(int row=0; row < igGray.Row(); row++){

            int pValue = igGray[row][col];

            if(pValue < _Min)   igGray[row][col] = pMin;
            else if(pValue > _Max)  igGray[row][col] = pMax;
            else{
                igGray[row][col] = (pMax-pMin) / (double)(_Max-_Min) * (pValue-_Min) + pMin;
            }
        }
    }
}

void KImgProcess::DrawHistgram(QVector<double>& x, QVector<double>& y, char* name)
{
    // x,y가 짝이 맞아야한다.
    if(x.size() != y.size()){
        return;
    }

    customPlot = new QCustomPlot;
    customPlot->resize(800, 400);

    // histogram 그리는 부분
    float xMin=std::numeric_limits<float>::max(), xMax=std::numeric_limits<float>::min(),
          yMin=std::numeric_limits<float>::max(), yMax=std::numeric_limits<float>::min();

    for(int i=0; i<x.size(); i++){
        if(x[i] < xMin)  xMin = x[i];
        if(x[i] > xMax)  xMax = x[i];
        if(y[i] < yMin)  yMin = y[i];
        if(y[i] > yMax)  yMax = y[i];
    }

    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // add title layout element:
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, name, QFont("sans", 12, QFont::Bold)));
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(xMin, xMax);
    customPlot->yAxis->setRange(yMin, yMax);
    customPlot->replot();

    customPlot->show();
}
