#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QtDebug>
#include <QMouseEvent>
#include <QBitmap> // for setting img to label!!!
#include <QFileDialog>
#include <windows.h>

#define tostr(string) QString::number(string)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mLabelImg = dynamic_cast<AvtemLabel*>(ui->labelImg);

    // MUST BE ABSOLUTE PATH
    QString path = QFileDialog::getOpenFileName(this, "Please choose photo with seeds");
    if(path.isEmpty())
    {
        MessageBoxW(0, L"No image was selected. Exiting the app", L"Error", 0);
        std::exit(1);
    }
    mSrcImg = QImage(path);
    //mSrcImg = mSrcImg.scaledToWidth(400);   // reduce img size to process it faster
    mChangedImg = mSrcImg;

    connect(mLabelImg, &AvtemLabel::mousePosChanged,
            this, &MainWindow::showMousePos);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCreate()
{
    mCreated = true;

    updatePreviewImg(mSrcImg);
}

void MainWindow::showMousePos(const int x, const int y)
{    
}

QColor MainWindow::colorAt(const int &x, const int &y, const QImage &img)
{
    // ONLY IF IMAGE TYPE IS BGRA!
    if(x >= img.width() || y >= img.height()) {
        qDebug() << "colorAt: x or y is too big!";
        return QColor();
    }

    const uchar *bits = img.constBits();

    quint32 pos = x + y * img.width();
    pos *= 4;

    const uchar &colR = bits[pos +2];
    const uchar &colG = bits[pos +1];
    const uchar &colB = bits[pos +0];

    return QColor(colR, colG, colB);
}

void MainWindow::setPixelAt(const int &x, const int &y, QImage &img,
                              const QColor &color)
{
    uchar *bits = img.bits();

    const uchar &R = color.red();
    const uchar &G = color.green();
    const uchar &B = color.blue();
    const uchar &A = color.alpha();

    int pos = x +y *img.width();
    pos *= 4; // because img format is RGBA
    if(pos /4 >= img.width() *img.height()) {
        qWarning() << "setPixelAt(): x or y is too big";
        return;
    }

    bits[pos +0] = B;
    bits[pos +1] = G;
    bits[pos +2] = R;
    bits[pos +3] = A;
}

void MainWindow::updatePreviewImg(const QImage &img)
{
    int availableW = ui->horizontalLayoutUpper->geometry().width();
    int availableH = ui->horizontalLayoutUpper->geometry().height();

    if(!availableH)
        return;

    mLabelImg->setPixmap(QPixmap::fromImage(img).scaled(availableW, availableH,
                                                 Qt::KeepAspectRatio));
}


void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    onCreate();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    updatePreviewImg(mChangedImg);
}


void MainWindow::on_btnRemoveBg_clicked()
{
    ui->sliderBgRemoval->setValue(200);
    ui->statusBar->showMessage("Background removed");
}

void MainWindow::removeBg(const int &hueValue)
{
    mChangedImg = mSrcImg;

    fillBlackPixelsWithHighHue(mChangedImg, hueValue);

    updatePreviewImg(mChangedImg);
}

void MainWindow::fillBlackPixelsWithHighHue(QImage &img, int hueThreshold)
{
    for(int y = 0; y < img.height(); ++y)
    {
        for(int x = 0; x < img.width(); ++x)
        {
            if(QColor(img.pixel(x,y)).hue() > hueThreshold)
                img.setPixel(x, y, QColor(Qt::white).rgba());
        }
    }
}



void MainWindow::on_btnFoundSeeds_clicked()
{
    Millet::findSeeds(mChangedImg);
    ui->statusBar->showMessage("Done. Found "
                            + QString::number(Millet::seedCount()) + " seeds");
}

void MainWindow::on_btnPaintSeeds_clicked()
{
    for(uint i=0; i < Millet::seedCount(); i++)
        Millet::millets.at(i).paintSeedOnImg(mChangedImg, QColor(Qt::red));

    updatePreviewImg(mChangedImg);
    ui->statusBar->showMessage("Seeds painted");
    Millet::deleteMisidentifiedSeeds();
}




void MainWindow::on_sliderBgRemoval_valueChanged(int value)
{
    ui->labelHUEValue->setText("HUE filter value: " +tostr(value));
    removeBg(value);
}

