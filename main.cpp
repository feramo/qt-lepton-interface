/*

To-do
- Mudar resolução para telas
- Clarificar variáveis range
- Mudar estrutura Run (cvMat, temp ponto, etc)
- Melhorar sistema de salvar/abrir

Done
- Abrir programa automaticamente
- Destravar atualização
- Outras paletas
- Atualizar temperatura com mouse parado
- Simplificar labels
- Pausar imagem e andar com mouse
- Salvar imagem e recuperar

*/

#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>
#include <QDesktopWidget>
#include <stdint.h>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QPalette>

#include "LeptonThread.h"
#include "MyLabel.h"
#include "buttons.h"

#define cam_width       80*10
#define cam_heigh       60*10
#define cam_x           10
#define cam_y           10
#define win_width       cam_width+200
#define win_heigh       cam_heigh+20

#define FULLSCREEN
//#define CAM_LUIZ

#define BORDER_RATIO        0.01F
#define BUTTON_RATIO        0.25F


int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );

    const QRect app_rect = a.desktop()->screenGeometry();
    const float aspect_ratio = static_cast<float>(app_rect.width())/static_cast<float>(app_rect.height());
    const float aspect_ratio_inv = static_cast<float>(app_rect.height())/static_cast<float>(app_rect.width());

    QWidget *myWidget = new QWidget();

    QPalette Pal(myWidget->palette());
    // set black background
    Pal.setColor(QPalette::Background, Qt::black);
    Pal.setColor(QPalette::WindowText, Qt::white);
    myWidget->setAutoFillBackground(true);
    myWidget->setPalette(Pal);

    //create an image placeholder for myLabel
    QImage myImage;

    myWidget->setGeometry(0, 0, app_rect.width(), app_rect.height());
    myWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    const int border = static_cast<int>(static_cast<float>(app_rect.width()) * BORDER_RATIO);
    const int button = static_cast<int>(static_cast<float>(app_rect.width()) * BUTTON_RATIO);

    {
        const int w = app_rect.width() - (border + border + button + border);
        const int h = app_rect.height() - (border + border);

        printf("Set QImage size: %dx%d\n", w, h);

        myImage = QImage( w, h, QImage::Format_RGB888);

        printf("QImage size: %dx%d\n", myImage.rect().width(), myImage.rect().height());
    }

	//create a label, and set it's image to the placeholder
    MyLabel myLabel(myWidget);
    myLabel.setGeometry( border,
                         (app_rect.height()-myImage.rect().height())/2,
                         myImage.rect().width(),
                         myImage.rect().height());

    const int column_x = border + myImage.rect().width() + border;
    myLabel.setPixmap(QPixmap::fromImage(myImage));

    int x = column_x;
    int y = border;
    const int half_space = border/2;
    const int single_space = border;
    const int element_space = 1.5*border;
    const int section_space = 3*border;

    //Create a label for ColorBar

    ColorBar arrow(myWidget);
    const int arrow_w = border;
    QRgb arrowColor = qRgb(255, 0, 0);

    const int arrow_h = 2*arrow_w-1;
    arrow.setGeometry(x, y-arrow_h+arrow_w, arrow_w, arrow_h);
    QImage arrowImg(arrow_w, arrow_h, QImage::Format_RGB888);
    arrowImg.fill(Qt::black);
    for(int i=0; i<arrow_w; i++)
    {
        for(int j=0; j<i; j++)
        {
            arrowImg.setPixel(j,i,arrowColor);
            arrowImg.setPixel(j,arrow_h-i,arrowColor);
        }
    }
    arrow.setPixmap(QPixmap::fromImage(arrowImg));
    arrow.setBarHeight(myImage.height(), myImage.height()+border);
    x += arrow.geometry().width();
    ColorBar colorbar(myWidget);
    colorbar.setGeometry(x, y, 10, myImage.height());
    colorbar.setPaletteIndex(0);
    colorbar.setStyleSheet("border: 1px solid gray");
    x += colorbar.geometry().width() + border;

    const int max_column_sz = app_rect.width() - x - border;

    //create a FFC button
    QPushButton* button1 = new QPushButton("Perform FFC", myWidget);
    button1->setGeometry(x, y, max_column_sz, 30);
    y += button1->geometry().height() + element_space;


    sButton *run_button = new sButton("Run", myWidget);
    run_button->setGeometry(x, y, floor(max_column_sz/2), 30);
    run_button->setEnabled(false);
    sButton *pause_button = new sButton("Pause", myWidget);
    pause_button->setGeometry(x+floor(max_column_sz/2), y, max_column_sz-floor(max_column_sz/2), 30);
    y += pause_button->geometry().height();


    //create a button to save the image as .bmp
    sButton *save_button = new sButton("Salvar", myWidget);
    save_button->setGeometry(x, y, floor(max_column_sz/2), 30);
    save_button->setEnabled(false);
    sButton *open_button = new sButton("Abrir", myWidget);
    open_button->setGeometry(x+floor(max_column_sz/2), y, max_column_sz-floor(max_column_sz/2), 30);
    y += save_button->geometry().height() + element_space;


    //create a label to show temperature
    QFont f_title("Times", 20, QFont::Bold);
    QFont f_temp("Times", 40, QFont::Bold);

//Label with temperature in U.A.
/*
    defLabel temp_right_title(myWidget);
    temp_right_title.setGeometry(x, y, column_x, f_title.pointSize());
    y += temp_right_title.geometry().height() + (border/2);
    temp_right_title.setText("Temp (U.A.):");
    temp_right_title.setFont(f_title);
    TempLabel temp_right(myWidget);
    temp_right.setGeometry(x, y, column_x, f_temp.pointSize());
    y += temp_right.geometry().height() + (border/2);
    temp_right.setText(QObject::trUtf8("---"));
    temp_right.setFont(f_temp);
    QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));*/

//Label with temperature in Celsius
    defLabel temp_C_right_title(myWidget);
    temp_C_right_title.setGeometry(x, y, column_x, floor(f_title.pointSize()*1.5));
    y += temp_C_right_title.geometry().height() + single_space;
    temp_C_right_title.setText(QObject::trUtf8("Temp (°C):"));
    temp_C_right_title.setFont(f_title);
    temp_C_right_title.setPalette(Pal);
    TempLabel temp_C_right(myWidget);
    temp_C_right.setGeometry(x, y, column_x, floor(f_temp.pointSize()*1.5));
    y += temp_C_right.geometry().height() + section_space;
    temp_C_right.setText(QObject::trUtf8("---"));
    temp_C_right.setFont(f_temp);
    temp_C_right.setPalette(Pal);
    QObject::connect(&myLabel, SIGNAL(enviaLabel_C(QString)), &temp_C_right, SLOT(atualizaTemp(QString)));

//Label with camera temperature
/*
    defLabel temp_cam_title(myWidget);
    temp_cam_title.setGeometry(x, y, column_x, f_title.pointSize());
    y += temp_cam_title.geometry().height() + (border/2);
    temp_cam_title.setText("Camera:");
    temp_cam_title.setFont(f_title);
    TempLabel temp_cam(myWidget);
    temp_cam.setGeometry(x, y, column_x, f_temp.pointSize());
    y += temp_cam.geometry().height() + (border/2);
    temp_cam.setText(QObject::trUtf8("---"));
    temp_cam.setFont(f_temp);
    //QObject::connect(&myLabel, SIGNAL(enviaLabel(QString)), &temp_right, SLOT(atualizaTemp(QString)));*/
    
//create a button to change camera radiometry
/*
    QPushButton *button3 = new QPushButton(myWidget);
    button3->setText("Telemetry");
    button3->setGeometry(x, y, max_column_sz, 30);
    y += button3->geometry().height() + (border/2);

    defLabel telemetry_State(myWidget);
    telemetry_State.setGeometry(x, y, column_x, f_title.pointSize());
    y += telemetry_State.geometry().height() + (border/2);
    telemetry_State.setText(QObject::trUtf8("Telemetry: ?"));
    telemetry_State.setFont(f_title);*/

    defLabel range_State(myWidget);
    range_State.setGeometry(x, y, column_x, floor(f_title.pointSize()*1.5));
    y += range_State.geometry().height() + element_space;
    range_State.setText(QObject::trUtf8("Range: 20 a 50"));
    range_State.setFont(f_title);

    QSlider qs_min_temp(Qt::Horizontal, myWidget);
    qs_min_temp.setGeometry(x, y, max_column_sz, 30);
    y += qs_min_temp.geometry().height() + half_space;
    qs_min_temp.setMinimum(15);
    qs_min_temp.setMaximum(50);
    qs_min_temp.setValue(20);

    QSlider qs_max_temp(Qt::Horizontal, myWidget);
    qs_max_temp.setGeometry(x, y, max_column_sz, 30);
    y += qs_max_temp.geometry().height() + section_space;
    qs_max_temp.setMinimum(15);
    qs_max_temp.setMaximum(50);
    qs_max_temp.setValue(50);

//Change image interpolation
    defLabel interp_title(myWidget);
    interp_title.setGeometry(x, y, column_x, floor(f_title.pointSize()*1.5));
    y += interp_title.geometry().height() + element_space;
    interp_title.setText(QObject::trUtf8("Interp.:"));
    interp_title.setFont(f_title);

    QComboBox qc_interpolation(myWidget);
    qc_interpolation.setGeometry(x, y, max_column_sz, 30);
    y += qc_interpolation.geometry().height() + section_space;
    qc_interpolation.addItem("Cubic");
    qc_interpolation.addItem("Nenhum");

//Change colormap
    defLabel colormap_title(myWidget);
    colormap_title.setGeometry(x, y, column_x, floor(f_title.pointSize()*1.5));
    y += colormap_title.geometry().height() + element_space;
    colormap_title.setText(QObject::trUtf8("Paleta:"));
    colormap_title.setFont(f_title);

    QComboBox qc_colormap(myWidget);
    qc_colormap.setGeometry(x, y, max_column_sz, 30);
    y += qc_colormap.geometry().height() + section_space;
    qc_colormap.addItem("Default");
    qc_colormap.addItem("Hot Blue");
    qc_colormap.addItem("Medic");
    qc_colormap.addItem("Medic II");
    qc_colormap.addItem("Medic III");
    qc_colormap.addItem("Tons cinza");

    //Close button
    QPushButton *close_button = new QPushButton(myWidget);
    close_button->setText("X");
    close_button->setGeometry(a.desktop()->width()-15, a.desktop()->height()-15, 15, 15);

    //create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
    LeptonThread *thread = LeptonThread::Instance();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	
    QObject::connect(&myLabel, SIGNAL(enviaLevel(int)), &arrow, SLOT(getLevel(int)));
    QObject::connect(thread, SIGNAL(enviaLevel(int)), &arrow, SLOT(getLevel(int)));

    //connect ffc button to the thread's ffc action
    QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));

    //connect save button to the save action
    QObject::connect(save_button, SIGNAL(clicked()), &myLabel, SLOT(salvaBMP()));

    QObject::connect(run_button, SIGNAL(clicked()), thread, SLOT(activate_run_state()));
    QObject::connect(run_button, SIGNAL(clicked()), run_button, SLOT(bt_disable()));
    QObject::connect(run_button, SIGNAL(clicked()), save_button, SLOT(bt_disable()));
    QObject::connect(run_button, SIGNAL(clicked()), pause_button, SLOT(bt_enable()));
    QObject::connect(pause_button, SIGNAL(clicked()), thread, SLOT(deactivate_run_state()));
    QObject::connect(pause_button, SIGNAL(clicked()), pause_button, SLOT(bt_disable()));
    QObject::connect(pause_button, SIGNAL(clicked()), save_button, SLOT(bt_enable()));
    QObject::connect(pause_button, SIGNAL(clicked()), run_button, SLOT(bt_enable()));
    QObject::connect(open_button, SIGNAL(clicked()), thread, SLOT(load_from_file()));
    QObject::connect(open_button, SIGNAL(clicked()), pause_button, SLOT(bt_disable()));
    QObject::connect(open_button, SIGNAL(clicked()), save_button, SLOT(bt_disable()));
    QObject::connect(open_button, SIGNAL(clicked()), run_button, SLOT(bt_enable()));

    //connect thread to camera internal temperature measurement
//    QObject::connect(thread, SIGNAL(getCamTemp(int)), &temp_cam, SLOT(updateCamTemp(int)));

    //connect temp button to the thread's gettemp action
//    QObject::connect(button3, SIGNAL(clicked()), thread, SLOT(toggleRadiometry()));
//    QObject::connect(thread, SIGNAL(updateRadiometry(QString)), &telemetry_State, SLOT(writeText(QString)));
    QObject::connect(thread, SIGNAL(updateRange(QString)), &range_State, SLOT(writeText(QString)));

    QObject::connect(&myLabel, SIGNAL(enviaMousePos(QPoint)), thread, SLOT(get_mousePos(QPoint)));
    QObject::connect(thread, SIGNAL(updateText(QString)), &temp_C_right, SLOT(atualizaTemp(QString)));

    QObject::connect(&qs_min_temp, SIGNAL(valueChanged(int)), thread, SLOT(get_temp_min(int)));
    QObject::connect(&qs_max_temp, SIGNAL(valueChanged(int)), thread, SLOT(get_temp_max(int)));

    QObject::connect(&qc_interpolation, SIGNAL(currentIndexChanged(int)), thread, SLOT(get_interpolation_method(int)));

    QObject::connect(&qc_colormap, SIGNAL(currentIndexChanged(int)), thread, SLOT(change_palette(int)));
    QObject::connect(&qc_colormap, SIGNAL(currentIndexChanged(int)), &colorbar, SLOT(setPaletteIndex(int)));

    QObject::connect(close_button, SIGNAL(clicked()), myWidget, SLOT(close()));

    thread->start();
	
	myWidget->show();

	return a.exec();
}
