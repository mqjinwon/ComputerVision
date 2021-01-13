#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QDialog>
#include "iostream"
#include "kfc.h"
#include "kimgprocess.h"

namespace Ui {
class MainFrame;

}

class ImageForm;
class KVoronoiDgm;
class KPotentialField;

class MainFrame : public QDialog
{
    Q_OBJECT

private:
    Ui::MainFrame *ui;

    KPtrList<ImageForm*>*   _plpImageForm;
    ImageForm*              _q_pFormFocused;
    KImgProcess             ImgProcess;


public:
    explicit MainFrame(QWidget *parent = 0);
    ~MainFrame();

    void            ImageFormFocused(ImageForm* q_pImageForm)
                    {   _q_pFormFocused  = q_pImageForm;   //활성화된 창의 포인터를 저장함
                        UpdateUI();                        //UI 활성화 갱신
                    }
    void            UpdateUI();
    void            CloseImageForm(ImageForm* pForm);

public:
    void            OnMousePos(const int& nX, const int& nY, ImageForm* q_pForm);
    void            DrawHist(KImageGray& igGray, char* name="plot");


private slots:
    void on_buttonOpen_clicked();
    void on_buttonDeleteContents_clicked();    
    void on_buttonSepiaTone_clicked();
    void on_buttonShowList_clicked();

    void on_buttonContrast_clicked();

    void on_buttonOtsu_clicked();

    void on_buttonMorp_clicked();

    void on_buttonLabel_clicked();

    void on_buttonHistoMatch_clicked();

    void on_buttonHistoEqual_clicked();

    void on_buttonGN_clicked();

    void on_buttonSalt_clicked();

    void on_buttonGF_clicked();

    void on_buttonMF_clicked();

    void on_buttonFDG_clicked();

    void on_buttonCE_clicked();

    void on_buttonCHT_clicked();

    void on_buttonGHT_clicked();

    void on_buttonOF_clicked();

protected:
    void closeEvent(QCloseEvent* event);
};

#endif // MAINFRAME_H
