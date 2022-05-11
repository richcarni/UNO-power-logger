#include "INA226.h"
#include "Wire.h"

#define INA226_SHUNT_VOLTAGE        0x01
#define INA226_BUS_VOLTAGE          0x02
#define INA226_MASK_ENABLE          0x06

const byte INA226_address = 0x40;

INA226 INA(INA226_address);

uint16_t readRegister(uint8_t reg)
{
  TwoWire* wire = &Wire;
  uint8_t address = INA226_address;
  
  wire->beginTransmission(address);
  wire->write(reg);
  wire->endTransmission();

  wire->requestFrom(address, (uint8_t)2);
  uint16_t value = wire->read();
  value <<= 8;
  value |= wire->read();
  return value;
}

void setup() {
  Serial.begin(115200);
  Serial.println(__FILE__);

  pinMode(7, INPUT_PULLUP);
  pinMode(5, OUTPUT);

  if (!INA.begin() ) {
    Serial.println("could not connect. Fix and Reboot");
  }
  Wire.setClock(400000);
  INA.setMaxCurrentShunt(0.5, 0.107);
  INA.setBusVoltageConversionTime(2);
  INA.setShuntVoltageConversionTime(2);
  INA.setAlertRegister(0x0400);
}

void loop() {
  Serial.println(INA.getCurrentLSB(), 8);
  Serial.print("#!#!#!\n"); // data start sequence
  while (1) {
    while (PIND&(1<<7)) {;}
      PIND = 1<<5;
      INA.getAlertFlag();

      byte out[4];
      uint16_t* val = (uint16_t*)&out[0];
      *val = readRegister(INA226_SHUNT_VOLTAGE); // shunt voltage
      val++;
      *val = readRegister(INA226_BUS_VOLTAGE); // bus voltage
      Serial.write(out, 4);  
      PIND = 1<<5;
  }
}
