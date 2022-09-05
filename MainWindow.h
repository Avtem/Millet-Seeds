#pragma once

#include <QMainWindow>
#include "AvtemLabel.h"
#include "Millet.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT
    Ui::MainWindow *ui;
    QImage mSrcImg;  // the original photo
    QImage mChangedImg;  // photo with changed pixels
    uchar *mImgPixels;
    AvtemLabel *mLabelImg;
    bool mCreated = false;

    QVector <Millet> mSeeds;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // AQImage. i expand the QImage class for convinience
    static QColor colorAt(const int &x, const int &y, const QImage &img);
    static void setPixelAt(const int &x, const int &y, QImage &img,
                             const QColor &color);

private slots:
    void onCreate();
    void showMousePos(const int x, const int y);
    void updatePreviewImg(const QImage &img);

    void on_btnRemoveBg_clicked();
    void removeBg(const int &hueValue);
    void fillBlackPixelsWithHighHue(QImage& img, int hueThreshold);

    void on_btnFoundSeeds_clicked();

    void on_btnPaintSeeds_clicked();

    void on_sliderBgRemoval_valueChanged(int value);

protected:
    virtual void showEvent(QShowEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

};
