#include "Lepton_I2C.h"

#include "leptonSDKEmb32PUB/LEPTON_SDK.h"
#include "leptonSDKEmb32PUB/LEPTON_SYS.h"
#include "leptonSDKEmb32PUB/LEPTON_Types.h"

#include "QDebug"

bool _connected;

LEP_CAMERA_PORT_DESC_T _port;

int lepton_connect() {
	LEP_OpenPort(1, LEP_CCI_TWI, 400, &_port);
	_connected = true;
	return 0;
}

void lepton_perform_ffc() {
	if(!_connected) {
		lepton_connect();
	}
	LEP_RunSysFFCNormalization(&_port);
}

float lepton_read_camtemp()
{
    LEP_RESULT result = LEP_OK;
    LEP_SYS_AUX_TEMPERATURE_KELVIN_T auxTemp=1;
    if(!_connected)
    {
        lepton_connect();
    }
    lepton_connect();
    if(_connected)
    {
        result = LEP_GetSysAuxTemperatureKelvin(&_port,&auxTemp);
        qDebug() << "Leu camtemp(): " << auxTemp << "; Result: " << result;
    }
    return (float) auxTemp;
}

//presumably more commands could go here if desired
