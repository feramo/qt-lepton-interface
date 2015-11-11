#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>
#include <QDesktopWidget>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>
#include <QSlider>

#include "LeptonThread.h"
#include "MyLabel.h"

#define cam_width       80*10
#define cam_heigh       60*10
#define cam_x           10
#define cam_y           10
#define win_width       cam_width+200
#define win_heigh       cam_heigh+20

//#define FULLSCREEN
#define CAM_LUIZ

#define BORDER_RATIO        0.01F
#define BUTTON_RATIO        0.25F


int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );

    const QRect app_rect = a.desktop()->screenGeometry();
    const float aspect_ratio = static_cast<float>(app_rect.width())/static_cast<float>(app_rect.height());
    const float aspect_ratio_inv = static_cast<float>(app_rect.height())/static_cast<float>(app_rect.width());

    printf("My resolution: %dx%d\n", app_rect.width(), app_rect.height());
    printf("My aspect ratio: %f\n", aspect_ratio);

    QWidget *myWidget = new QWidget();

    //create an image placeholder for myLabel
    //fill the top left corner with red, just bcuz
    QImage myImage;

#ifdef FULLSCREEN
    myWidget->setGeometry(0, 0, app_rect.width(), app_rect.height());
    myWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    const int border = static_cast<int>(static_cast<float>(app_rect.width()) * BORDER_RATIO);
    const int button = static_cast<int>(static_cast<float>(app_rect.width()) * BUTTON_RATIO);

    printf("Border %d\n", border);
    printf("Button %d\n", button);

    {
        const int w = app_rect.width() - (border + border + button + border);
        const int h = static_cast<int>(static_cast<float>(w)*aspect_ratio_inv);

        printf("Set QImage size: %dx%d\n", w, h);

        myImage = QImage( w, h, QImage::Format_RGB888);

        printf("QImage size: %dx%d\n", myImage.rect().width(), myImage.rect().height());
    }
#else
    myWidget->setGeometry(0, 60, win_width, win_heigh);

    myImage = QImage(640, 480, QImage::Format_RGB888);
#endif

/*
    QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
	}
*/

	//create a label, and set it's image to the placeholder
    MyLabel myLabel(myWidget);
#ifdef FULLSCREEN
    myLabel.setGeometry( border,
                         (app_rect.height()-myImage.rect().height())/2,
                         myImage.rect().width(),
                         myImage.rect().height());

    const int column_x = border + myImage.rect().width() + border;
#else
    myLabel.setGeometry(cam_x, cam_y, cam_width, cam_heigh);
#endif
    myLabel.setPixmap(QPixmap::fromImage(myImage));


    //create a FFC button
    QPushButton* button1 = new QPushButton("Perform FFC", myWidget);
#ifdef FULLSCREEN
    int x = column_x;
    int y = border;

    const int max_column_sz = app_rect.width() - column_x - border;

    button1->setGeometry(x, y, max_column_sz, 30);
    y += button1->geometry().height() + border + (border/2);
#else
    button1->setGeometry(cam_x+cam_width+10, 10, 100, 30);
#endif


    //create a button to save the image as .bmp
    QPushButton *button2 = new QPushButton("Salvar BMP", myWidget);
#ifdef FULLSCREEN
    button2->setGeometry(x, y, max_column_sz, 30);
    y += button2->geometry().height() + border + (border/2);
#else
    button2->setGeometry(cam_x+cam_width+10, 50, 100, 30);
#endif


    //create a label to show temperature
    QFont f_title("Times", 20, QFont::Bold);
    QFont f_temp("Times", 40, QFont::Bold);


    defLabel temp_right_title(myWidget);
#ifdef FULLSCREEN
    temp_right_title.setGeometry(x, y, column_x, f_title.pointSize());
    y += temp_right_title.geometry().height() + (border/2);
#else
    temp_right_title.setGeometry(cam_x+cam_width+10, 90, 150, 20);
#endif
    temp_right_title.setText("Temp (U.A.):");
    temp_right_title.setFont(f_title);

    TempLabel temp_right(myWidget);
#ifdef FULLSCREEN
    temp_right.setGeometry(x, y, column_x, f_temp.pointSize());
    y += temp_right.geometry().height() + (border/2);
#else
    temp_right.setGeometry(cam_x+cam_width+10, 115, 200, 70);
#endif
    temp_right.setText(QObject::trUtf8("---"));
    temp_right.setFont(f_temp);
    QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));



    defLabel temp_C_right_title(myWidget);
#ifdef FULLSCREEN
    temp_C_right_title.setGeometry(x, y, column_x, f_title.pointSize());
    y += temp_C_right_title.geometry().height() + (border/2);
#else
    temp_C_right_title.setGeometry(cam_x+cam_width+10, 190, 150, 20);
#endif
    temp_C_right_title.setText(QObject::trUtf8("Temp (°C):"));
    temp_C_right_title.setFont(f_title);
    TempLabel temp_C_right(myWidget);
#ifdef FULLSCREEN
    temp_C_right.setGeometry(x, y, column_x, f_temp.pointSize());
    y += temp_C_right.geometry().height() + (border/2);
#else
    temp_C_right.setGeometry(cam_x+cam_width+10, 215, 200, 70);
#endif
    temp_C_right.setText(QObject::trUtf8("---"));
    temp_C_right.setFont(f_temp);
    QObject::connect(&myLabel, SIGNAL(enviaLabel_C(QString)), &temp_C_right, SLOT(atualizaTemp(QString)));

    defLabel temp_cam_title(myWidget);
#ifdef FULLSCREEN
    temp_cam_title.setGeometry(x, y, column_x, f_title.pointSize());
    y += temp_cam_title.geometry().height() + (border/2);
#else
    temp_cam_title.setGeometry(cam_x+cam_width+10, 290, 150, 20);
#endif
    temp_cam_title.setText("Camera:");
    temp_cam_title.setFont(f_title);
    TempLabel temp_cam(myWidget);
#ifdef FULLSCREEN
    temp_cam.setGeometry(x, y, column_x, f_temp.pointSize());
    y += temp_cam.geometry().height() + (border/2);
#else
    temp_cam.setGeometry(cam_x+cam_width+10, 315, 200, 70);
#endif
    temp_cam.setText(QObject::trUtf8("---"));
    temp_cam.setFont(f_temp);
    //QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));
    


    //create a button to change camera radiometry
    QPushButton *button3 = new QPushButton(myWidget);
    button3->setText("Telemetry");
#ifdef FULLSCREEN
    button3->setGeometry(x, y, max_column_sz, 30);
    y += button3->geometry().height() + (border/2);
#else
    button3->setGeometry(cam_x+cam_width+10, 390, 100, 30);
#endif

    defLabel telemetry_State(myWidget);
#ifdef FULLSCREEN
    telemetry_State.setGeometry(x, y, column_x, f_title.pointSize());
    y += telemetry_State.geometry().height() + (border/2);
#else
    telemetry_State.setGeometry(cam_x+cam_width+10, 430, 200, 20);
#endif
    telemetry_State.setText(QObject::trUtf8("Telemetry: ?"));
    telemetry_State.setFont(f_title);

    QSlider qs_min_temp(Qt::Horizontal, myWidget);
#ifdef FULLSCREEN
    qs_min_temp.setGeometry(x, y, max_column_sz, 30);
    y += qs_min_temp.geometry().height() + (border/2);
#else
    qs_min_temp.setGeometry(cam_x+cam_width+10, 480, 150, 20);
#endif
    qs_min_temp.setMinimum(15);
    qs_min_temp.setMaximum(50);
    qs_min_temp.setValue(20);

    QSlider qs_max_temp(Qt::Horizontal, myWidget);
#ifdef FULLSCREEN
    qs_max_temp.setGeometry(x, y, max_column_sz, 30);
    y += qs_max_temp.geometry().height() + (border/2);
#else
    qs_max_temp.setGeometry(cam_x+cam_width+10, 520, 150, 20);
#endif
    qs_max_temp.setMinimum(15);
    qs_max_temp.setMaximum(50);
    qs_max_temp.setValue(50);

    defLabel range_State(myWidget);
#ifdef FULLSCREEN
    range_State.setGeometry(x, y, column_x, f_title.pointSize());
    y += range_State.geometry().height() + (border/2);
#else
    range_State.setGeometry(cam_x+cam_width+10, 550, 200, 20);
#endif
    range_State.setText(QObject::trUtf8("Range: 20 a 50"));
    range_State.setFont(f_title);


    //create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
    LeptonThread *thread = LeptonThread::Instance();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	
    //connect ffc button to the thread's ffc action
    QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));

    //connect save button to the save action
    QObject::connect(button2, SIGNAL(clicked()), &myLabel, SLOT(salvaBMP()));

    //connect thread to camera internal temperature measurement
    QObject::connect(thread, SIGNAL(getCamTemp(int)), &temp_cam, SLOT(updateCamTemp(int)));

    //connect temp button to the thread's gettemp action
    QObject::connect(button3, SIGNAL(clicked()), thread, SLOT(toggleRadiometry()));
    QObject::connect(thread, SIGNAL(updateRadiometry(QString)), &telemetry_State, SLOT(writeText(QString)));
    QObject::connect(thread, SIGNAL(updateRange(QString)), &range_State, SLOT(writeText(QString)));

    QObject::connect(&qs_min_temp, SIGNAL(valueChanged(int)), thread, SLOT(get_temp_min(int)));
    QObject::connect(&qs_max_temp, SIGNAL(valueChanged(int)), thread, SLOT(get_temp_max(int)));
    thread->start();
	
	myWidget->show();

	return a.exec();
}
