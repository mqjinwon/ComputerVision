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

    if(_q_pFormFocused != 0 && _q_pFormFocused->ID() == "OPEN"){
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
    }


    _plpImageForm->Add(q_pForm);
    q_pForm->show();

    //UI 활성화 갱신
    UpdateUI();
}
