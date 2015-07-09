#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>

#include "LeptonThread.h"
#include "MyLabel.h"

#define cam_width   80*10
#define cam_heigh   60*10
#define cam_x       10
#define cam_y       10
#define win_width   cam_width+200
#define win_heigh   cam_heigh+20

int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );
	
	QWidget *myWidget = new QWidget;
    myWidget->setGeometry(0, 60, win_width, win_heigh);

	//create an image placeholder for myLabel
	//fill the top left corner with red, just bcuz
	QImage myImage;
	myImage = QImage(640, 480, QImage::Format_RGB888);
	QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
	}

	//create a label, and set it's image to the placeholder
	MyLabel myLabel(myWidget);
    myLabel.setGeometry(cam_x, cam_y, cam_width, cam_heigh);
	myLabel.setPixmap(QPixmap::fromImage(myImage));

    //create a label to show temperature
    QFont f_title("Times", 20, QFont::Bold);
    QFont f_temp("Times", 70, QFont::Bold);

    defLabel temp_right_title(myWidget);
    temp_right_title.setGeometry(cam_x+cam_width+10, 90, 150, 20);
    temp_right_title.setText("Temp (ADU):");
    temp_right_title.setFont(f_title);
    TempLabel temp_right(myWidget);
    temp_right.setGeometry(cam_x+cam_width+10, 115, 200, 70);
    temp_right.setText(QObject::trUtf8("---"));
    temp_right.setFont(f_temp);
    QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));

    defLabel temp_cam_title(myWidget);
    temp_cam_title.setGeometry(cam_x+cam_width+10, 190, 150, 20);
    temp_cam_title.setText("Camera:");
    temp_cam_title.setFont(f_title);
    TempLabel temp_cam(myWidget);
    temp_cam.setGeometry(cam_x+cam_width+10, 215, 200, 70);
    temp_cam.setText(QObject::trUtf8("---"));
    temp_cam.setFont(f_temp);
    //QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));
    
	//create a FFC button
	QPushButton *button1 = new QPushButton("Perform FFC", myWidget);
    button1->setGeometry(cam_x+cam_width+10, 10, 100, 30);

    //create a button to save the image as .bmp
    QPushButton *button2 = new QPushButton("Salvar BMP", myWidget);
    button2->setGeometry(cam_x+cam_width+10, 50, 100, 30);

    //create a button get camera temperature
    QPushButton *button3 = new QPushButton("Temp. câmera", myWidget);
    button3->setGeometry(cam_x+cam_width+10, 290, 100, 30);

    //create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
    LeptonThread *thread = LeptonThread::Instance();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	
    //connect ffc button to the thread's ffc action
    QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));

    //connect save button to the save action
    QObject::connect(button2, SIGNAL(clicked()), &myLabel, SLOT(salvaBMP()));

    //connect temp button to the thread's gettemp action
    //QObject::connect(button3, SIGNAL(clicked()), &myLabel, SLOT(updateCamTemp()));
    QObject::connect(thread, SIGNAL(getCamTemp(float)), &temp_cam, SLOT(updateCamTemp(float)));

    thread->start();
	
	myWidget->show();

	return a.exec();
}
