#ifndef KIMGPROCESS_H
#define KIMGPROCESS_H

#include "kfc.h"
#include <vector>
#include <queue>
#include <stack>
#include <qcustomplot.h>
#include <fstream>
#include "Eigen/Dense"

using namespace std;

#define DEBUG 1

/* 0 : 검은색, 255 : 흰색
 * _BACKGROUND : 255(foreground), 0(background)
 * _FOREGROUND : 0(foreground), 255(background)
 */
#define		_BACKGROUND         0
#define		_FOREGROUND         1

// edge 관련 정보이다.
struct EdgeData{
    double dx;
    double dy;
    double magn;
    unsigned char val;
    int    dir;     // 8개의 방향이 존재할 수 있다.
};

typedef     vector<vector<int>*>        _1DPIXEL;
typedef     vector<vector<double>>      _JMAT;
typedef     vector<double>              _JCOL;
typedef     vector<vector<EdgeData>*>   _EDGEIMG;   //edge 정보를 담은 이미지 데이터이다.
typedef     vector<pair<int, int>>      _POINTS;   //좌표 정보를 담은 데이터이다.


template<typename T> class JMatrix{
public:

    vector<vector<T>>  AddMatrix(const vector<vector<T>> &a, const vector<vector<T>> &b){
        if(a.empty() || b.empty())  exit(0);
        if(a.size() != b.size() || a[0].size() != b[0].size()) exit(1);

        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a[0].size(), 0.);
        resultMatrix.assign(a.size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < b[0].size(); nCol++){

                resultMatrix[nRow][nCol] = a[nRow][nCol] + b[nRow][nCol];
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  MinusMatrix(const vector<vector<T>> &a, const vector<vector<T>> &b){
        if(a.empty() || b.empty())  exit(0);
        if(a.size() != b.size() || a[0].size() != b[0].size()) exit(1);

        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a[0].size(), 0.);
        resultMatrix.assign(a.size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < b[0].size(); nCol++){

                resultMatrix[nRow][nCol] = a[nRow][nCol] - b[nRow][nCol];
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  MultiMatrix(const vector<vector<T>> &a, const vector<vector<T>> &b){
        if(a.empty() || b.empty())  exit(0);
        if(a.size() != b.size() || a[0].size() != b[0].size()) exit(1);

        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a[0].size(), 0.);
        resultMatrix.assign(a.size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < b[0].size(); nCol++){

                resultMatrix[nRow][nCol] = a[nRow][nCol] * b[nRow][nCol];
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  DivideMatrix(const vector<vector<T>> &a, const vector<vector<T>> &b){
        if(a.empty() || b.empty())  exit(0);
        if(a.size() != b.size() || a[0].size() != b[0].size()) exit(1);

        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a[0].size(), 0.);
        resultMatrix.assign(a.size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < b[0].size(); nCol++){

                resultMatrix[nRow][nCol] = a[nRow][nCol] / b[nRow][nCol];
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  DotProduct(const vector<vector<T>> &a, const vector<vector<T>> &b){
        if(a.empty() || b.empty())  exit(0);
        if(a[0].size() != b.size()) exit(1);

        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(b[0].size(), 0.);
        resultMatrix.assign(a.size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < b[0].size(); nCol++){
                T tmp = 0;
                for(int mult = 0;  mult < b.size(); mult++){
                    tmp += a[nRow][mult] * b[mult][nCol];
                }

                resultMatrix[nRow][nCol] = tmp;
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  TransposeMatrix(const vector<vector<T>> &a){
        if(a.empty())  exit(0);


        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a.size(), 0.);
        resultMatrix.assign(a[0].size(), tmp);

        for(int nRow=0; nRow < a.size(); nRow++){
            for(int nCol=0; nCol < a[0].size(); nCol++){

                resultMatrix[nCol][nRow] = a[nRow][nCol];
            }
        }

        return resultMatrix;
    }

    vector<vector<T>>  Inverse2Matrix(const vector<vector<T>> &a){
        if(a.empty())  exit(0);
        if(a.size() !=2 || a[0].size() !=2) exit(1);

        //행과 열 할당
        vector<vector<T> > resultMatrix; vector<double> tmp;
        tmp.assign(a.size(), 0.);
        resultMatrix.assign(a[0].size(), tmp);

        double det = a[0][0] * a[1][1] -a[0][1]* a[1][0]; // ad-bc

        if(det != 0){
            resultMatrix[0][0] = 1. / det * a[1][1];
            resultMatrix[0][1] = -1. / det * a[0][1];
            resultMatrix[1][0] = -1. / det * a[1][0];
            resultMatrix[1][1] = 1. / det * a[0][0];
        }

        return resultMatrix;
    }
};

class KImgProcess
{
private:
    QCustomPlot*    customPlot;

    // 우측부터 아래로 탐색
    int             eightNeighborRow[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    int             eightNeighborCol[8] = {1, 1, 0, -1, -1, -1, 0, 1};

    double          gaussianVoting5[5][5];
    double          gaussianVoting11[11][11];

    KRandom*        Random;
    KGaussian*      GRandom;

    _EDGEIMG        edgeData;   // edge 정보를 담은 이미지 데이터이다.

    KImageGray      resultIgImg;
    KImageColor     resultIcImg;

    JMatrix<double> jMatrix;

public:

    KImgProcess(double sigma=0.3);
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

    void        HistogramEqualization(KImageGray& igImg);
    void        HistogramEqualization(KImageColor& icImg);
    void        HistogramMatching(KImageGray& oriImg, KImageGray& dstImg);
    void        HistogramMatching(KImageColor& oriImg, KImageColor& dstImg);

    KImageGray  GaussianNoiseToGrayImage(KImageGray& igImg, double mean, double var);
    KImageColor GaussianNoiseToColorImage(KImageColor& icImg, double* mean, double* var);
    KImageGray  PepperSaltToGrayImage(KImageGray& igImg, double percent = 0.1);
    KImageColor PepperSaltToColorImage(KImageColor& icImg, double percent = 0.1);
    KImageGray  GaussianSmoothing(KImageGray& igImg, const double& sigma = 0.3);
    KImageColor GaussianSmoothing(KImageColor& icImg, const double& sigma = 0.3);
    KImageGray  MedianFiltering(KImageGray& igImg, const int kSize = 3);
    KImageColor MedianFiltering(KImageColor& icImg, const int kSize = 3);

    KImageGray  FDG(KImageGray& igImg, const double& sigma = 0.3);
    KImageGray  CannyEdge(KImageGray& igImg, double t0 = 10, double t1 = 30, const double &sigma=0.3);

    _POINTS     CircleHoughTransform(KImageGray& igImg, double mRadius, double threshold);
    _POINTS     CircleHoughTransform(KImageGray& igImg, double mMinR, double mMaxR, double threshold);
    _POINTS     GeneralHoughTransform(KImageGray& igImg, string dir, double threshold);

    _EDGEIMG    OpticalFlow(KImageGray& igImg1, KImageGray& igImg2, int iter = 100);
    vector<vector<double>>  GetValMatrix(const _EDGEIMG& matrix);

    void        DrawHistgram(QVector<double>& x, QVector<double>& y, char* name = "Plot");
    KImageGray  GetResultIgImg(){return resultIgImg;}
    KImageColor GetResultIcImg(){return resultIcImg;}


    _EDGEIMG    showGradient(){return edgeData;};

};

class ImgPyramid{
private:
    KImageGray _igImg;
    int _rowSize;
    int _colSize;
    vector<_EDGEIMG> pyImg;

public:

    ImgPyramid(){};
    ImgPyramid(KImageGray & igImg) : _igImg(igImg){_rowSize = _igImg.Row(); _colSize = _igImg.Col();}

    void Create(int num);
    vector<_EDGEIMG>* getPyImg(){return &pyImg;}

};


#endif // KIMGPROCESS_H
