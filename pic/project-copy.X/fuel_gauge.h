#ifndef FUEL_GAUGE_H
#define	FUEL_GAUGE_H

#define FUELGAUGE_ADDRESS 0b01101100
#define FUELGAUGE_SOC 0x04
#define FUELGAUGE_CONFIG 0x0C

void LDWordWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned  char data1, unsigned char data2);
void LDWordReadI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char *data1, unsigned char *data2);


#endif	/* FUEL_GAUGE_H */

