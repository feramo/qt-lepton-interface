#include "MyLabel.h"
#include "LeptonThread.h"

#include <QMouseEvent>
#include <QString>
#include <QPicture>
#include <QColor>

MyLabel::MyLabel(QWidget *parent) : QLabel(parent)
{
    setMouseTracking(true);
}
MyLabel::~MyLabel()
{
}

void MyLabel::mouseMoveEvent(QMouseEvent *ev)
{
    QImage temp_Img = this->pixmap()->toImage();
    QColor c_pix(temp_Img.pixel(ev->pos()));

    //frameBuffer = (uint16_t *) lepton_result;
    LeptonThread* flir = new LeptonThread();
    flir->get_result(frameBuffer);

    //int pix_temp = (int) frameBuffer[(ev->x())];
    int pix_temp = 0;
    qDebug() << "FrameBuffer[0]= " << frameBuffer[0];

    emit enviaLabel(QString::number(pix_temp));
}

//when the system calls setImage, we'll set the label's pixmap
void MyLabel::setImage(QImage image) {
  QPixmap pixmap = QPixmap::fromImage(image);
  int w = this->width();
  int h = this->height();
  setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
}

TempLabel::TempLabel(QWidget *parent) : QLabel(parent)
{
}
TempLabel::~TempLabel()
{
}

void TempLabel::atualizaTemp(QString temp_str)
{
  this->setText(temp_str + "ADU");
}
