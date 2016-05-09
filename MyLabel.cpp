#include "MyLabel.h"
#include "LeptonThread.h"
#include "Palettes.h"

#include <QMouseEvent>
#include <QString>
#include <QPicture>
#include <QColor>
#include <QFileDialog>
#include <QDataStream>
#include <QFile>

#include <stdio.h>

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

    LeptonThread::Instance()->get_result(frameBuffer);
    int x_eq = (ev->x() * 80 / this->width());
    int y_eq = (ev->y() * 60 / this->height());

    uint32_t pix_temp = (uint32_t) frameBuffer[x_eq + 2 + (y_eq * 82)];

    emit enviaLabel(QString::number(pix_temp));

    pix_temp = (uint32_t) ((pix_temp-a_fact)*b_fact_inv100);
    emit enviaLevel((int) pix_temp);
    uint8_t temp_integ = pix_temp/100;
    uint8_t temp_dec = pix_temp/10%10;
    uint8_t temp_cent = pix_temp%10;

    emit enviaLabel_C(QString("%1.%2%3").arg(temp_integ).arg(temp_dec).arg(temp_cent));

    emit enviaMousePos(QPoint(x_eq, y_eq));
}

//when the system calls setImage, we'll set the label's pixmap
void MyLabel::setImage(QImage image) {
  QPixmap pixmap = QPixmap::fromImage(image);
  int w = this->width();
  int h = this->height();
  setPixmap(pixmap.scaled(w, h, Qt::IgnoreAspectRatio));
}

TempLabel::TempLabel(QWidget *parent) : QLabel(parent)
{
}
TempLabel::~TempLabel()
{
}

void TempLabel::atualizaTemp(QString temp_str)
{
  this->setText(temp_str);
}

defLabel::defLabel(QWidget *parent) : QLabel(parent)
{
}
defLabel::~defLabel()
{
}

void defLabel::writeText(QString temp_str)
{
  this->setText(temp_str);
}

void MyLabel::salvaBMP()
{
    QString filename = QFileDialog::getSaveFileName(parentWidget(),tr("Salvar BMP"), "/home/root", "Imagem de bitmap (*.BMP)");
    QFileInfo fi(filename);
    if(fi.baseName() != "")
    {
        filename = fi.baseName() + ".bmp";
        QImage temp_Img = this->pixmap()->toImage();
        temp_Img.save(filename, "BMP", 100);

        filename =filename + ".raw";
        QFile raw(filename);
        if(raw.open(QFile::WriteOnly))
        {
          LeptonThread::Instance()->get_result(frameBuffer);
          QDataStream out(&raw);

          for(int i=0; i<(PACKET_SIZE*PACKETS_PER_FRAME/2);i++)
          {
            out << frameBuffer[i];
          }
          raw.close();
        }
    }
}

void TempLabel::updateCamTemp(int camTemp)
{
    QString qs_temp;
    this->setText(QString("%1.%2%3K").arg(camTemp/100).arg(camTemp/10%10).arg(camTemp%10));
}

ColorBar::ColorBar(QWidget *parent) : QLabel(parent)
{
//    getPaletteIndex(0);
}
ColorBar::~ColorBar()
{
}

void ColorBar::setPaletteIndex(int index)
{
    int *cm_addr;

    switch(index)
    {
    case 0:
      cm_addr = cm_lla_default;
      break;
    case 1:
      cm_addr = cm_lla_hotblue;
      break;
    case 2:
      cm_addr = cm_lla_medic;
      break;
    case 3:
      cm_addr = cm_lla_medic2;
      break;
    case 4:
      cm_addr = cm_lla_medic3;
      break;
    case 5:
      cm_addr = cm_lla_grayscale;
      break;
    default:
      break;
    }
    int *colormap = (int *) cm_addr;

    QImage paletteImage(1, 256, QImage::Format_RGB888);
    int w = this->width();
    int h = this->height();
    for ( int i = 0; i < 256; ++i )
    {
        paletteImage.setPixel(0,256-i,qRgb(colormap[3*i],colormap[3*i+1],colormap[3*i+2]));
    }
    setPixmap((QPixmap::fromImage(paletteImage)).scaled(w, h, Qt::IgnoreAspectRatio));
}

void ColorBar::setBarHeight(uint16_t height, uint16_t bottom)
{
    bar_height = height;
    bar_bottom = bottom;
}

void ColorBar::getLevel(int level)
{
    LeptonThread::Instance()->get_range(&range_min, &range_max);
    if(level>range_max*100)
    {
        level=range_max*100;
    }
    else if(level<range_min*100)
    {
        level=range_min*100;
    }
    int pos_h = bar_bottom-(level-range_min*100)*bar_height/(range_max-range_min)/100 - this->geometry().width();
    this->setGeometry(this->geometry().x(),pos_h,this->geometry().width(),this->geometry().height());
}
