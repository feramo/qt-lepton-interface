#include "MyLabel.h"
#include "LeptonThread.h"

#include <QMouseEvent>
#include <QString>
#include <QPicture>
#include <QColor>
#include <QFileDialog>

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
    LeptonThread::Instance()->get_result(frameBuffer);
    int x_eq = (ev->x() * 80 / this->width());
    int y_eq = (ev->y() * 60 / this->height());

    uint32_t pix_temp = (uint32_t) frameBuffer[x_eq + 2 + (y_eq * 82)];

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
  this->setText("Temp: " + temp_str);
}

void MyLabel::salvaBMP()
{
    QString filename = QFileDialog::getSaveFileName(parentWidget(),tr("Salvar BMP"), "/boot/", "Imagem de bitmap (*.BMP)");
    if(filename != "")
    {
        QImage temp_Img = this->pixmap()->toImage();
        temp_Img.save(filename, "BMP", 100);
    }
}
