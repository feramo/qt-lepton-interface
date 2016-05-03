#ifndef TEXTTHREAD
#define TEXTTHREAD

#include <ctime>
#include <stdint.h>

#include <QThread>
#include <QtCore>
#include <QPixmap>
#include <QImage>
#include <QFileDialog>

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)

#ifdef CAM_LUIZ
    #define a_fact 6809
    #define b_fact 31.737
    #define b_fact_inv100 3.1509 //binv = 100 / b
#else
    #define a_fact 7377.9
    #define b_fact 27.642
    #define b_fact_inv100 3.6177 //binv = 100 / b
#endif

class LeptonThread : public QThread
{
  Q_OBJECT;

public:
  void run();
  void get_result(uint16_t* target_result);
  static LeptonThread* Instance();

public slots:
  void performFFC();
  void toggleRadiometry();
  void get_temp_min(int);
  void get_temp_max(int);
  void get_interpolation_method(int);
  void get_mousePos(QPoint);
  void activate_run_state();
  void deactivate_run_state();
  void load_from_file();
  void change_palette(int);

signals:
  void updateText(QString);
  void updateImage(QImage);
  void getCamTemp(int);
  void updateRadiometry(QString);
  void updateRange(QString);

private:
  LeptonThread();
  ~LeptonThread();
  LeptonThread(LeptonThread const&){};
  LeptonThread& operator=(LeptonThread const&){};
  static LeptonThread* m_pInstance;
  void update_temp_range();
  void get_palette(int);
  void get_spi_data();
  void swap_buffer();
  void limit_framebuf();
  void generate_bw();
  void update_image();
  void update_point_temp();
  void update_camera_temp();

  QImage myImage;
  uint16_t* frameBuffer;
  uint8_t lepton_result[PACKET_SIZE*PACKETS_PER_FRAME];
  uint16_t lepton_result_swapped[(PACKET_SIZE*PACKETS_PER_FRAME)/2];
  uint16_t lepton_frames = 100;

  QImage cvImage;
  uint8_t frameBW[(PACKET_SIZE*PACKETS_PER_FRAME)/2];

  uint8_t temp_min, temp_max;
  uint8_t min_temp = 20;
  uint8_t max_temp = 50;
  uint8_t method_index = 0;
  uint8_t palette_index = 0;
  QVector<QRgb>  sColorTable;
  uint8_t last_pos_x, last_pos_y;
  bool is_running;

};
class Logger{
    public:
       static Logger* Instance();
       bool openLogFile(std::string logFile);
       void writeToLogFile();
       bool closeLogFile();

    private:
       Logger(){};  // Private so that it can  not be called
    };
#endif
