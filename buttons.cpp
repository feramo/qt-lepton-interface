#include "buttons.h"

#include <QString>
#include <QPicture>
#include <QColor>
#include <QFileDialog>

sButton::sButton(const QString &text, QWidget *parent) : QPushButton(parent)
{
    this->setText(text);
}
sButton::~sButton()
{
}

void sButton::bt_enable()
{
    this->setEnabled(true);
    this->repaint();
}

void sButton::bt_disable()
{
    this->setEnabled(false);
    this->repaint();
}
