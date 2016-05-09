#ifndef MYLABEL_H
#define MYLABEL_H

#include <QtCore>
#include <QWidget>
#include <QLabel>

#include <stdint.h>

//we extend QLabel to give it an extra slot, setImage
//this is because we can't pass a QPixmap from our thread
//so we have to pass a QImage and turn the QImage into a QPixmap on our end

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)

class MyLabel : public QLabel {
  Q_OBJECT;

  public:
    MyLabel(QWidget *parent = 0);
    ~MyLabel();
    void mouseMoveEvent(QMouseEvent *ev);

  public slots:
    void setImage(QImage);
    void salvaBMP();

  signals:
    void enviaLabel(QString);
    void enviaLabel_C(QString);
    void enviaMousePos(QPoint);
    void enviaLevel(int);

private:
  uint16_t frameBuffer[(PACKET_SIZE*PACKETS_PER_FRAME/2)];

};

class TempLabel : public QLabel {
  Q_OBJECT;

  public:
    TempLabel(QWidget *parent = 0);
    ~TempLabel();

  public slots:
    void updateCamTemp(int);
    void atualizaTemp(QString);
};

class defLabel : public QLabel {
  Q_OBJECT;

  public:
    defLabel(QWidget *parent = 0);
    ~defLabel();

  public slots:
    void writeText(QString);
};

class ColorBar : public QLabel {
  Q_OBJECT;

  public:
    ColorBar(QWidget *parent = 0);
    ~ColorBar();
    void setBarHeight(uint16_t height, uint16_t bottom);

  public slots:
    void setPaletteIndex(int);
    void getLevel(int);

  private:
    uint8_t range_min, range_max;
    uint16_t bar_height, bar_bottom;

};

#endif
