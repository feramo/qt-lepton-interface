#ifndef TEXTTHREAD
#define TEXTTHREAD

#include <ctime>
#include <stdint.h>

#include <QThread>
#include <QtCore>
#include <QPixmap>
#include <QImage>

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)

class LeptonThread : public QThread
{
  Q_OBJECT;

public:
  void run();
  void get_result(uint16_t* target_result);
  static LeptonThread* Instance();

public slots:
  void performFFC();

signals:
  void updateText(QString);
  void updateImage(QImage);
  void getCamTemp(float);

private:
  LeptonThread();
  ~LeptonThread();
  LeptonThread(LeptonThread const&){};
  LeptonThread& operator=(LeptonThread const&){};
  static LeptonThread* m_pInstance;

  QImage myImage;
  uint16_t* frameBuffer;
  uint8_t lepton_result[PACKET_SIZE*PACKETS_PER_FRAME];
  uint16_t lepton_result_swapped[(PACKET_SIZE*PACKETS_PER_FRAME)/2];

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
