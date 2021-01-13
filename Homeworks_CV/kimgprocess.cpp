#include "kimgprocess.h"

KImgProcess::KImgProcess(double sigma)
{
    for(int nRow = -2; nRow <3; nRow++){
        for(int nCol= -2; nCol < 3; nCol++){
            gaussianVoting5[nRow+2][nCol+2] = 1 / (_2PI * pow(sigma, 2)) * exp(-0.5 * ( pow(nRow,2) + pow(nCol,2) / pow(sigma, 2)));
        }
    }

    for(int nRow = -5; nRow <6; nRow++){
        for(int nCol= -5; nCol < 6; nCol++){
            gaussianVoting11[nRow+5][nCol+5] = 1 / (_2PI * pow(sigma, 2)) * exp(-0.5 * ( pow(nRow,2) + pow(nCol,2) / pow(sigma, 2)));
        }
    }
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

    for(int nRow=0; nRow < igHSV.Row(); nRow++){
        for(int nCol=0; nCol < igHSV.Col(); nCol++){

            int pValue = igGray[nRow][nCol];

            if(pValue < _Min)   igGray[nRow][nCol] = pMin;
            else if(pValue > _Max)  igGray[nRow][nCol] = pMax;
            else{
                igHSV[nRow][nCol].v = ((pMax-pMin) / (double)(_Max-_Min) * (pValue-_Min) + pMin);
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

void KImgProcess::OtsuThreshold(KImageGray &igGray)
{
    // histogram 구하기
    KHisto hTmp;
    hTmp.Histogram(igGray);

    int     maxT    = 0;                                                // t : 최대가 되도록 하는 t값
    double  m_g     = 0;                                                // 히스토그램 기댓값
    double  pNum    = igGray.Row() * igGray.Col();                      // 총 픽셀 수
    for(int i=0; i < hTmp.Size(); i++)        m_g += i*hTmp[i]/pNum;

    double  q_1     = hTmp[0] ==0 ? std::numeric_limits<float>::min() : hTmp[0]/pNum , q_2= 1- q_1, m_1=0, m_2=m_g;        // q : 픽셀 갯수 합, m : 평균

    int     BCV     = 0;                                                // Between-class Variance


#if DEBUG
    qDebug() << "m_g : " << m_g << "\t" << "pNum : " << pNum;
#endif

    // 최대가 되는 t를 찾는 과정
    for(int t=0; t < 255; t++){

        // Between-class Variance 비교
        int tmp = q_1*q_2*pow(m_1-m_2, 2);

        if(BCV < tmp){
            BCV = tmp;
            maxT = t;
        }

        m_1 = q_1*m_1 + (t+1) * hTmp[t+1]/pNum;
        q_1 += hTmp[t+1]/pNum;
        m_1 /= q_1;

        m_2 = (m_g-q_1*m_1) / (1-q_1);

        q_2 = 1 - q_1;

//        qDebug() << t << " : " << hTmp[t];

#if DEBUG
    qDebug() << t+1 << " : m1 : " << m_1  << ", m2 : " << m_2 << ", tmp : " << tmp;
#endif


    }

#if DEBUG
    qDebug() << "maxT : " << maxT << "BCV : " << BCV;
#endif

    for(int nRow=0; nRow < igGray.Row(); nRow++){
        for(int nCol=0; nCol < igGray.Col(); nCol++){
            if(igGray[nRow][nCol] < maxT){
                igGray[nRow][nCol] = 0;
            }
            else{
                igGray[nRow][nCol] = 255;
            }
        }
    }

}

KImageGray KImgProcess::Dilation(KImageGray &igGray, int mSize, int nType)
{
    // 마스트 사이즈 error 탐지
    if(mSize%2 == 0)    return igGray;

    // 전경, 배경에 대한 설정
    int fore, back;

    if(nType == _BACKGROUND){
        fore = 255;
        back = 0;
    }
    else if(nType == _FOREGROUND){
        fore = 0;
        back = 255;
    }

    KImageGray  resultImg(igGray.Row(), igGray.Col());
    int         edgeSize = mSize / 2;                   // 모서리 부분은 마스크가 갈 수 없으므로, 기존의 픽셀을 사용함
    bool        checkFlag = false;

    for(int nRow=0; nRow < igGray.Row(); nRow++){
        for(int nCol=0; nCol < igGray.Col(); nCol++){
            checkFlag = false;

            // 모서리의 경우 같은 값을 넣어준다.
            if(nRow < edgeSize || nRow >= igGray.Row() - edgeSize || nCol < edgeSize || nCol >= igGray.Col() - edgeSize){
                resultImg[nRow][nCol] = igGray[nRow][nCol];
            }
            // 나머지 픽셀은 알고리즘을 적용한다.
            else{
                //background에서만 알고리즘을 적용하면 된다.
                if(igGray[nRow][nCol] == back){
                    for(int i=0; i<8; i++){
                        if(igGray[nRow + eightNeighborRow[i]][nCol + eightNeighborCol[i]] == fore){
                            resultImg[nRow][nCol] = fore;
                            checkFlag = true;
                            break;
                        }
                    }

                    if(!checkFlag){
                        resultImg[nRow][nCol] = back;
                    }

                }
                // foreground는 원래 픽셀값 대입
                else{
                    resultImg[nRow][nCol] = fore;
                }
            }
        }
    }

    return resultImg;
}

KImageGray KImgProcess::Erosion(KImageGray &igGray, int mSize, int nType)
{
    // 마스트 사이즈 error 탐지
    if(mSize%2 == 0)    return igGray;

    // 전경, 배경에 대한 설정
    int fore, back;

    if(nType == _BACKGROUND){
        fore = 255;
        back = 0;
    }
    else if(nType == _FOREGROUND){
        fore = 0;
        back = 255;
    }

    KImageGray  resultImg(igGray.Row(), igGray.Col());
    int         edgeSize = mSize / 2;                   // 모서리 부분은 마스크가 갈 수 없으므로, 기존의 픽셀을 사용함
    bool        checkFlag = false;

    for(int nRow=0; nRow < igGray.Row(); nRow++){
        for(int nCol=0; nCol < igGray.Col(); nCol++){
            checkFlag = false;

            // 모서리의 경우 같은 값을 넣어준다.
            if(nRow < edgeSize || nRow >= igGray.Row() - edgeSize || nCol < edgeSize || nCol >= igGray.Col() - edgeSize){
                resultImg[nRow][nCol] = igGray[nRow][nCol];
            }
            // 나머지 픽셀은 알고리즘을 적용한다.
            else{
                //foreground에서만 알고리즘을 적용하면 된다.
                if(igGray[nRow][nCol] == fore){
                    for(int i=0; i<8; i++){
                        if(igGray[nRow + eightNeighborRow[i]][nCol + eightNeighborCol[i]] == back){
                            resultImg[nRow][nCol] = back;
                            checkFlag = true;
                            break;
                        }
                    }

                    if(!checkFlag){
                        resultImg[nRow][nCol] = fore;
                    }

                }
                // background는 원래 픽셀값 대입
                else{
                    resultImg[nRow][nCol] = back;
                }
            }
        }
    }

    return resultImg;
}

KImageGray KImgProcess::Opening(KImageGray &igGray, int mSize, int nType)
{
    if(mSize%2 == 0)    return igGray;

    KImageGray  resultImg(igGray.Row(), igGray.Col());

    resultImg = Erosion(igGray, mSize, nType);
    resultImg = Dilation(resultImg, mSize, nType);


    return resultImg;
}

KImageGray KImgProcess::Closing(KImageGray &igGray, int mSize, int nType)
{
    if(mSize%2 == 0)    return igGray;

    KImageGray  resultImg(igGray.Row(), igGray.Col());

    resultImg = Dilation(igGray, mSize, nType);
    resultImg = Erosion(resultImg, mSize, nType);

    return resultImg;
}

_1DPIXEL KImgProcess::Labeling(KImageGray &igGray, int nType)
{
    // 전경, 배경에 대한 설정
    int fore, back;

    if(nType == _BACKGROUND){
        fore = 255;
        back = 0;
    }
    else if(nType == _FOREGROUND){
        fore = 0;
        back = 255;
    }


    int** labelImg;
    labelImg = new int*[igGray.Row()];                      // label 정보를 담고 있는 이미지이다.
    for(int nRow=0; nRow< igGray.Row(); nRow++){
        labelImg[nRow] = new int[igGray.Col()];
    }

    for(int nRow=0; nRow < igGray.Row(); nRow++)
        for(int nCol=0; nCol < igGray.Col(); nCol++){
            labelImg[nRow][nCol] = -1;
        }

#if DEBUG
    for(int nRow=0; nRow < igGray.Row(); nRow++)
        for(int nCol=0; nCol < igGray.Col(); nCol++){
            qDebug() << nRow << ", " << nCol << " : " << labelImg[nRow][nCol];
        }
#endif

    _1DPIXEL        labelDatas;                             // label 정보들을 담고 있는 탐색용 컨테이너이다.
    vector<int>*    labelData;                              // label 정보를 담고 있는 컨테이너이다.
    queue<int>      finalLabel;                             // 최종 label의 index를 가진다.
    _1DPIXEL        finalDatas;                             // 최종 label 컨테이너이다.


    int             labelNum=0;                             // 라벨 값
    int             compareLabel=0;                         // 비교를 위한 라벨 값


    for(int nRow=1; nRow < igGray.Row(); nRow++){
        for(int nCol=1; nCol < igGray.Col(); nCol++){

#if DEBUG
            qDebug() << nRow << ", " << nCol << " : " << igGray[nRow][nCol];
#endif

            // foreground에서의 알고리즘
            if(igGray[nRow][nCol] == fore){

                // '좌상'에 라벨이 있다면,
                if(labelImg[nRow + eightNeighborRow[5]][nCol + eightNeighborCol[5]] != -1){
                    labelNum = labelImg[nRow + eightNeighborRow[5]][nCol + eightNeighborCol[5]];

                    labelImg[nRow][nCol] = labelNum;
                    labelDatas[labelNum]->push_back(nRow * igGray.Col() + nCol);
                }
                // '좌', '상' 모두에 라벨 값이 있다면, 상의 라벨로 맞춰준다.
                else if( (labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]] != -1)&&
                        (labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]] != -1) ){

                        labelNum = labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]];
                        compareLabel = labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]];

                        labelImg[nRow][nCol] = labelNum;
                        labelDatas[labelNum]->push_back(nRow * igGray.Col() + nCol);

                        if(labelNum != compareLabel){
                            for(int i=0; i < labelDatas[compareLabel]->size(); i++){
                                // label 이미지 상의 값 변환 후, 라벨 데이터 통합
#if DEBUG
                                qDebug() << nRow << ", " << nCol << " : " << labelDatas[compareLabel]->at(i)/igGray.Col() << ", " << labelDatas[compareLabel]->at(i)%igGray.Col();
#endif
                                labelImg[labelDatas[compareLabel]->at(i)/igGray.Col()][labelDatas[compareLabel]->at(i)%igGray.Col()] = labelNum;
                                labelDatas[labelNum]->push_back(labelDatas[compareLabel]->at(i));

                                // compareLabel을 최종 Label list에서 없앤다.
                                for(int j=0; j < finalLabel.size(); j++){
                                    int tmpLabelInfo = finalLabel.front();
                                    finalLabel.pop();
                                    if(tmpLabelInfo != compareLabel)    finalLabel.push(tmpLabelInfo);
                                }
                            }
                        }
                }
                // 좌에만 라벨 값이 있다면
                else if(labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]] != -1){
                    labelNum = labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]];
                    labelImg[nRow][nCol] = labelNum;
                    labelDatas[labelNum]->push_back(nRow * igGray.Col() + nCol);
                }
                // 상에만 라벨 값이 있다면
                else if(labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]] != -1){
                    labelNum = labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]];
                    labelImg[nRow][nCol] = labelNum;
                    labelDatas[labelNum]->push_back(nRow * igGray.Col() + nCol);
                }
                // 새로운 라벨이 등장할 때,
                else{
                    // index는 0부터 시작해야하기 때문에 먼저 넣어줌
                    finalLabel.push(labelDatas.size());
                    labelImg[nRow][nCol] = labelDatas.size();

                    labelData = new vector<int>;
                    labelData->push_back(nRow * igGray.Col() + nCol);
                    labelDatas.push_back(labelData);
                }
            }
            // background에서의 알고리즘
            else{
                // '좌', '상' 에 라벨 값이 존재할 때, 같은 라벨로 만든다.
                if( (labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]] != -1)&&
                        (labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]] != -1) ){

                    labelNum = labelImg[nRow + eightNeighborRow[6]][nCol + eightNeighborCol[6]];
                    compareLabel = labelImg[nRow + eightNeighborRow[4]][nCol + eightNeighborCol[4]];

                    if(labelNum != compareLabel){
                        for(int i=0; i < labelDatas[compareLabel]->size(); i++){
                            // label 이미지 상의 값 변환 후, 라벨 데이터 통합
                            labelImg[labelDatas[compareLabel]->at(i)/igGray.Col()][labelDatas[compareLabel]->at(i)%igGray.Col()] = labelNum;
                            labelDatas[labelNum]->push_back(labelDatas[compareLabel]->at(i));

                            // compareLabel을 최종 Label list에서 없앤다.
                            for(int j=0; j < finalLabel.size(); j++){
                                int tmpLabelInfo = finalLabel.front();
                                finalLabel.pop();
                                if(tmpLabelInfo != compareLabel)    finalLabel.push(tmpLabelInfo);
                            }
                        }
                    }

                }
            }
        }
    }

#if DEBUG
    qDebug() << "final " << labelDatas.size();
#endif

    // 최종 라벨 복사
    while(!finalLabel.empty()){
        int label = finalLabel.front();
        finalLabel.pop();

        labelData = new vector<int>;
        labelData->assign( labelDatas[label]->begin(), labelDatas[label]->end());
        finalDatas.push_back(labelData);

    }

    // 탐색용 라벨 데이터 삭제
    for(int i=0; i< labelDatas.size(); i++){
        delete labelDatas[i];
    }

    return finalDatas;
}

void KImgProcess::HistogramEqualization(KImageGray &igImg)
{
    //histograming according to intensities
    KHisto Histogram(igImg);

    //accumulated histogram
    double*  dpHisto = Histogram.Address();
    for(int i=1, ic=Histogram.Size()-1; ic; ic--, i++)
        dpHisto[i] += dpHisto[i-1];

    //Histogram Equalization
    for(int i=0, ic=Histogram.Size(); ic; ic--, i++)
        dpHisto[i] = dpHisto[i] / (double)(igImg.Size());

    //transform the original image
    for(int ic=igImg.Row(),i=0; ic; ic--,i++)
        for(int jc=igImg.Col(),j=0; jc; jc--,j++)
            igImg[i][j] = (unsigned char)(dpHisto[igImg[i][j]]*255.0 + 0.5);

}

void KImgProcess::HistogramEqualization(KImageColor &icImg)
{
    KImageGray colorImg[3];

    for(int i=0; i<3; i++){
        colorImg[i].Create(icImg.Row(), icImg.Col());
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            colorImg[0][nRow][nCol] = icImg[nRow][nCol].r;
            colorImg[1][nRow][nCol] = icImg[nRow][nCol].g;
            colorImg[2][nRow][nCol] = icImg[nRow][nCol].b;
        }
    }

    for(int i=0; i<3; i++){
        HistogramEqualization(colorImg[i]);
    }


    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            icImg[nRow][nCol].r = colorImg[0][nRow][nCol];
            icImg[nRow][nCol].g = colorImg[1][nRow][nCol];
            icImg[nRow][nCol].b = colorImg[2][nRow][nCol];
        }
    }

}

void KImgProcess::HistogramMatching(KImageGray &oriImg, KImageGray &dstImg)
{
    // histogram 구하기
    KHisto hOri, hDst;
    hOri.Histogram(oriImg); hDst.Histogram(dstImg);

    //accumulatied histgram
    double* ahOri = hOri.Address();
    for(int i=1; i<hOri.Dim(); i++) ahOri[i] += ahOri[i-1];

    double* ahDst = hDst.Address();
    for(int i=1; i<hDst.Dim(); i++) ahDst[i] += ahDst[i-1];

    //histogram equalization
    for(int i=0; i<hOri.Dim(); i++)
    ahOri[i] = ahOri[i]/(double)(oriImg.Size());

    for(int i=0; i<hDst.Dim(); i++)
    ahDst[i] = ahDst[i]/(double)(dstImg.Size());


    int     argmin = 0;
    double  argValue = 1.;

    for(int i=0; i<hOri.Dim(); i++){
        argValue = 1.;

        // 목표 Histogram의 CDF를 증가시켜가며, 기존의 Histogram의 CDF와 값이 가장 근접한 것을 채택한다.
        for(int j=0; j<hDst.Dim(); j++){
            if (argValue > abs(ahOri[i] - ahDst[j])){
                argValue = abs(ahOri[i] - ahDst[j]);
                argmin = j;
            }
        }
        ahOri[i] = argmin;
    }

    //transform the original image
    for(int nRow=0; nRow < oriImg.Row(); nRow++){
        for(int nCol=0; nCol < oriImg.Col(); nCol++){
            oriImg[nRow][nCol] = ahOri[oriImg[nRow][nCol]];
        }
    }


}

void KImgProcess::HistogramMatching(KImageColor &oriImg, KImageColor &dstImg)
{
    KImageGray tmpOri[3], tmpDst[3];

    for(int i=0; i<3; i++){
        tmpOri[i].Create(oriImg.Row(), oriImg.Col());
        tmpDst[i].Create(dstImg.Row(), dstImg.Col());
    }

    for(int nRow=0; nRow < oriImg.Row(); nRow++){
        for(int nCol=0; nCol < oriImg.Col(); nCol++){
            tmpOri[0][nRow][nCol] = oriImg[nRow][nCol].r;
            tmpOri[1][nRow][nCol] = oriImg[nRow][nCol].g;
            tmpOri[2][nRow][nCol] = oriImg[nRow][nCol].b;
        }
    }

    for(int nRow=0; nRow < dstImg.Row(); nRow++){
        for(int nCol=0; nCol < dstImg.Col(); nCol++){
            tmpDst[0][nRow][nCol] = dstImg[nRow][nCol].r;
            tmpDst[1][nRow][nCol] = dstImg[nRow][nCol].g;
            tmpDst[2][nRow][nCol] = dstImg[nRow][nCol].b;
        }
    }

    for(int i=0; i<3; i++){
        HistogramMatching(tmpOri[i], tmpDst[i]);
    }

    for(int nRow=0; nRow < oriImg.Row(); nRow++){
        for(int nCol=0; nCol < oriImg.Col(); nCol++){
            oriImg[nRow][nCol].r = tmpOri[0][nRow][nCol];
            oriImg[nRow][nCol].g = tmpOri[1][nRow][nCol];
            oriImg[nRow][nCol].b = tmpOri[2][nRow][nCol];
        }
    }

}

KImageGray KImgProcess::GaussianNoiseToGrayImage(KImageGray &igImg, double mean, double var)
{
    KImageGray dstImg(igImg.Row(), igImg.Col());

    GRandom = new KGaussian(mean, var);
    GRandom->OnRandom(igImg.Row() * igImg.Col());

    int pixelValue = 0;
    int noise = 0;

    for(int nRow=0; nRow < igImg.Row(); nRow++){
        for(int nCol=0; nCol < igImg.Col(); nCol++){
            pixelValue = igImg[nRow][nCol];
            noise = GRandom->Generate();

            if(pixelValue + noise > 255) dstImg[nRow][nCol] = 255;
            else if(pixelValue + noise < 0) dstImg[nRow][nCol] = 0;
            else{
                dstImg[nRow][nCol] = pixelValue + noise;
            }
        }
    }

    delete GRandom;

    return dstImg;
}

KImageColor KImgProcess::GaussianNoiseToColorImage(KImageColor&icImg, double *mean, double *var)
{
    KImageColor dstImg(icImg.Row(), icImg.Col());

    KImageGray tmpImg[3];

    for(int i=0; i<3; i++){
        tmpImg[i].Create(icImg.Row(), icImg.Col());
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            tmpImg[0][nRow][nCol] = icImg[nRow][nCol].r;
            tmpImg[1][nRow][nCol] = icImg[nRow][nCol].g;
            tmpImg[2][nRow][nCol] = icImg[nRow][nCol].b;
        }
    }

    for(int i=0; i<3; i++)   tmpImg[i] = GaussianNoiseToGrayImage(tmpImg[i], mean[i], var[i]);

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            dstImg[nRow][nCol].r = tmpImg[0][nRow][nCol];
            dstImg[nRow][nCol].g = tmpImg[1][nRow][nCol];
            dstImg[nRow][nCol].b = tmpImg[2][nRow][nCol];
        }
    }

    return dstImg;
}

KImageGray KImgProcess::PepperSaltToGrayImage(KImageGray &igImg, double percent)
{
    KImageGray dstImg(igImg.Row(), igImg.Col());

    Random = new KRandom(igImg.Row() * igImg.Col());

    double rVal = 0.;

    for(int nRow=0; nRow < igImg.Row(); nRow++){
        for(int nCol=0; nCol < igImg.Col(); nCol++){
            rVal = Random->Generate();
            if(rVal < percent){
                if(rVal < percent / 2.) dstImg[nRow][nCol] = 255;
                else                    dstImg[nRow][nCol] = 0;
            }
            else{
                dstImg[nRow][nCol] = igImg[nRow][nCol];
            }
        }
    }

    delete Random;

    return dstImg;
}

KImageColor KImgProcess::PepperSaltToColorImage(KImageColor &icImg, double percent)
{
    KImageColor dstImg(icImg.Row(), icImg.Col());

    Random = new KRandom(icImg.Row() * icImg.Col());

    double rVal = 0.;

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            rVal = Random->Generate();
            if(rVal < percent){
                if(rVal < percent / 2.){
                    dstImg[nRow][nCol].r = 255;
                    dstImg[nRow][nCol].g = 255;
                    dstImg[nRow][nCol].b = 255;
                }
                else{
                    dstImg[nRow][nCol].r = 0;
                    dstImg[nRow][nCol].g = 0;
                    dstImg[nRow][nCol].b = 0;
                }
            }
            else{
                dstImg[nRow][nCol].r = icImg[nRow][nCol].r;
                dstImg[nRow][nCol].g = icImg[nRow][nCol].g;
                dstImg[nRow][nCol].b = icImg[nRow][nCol].b;
            }
        }
    }

    delete Random;

    return dstImg;
}

KImageGray KImgProcess::GaussianSmoothing(KImageGray &igImg, const double &sigma)
{
    KImageGray processImg(igImg.Row(), igImg.Col());

    int kernalH = 3*sigma + 0.3;

    // 1차원 커널 만들어서 두번 사용함(속도가 훨씬 빠르다)
    double* GMask = new double[kernalH*2 + 1];

    double dScale = 0;
    double dConst = 1. / (2. * _PI * pow(sigma, 2) );

    for(int i = -kernalH; i<=kernalH; i++){
        GMask[i+kernalH] = dConst * exp( -(i*i) / (2*pow(sigma, 2) ) );
        dScale += GMask[i+kernalH];
    }

    // Scaling
    for(int i = -kernalH; i<=kernalH; i++){
        GMask[i+kernalH] /= dScale;
    }

    // Convolution
    double pixVal = 0.;

    for(int nRow=kernalH; nRow < igImg.Row() - kernalH; nRow++){
        for(int nCol=kernalH; nCol < igImg.Col() - kernalH; nCol++){
            pixVal = 0.;

            for(int i = -kernalH; i<=kernalH; i++){
                pixVal += igImg[nRow][nCol + i] * GMask[i+kernalH];
            }
            processImg[nRow][nCol] = (unsigned char)pixVal;
        }
    }



    for(int nRow=kernalH; nRow < processImg.Row() - kernalH; nRow++){
        for(int nCol=kernalH; nCol < processImg.Col() - kernalH; nCol++){
            pixVal = 0.;

            for(int i = -kernalH; i<=kernalH; i++){
                pixVal += processImg[nRow + i][nCol] * GMask[i+kernalH];
            }
            igImg[nRow][nCol] = (unsigned char)pixVal;
        }
    }

    delete[] GMask;

    return igImg;
}

KImageColor KImgProcess::GaussianSmoothing(KImageColor &icImg, const double &sigma)
{
    KImageGray tmpImg[3];

    for(int i=0; i<3; i++){
        tmpImg[i].Create(icImg.Row(), icImg.Col());
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            tmpImg[0][nRow][nCol] = icImg[nRow][nCol].r;
            tmpImg[1][nRow][nCol] = icImg[nRow][nCol].g;
            tmpImg[2][nRow][nCol] = icImg[nRow][nCol].b;
        }
    }

    for(int i=0; i<3; i++){
        GaussianSmoothing(tmpImg[i], sigma);
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            icImg[nRow][nCol].r = tmpImg[0][nRow][nCol];
            icImg[nRow][nCol].g = tmpImg[1][nRow][nCol];
            icImg[nRow][nCol].b = tmpImg[2][nRow][nCol];
        }
    }

    return icImg;
}

KImageGray KImgProcess::MedianFiltering(KImageGray &igImg, const int kSize)
{
    KImageGray dstImg(igImg.Row(), igImg.Col());

    // 커널 크기가 홀수 일 때만 함수 실행
    if(kSize % 2 != 1) return igImg;

    int     nHalf = kSize / 2;
    int*    MKernel = new int[kSize*kSize];
    int     tmp = 0;

    for(int nRow=nHalf; nRow < igImg.Row()-nHalf; nRow++){
        for(int nCol=nHalf; nCol < igImg.Col()-nHalf; nCol++){

            for(int i = -nHalf; i <= nHalf; i++){
                // 픽셀 값 저장 단계
                for(int j = -nHalf; j <= nHalf; j++){
                    MKernel[(i+nHalf)*kSize + (j+nHalf)] = igImg[nRow + i][nCol + j];
                }
            }

            // 중간값 찾기 - 오름 차순으로 정렬 후 중간값 사용
            for(int i=0; i < kSize*kSize-1; i++){
                for(int j=i+1; j < kSize*kSize; j++){

                    //우측에 있는 숫자가 좌측에 있는 숫자보다 작다면 교환해준다.
                    if( MKernel[j] < MKernel[i]){
                        tmp = MKernel[j];
                        MKernel[j] = MKernel[i];
                        MKernel[i] = tmp;
                    }
                }
            }
            // 중간값 대입
#if DEBUG
            for(int i=0; i< kSize*kSize; i++)
                qDebug() << MKernel[i];
#endif
            dstImg[nRow][nCol] = MKernel[ (kSize*kSize) / 2];
        }
    }

    delete[] MKernel;


    return dstImg;
}

KImageColor KImgProcess::MedianFiltering(KImageColor &icImg, const int kSize)
{

    // 커널 크기가 홀수 일 때만 함수 실행
    KImageGray tmpImg[3];

    for(int i=0; i<3; i++){
        tmpImg[i].Create(icImg.Row(), icImg.Col());
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            tmpImg[0][nRow][nCol] = icImg[nRow][nCol].r;
            tmpImg[1][nRow][nCol] = icImg[nRow][nCol].g;
            tmpImg[2][nRow][nCol] = icImg[nRow][nCol].b;
        }
    }

    for(int i=0; i<3; i++){
        tmpImg[i] = MedianFiltering(tmpImg[i], kSize);
    }

    for(int nRow=0; nRow < icImg.Row(); nRow++){
        for(int nCol=0; nCol < icImg.Col(); nCol++){
            icImg[nRow][nCol].r = tmpImg[0][nRow][nCol];
            icImg[nRow][nCol].g = tmpImg[1][nRow][nCol];
            icImg[nRow][nCol].b = tmpImg[2][nRow][nCol];
        }
    }

    return icImg;
}

KImageGray KImgProcess::FDG(KImageGray &igImg, const double &sigma)
{

    KImageGray dstImg(igImg.Row(), igImg.Col());

    // 이전에 미분한 데이터가 있었다면 삭제해준다. - 메모리 누수 방지
    if(!edgeData.empty()){
        for(int i=0; i< edgeData.size(); i++){
            delete edgeData[i];
        }

        edgeData.clear();
    }

    // 미분데이터를 담을 이미지 container 생성
    vector<EdgeData> *colEdge;  // edge 데이터의 열 성분들

    for(int i=0; i < igImg.Row(); i++){
        colEdge = new vector<EdgeData>(igImg.Col());
        edgeData.push_back(colEdge);
    }

    // FDG 시작
    int kernalH = 3*sigma + 0.3;

    // 2차원 커널 사용
    double** GMaskX;
    double** GMaskY;
    GMaskX = new double*[kernalH*2 + 1];
    for(int nRow=0; nRow< kernalH*2 + 1; nRow++){
        GMaskX[nRow] = new double[kernalH*2 + 1];
    }

    GMaskY = new double*[kernalH*2 + 1];
    for(int nRow=0; nRow< kernalH*2 + 1; nRow++){
        GMaskY[nRow] = new double[kernalH*2 + 1];
    }

    double dScale = 0;
    double dConst = 1. / (2. * _PI * pow(sigma, 4) );
    double dTmp;

    for(int nRow = -kernalH; nRow<=kernalH; nRow++){
        dTmp = -nRow * exp(-(nRow*nRow)/(2. * pow(sigma, 2)));

        for(int nCol = -kernalH; nCol<=kernalH; nCol++){
            GMaskY[nRow+kernalH][nCol+kernalH] = dConst * dTmp * exp( -(nCol*nCol) / (2*pow(sigma, 2))) ;

            GMaskX[nCol+kernalH][nRow+kernalH] = GMaskY[nRow+kernalH][nCol+kernalH];

            if(nRow < 0) dScale += GMaskY[nRow+kernalH][nCol+kernalH];
        }
    }

    // Scaling
    for(int nRow = -kernalH; nRow<=kernalH; nRow++){
        for(int nCol = -kernalH; nCol<=kernalH; nCol++){
            GMaskX[nRow+kernalH][nCol+kernalH] /= -dScale;
            GMaskY[nRow+kernalH][nCol+kernalH] /= -dScale;
        }
    }

    // Image Convolution
    for(int nRow = kernalH; nRow<igImg.Row()-kernalH; nRow++){
        for(int nCol = kernalH; nCol<igImg.Col()-kernalH; nCol++){

            double tmpX=0., tmpY=0.;

            // Convolution
            for(int nKRow = -kernalH; nKRow<=kernalH; nKRow++){
                for(int nKCol = -kernalH; nKCol<=kernalH; nKCol++){
                    tmpX += GMaskX[nKRow+kernalH][nKCol+kernalH] * igImg[nRow+nKRow][nCol+nKCol];
                    tmpY += GMaskY[nKRow+kernalH][nKCol+kernalH] * igImg[nRow+nKRow][nCol+nKCol];
                }
            }

            // 출력이미지
            dstImg[nRow][nCol] = abs(tmpX)+abs(tmpY);

            // 크기가 1인 벡터로 scaling 및 gradient 계산
            edgeData[nRow]->at(nCol).magn  = abs(tmpX)+abs(tmpY);
            edgeData[nRow]->at(nCol).dx    = tmpX / sqrt(tmpX*tmpX + tmpY*tmpY);
            edgeData[nRow]->at(nCol).dy    = tmpY / sqrt(tmpX*tmpX + tmpY*tmpY);
            edgeData[nRow]->at(nCol).dir   = (unsigned char)((((int)(atan2(tmpY, tmpX) * (180.0/M_PI)/22.5)+1)>>1) & 0x00000003);

        }
    }

    // 데이터 삭제
    for(int nRow = -kernalH; nRow<=kernalH; nRow++){
        for(int nCol = -kernalH; nCol<=kernalH; nCol++){
            delete[] GMaskX[nRow+kernalH];
            delete[] GMaskY[nRow+kernalH];
        }
    }

    delete[] GMaskX;
    delete[] GMaskY;

    return dstImg;
}

KImageGray KImgProcess::CannyEdge(KImageGray& igImg, double t0, double t1, const double &sigma)
{
    FDG(igImg, sigma);

    KImageGray dstImg(edgeData.size(), edgeData[0]->size());

    // Non-Maxima Suppression
    int nDx[4] = {+1, +1, 0, -1};
    int nDy[4] = { 0, +1, +1, +1};

    int tmpdir=0;
    for(int nRow=1; nRow<dstImg.Row()-1; nRow++){
        for(int nCol=1; nCol<dstImg.Col()-1; nCol++){
            tmpdir = edgeData[nRow]->at(nCol).dir;

            // 주변의 것들보다 크다면!
            if(edgeData[nRow]->at(nCol).magn > edgeData[nRow + nDy[tmpdir]]->at(nCol + nDx[tmpdir]).magn &&
                    edgeData[nRow]->at(nCol).magn > edgeData[nRow - nDy[tmpdir]]->at(nCol - nDx[tmpdir]).magn){

                // 임계값 t1보다 큰 놈들은 확실히 edge라고 판별
                if(edgeData[nRow]->at(nCol).magn >= t1){
                    dstImg[nRow][nCol] = 255;
                }
                // 임계값 t0보다 작은 놈들은 확실히 edge가 아니라고 판별
                else if(edgeData[nRow]->at(nCol).magn < t0){
                    dstImg[nRow][nCol] = 0;
                }
            }

            // 주변의 것들보다 크지 않다면!
            else{
                edgeData[nRow]->at(nCol).magn = 0;
            }
        }
    }

    // Hysteresis Thresholding

    stack<pair<int,int>> pointStack;

    for(int nRow=0; nRow<dstImg.Row(); nRow++){
        for(int nCol=0; nCol<dstImg.Col(); nCol++){

            // 확실한 edge라면 stack을 사용해서 주변을 탐색해 t0과 t1사이의 값을 찾아내 연결해준다.
            if(dstImg[nRow][nCol] == 255){
                pointStack.push(make_pair(nRow, nCol));

                while(!pointStack.empty()){

                    // 좌표를 꺼내고 자료구조를 Pop함
                    int tmpRow = pointStack.top().first;
                    int tmpCol = pointStack.top().second;
                    pointStack.pop();

                    // 확실한 edge 주변의 중간 값을 엣지로써 추가시켜준다.
                    for(int i=0; i<8; i++){

                        // 모서리를 벗어나면 패스
                        if(tmpRow + eightNeighborRow[i] == -1 || tmpRow + eightNeighborRow[i]== dstImg.Row() ||
                                tmpCol + eightNeighborCol[i] == -1 || tmpCol + eightNeighborCol[i] == dstImg.Col()){
                            continue;
                        }
                        if((edgeData[tmpRow + eightNeighborRow[i]]->at(tmpCol + eightNeighborCol[i]).magn > t0) && dstImg[nRow][nCol] != 255){
                            dstImg[tmpRow + eightNeighborRow[i]][tmpCol + eightNeighborCol[i]] = 255;
                            pointStack.push(make_pair(tmpRow + eightNeighborRow[i], tmpCol + eightNeighborCol[i]));
                        }
                    }
                }
            }
        }
    }


    return dstImg;
}

_POINTS KImgProcess::CircleHoughTransform(KImageGray &igImg, double mRadius, double threshold)
{
    KImageGray edgeImg = CannyEdge(igImg);
    KImageDouble dstImg(igImg.Row(), igImg.Col());

    int tmpCol, tmpRow;

    for(int nRow=0; nRow<edgeImg.Row(); nRow++){
        for(int nCol=0; nCol<edgeImg.Col(); nCol++){

            // edge 일때,
            if(edgeImg[nRow][nCol] == 255){

                // 기울기로 거리를 구한다.
                tmpCol = nCol + edgeData[nRow]->at(nCol).dx * mRadius;
                tmpRow = nRow + edgeData[nRow]->at(nCol).dy * mRadius;

                // 이미지 범위를 벗어나면 무시한다.
                if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                    continue;
                }

                //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                for(int i=-5; i<6; i++)
                for(int j=-5; j<6; j++){

                    // 이미지 범위를 벗어나면 무시한다.
                    if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                        continue;
                    }

                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                }

                // 기울기로 거리를 구한다.
                tmpCol = nCol - edgeData[nRow]->at(nCol).dx * mRadius;
                tmpRow = nRow - edgeData[nRow]->at(nCol).dy * mRadius;

                // 이미지 범위를 벗어나면 무시한다.
                if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                    continue;
                }

                //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                for(int i=-5; i<6; i++)
                for(int j=-5; j<6; j++){

                    // 이미지 범위를 벗어나면 무시한다.
                    if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                        continue;
                    }

                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                }

            }
        }
    }

    _POINTS points;

    // Non-Maxima Suppression
    for(int nRow=1; nRow<dstImg.Row()-1; nRow++){
        for(int nCol=1; nCol<dstImg.Col()-1; nCol++){
            if(dstImg[nRow][nCol] >= threshold){

                bool isMax = true;

                for(int i = 0; i < 8 ; i++){
                    if(dstImg[nRow][nCol] < dstImg[nRow + eightNeighborRow[i]][nCol + eightNeighborCol[i]]){
                        isMax = false;
                        break;
                    }
                }

                if(isMax)  points.push_back(make_pair(nRow, nCol));
            }
        }
    }

    resultIgImg = dstImg.ToGray(_USER_SCALING);
    resultIcImg = resultIgImg.GrayToRGB();

    for(int i=0; i < points.size(); i++){
        tmpRow = points[i].first;
        tmpCol = points[i].second;

        resultIcImg[tmpRow][tmpCol].r = 255;
        resultIcImg[tmpRow][tmpCol].g = 0;
        resultIcImg[tmpRow][tmpCol].b = 0;
    }

    return points;
}

_POINTS KImgProcess::CircleHoughTransform(KImageGray &igImg, double mMinR, double mMaxR, double threshold)
{
    KImageGray edgeImg = CannyEdge(igImg);
    KImageDouble dstImg(igImg.Row(), igImg.Col());

    int tmpCol, tmpRow;

    for(int nRow=0; nRow<edgeImg.Row(); nRow++){
        for(int nCol=0; nCol<edgeImg.Col(); nCol++){

            // 범위안에 있는 반지름에 대해서 계산
            for(int mRadius=mMinR; mRadius<=mMaxR; mRadius++){
                // edge 일때,
                if(edgeImg[nRow][nCol] == 255){

                    // 기울기로 거리를 구한다.
                    tmpCol = nCol + edgeData[nRow]->at(nCol).dx * mRadius;
                    tmpRow = nRow + edgeData[nRow]->at(nCol).dy * mRadius;

                    // 이미지 범위를 벗어나면 무시한다.
                    if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                        continue;
                    }

                    //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                    for(int i=-5; i<6; i++)
                    for(int j=-5; j<6; j++){

                        // 이미지 범위를 벗어나면 무시한다.
                        if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                            continue;
                        }

                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    }

                    // 기울기로 거리를 구한다.
                    tmpCol = nCol - edgeData[nRow]->at(nCol).dx * mRadius;
                    tmpRow = nRow - edgeData[nRow]->at(nCol).dy * mRadius;

                    // 이미지 범위를 벗어나면 무시한다.
                    if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                        continue;
                    }

                    //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                    for(int i=-5; i<6; i++)
                    for(int j=-5; j<6; j++){

                        // 이미지 범위를 벗어나면 무시한다.
                        if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                            continue;
                        }

                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                    }

                }
            }
        }
    }

    _POINTS points;

    // Non-Maxima Suppression
    for(int nRow=1; nRow<dstImg.Row()-1; nRow++){
        for(int nCol=1; nCol<dstImg.Col()-1; nCol++){
            if(dstImg[nRow][nCol] >= threshold){

                bool isMax = true;

                for(int i = 0; i < 8 ; i++){
                    if(dstImg[nRow][nCol] < dstImg[nRow + eightNeighborRow[i]][nCol + eightNeighborCol[i]]){
                        isMax = false;
                        break;
                    }
                }

                if(isMax)  points.push_back(make_pair(nRow, nCol));
            }
        }
    }

    // scaling 후 값을 gray 이미지로 저장
    resultIgImg = dstImg.ToGray(_USER_SCALING);
    resultIcImg = resultIgImg.GrayToRGB();

    // 검출된 좌표를 빨간색 점으로 바꿔준다.
    for(int i=0; i < points.size(); i++){
        tmpRow = points[i].first;
        tmpCol = points[i].second;

        resultIcImg[tmpRow][tmpCol].r = 255;
        resultIcImg[tmpRow][tmpCol].g = 0;
        resultIcImg[tmpRow][tmpCol].b = 0;
    }

    return points;
}

_POINTS KImgProcess::GeneralHoughTransform(KImageGray &igImg, string dir, double threshold)
{
    vector<pair<int,int>> points;
    vector<pair<double,double>> mTable[4];
    KImageDouble dstImg(igImg.Row(), igImg.Col());

    // 파일 읽는 부분
    ifstream fin;
    fin.open(dir);

    int nRow, nCol;
    long    centerRow=0, centerCol=0;

    while(! fin.eof()){
        fin >> nRow >> nCol;
        points.push_back(make_pair(nRow, nCol));
    }
    if(fin.is_open())    fin.close();

    // 중심점 계산
    for(int i=0; i<points.size(); i++){
        centerRow += points[i].first;
        centerCol += points[i].second;
    }
    centerRow /= points.size();
    centerCol /= points.size();


    /////////// table 생성 ///////////
    double nTmp;
    int nDir;   //edge 방향
    double nDist, nAng;

    for(int j=1, jj=points.size()-2; jj; j++,jj--) {

        //에지 방향 설정 - 양 옆 점의 기울기로 부터 에지 방향 계산
        nTmp = _DEGREE( atan2((float)(points[j+1].first - points[j-1].first),
                                        (float)(points[j+1].second - points[j-1].second) + 1e-8));
        nTmp += 90.0; //에지 방향(0~360)
        nDir = ((((int)(abs(nTmp)/22.5)+1)>>1) & 0x00000003); //에지 방향(0~3)
        //중심과의 거리
        nDist = sqrt(_SQR(centerCol - points[j].second) + _SQR(centerRow - points[j].first));

        //중심과 이루는 각도
        nAng = _DEGREE(atan2(points[j].first - centerRow, points[j].second - centerCol + 1e-8));

        //형상 테이블에 추가
        mTable[nDir].push_back(make_pair(nDist, nAng));
    }
    /////////// table 생성 ///////////

    /////////// edge 추출 ///////////
    KImageGray edgeImg = CannyEdge(igImg);

    int tmpCol, tmpRow;
    int degreeFactor = 4;

    for(int nRow=0; nRow<edgeImg.Row(); nRow++){
        for(int nCol=0; nCol<edgeImg.Col(); nCol++){

            // edge 일때,
            if(edgeImg[nRow][nCol] == 255){

                for(int nTheta = 0; nTheta < degreeFactor; nTheta++){
                    // 같은 방향의 table을 찾고,
                    //int tmpDir = edgeData[nRow]->at(nCol).dir;
                    int tmpDir = (unsigned char)((((int)( (atan2(edgeData[nRow]->at(nCol).dy, edgeData[nRow]->at(nCol).dx)
                                                           * (180.0/M_PI) - 360/ degreeFactor * nTheta) /22.5)+1)>>1) & 0x00000003);

                    // 그 안의 데이터를 모두 넣는다.
                    for(int i=0; i< mTable[tmpDir].size(); i++){

                        for(double nScale = 1.; nScale <= 1.; nScale += 0.05){
                            // 기울기로 거리를 구한다.
                            tmpCol = nCol + nScale * mTable[tmpDir][i].first * cos(_RADIAN(mTable[tmpDir][i].second + 360/ degreeFactor * nTheta));
                            tmpRow = nRow + nScale * mTable[tmpDir][i].first * sin(_RADIAN(mTable[tmpDir][i].second + 360/ degreeFactor * nTheta));

                            // 이미지 범위를 벗어나면 무시한다.
                            if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                                continue;
                            }

                            //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                            for(int i=-5; i<6; i++)
                            for(int j=-5; j<6; j++){

                                // 이미지 범위를 벗어나면 무시한다.
                                if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                                    continue;
                                }

                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                            }

                            // 기울기로 거리를 구한다.
                            tmpCol = nCol - nScale * mTable[tmpDir][i].first * cos(_RADIAN(mTable[tmpDir][i].second + 360/ degreeFactor * nTheta));
                            tmpRow = nRow - nScale * mTable[tmpDir][i].first * sin(_RADIAN(mTable[tmpDir][i].second + 360/ degreeFactor * nTheta));

                            // 이미지 범위를 벗어나면 무시한다.
                            if(tmpCol < 0 || tmpRow < 0 || tmpCol >= igImg.Col() || tmpRow >= igImg.Row()){
                                continue;
                            }

                            //votes to avoid edge noises - 노이즈로 인해 제대로 추적을 못할 수 있다. 주변의 값에도 voting을 해준다.
                            for(int i=-5; i<6; i++)
                            for(int j=-5; j<6; j++){

                                // 이미지 범위를 벗어나면 무시한다.
                                if(tmpCol+j < 0 || tmpRow+i < 0 || tmpCol+j >= igImg.Col() || tmpRow+i >= igImg.Row()){
                                    continue;
                                }

                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting11[i+5][j+5];

                            }

                        }

                    }
                }

            }
        }
    }
    /////////// edge 추출 ///////////

    points.clear();

    // Non-Maxima Suppression
    for(int nRow=1; nRow<dstImg.Row()-1; nRow++){
        for(int nCol=1; nCol<dstImg.Col()-1; nCol++){
            if(dstImg[nRow][nCol] >= threshold){

                bool isMax = true;

                for(int i = 0; i < 8 ; i++){
                    if(dstImg[nRow][nCol] < dstImg[nRow + eightNeighborRow[i]][nCol + eightNeighborCol[i]]){
                        isMax = false;
                        break;
                    }
                }

                if(isMax)  points.push_back(make_pair(nRow, nCol));
            }
        }
    }

    resultIgImg = dstImg.ToGray(_USER_SCALING);
    resultIcImg = resultIgImg.GrayToRGB();

    for(int i=0; i < points.size(); i++){
        tmpRow = points[i].first;
        tmpCol = points[i].second;

        resultIcImg[tmpRow][tmpCol].r = 255;
        resultIcImg[tmpRow][tmpCol].g = 0;
        resultIcImg[tmpRow][tmpCol].b = 0;
    }

    return points;
}

_EDGEIMG KImgProcess::OpticalFlow(KImageGray &igImg1, KImageGray &igImg2, int iter)
{
    int kernelSize = 5;
    int kernalH = kernelSize/2;

    int minSize = _MIN(igImg2.Row(), igImg2.Col());

    KImageColor resultImg;


    // 반복횟수로 인한 이미지 축소가 minsize 보다 크면 iter를 다시 정의한다.
    if(pow(2, iter-1) > minSize / kernelSize){
        iter = 0;
        int tmpValue = 1;

        while(1){

            iter++;
            tmpValue *= 2;

            if(tmpValue > minSize / kernelSize){
                iter--;
                break;
            }
        }
    }

    // iter + 1개의 이미지 피라미드 생성(원본의 이미지도 저장되기 때문에)
    ImgPyramid _imgPyramidSrc1(igImg1);
    _imgPyramidSrc1.Create(iter);
    ImgPyramid _imgPyramidSrc2(igImg2);
    _imgPyramidSrc2.Create(iter);

    ////lucas & kanade method 사용
    vector<_EDGEIMG>* pyImgSrc1 = _imgPyramidSrc1.getPyImg();
    vector<_EDGEIMG>* pyImgSrc2 = _imgPyramidSrc2.getPyImg();


    // 상위의 피라미드 이미지(작은 이미지)부터 차근 차근 구하기
    _JMAT srcMat1 = GetValMatrix((*pyImgSrc1)[iter]);
    _JMAT srcMat2 = GetValMatrix((*pyImgSrc2)[iter]);

    // lucas & kanade를 적용하기 위한 가중치 벡터 생성
    Eigen::Matrix<double, 25, 25> weightedMat5;   weightedMat5.fill(0.);

    for(int nRow = 0; nRow<kernelSize; nRow++){
        for(int nCol = 0; nCol<kernelSize; nCol++){
            weightedMat5(nRow*kernelSize + nCol,nRow*kernelSize + nCol) = gaussianVoting5[nRow][nCol];
        }
    }

    Eigen::Matrix<double, 25, 2> dxdy;
    Eigen::Matrix<double, 25, 1> dt;
    Eigen::Matrix<double, 2, 25> aTw;
    Eigen::Matrix<double, 2, 1> zeta;

    //dx, dy 찾기 - 맨 상단의 이미지에서 dx, dy 추출
    for(int nRow=kernalH+1; nRow < srcMat1.size()-kernalH-1; nRow++){
        for(int nCol=kernalH+1; nCol < srcMat1[0].size()-kernalH-1; nCol++){

                for(int kRow = nRow-kernalH, _i=0; kRow <= nRow+kernalH; kRow++, _i++){
                    for(int kCol = nCol-kernalH, _j=0; kCol <= nCol+kernalH; kCol++, _j++){

                        // 앞선 이미지의 크기는 1/2, 거리는 2배
                        double moveX = kCol + (*pyImgSrc2)[iter][nRow]->at(nCol).dx;
                        double moveY = kRow + (*pyImgSrc2)[iter][nRow]->at(nCol).dy;

                        if(moveX <0) moveX = 0; if(moveX > srcMat1[0].size()-1) moveX = srcMat1[0].size()-1;
                        if(moveY <0) moveY = 0; if(moveY > srcMat1.size()-1) moveY = srcMat1.size()-1;

                        dxdy.row(_i*kernelSize + _j) << srcMat1[kRow][kCol+1] - srcMat1[kRow][kCol-1], srcMat1[kRow+1][kCol] - srcMat1[kRow-1][kCol];
                        dt.row(_i*kernelSize + _j) << srcMat1[moveY][moveX] - srcMat2[kRow][kCol];

                    }
                }

                //역행렬을 통해 d값을 구하는 과정
                aTw = dxdy.transpose() * weightedMat5;
                zeta = (aTw * dxdy).inverse() * aTw * dt;

                (*pyImgSrc2)[iter][nRow]->at(nCol).dx += zeta(0,0);
                (*pyImgSrc2)[iter][nRow]->at(nCol).dy += zeta(1,0);
        }
    }

    for(int i = iter-1; i >= 0; i--){
        srcMat1 = GetValMatrix((*pyImgSrc1)[i]);
        srcMat2 = GetValMatrix((*pyImgSrc2)[i]);
        _EDGEIMG refMat = (*pyImgSrc2)[i+1];    //상단 이미지(래퍼런스)의 dx,dy를 가져오기 위해

        // 커널을 옮겨가면서 dx, dy 찾기 - 맨 상단의 이미지에서 dx, dy 추출
        for(int nRow=kernalH+1; nRow < srcMat1.size()-kernalH-1; nRow++){
            for(int nCol=kernalH+1; nCol < srcMat1[0].size()-kernalH-1; nCol++){

                    // 커널안의 점들을 통해 lucas & kanade를 위한 행렬을 만듬
                    for(int kRow = nRow-kernalH, _i=0; kRow <= nRow+kernalH; kRow++, _i++){
                        for(int kCol = nCol-kernalH, _j=0; kCol <= nCol+kernalH; kCol++, _j++){
//                        qDebug() << i << ": "<< nRow <<", "<< nCol <<", "<< kRow <<", "<< nRow <<", "<< kCol <<", ";

                        // 앞선 이미지의 크기는 1/2, 거리는 2배
                        double moveX = kCol + refMat[nRow/2]->at(nCol/2).dx * 2 + (*pyImgSrc2)[i][nRow]->at(nCol).dx; //이문게 제
                        double moveY = kRow + refMat[nRow/2]->at(nCol/2).dy * 2 + (*pyImgSrc2)[i][nRow]->at(nCol).dy;
                        if(moveX <0) moveX = 0; if(moveX > srcMat1[0].size()-1) moveX = srcMat1[0].size()-1;
                        if(moveY <0) moveY = 0; if(moveY > srcMat1.size()-1) moveY = srcMat1.size()-1;


                        dxdy.row(_i*kernelSize + _j) << srcMat1[kRow][kCol+1] - srcMat1[kRow][kCol-1],
                                                      srcMat1[kRow+1][kCol] - srcMat1[kRow-1][kCol];

                        dt.row(_i*kernelSize + _j) << srcMat1[moveY][moveX] - srcMat2[kRow][kCol];

                        }
                    }

                    //역행렬을 통해 d값을 구하는 과정
                    aTw = dxdy.transpose() * weightedMat5;
                    zeta = (aTw * dxdy).inverse() * aTw * dt;

                    (*pyImgSrc2)[i][nRow]->at(nCol).dx += zeta(0,0);
                    (*pyImgSrc2)[i][nRow]->at(nCol).dy += zeta(1,0);

            }
        }
    }

    // 그냥 2번째 이미지를 컬러 이미지로 변환해서 저장함
    resultIcImg = igImg2.GrayToRGB();

    return (*pyImgSrc2)[0];
}

_JMAT KImgProcess::GetValMatrix(const _EDGEIMG &matrix)
{
    _JMAT resultMat; _JCOL tmp;
    tmp.assign(matrix[0]->size(), 0.);
    resultMat.assign(matrix.size(), tmp);

    for(int i=0; i<resultMat.size(); i++){
        resultMat[i].reserve(matrix[0]->size());
    }

    for(int nRow=0; nRow<resultMat.size(); nRow++){
        for(int nCol=0; nCol<resultMat[0].size(); nCol++){
            resultMat[nRow][nCol] = matrix[nRow]->at(nCol).val;
        }
    }

    return resultMat;
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

void ImgPyramid::Create(int num)
{

    int minSize = _MIN(_rowSize, _colSize);

    // 예외처리
    if(pow(2, num-1) > minSize)   return;

    // 이전의 데이터가 있었다면 삭제해준다. - 메모리 누수 방지
    if(!pyImg.empty()){
        for(int i=0; i< pyImg.size(); i++){
            for(int j=0; i< pyImg[i].size(); j++){
                delete pyImg[i][j];
            }
        }
        pyImg.clear();
    }

    // 이미지 하나 하나를 잠시 저장
    _EDGEIMG tmpImg;

    // 미분데이터를 담을 이미지 container 생성
    vector<EdgeData> *colEdge;  // edge 데이터의 열 성분들

    for(int i=0; i < _igImg.Row(); i++){
        colEdge = new vector<EdgeData>(_igImg.Col());
        tmpImg.push_back(colEdge);
    }

    for(int nRow=0; nRow<_igImg.Row(); nRow++){
        for(int nCol=0; nCol<_igImg.Col(); nCol++){
            tmpImg[nRow]->at(nCol).val = _igImg[nRow][nCol];
        }
    }

    pyImg.push_back(tmpImg);

    //피라미드 이미지를 생성
    for(int i = 1; i <= num; i++){

        tmpImg.clear();

        for(int j=0; j < pyImg[i-1].size() / 2; j++){
            colEdge = new vector<EdgeData>(pyImg[i-1][0]->size() / 2);
            tmpImg.push_back(colEdge);
        }

        for(int nRow=0; nRow<pyImg[i-1].size() / 2; nRow++){
            for(int nCol=0; nCol<pyImg[i-1][0]->size() / 2; nCol++){
                // 제대로 하려면, 중간값을 넣든가 하는것이 좋을 듯...
                tmpImg[nRow]->at(nCol).val = pyImg[i-1][nRow*2]->at(nCol*2).val;
            }
        }

        pyImg.push_back(tmpImg);
    }

}

