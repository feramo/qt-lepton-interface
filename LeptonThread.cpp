#include "LeptonThread.h"

#include "Palettes.h"
#include "SPI.h"
#include "Lepton_I2C.h"

#define PACKET_SIZE 164
#define PACKET_SIZE_UINT16 (PACKET_SIZE/2)
#define PACKETS_PER_FRAME 60
#define FRAME_SIZE_UINT16 (PACKET_SIZE_UINT16*PACKETS_PER_FRAME)
#define FPS 27;

//#define min_temp 23
//#define max_temp 50

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
    myImage = QImage(80, 60, QImage::Format_Indexed8);

	//open spi port
	SpiOpenPort(0);
    min_temp = 20;
    max_temp = 50;
    temp_min = 20;
    temp_max = 50;

    method_index = 0;

    cv::Mat im_grey(60,80,CV_8UC1, &frameBW);
#ifdef FULLSCREEN
    cv::Mat im_grey_rs(584, 576,CV_8UC1);
#else
    cv::Mat im_grey_rs(480, 640,CV_8UC1);
#endif

    if ( sColorTable.isEmpty() )
    {
      get_palette(0);
    }

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
        uint16_t minValue = (min_temp*b_fact+a_fact);
        uint16_t maxValue = (max_temp*b_fact+a_fact);
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
        int j=0;
        for(int i=0;i<FRAME_SIZE_UINT16;i++) {
			if(i % PACKET_SIZE_UINT16 < 2) {
				continue;
			}
            frameBW[j] = static_cast<uint8_t>((frameBuffer[i] - minValue) * scale);
            j++;
		}

        if(method_index==1)
        {
            myImage = QImage(frameBW, 80, 60, QImage::Format_Indexed8);
            myImage.setColorTable( sColorTable );
            emit updateImage(myImage);
        }
        else if(method_index==0)
        {
            cv::resize(im_grey, im_grey_rs, im_grey_rs.size(), 0, 0, cv::INTER_CUBIC);
            cvImage = QImage(im_grey_rs.data, im_grey_rs.cols, im_grey_rs.rows, QImage::Format_Indexed8);
            cvImage.setColorTable( sColorTable );

            emit updateImage(cvImage);
        }

        if(last_pos_x>0)
        {
            uint32_t pix_temp = (uint32_t) lepton_result_swapped[last_pos_x + 2 + (last_pos_y * 82)];
            pix_temp = (uint32_t) ((pix_temp-a_fact)*b_fact_inv100);
            uint8_t temp_integ = pix_temp/100;
            uint8_t temp_dec = pix_temp/10%10;
            uint8_t temp_cent = pix_temp%10;

            emit updateText(QString("%1.%2%3").arg(temp_integ).arg(temp_dec).arg(temp_cent));
        }

        ++lepton_frames;
        if(lepton_frames > 100)
        {
            lepton_frames = 0;
            lepton_read_camtemp(&f_cam_temp);
            emit getCamTemp(f_cam_temp);
        }
        this->msleep(5);
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

void LeptonThread::get_temp_min(int t_min)
{
    temp_min = t_min;
    update_temp_range();
}
void LeptonThread::get_temp_max(int t_max)
{
    temp_max = t_max;
    update_temp_range();
}
void LeptonThread::update_temp_range()
{
    int temp_gap = static_cast<int>(temp_max)-static_cast<int>(temp_min);
    if(temp_gap>5)
    {
        min_temp = temp_min;
        max_temp = temp_max;
        emit(updateRange(QString("Range: %1 a %2").arg(min_temp).arg(max_temp)));
    }
}

void LeptonThread::get_interpolation_method(int index)
{
    method_index = index;

}

void LeptonThread::get_palette(int index)
{
    palette_index = index;
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
    sColorTable.clear();
    for ( int i = 0; i < 256; ++i )
    sColorTable.push_back(qRgb(colormap[3*i], colormap[3*i+1], colormap[3*i+2]));
    printf("Mudou, cor[0]=%d", qRed(sColorTable[0]));
}

void LeptonThread::get_mousePos(QPoint pos)
{
    last_pos_x = pos.x();
    last_pos_y = pos.y();
}
