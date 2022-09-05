#include "AvtemLabel.h"
#include <QMouseEvent>
#include "MainWindow.h"

AvtemLabel::AvtemLabel(QWidget *parent)
    : QLabel(parent)
{

}


void AvtemLabel::mouseMoveEvent(QMouseEvent *event)
{
    emit mousePosChanged(event->x(), event->y());
}
