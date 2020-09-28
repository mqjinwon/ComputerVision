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
