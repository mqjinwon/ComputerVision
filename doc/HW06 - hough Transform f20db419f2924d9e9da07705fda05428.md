# HW06 - hough Transform

- [ ]  과   목 : Computer Vision
- [ ]  작성일 : 2020.11.20
- [ ]  작성자 : 2015741076 김진원

**목차**

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled.png)

# 1. 반지름을 모르는 상태의 Circle Hough Transform

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%201.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%201.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%202.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%202.png)

- 반지름을 모른다면 radius 체크박스를 uncheck한 상태로 두면 된다.
- 이때 탐색하는 반지름 범위는 [MinR, MaxR]이다.
- 원이라 인식하는 Threshold 값은 아래의 spinbox에서 조절하면 된다.

## 실험결과

### MinR : 30, MaxR : 70, Threshold : 100

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%203.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%203.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%204.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%204.png)

- 원이라 인식하는 부분이 263개이다. 상당히 많으므로 임계값을 높혀야한다.

### MinR : 30, MaxR : 70, Threshold : 1000

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%205.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%205.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%206.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%206.png)

- 원이라 인식하는 부분이 3개이다. 거의 근접했다.

## 고찰

- 이 소스에서는 임계값을 넘는 좌표에서 주변 8방향에 한해 Non-Maxima Suppression을 적용했는데, 그 결과 임계값을 높이더라도 주변의 노이즈가 조금 취약한 것을 볼 수 있었다.
- 이를 해결하기 위해서는 8방향이 아닌, MinR값에 대해서 Non-Maxima Suppression을 적용하면 좋을 것 같다.
- 또한 임계값이 매우 큰데, 이는 반지름을 1씩 늘려가면서 총합한 결과이기 때문이다. 이를 반지름에 따른 반복횟수로 나눠주면 좀 더 작은 값으로 사용할 수 있을 것 같다.

## 소스코드

```cpp
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

                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
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

                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                        dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
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
```

# 2. 반지름을 아는 상태의 Circle Hough Transform

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%207.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%207.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%202.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%202.png)

- 반지름을 안다면 radius 체크박스를 check한 상태로 두면 된다.
- 이때 반지름 길이는 radius이다.
- 원이라 인식하는 Threshold 값은 아래의 spinbox에서 조절하면 된다.

## 실험결과

### radius: 51.5, Threshold : 60

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%208.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%208.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%209.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%209.png)

- radius: 51.5, Threshold : 60에서 정확히 원의 좌표를 찾아내는 것을 볼 수 있다.

## 소스코드

- 1. 번의 것에서 반지름의 길이만 특정해주면 되는 것이므로 생략

# 3. Generalized Hough Transform

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2010.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2010.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2011.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2011.png)

- 물체라 인식하는 Threshold 값은 아래의 spinbox에서 조절하면 된다.
- 이미지를 켜놓고, General Hough Transform 버튼을 누르면 이미지 좌표가 저장된 txt파일을 고를 수 있다.

## 실험결과

### Threshold : 60

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2012.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2012.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2013.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2013.png)

- 3098개의 점이 검출되었다. 너무 낮은 값이었던거 같다.

### Threshold : 500

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2014.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2014.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2015.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2015.png)

- 임계값을 높이니, 8개의 점으로 줄었다.
- 재미있는 점은, 주어진 템플릿과 원도 매우 흡사하여, 원의 중심부에서도 점이 많이 검출되고 있다.

### Threshold : 550

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2016.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2016.png)

![HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2017.png](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Untitled%2017.png)

- 하지만 정확히 table에 맞는 것은 plug 모양이므로 결국 plug에서 가장 많이 voting되는 것을 볼 수 있다.

## 고찰

- 이 소스에서는 임계값을 넘는 좌표에서 주변 8방향에 한해 Non-Maxima Suppression을 적용했는데, 그 결과 임계값을 높이더라도 주변의 노이즈가 조금 취약한 것을 볼 수 있었다.
- 이를 해결하기 위해서는 계산된 템플릿의 중심으로부터의 거리를 이용해서 Non-Maxima Suppression을 하면 좋을 것 같다.

## 소스코드

```cpp
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

                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
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

                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];
                                dstImg[tmpRow+i][tmpCol+j] += gaussianVoting[i+5][j+5];

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
```

# 4. 전체 소스코드

[Homeworks_CV.zip](HW06%20-%20hough%20Transform%20f20db419f2924d9e9da07705fda05428/Homeworks_CV.zip)