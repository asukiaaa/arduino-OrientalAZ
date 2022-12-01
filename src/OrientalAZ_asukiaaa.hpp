#pragma once
#include <rs485_asukiaaa.h>

namespace OrientalAZ_asukiaaa {

namespace Registers {
const uint16_t driverInputCommand = 0x007d;
const uint16_t speed = 0x0480;
const uint16_t changeSpeedRate = 0x0602;
const uint16_t stopSpeedRate = 0x0680;
}  // namespace Registers

struct DriverInputCommand_t {
  uint8_t m0 : 1;
  uint8_t m1 : 1;
  uint8_t m2 : 1;
  uint8_t start : 1;
  uint8_t zHome : 1;
  uint8_t stop : 1;
  uint8_t free : 1;
  uint8_t almRst : 1;
  uint8_t dSel0 : 1;
  uint8_t dSel1 : 1;
  uint8_t dSel2 : 1;
  uint8_t sStart : 1;
  uint8_t fwJogP : 1;
  uint8_t rvJogP : 1;
  uint8_t fwPos : 1;
  uint8_t rvPos : 1;
};

struct DriverInputCommand : public DriverInputCommand_t {
  DriverInputCommand() {
    m0 = m1 = m2 = start = zHome = stop = free = almRst = dSel0 = dSel1 =
        dSel2 = sStart = fwJogP = rvJogP = fwPos = rvPos = 0;
  }
};

struct DriverInputCommandUnified {
  union {
    DriverInputCommand_t c;
    uint16_t u16;
  };
  DriverInputCommandUnified() { u16 = 0; }
};

class Core {
 public:
  Core(HardwareSerial *serial, uint8_t address, uint16_t dePin, uint16_t rePin)
      : createdModbus(true) {
    this->address = address;
    modbus = new rs485_asukiaaa::ModbusRtu::Central(serial, dePin, rePin);
  }

  Core(rs485_asukiaaa::ModbusRtu::Central *modbus, uint8_t address)
      : createdModbus(false) {
    this->address = address;
    this->modbus = modbus;
  }

  ~Core() {
    if (createdModbus) {
      delete modbus;
    }
  }

  void begin(unsigned long baudrate, unsigned long config = SERIAL_8E1) {
    beginModbus(modbus, baudrate, config);
    beginWithoutModbus();
  }

  void beginWithoutModbus() {
    // TODO
  }

  void beginModbus(rs485_asukiaaa::ModbusRtu::Central *modbus,
                   unsigned long baudrate, unsigned long config = SERIAL_8E1) {
    modbus->begin(baudrate, config);
    modbus->msSilentInterval = getMsSilentInterval(baudrate);
  }

  unsigned long getMsSilentInterval(unsigned long baudrate) {
    return baudrate <= 9600 ? 5 : 3;
  }

  uint8_t writeSpeed(int32_t speed, uint8_t number = 0) {
    return modbus->writeRegisterBy32t(address, Registers::speed + number * 2,
                                      (uint32_t)speed);
  }

  uint8_t writeChangeRate(uint32_t speed, uint8_t number = 0) {
    return modbus->writeRegisterBy32t(
        address, Registers::changeSpeedRate + number * 2, (uint32_t)speed);
  }

  uint8_t writeStopRate(uint32_t speed, uint8_t number = 0) {
    return modbus->writeRegisterBy32t(
        address, Registers::stopSpeedRate + number * 2, (uint32_t)speed);
  }

  uint8_t writeCommandForward() {
    DriverInputCommand command;
    command.fwPos = true;
    return writeDriverInputCommand(command);
  }

  uint8_t writeCommandReverse() {
    DriverInputCommand command;
    command.rvPos = true;
    return writeDriverInputCommand(command);
  }

  uint8_t writeCommandStop() {
    DriverInputCommand command;
    return writeDriverInputCommand(command);
  }

  uint8_t writeDriverInputCommand(DriverInputCommandUnified &command) {
    return modbus->writeRegisterBy16t(address, Registers::driverInputCommand,
                                      command.u16);
  }

  uint8_t writeDriverInputCommand(DriverInputCommand_t &command) {
    DriverInputCommandUnified unified;
    unified.c = command;
    return writeDriverInputCommand(unified);
  }

 private:
  rs485_asukiaaa::ModbusRtu::Central *modbus;
  const bool createdModbus;
  uint8_t address;
};

}  // namespace AZD_KD_asukiaaa
