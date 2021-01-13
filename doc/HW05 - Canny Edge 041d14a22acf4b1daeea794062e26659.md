# HW05 - Canny Edge

- [ ]  과   목 : Computer Vision
- [ ]  작성일 : 2020.11.02
- [ ]  작성자 : 2015741076 김진원
- 목차

# 1. FDG

---

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled.png)

Main 화면

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%201.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%201.png)

원본 이미지

- Main화면에서 "파일" 아이콘을 클릭하여 원하는 gray Image를 불러온다.

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%202.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%202.png)

- FDG 버튼을 클릭하면 위와 같은 이미지가 생성된다.
- source code

    ```cpp
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
                edgeData[nRow]->at(nCol).dx    = abs(tmpX);
                edgeData[nRow]->at(nCol).dy    = abs(tmpY);
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
    ```

# 2. Canny Edge

---

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled.png)

Main 화면

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%201.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%201.png)

원본 이미지

- Main화면에서 "파일" 아이콘을 클릭하여 원하는 gray Image를 불러온다.

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%203.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%203.png)

t0 = 10, t1 = 30인 경우

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%204.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%204.png)

t0 = 10, t1 = 100인 경우

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%205.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%205.png)

t0 = 10, t1 = 50인 경우

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%206.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%206.png)

t0 = 1, t1 = 100인 경우

![HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%207.png](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/Untitled%207.png)

t0 = 60, t1 = 90인 경우

- t0값과 t1값을 조정한 뒤, canney edge 버튼을 클릭하면 결과값을 볼 수 있다.
- t0과 t1을 다양하게 바꿔보며 구현해봤다.
- t1이 커질수록 노이즈 edge가 줄어들고 확실한 edge만 찾아내는 것을 볼 수 있다.
- t0가 커질수록 노이즈 edge가 줄어들지만, 선이 덜 연결되는 것을 볼 수 있다.
- Source code

    ```cpp
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
    ```

# 3. 전체 소스코드

[2015741076_kimjinwon_homework5.zip](HW05%20-%20Canny%20Edge%20041d14a22acf4b1daeea794062e26659/2015741076_kimjinwon_homework5.zip)