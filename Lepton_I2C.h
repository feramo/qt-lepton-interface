#ifndef LEPTON_I2C
#define LEPTON_I2C

void lepton_perform_ffc();
void lepton_read_camtemp(unsigned short *return_temp);
bool lepton_toggle_radiometry();

#endif
