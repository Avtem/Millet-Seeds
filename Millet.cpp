#include "Millet.h"
#include "MainWindow.h" // for "colorAt()"
#include <QDebug>

QVector <Millet> Millet::millets;

uint Millet::seedCount()
{
    return millets.size();
}

void nextPixelPos(int &x, int &y, const int &width) {
    ++x;
    if(x == width) {
        ++y;
        x = 0;
    }
}

void prevPixelPos(int &x, int &y, const int &width) {
    --x;
    if(x == -1) {
        --y;
        x = width -1;
    }
}

/*static*/ void Millet::findSeeds(QImage &img)
{
    // parameters, variables
    int maxPixelGap = img.width() *0.03; // search +- 3 pixels (because we don't get seed pixels
    QVector <QPoint> foundPixels;    // pixels, that are in a row like 100% of times
    bool lastPixelInRow = false;  // local variable

    // aliases
    auto colorAt = MainWindow::colorAt;
    const int &width = img.width();

  //////// algoritm starts here
    millets.clear();

    // checking every pixel...
    for(int x=0, y=0; y < img.height(); )
    {
        if(colorAt(x, y, img) != Qt::white
        && !foundPixels.contains({x,y}))
        {
            // we found the first pixel [of a seed?]!
            // now find reminding pixels [of this seed?]!

            millets.push_back(Millet());
            millets.last().mIndex = millets.size();

            // 1-st row
            lastPixelInRow = false;
            while(true) {
                millets.last().mPixels.push_back({{x,y}, colorAt(x, y, img)});
                nextPixelPos(x, y, width);

                for(int i=0; i < maxPixelGap; i++) {
                    // that's a seed pixel! Add it
                    if(colorAt(x,y,img) != Qt::white)
                        break;

                    nextPixelPos(x, y, width);

                    // there are no more pixels(
                    if(i+1 == maxPixelGap)
                        lastPixelInRow = true;
                }
                // all pixels from the first row are found.
                if(lastPixelInRow)
                    break;
            }

            // 2-nd, 3-rd etc. rows
            // init pos (for 2nd row)
            QPoint middlePixel = millets.last().mPixels.at(
                        millets.last().mPixels.size() / 2).pos;
            int constTempX = middlePixel.x();
            int constTempY = middlePixel.y() +1;

            for(int rowAdder = 0; true; rowAdder++)
            {
                int pixFound = 0;
                // set pos for this row
                int tempX = constTempX;
                int tempY = constTempY +rowAdder;
                if(tempY >= img.height())
                    break;

                // check to right
                lastPixelInRow = false;
                while(lastPixelInRow == false) {
                    for(int i=0; i < maxPixelGap; i++)
                    {
                        // that's a seed pixel! Add it
                        if(colorAt(tempX, tempY, img) != Qt::white){
                            millets.last().mPixels.push_back({{tempX, tempY},
                                                 colorAt(tempX, tempY, img)});
                            pixFound ++;
                            break;
                        }

                        nextPixelPos(tempX, tempY, width);

                        // there are no more pixels to the right
                        if(i+1 == maxPixelGap)
                            lastPixelInRow = true;
                    }

                    nextPixelPos(tempX, tempY, width);
                }

                // check to left
                // get pos for 2nd row, but first pos to left
                tempX = constTempX;
                tempY = constTempY +rowAdder;
                prevPixelPos(tempX, tempY, width);
                lastPixelInRow = false;
                while(lastPixelInRow == false ) {
                    for(int i=0; i < maxPixelGap; i++)
                    {
                        // that's a seed pixel! Add it
                        if(colorAt(tempX, tempY, img) != Qt::white){
                            millets.last().mPixels.push_back({{tempX, tempY},
                                                 colorAt(tempX, tempY, img)});
                            pixFound ++;
                            break;
                        }

                        prevPixelPos(tempX, tempY, width);

                        // there are no more pixels to the right
                        if(i+1 == maxPixelGap)
                            lastPixelInRow = true;
                    }

                    prevPixelPos(tempX, tempY, width);
                }

                // here current row finished searching.
                // if there were no pixels, seed is ready
                if(pixFound == 0) {
                    millets.last().setIsBad();
                    break;
                }
            }

            // add to local variable pixels of this seed
            for(int i=0; i < millets.constLast().mPixels.size(); i++)
                foundPixels.push_back(millets.constLast().mPixels.at(i).pos);
        }
        nextPixelPos(x, y, width);
    }

    deleteMisidentifiedSeeds();
}

void Millet::deleteMisidentifiedSeeds()
{
    const int biggestSeedSize = getBiggestSeedSize();

    // remove seed if its size less than 10% of the biggest seed
    for(int i=millets.size() -1; i >= 0; --i)
    {
        float size = millets.at(i).sizeInPixels();
        if(size / biggestSeedSize < 0.4f)
            millets.removeAt(i);
    }
}

int Millet::getBiggestSeedSize()
{
    uint biggestBoiSizeInPixels = 0;

    for(int i=0; i < millets.size(); ++i)
        biggestBoiSizeInPixels = qMax(biggestBoiSizeInPixels,
                                      millets.at(i).sizeInPixels());

    return biggestBoiSizeInPixels;
}


QVector<Millet::Pixel> Millet::pixels() const
{
    return mPixels;
}

void Millet::paintSeedOnImg(QImage &img, const QColor &color) const
{
    // highlight all pixels
    for(int i=0; i < mPixels.size(); i++) {
        const QPoint *pixelPos = &mPixels.at(i).pos;
        MainWindow::setPixelAt(pixelPos->x(), pixelPos->y(), img, color);
    }
}

bool Millet::isBad() const
{
    return mIsBad;
}

uint Millet::sizeInPixels() const
{
    return mPixels.size();
}


void Millet::setIsBad()
{
    float badSeedCount = 0;

    for(int i=0; i < mPixels.size(); i++) {
        mAverageHue += mPixels.at(i).color.hue();
        if(mPixels.at(i).color.hue() < 40)
            badSeedCount++;
    }
    mAverageHue /= mPixels.size();

    float bad = (badSeedCount / (float)mPixels.size()) ;
    mIsBad = bad > 0.7;
}

Millet::Millet()
    :mIsBad(false),
      mAverageHue(0),
      mCellIndex(-1)
{

}
