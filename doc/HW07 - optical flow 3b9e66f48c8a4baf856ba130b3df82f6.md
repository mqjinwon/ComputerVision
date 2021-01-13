# HW07 - optical flow

- [ ]  과   목 : Computer Vision
- [ ]  작성일 : 2020.12.02
- [ ]  작성자 : 2015741076 김진원

**목차**

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled.png)

# 1. 사용 방법

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%201.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%201.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%202.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%202.png)

- Optical Flow 버튼을 누르면 파일을 선택할 수 있다.
- 비교를 원하는 파일 2개를 선택하면 결과 값을 볼 수 있다.(이미지 선택 창이 두번 뜬다.)

# 2. 실험 결과

## 3번째 이미지 → 4번째 이미지

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%203.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%203.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%204.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%204.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%205.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%205.png)

## 3번째 이미지 → 7번째 이미지

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%203.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%203.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%206.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%206.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%207.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%207.png)

## 3번째 이미지 → 11번째 이미지

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%208.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%208.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%209.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%209.png)

![HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%2010.png](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/Untitled%2010.png)

## 고찰

- 이미지의 변화량이 커지면 커질수록 극명하게 변화가 생기는 것이 보인다.
- 비교적 단순한 하늘의 경우, 이동한 픽셀을 잘 찾아 매칭하는 것을 볼 수 있지만, 복잡한 풍경(산골짜기)에서는 어디로 이동했는지를 찾는 것을 힘들어한다.

# 3. 소스 분석

## Pyramid Image 생성

```cpp
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
```

## Optical Flow

```cpp
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
```

- 원래 iteration을 반복하며 정확한 zeta값을 만들어줘야하는데, 이 부분은 생략하고 소스를 구현하였다.

[2015741076_kimjinwon_homework7.zip](HW07%20-%20optical%20flow%203b9e66f48c8a4baf856ba130b3df82f6/2015741076_kimjinwon_homework7.zip)