#include <QFileDialog>
#include <QPainter>

#include "mainframe.h"
#include "ui_mainframe.h"
#include "imageform.h"

MainFrame::MainFrame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);

    _plpImageForm       = new KPtrList<ImageForm*>(10,false,false);
    _q_pFormFocused     = 0;

    //객체 맴버의 초기화

    //get a current directory
    char st[100];
    GetCurrentDirectoryA(100,st);

    //리스트 출력창을 안보이게
    ui->listWidget->setVisible(false);
    this->adjustSize();

    //UI 활성화 갱신
    UpdateUI();
}

MainFrame::~MainFrame()
{ 
    delete ui;         
    delete _plpImageForm;


}

void MainFrame::CloseImageForm(ImageForm *pForm)
{
    //ImageForm 포인터 삭제
    _plpImageForm->Remove(pForm);

    //활성화 ImageForm 초기화
    _q_pFormFocused     = 0;

    //관련 객체 삭제

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::UpdateUI()
{
    if(ui->tabWidget->currentIndex() == 0)
    {
        ui->buttonSepiaTone->setEnabled( _q_pFormFocused &&  _q_pFormFocused->ID() == "OPEN" );
        ui->buttonContrast->setEnabled( _q_pFormFocused &&  _q_pFormFocused->ID() == "OPEN" );

    }
    else if(ui->tabWidget->currentIndex() == 1)
    {

    }
    else if(ui->tabWidget->currentIndex() == 2)
    {

    }    
}

void MainFrame::OnMousePos(const int &nX, const int &nY, ImageForm* q_pForm)
{

    UpdateUI();
}

void MainFrame::DrawHist(KImageGray &igGray, char *name)
{
    // histogram 구하기
    KHisto hTmp;
    hTmp.Histogram(igGray);

    QVector<double> x(256), y(256);
    for(int i=0; i<256; i++){
        x[i] = i;
        y[i] = hTmp[i];
    }

    ImgProcess.DrawHistgram(x, y, name);
}



void MainFrame::closeEvent(QCloseEvent* event)
{
    //생성된 ImageForm을 닫는다.
    for(int i=_plpImageForm->Count()-1; i>=0; i--)
        _plpImageForm->Item(i)->close();

    //리스트에서 삭제한다.
    _plpImageForm->RemoveAll();
}


void MainFrame::on_buttonOpen_clicked()
{
    //이미지 파일 선택
    QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
    QString                 q_stFile    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);

    if(q_stFile.length() == 0)
        return;

    //이미지 출력을 위한 ImageForm 생성    
    ImageForm*              q_pForm   = new ImageForm(q_stFile, "OPEN", this);

    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonDeleteContents_clicked()
{
    //생성된 ImageForm을 닫는다.
    for(int i=_plpImageForm->Count()-1; i>=0; i--)
        _plpImageForm->Item(i)->close();

    //리스트에서 삭제한다.
    _plpImageForm->RemoveAll();
}


// execute SepiaTone Transform
void MainFrame::on_buttonSepiaTone_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageColor   icMain;    
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageColor().Address() &&  _q_pFormFocused->ID() == "OPEN")
        icMain = _q_pFormFocused->ImageColor();
    else
        return;

    KArray<KHSV> igHSV;
    icMain.RGBtoHSV(igHSV);

    KImageGray HImg(icMain.Row(), icMain.Col()), SImg(icMain.Row(), icMain.Col()), VImg(icMain.Row(), icMain.Col());

    for(int nRow=0; nRow < igHSV.Row(); nRow++)
        for(int nCol=0; nCol < igHSV.Col(); nCol++){
            HImg[nRow][nCol] = igHSV[nRow][nCol].h * 255. / 360.;
            SImg[nRow][nCol] = igHSV[nRow][nCol].s * 255.;
            VImg[nRow][nCol] = igHSV[nRow][nCol].v;
        }


    //hue, saturation 값 가져오기
    double dHue = ui->spinHue->text().toDouble();
    if(dHue>=360)   dHue = 0;
    double dSat = ui->spinSaturation->text().toDouble();


    //
    //icMain 변환
    //:

    // 방법 1 : kfc 라이브러리 이용

    KImageGray igGray = icMain.ColorToGray();
    DrawHist(igGray, "before executing SepiaTone Transform");

    icMain = ImgProcess.SepiaTransform(icMain, dHue, dSat);

    igGray = icMain.ColorToGray();
    DrawHist(igGray, "after executing SepiaTone Transform");

    // 방법 2 : 직접 노가다 구현
#if 0
    for(int col=0; col < icMain.Col(); col++){
        for(int row=0; row < icMain.Row(); row++){

            //v : rgb중, max를 찾음
            int v=0;
            if(icMain[row][col].r > v) v = icMain[row][col].r;
            if(icMain[row][col].g > v) v = icMain[row][col].g;
            if(icMain[row][col].b > v) v = icMain[row][col].b;

            // hsv to rgb 과정
            double c = v * dSat;
            int h = dHue/60;
            double x = c*(1- abs(h%2 - 1));

            switch(h){
            case 0:
                icMain[row][col].r = c;
                icMain[row][col].g = x;
                icMain[row][col].b = 0;
                break;
            case 1:
                icMain[row][col].r = x;
                icMain[row][col].g = c;
                icMain[row][col].b = 0;
                break;
            case 2:
                icMain[row][col].r = 0;
                icMain[row][col].g = c;
                icMain[row][col].b = x;
                break;
            case 3:
                icMain[row][col].r = 0;
                icMain[row][col].g = x;
                icMain[row][col].b = c;
                break;
            case 4:
                icMain[row][col].r = x;
                icMain[row][col].g = 0;
                icMain[row][col].b = c;
                break;
            case 5:
                icMain[row][col].r = c;
                icMain[row][col].g = 0;
                icMain[row][col].b = x;
                break;

            default:
                break;

            }

            int m = v - c;
            icMain[row][col].r += m;
            icMain[row][col].g += m;
            icMain[row][col].b += m;

        }
    }
#endif

    //출력을 위한 ImageForm 생성

    ImageForm*  q_pForm = new ImageForm(HImg, "Hue", this);
    _plpImageForm->Add(q_pForm);
    q_pForm ->show();
    q_pForm = new ImageForm(SImg, "Sat.", this);
    _plpImageForm->Add(q_pForm);
    q_pForm->show();
    q_pForm = new ImageForm(VImg, "Value", this);
    _plpImageForm->Add(q_pForm);
    q_pForm->show();
    q_pForm = new ImageForm(icMain, "Sepia Tone", this);
    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}


void MainFrame::on_buttonShowList_clicked()
{
    static int nWidthOld = ui->tabWidget->width();

    if(ui->listWidget->isVisible())
    {
        nWidthOld = ui->listWidget->width();
        ui->listWidget->hide();
        this->adjustSize();
    }
    else
    {        
        ui->listWidget->show();
        QRect q_rcWin = this->geometry();

        this->setGeometry(q_rcWin.left(), q_rcWin.top(), q_rcWin.width()+nWidthOld, q_rcWin.height());
    }
}


// execute luminance contrast transform
void MainFrame::on_buttonContrast_clicked()
{
    // Min, Max 값 가져오기
    int dMin = ui->spinMin->text().toDouble();  if(dMin < 0)    dMin = 0;
    int dMax = ui->spinMax->text().toDouble();  if(dMax > 255)  dMax = 255;

    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor icMain = _q_pFormFocused->ImageColor();


            KImageGray igGray = icMain.ColorToGray();
            DrawHist(igGray, "before executing luminance contrast transform");

            icMain = ImgProcess.ContrastTransform(icMain, dMin, dMax);

            igGray = icMain.ColorToGray();
            DrawHist(igGray, "after executing luminance contrast transform");

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "Contrast Image", this);
        }
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            DrawHist(igMain, "before executing luminance contrast transform");

            ImgProcess.ContrastTransform(igMain, dMin, dMax);

            DrawHist(igMain, "after executing luminance contrast transform");


            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "Contrast Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();

    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonOtsu_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageGray   igMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageGray().Address() &&  _q_pFormFocused->ID() == "OPEN")
        igMain = _q_pFormFocused->ImageGray();
    else
        return;

    ImageForm*  q_pForm;

    ImgProcess.OtsuThreshold(igMain);

    // 출력을 위한 ImageForm 생성
    q_pForm = new ImageForm(igMain, "Otsu Image", this);

    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonMorp_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageGray   igMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageGray().Address())
        igMain = _q_pFormFocused->ImageGray();
    else
        return;

    ImageForm*  q_pForm;

    std::string cbitem = ui->cbMorp->currentText().toUtf8().constData();

    if(cbitem == "Dilation"){
        igMain = ImgProcess.Dilation(igMain);
        q_pForm = new ImageForm(igMain, "Dilation Image", this);
    }
    else if(cbitem == "Erosion"){
        igMain = ImgProcess.Erosion(igMain);
        q_pForm = new ImageForm(igMain, "Erosion Image", this);
    }
    else if(cbitem == "Opening"){
        igMain = ImgProcess.Opening(igMain);
        q_pForm = new ImageForm(igMain, "Opening Image", this);
    }
    else if(cbitem == "Closing"){
        igMain = ImgProcess.Closing(igMain);
        q_pForm = new ImageForm(igMain, "Closing Image", this);
    }
    else{
        q_pForm = new ImageForm(igMain, "noChange Image", this);
    }

    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonLabel_clicked()
{
    //포커스 된 ImageForm으로부터 영상을 가져옴
    KImageGray   igMain;
    if(_q_pFormFocused != 0 && _q_pFormFocused->ImageGray().Address())
        igMain = _q_pFormFocused->ImageGray();
    else
        return;

    ImageForm*  q_pForm;

    // 라벨 색깔을 표시하기 위해서
    KImageColor  labelImg(igMain.Row(), igMain.Col());
    int r,g,b;

    // 난수 생성
    srand(time(NULL));

    _1DPIXEL    labelData;
    labelData = ImgProcess.Labeling(igMain);

    for(int nLabel=0; nLabel<labelData.size(); nLabel++){
        r = rand() % 256;
        g = rand() % 256;
        b = rand() % 256;
        for(int nData=0; nData<labelData[nLabel]->size(); nData++){
            labelImg[labelData[nLabel]->at(nData)/igMain.Col()][labelData[nLabel]->at(nData)%igMain.Col()].r = r;
            labelImg[labelData[nLabel]->at(nData)/igMain.Col()][labelData[nLabel]->at(nData)%igMain.Col()].g = g;
            labelImg[labelData[nLabel]->at(nData)/igMain.Col()][labelData[nLabel]->at(nData)%igMain.Col()].b = b;
        }
    }

    q_pForm = new ImageForm(labelImg, "Label Image", this);

    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonHistoEqual_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            // histogram을 보기 위한 장치
            KImageGray colorImg[3];
            for(int i=0; i<3; i++){
                colorImg[i].Create(icMain.Row(), icMain.Col());
            }

            for(int nRow=0; nRow < icMain.Row(); nRow++){
                for(int nCol=0; nCol < icMain.Col(); nCol++){
                    colorImg[0][nRow][nCol] = icMain[nRow][nCol].r;
                    colorImg[1][nRow][nCol] = icMain[nRow][nCol].g;
                    colorImg[2][nRow][nCol] = icMain[nRow][nCol].b;
                }
            }

            DrawHist(colorImg[0], "before HE red");
            DrawHist(colorImg[1], "before HE green");
            DrawHist(colorImg[2], "before HE blue");


            ImgProcess.HistogramEqualization(icMain);

            for(int nRow=0; nRow < icMain.Row(); nRow++){
                for(int nCol=0; nCol < icMain.Col(); nCol++){
                    colorImg[0][nRow][nCol] = icMain[nRow][nCol].r;
                    colorImg[1][nRow][nCol] = icMain[nRow][nCol].g;
                    colorImg[2][nRow][nCol] = icMain[nRow][nCol].b;
                }
            }

            DrawHist(colorImg[0], "after HE red");
            DrawHist(colorImg[1], "after HE green");
            DrawHist(colorImg[2], "after HE blue");

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "HE Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            DrawHist(igMain, "before executing Histogram Equalization transform");

            ImgProcess.HistogramEqualization(igMain);

            DrawHist(igMain, "after executing Histogram Equalization transform");


            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "HE Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}


void MainFrame::on_buttonHistoMatch_clicked()
{

    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            // COLOR 이미지를 고를때까지 반복
            do{
            //이미지 파일 선택
            QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
            QString                 q_stFile    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);

            if(q_stFile.length() == 0)
                return;

            //비교를 위한 이미지 폼 생성
            q_pForm     = new ImageForm(q_stFile, "Tmp image", this);


            if(!q_pForm->ImageColor().Address()){
                delete q_pForm;
            }

            }
            while(!q_pForm->ImageColor().Address());

            ImgProcess.HistogramMatching(icMain, q_pForm->ImageColor());


            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "HM Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            // GRAYSCALE 이미지를 고를때까지 반복
            do{
            //이미지 파일 선택
            QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
            QString                 q_stFile    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);

            if(q_stFile.length() == 0)
                return;

            //비교를 위한 이미지 폼 생성
            q_pForm     = new ImageForm(q_stFile, "Tmp image", this);


            if(!q_pForm->ImageGray().Address()){
                delete q_pForm;
            }

            }
            while(!q_pForm->ImageGray().Address());

            DrawHist(q_pForm->ImageGray(), "Target Histogram");

            DrawHist(igMain, "before executing Histogram Matching transform");

            ImgProcess.HistogramMatching(igMain, q_pForm->ImageGray());

            DrawHist(igMain, "after executing Histogram Matching transform");


            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "HM Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonGN_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            double mean[3]  = {20, 20, 20};
            double var[3]   = {50, 50, 50};

            icMain = ImgProcess.GaussianNoiseToColorImage(icMain, mean, var);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "GN Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.GaussianNoiseToGrayImage(igMain, 20, 50);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "GN Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonSalt_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            icMain = ImgProcess.PepperSaltToColorImage(icMain);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "PS Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.PepperSaltToGrayImage(igMain);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "PS Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonGF_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            icMain = ImgProcess.GaussianSmoothing(icMain, 0.7);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "PS Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.GaussianSmoothing(igMain, 0.7);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "GS Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonMF_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){
        // color image
        if(_q_pFormFocused->ImageColor().Address()){
            KImageColor   icMain;

            icMain = _q_pFormFocused->ImageColor();

            icMain = ImgProcess.MedianFiltering(icMain, 3);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(icMain, "MF Image", this);

        }
        // gray image
        else if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.MedianFiltering(igMain, 3);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "MF Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonFDG_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){

        if(_q_pFormFocused->ImageGray().Address()){
            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.FDG(igMain);


#if DEBUG
            _EDGEIMG tmpGradient = ImgProcess.showGradient();

            for(int nRow = 0; nRow < tmpGradient.size(); nRow++){
                for(int nCol = 0; nCol < tmpGradient[nRow]->size(); nCol++){
                    printf("%.0f, ", (*tmpGradient[nRow])[nCol].magn);
                }
                printf("\n");
            }
#endif

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "FDG Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();

}

void MainFrame::on_buttonCE_clicked()
{
    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){

        if(_q_pFormFocused->ImageGray().Address()){

            // t0, t1 값 가져오기
            double t0 = ui->spinMin_t0->text().toDouble();
            double t1 = ui->spinMin_t1->text().toDouble();

            KImageGray   igMain;

            igMain = _q_pFormFocused->ImageGray();

            igMain = ImgProcess.CannyEdge(igMain, t0, t1);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(igMain, "Canny Edge Image", this);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonCHT_clicked()
{
    ui->textBrowserResult->clear();

    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){

        if(_q_pFormFocused->ImageGray().Address()){

            KImageGray   igMain;
            igMain = _q_pFormFocused->ImageGray();

            _POINTS result;
            QString resultTxt;

            double threshold = ui->doubleSpinBoxThreshold->text().toDouble();

            // 반지름이 정해졌을 때
            int isRadius = ui->checkBoxRadius->checkState();

            if(isRadius == Qt::CheckState::Checked){
                double radius = ui->plainTextEditRadius->toPlainText().toDouble();
                result = ImgProcess.CircleHoughTransform(igMain, radius, threshold);

            }
            // 반지름이 정해지지 않았을 때
            else{
                double minR = ui->plainTextEditMinR->toPlainText().toDouble();
                double maxR = ui->plainTextEditMaxR->toPlainText().toDouble();

                result = ImgProcess.CircleHoughTransform(igMain, minR, maxR, threshold);
            }

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(ImgProcess.GetResultIcImg(), "circle HoughTransform Image", this);

            for(int i=0; i < result.size(); i++){
                resultTxt += QString::number(i+1);
                resultTxt += ". row : ";
                resultTxt += QString::number(result[i].first);
                resultTxt += ", col : ";
                resultTxt += QString::number(result[i].second);
                resultTxt += "\n";
            }

            ui->textBrowserResult->setText(resultTxt);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonGHT_clicked()
{
    ui->textBrowserResult->clear();

    ImageForm*  q_pForm;

    if(_q_pFormFocused != 0){

        if(_q_pFormFocused->ImageGray().Address()){

            //txt 파일 선택
            QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
            QString                 q_stFile    =  QFileDialog::getOpenFileName(this, tr("Select a txt File"),  "./data", "txt Files(*.txt)",0, q_Options);

            if(q_stFile.length() == 0)
                return;

            KImageGray   igMain;
            igMain = _q_pFormFocused->ImageGray();

            _POINTS result;
            QString resultTxt;

            double threshold = ui->doubleSpinBoxThreshold->text().toDouble();

            result = ImgProcess.GeneralHoughTransform(igMain, q_stFile.toUtf8().constData(), threshold);

            // 출력을 위한 ImageForm 생성
            q_pForm = new ImageForm(ImgProcess.GetResultIcImg(), "general HoughTransform Image", this);

            for(int i=0; i < result.size(); i++){
                resultTxt += QString::number(i+1);
                resultTxt += ". row : ";
                resultTxt += QString::number(result[i].first);
                resultTxt += ", col : ";
                resultTxt += QString::number(result[i].second);
                resultTxt += "\n";
            }

            ui->textBrowserResult->setText(resultTxt);
        }
        else{
            return;
        }

        _plpImageForm->Add(q_pForm);
        q_pForm->show();
    }

    //UI 활성화 갱신
    UpdateUI();
}

void MainFrame::on_buttonOF_clicked()
{
    //이미지 파일 선택
    QFileDialog::Options    q_Options   =  QFileDialog::DontResolveSymlinks  | QFileDialog::DontUseNativeDialog; // | QFileDialog::ShowDirsOnly
    QString                 q_stFile1    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);
    QString                 q_stFile2    =  QFileDialog::getOpenFileName(this, tr("Select a Image File"),  "./data", "Image Files(*.bmp *.ppm *.pgm *.png)",0, q_Options);

    if(q_stFile1.length() == 0 || q_stFile2.length() == 0)
        return;



    //이미지 출력을 위한 ImageForm 생성
    ImageForm*  q_pFormSrc1   = new ImageForm(q_stFile1, "src1", this);
    ImageForm*  q_pFormSrc2   = new ImageForm(q_stFile2, "src2", this);
    ImageForm*  q_pFormRes;


    _plpImageForm->Add(q_pFormSrc1);
    _plpImageForm->Add(q_pFormSrc2);

    q_pFormSrc1->show();
    q_pFormSrc2->show();

    KImageGray igSrc1 = q_pFormSrc1->ImageGray();
    KImageGray igSrc2 = q_pFormSrc2->ImageGray();

    KImageColor icMain;
    _EDGEIMG    edgeImg;

    vector<vector<double>> a;
    vector<double> tmp;
    tmp.assign(2, 0.);
    a.assign(2, tmp);

    vector<vector<double>> b;
    b.assign(2, tmp);

    for(int nRow=0; nRow < 2; nRow++){
        for(int nCol=0; nCol < 2; nCol++){
           a[nRow][nCol] = nCol + nRow*2;
           b[nRow][nCol] = nCol + nRow*2;
        }
    }

//    JMatrix<double> mats;
//    _JMAT m1 = mats.AddMatrix(a, b);
//    _JMAT m2 = mats.DotProduct(a, b);
//    _JMAT m3 = mats.MinusMatrix(a, b);
//    _JMAT m4 = mats.DivideMatrix(a, b);
//    _JMAT m5 = mats.Inverse2Matrix(a);
//    _JMAT m6 = mats.TransposeMatrix(a);


    edgeImg = ImgProcess.OpticalFlow(igSrc1, igSrc2);
    icMain = ImgProcess.GetResultIcImg();

    q_pFormRes = new ImageForm(icMain, "Optical Flow Image", this);

    int nScale = 8;

    for(int nRow = 0; nRow < icMain.Row()/nScale; nRow++){
        for(int nCol=0; nCol < icMain.Col()/nScale; nCol++){
            int nDx = edgeImg[nRow*nScale]->at(nCol*nScale).dx;
            int nDy = edgeImg[nRow*nScale]->at(nCol*nScale).dy;

            q_pFormRes->DrawLine(nCol*nScale, nRow*nScale, nCol*nScale+nDx, nRow*nScale+nDy,
                                 QColor(255, 255, 0), 1);
        }
    }

    _plpImageForm->Add(q_pFormRes);
    q_pFormRes->show();


    //UI 활성화 갱신
    UpdateUI();

}
