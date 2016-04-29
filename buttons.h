#ifndef BUTTONS_H
#define BUTTONS_H

#include <ctime>
#include <stdint.h>

#include <QThread>
#include <QtCore>
#include <QPixmap>
#include <QPushButton>

class sButton : public QPushButton {
  Q_OBJECT;

public:
    explicit sButton(QWidget *parent=0);
    explicit sButton(const QString &text, QWidget *parent=0);
    sButton(const QIcon& icon, const QString &text, QWidget *parent=0);
    ~sButton();

public slots:
  void bt_disable();
  void bt_enable();

signals:

private:

};

#endif // BUTTONS_H

