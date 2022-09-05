#pragma once

#include <QWidget>
#include <QLabel>

class AvtemLabel : public QLabel
{
    Q_OBJECT
public:
    explicit AvtemLabel(QWidget *parent = nullptr);

signals:
    void mousePosChanged(const int x, const int y);
    // QWidget interface
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
};

