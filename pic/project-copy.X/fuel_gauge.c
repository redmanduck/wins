#include <i2c.h>
#include "fuel_gauge.h"

void LDWordWriteI2C(unsigned char SlaveAddress, unsigned char reg, unsigned char data1, unsigned char data2){
    StartI2C2();	//Send the Start Bit
    IdleI2C2();		//Wait to complete
    MasterWriteI2C2(SlaveAddress); //transmit write command
    IdleI2C2();		//Wait to complete
    MasterWriteI2C2(reg);
    IdleI2C2();
    MasterWriteI2C2(data1);
    IdleI2C2();
    MasterWriteI2C2(data2);
    IdleI2C2();
    StopI2C2();
    IdleI2C2();
}

void LDWordReadI2C(unsigned char SlaveAddress, unsigned  char reg, unsigned char *data1, unsigned char *data2){
                StartI2C2();	//Send the Start Bit
		IdleI2C2();		//Wait to complete
		MasterWriteI2C2(SlaveAddress); //transmit write command
		IdleI2C2();		//Wait to complete
                MasterWriteI2C2(reg);
                IdleI2C2();
                StopI2C2();
                IdleI2C2();


                StartI2C2();	//Send the Start Bit
		IdleI2C2();		//Wait to complete
		MasterWriteI2C2(SlaveAddress|0x01); //transmit read command
		IdleI2C2();		//Wait to complete
                unsigned char data[2];
      		MastergetsI2C2(2, data, 30);		// "MCHP I2C"
                *data1=data[0];
                *data2=data[1];
//		if (status!=0)
//			while(1);

		StopI2C2();	//Send the Stop condition
		IdleI2C2();	//Wait to complete


}