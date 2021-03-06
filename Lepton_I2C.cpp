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

void lepton_read_camtemp(short unsigned int *return_temp)
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
    }
    *return_temp = auxTemp;
}

bool lepton_toggle_radiometry()
{
    LEP_SYS_TELEMETRY_ENABLE_STATE_E tele_state;
    LEP_GetSysTelemetryEnableState(&_port, &tele_state);

    if(tele_state == LEP_TELEMETRY_DISABLED)
    {
        LEP_SetSysTelemetryEnableState(&_port, LEP_TELEMETRY_ENABLED);
        return true;
    }
    else
    {
        LEP_SetSysTelemetryEnableState(&_port, LEP_TELEMETRY_DISABLED);
        return false;
    }
}

//presumably more commands could go here if desired
