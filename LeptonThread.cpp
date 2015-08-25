#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

#define min_temp 23
#define max_temp 50

LeptonThread* LeptonThread::m_pInstance = NULL;

LeptonThread* LeptonThread::Instance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new LeptonThread;

    return m_pInstance;
}

LeptonThread::LeptonThread() : QThread()
{
}

LeptonThread::~LeptonThread() {
}

void LeptonThread::run()
{
    uint16_t f_cam_temp;
	//create the initial image
	myImage = QImage(80, 60, QImage::Format_RGB888);

	//open spi port
	SpiOpenPort(0);

	while(true) {

		//read data packets from lepton over SPI
		int resets = 0;
		for(int j=0;j<PACKETS_PER_FRAME;j++) {
			//if it's a drop packet, reset j to 0, set to -1 so he'll be at 0 again loop
            read(spi_cs0_fd, lepton_result+sizeof(uint8_t)*PACKET_SIZE*j, sizeof(uint8_t)*PACKET_SIZE);
            int packetNumber = lepton_result[j*PACKET_SIZE+1];
            if(packetNumber != j) {
				j = -1;
				resets += 1;
				usleep(1000);
				//Note: we've selected 750 resets as an arbitrary limit, since there should never be 750 "null" packets between two valid transmissions at the current poll rate
				//By polling faster, developers may easily exceed this count, and the down period between frames may then be flagged as a loss of sync
				if(resets == 750) {
					SpiClosePort(0);
					usleep(750000);
					SpiOpenPort(0);
				}
			}
		}
		if(resets >= 30) {
			qDebug() << "done reading, resets: " << resets;
		}

        frameBuffer = (uint16_t *)lepton_result;
		int row, column;
		uint16_t value;
        uint16_t minValue = round(min_temp*31.737+6809);
        uint16_t maxValue = round(max_temp*31.737+6809);
/*        uint16_t minValue = 65535;
        uint16_t maxValue = 0;*/

		
		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			//skip the first 2 uint16_t's of every packet, they're 4 header bytes
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
			
			//flip the MSB and LSB at the last second
            int temp = lepton_result[i*2];
            lepton_result[i*2] = lepton_result[i*2+1];
            lepton_result[i*2+1] = temp;

            lepton_result_swapped[i] = frameBuffer[i];

			value = frameBuffer[i];
/*            if(value > maxValue) {
                maxValue = value;
            }
            if(value < minValue) {
                minValue = value;
            }*/
            if(value > maxValue) {
                frameBuffer[i] = (uint16_t) maxValue;
            }
            else if(value < minValue) {
                frameBuffer[i] = (uint16_t) minValue;
            }
            column = i % PACKET_SIZE_UINT16 - 2;
			row = i / PACKET_SIZE_UINT16 ;
		}

        float diff = maxValue - minValue;
		float scale = 255/diff;
		QRgb color;
		for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
			value = (frameBuffer[i] - minValue) * scale;
            const int *colormap = colormap_maplin;
			color = qRgb(colormap[3*value], colormap[3*value+1], colormap[3*value+2]);
			column = (i % PACKET_SIZE_UINT16 ) - 2;
			row = i / PACKET_SIZE_UINT16;
			myImage.setPixel(column, row, color);
		}
        //lets emit the signal for update
		emit updateImage(myImage);

        ++lepton_frames;
        if(lepton_frames > 100)
        {
            lepton_frames = 0;
            lepton_read_camtemp(&f_cam_temp);
            emit getCamTemp(f_cam_temp);
        }
	}
	
	//finally, close SPI port just bcuz
	SpiClosePort(0);
}

void LeptonThread::performFFC() {
	//perform FFC
	lepton_perform_ffc();
}

void LeptonThread::get_result(uint16_t* target_result)
{
    memcpy(target_result, lepton_result_swapped, sizeof(lepton_result_swapped));
}

void LeptonThread::toggleRadiometry()
{
    if(lepton_toggle_radiometry())
    {
        emit(updateRadiometry(QString("Telemetry: on")));
    }
    else
    {
        emit(updateRadiometry(QString("Telemetry: off")));
    }
}
