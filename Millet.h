#pragma once

#include <QObject>
#include <QMainWindow>
#include <QPoint>

class Millet
{
public:
     // struct representing a single pixel in an image
    // (where pos is position on the Source image)
    struct Pixel
    {
        QPoint pos;
        QColor color;
    };
    Millet();

    static QVector <Millet> millets;

    QVector<Pixel> pixels() const;
    void paintSeedOnImg(QImage &img, const QColor &color) const;
    bool isBad() const;
    uint sizeInPixels() const;

    static uint seedCount();
    static void findSeeds(QImage &img);
    static void deleteMisidentifiedSeeds();  // removes tiny dots that algorithm considered as a millet seed
    static int getBiggestSeedSize();

private:
    bool mIsBad;  // not eatable seeds, junk
    int mIndex;
    int mAverageHue;
    int mCellIndex; // index of the MOVABLE COMPONENT IRL!!!
    QVector<Pixel> mPixels;  // positions on the SourceImage

    void setIsBad();
};

