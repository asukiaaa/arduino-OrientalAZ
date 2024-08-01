#pragma once
#include <rs485_asukiaaa.h>

#include <OrientalCommon_asukiaaa.hpp>
#include <OrientalCommon_asukiaaa/BLx.hpp>
#include <OrientalCommon_asukiaaa/StepMotorDirectDataOperation.hpp>

namespace OrientalAZ_asukiaaa {

namespace Registers {
const uint16_t driverInputCommand = 0x007d;
const uint16_t alarmH = 0x80;
const uint16_t alarmL = 0x81;
const uint16_t speedHz0 = 0x0480;
const uint16_t loadTorqueH = 0x00d6;
const uint16_t loadTorqueL = 0x00d7;
const uint16_t loadRpm = 0x00ce;
const uint16_t loadHz = 0x00d0;
const uint16_t changeSpeedRate = 0x0600;
const uint16_t stopSpeedRate = 0x0680;
const uint16_t directDataOperationDataNumber = 0x0058;
const uint16_t directDataOperationType = 0x005a;
// const uint16_t directDataOperationTrigger = 0x0222;
const uint16_t directDataOperationPosition = 0x005c;
const uint16_t directDataOperationSpeed = 0x005e;
const uint16_t directDataOperationChangeRate = 0x0060;
const uint16_t directDataOperationStopRate = 0x0062;
const uint16_t directDataOperationCurrent = 0x0064;
const uint16_t directDataOperationTrigger = 0x0066;
const uint16_t directDataOperationForwardingDestination = 0x0068;
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

namespace DirectOperationType =
    OrientalCommon_asukiaaa::StepMotorDirectOperation::DirectOperationType;

class Core : public OrientalCommon_asukiaaa::StepMotorDirectOperation::Core {
 public:
  Core(HardwareSerial *serial, uint8_t address, uint16_t dePin, uint16_t rePin)
      : createdModbus(true),
        address(address),
        modbus(new rs485_asukiaaa::ModbusRtu::Central(serial, dePin, rePin)) {}

  Core(rs485_asukiaaa::ModbusRtu::Central *modbus, uint8_t address)
      : createdModbus(false), address(address), modbus(modbus) {}

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
    // do nothing
  }

  void beginModbus(rs485_asukiaaa::ModbusRtu::Central *modbus,
                   unsigned long baudrate, unsigned long config = SERIAL_8E1) {
    modbus->begin(baudrate, config);
    modbus->msSilentInterval = getMsSilentInterval(baudrate);
  }

  uint8_t readAlarm(uint32_t *alarm) {
    return modbus->readRegistersBy32t(address, Registers::alarmL, alarm, 1);
  }

  uint8_t readLoadTorque(int32_t *torque) {
    return modbus->readRegistersBy32t(address, Registers::loadTorqueH,
                                      (uint32_t *)torque, 1);
  }

  uint8_t readLoadHz(int32_t *hz) {
    return modbus->readRegistersBy32t(address, Registers::loadHz,
                                      (uint32_t *)hz, 1);
  }

  uint8_t readLoadRpm(uint32_t *rpm) {
    return modbus->readRegistersBy32t(address, Registers::loadRpm, rpm, 1);
  }

  unsigned long getMsSilentInterval(unsigned long baudrate) {
    return baudrate <= 9600 ? 5 : 3;
  }

  uint8_t writeSpeed(int32_t speed, uint8_t number = 0) {
    return modbus->writeRegisterBy32t(address, Registers::speedHz0 + number * 2,
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

  rs485_asukiaaa::ModbusRtu::Central *getModbus() { return modbus; }
  uint8_t getAddress() { return address; }

 private:
  rs485_asukiaaa::ModbusRtu::Central *modbus;
  const bool createdModbus;
  uint8_t address;
};

class CoreCompatibleForBLx : public Core,
                             public OrientalCommon_asukiaaa::BLx::Base {
 public:
  CoreCompatibleForBLx(HardwareSerial *serial, uint8_t address, uint16_t dePin,
                       uint16_t rePin, uint32_t hzMax,
                       float numMultiplyToDecideHzBySpeed = 1)
      : Core(serial, address, dePin, rePin),
        hzMax(hzMax),
        numMultiplyToDecideHzBySpeed(numMultiplyToDecideHzBySpeed) {}

  CoreCompatibleForBLx(rs485_asukiaaa::ModbusRtu::Central *modbus,
                       uint8_t address, uint32_t hzMax,
                       float numMultiplyToDecideHzBySpeed = 1)
      : Core(modbus, address),
        hzMax(hzMax),
        numMultiplyToDecideHzBySpeed(numMultiplyToDecideHzBySpeed) {}

  void begin(unsigned long baudrate, unsigned long config = SERIAL_8E1) {
    Core::begin(baudrate, config);
  }
  void beginWithoutModbus() { Core::beginWithoutModbus(); }
  uint8_t writeLock() {
    return writeSpeed(0);
    // return writeCommandStop();
  }
  uint8_t writeStop() {
    return writeSpeed(0);
    // return writeCommandStop();
  }
  uint8_t writeSpeed32t(int32_t speed) {
    int32_t speedInt32t = speed * numMultiplyToDecideHzBySpeed;
    static const uint32_t rateDefault = 1000000;
    uint32_t rate = rateDefault / 50;
    return Core::writeDirectOperation(
        OrientalCommon_asukiaaa::StepMotorDirectOperation::DirectOperation{
            .speed = speedInt32t, .changeRate = rate, .stopRate = rate});
  }
  uint8_t readAlarmU32t(uint32_t *alarm) { return Core::readAlarm(alarm); }
  uint8_t readLoadTorquePercent(float *torquePercent) {
    int32_t torque;
    auto result = Core::readLoadTorque(&torque);
    if (result == 0) {
      *torquePercent = torque;
    }
    return result;
  }
  uint8_t readFeedbackSpeed32t(int32_t *speed) {
    int32_t hz;
    auto result = Core::readLoadHz(&hz);
    if (result == 0) {
      *speed = hz / numMultiplyToDecideHzBySpeed;
    }
    return result;
  }
  uint8_t writeSetupConfiglIfNeeded() { return 0; };
  rs485_asukiaaa::ModbusRtu::Central *getModbus() { return Core::getModbus(); }
  uint32_t getRpmMin() { return 0; }
  uint32_t getRpmMax() { return hzMax / numMultiplyToDecideHzBySpeed; }

 private:
  uint32_t hzMax;
  float numMultiplyToDecideHzBySpeed;
};

}  // namespace OrientalAZ_asukiaaa
