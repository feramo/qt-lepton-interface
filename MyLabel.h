#ifndef MYLABEL_H
#define MYLABEL_H

#include <QtCore>
#include <QWidget>
#include <QLabel>

#include <stdint.h>

//we extend QLabel to give it an extra slot, setImage
//this is because we can't pass a QPixmap from our thread
//so we have to pass a QImage and turn the QImage into a QPixmap on our end

class MyLabel : public QLabel {
  Q_OBJECT;

  public:
    MyLabel(QWidget *parent = 0);
    ~MyLabel();
    void mouseMoveEvent(QMouseEvent *ev);

  public slots:
    void setImage(QImage);

  signals:
    void enviaLabel(QString);

private:
  uint16_t *frameBuffer;

};

class TempLabel : public QLabel {
  Q_OBJECT;

  public:
    TempLabel(QWidget *parent = 0);
    ~TempLabel();

  public slots:
    void atualizaTemp(QString);
};

#endif
