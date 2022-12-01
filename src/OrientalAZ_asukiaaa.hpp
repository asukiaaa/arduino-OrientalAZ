#pragma once
#include <rs485_asukiaaa.h>

namespace OrientalAZ_asukiaaa {

namespace Registers {
const uint16_t driverInputCommand = 0x007d;
const uint16_t speed = 0x0480;
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

namespace DirectOperationType {
const uint16_t NoSetting = 0;
const uint16_t AbsolutePositioning = 1;
const uint16_t IncrementalPositioningByCommandPosition = 2;
const uint16_t IncrementalPositioningbyFeedbackPosition = 3;
const uint16_t ContinuousOperationByPosition = 7;
const uint16_t WrapAbsolutePositioning = 8;
const uint16_t WrapProximityPositioning = 9;
const uint16_t WrapAbsolutePositioningFw = 10;
const uint16_t WrapAbsolutePositioningRv = 11;
const uint16_t WrapAbsolutePushMotion = 12;
const uint16_t WrapProximityPushMotion = 13;
const uint16_t WrapPushMotionFw = 14;
const uint16_t WrapPushMotionRv = 15;
const uint16_t ContinuousOperationBySpeed = 16;
const uint16_t ContinuousOperationByPush = 17;
const uint16_t ContinuousOperationByTorque = 18;
const uint16_t AbsolutePushMotion = 20;
const uint16_t IncrementalPushMotionByCommandPosition = 21;
const uint16_t IncrementalPushMotionByFeedbackPosition = 22;
}  // namespace DirectOperationType

namespace DirectDataOperationTrigger {
const int32_t All = 1;
const int32_t Disable = 0;
const int32_t OperatingCurrentUpdate = -1;
const int32_t StoppingDecelationUpdate = -2;
const int32_t ChangeRateUpdate = -3;
const int32_t OperatingSpeedUpdate = -4;
const int32_t PositionUpdate = -5;
const int32_t OperationTypeUpdate = -6;
const int32_t OperationDataNumberUpdate = -7;
}  // namespace DirectDataOperationTrigger

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
  uint32_t maxSpeed = 500000;
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

  // uint8_t test() {
  //   uint32_t data[] = {
  //       0,    DirectOperationType::IncrementalPositioningByCommandPosition,
  //       8500, 2000,
  //       1500, 1500,
  //       1000, 1};
  //   return modbus->writeRegistersBy32t(address,
  //                                      Registers::directDataOperationDataNumber,
  //                                      data, sizeof(data) / sizeof(data[0]));
  // }

  // uint8_t writeDirectOperatoinWithTriggerConfig(
  //     bool changeDirectOperationTrigger, uint32_t trigger,
  //     uint8_t (*fnWrite)()
  // ) {
  //   if (changeDirectOperationTrigger) {
  //     auto error = writeDirectOperationTrigger(trigger);
  //     if (error != 0) {
  //       return error;
  //     }
  //     return fnWrite();
  //   }
  // }

  uint8_t writeDirectOperatonStopRate(
      uint32_t stopRate, bool changeDirectOperationTrigger = true) {
    if (changeDirectOperationTrigger) {
      auto error = writeDirectOperationTrigger(
          DirectDataOperationTrigger::StoppingDecelationUpdate);
      if (error != 0) {
        return error;
      }
    }
    return modbus->writeRegisterBy32t(
        address, Registers::directDataOperationStopRate, stopRate);
  }

  uint8_t writeDirectOperatonChangeRate(
      uint32_t changeRate, bool changeDirectOperationTrigger = true) {
    if (changeDirectOperationTrigger) {
      auto error = writeDirectOperationTrigger(
          DirectDataOperationTrigger::ChangeRateUpdate);
      if (error != 0) {
        return error;
      }
    }
    return modbus->writeRegisterBy32t(
        address, Registers::directDataOperationChangeRate, changeRate);
  }

  uint8_t writeDirectOperatonType(uint32_t operatonType,
                                  bool changeDirectOperationTrigger = true) {
    if (changeDirectOperationTrigger) {
      auto error = writeDirectOperationTrigger(
          DirectDataOperationTrigger::OperationTypeUpdate);
      if (error != 0) {
        return error;
      }
    }
    return modbus->writeRegisterBy32t(
        address, Registers::directDataOperationType, operatonType);
  }

  uint8_t writeDirectOperationSpeed(int32_t speed,
                                    bool changeDirectOperationTrigger = true) {
    if (changeDirectOperationTrigger) {
      auto error = writeDirectOperationTrigger(
          DirectDataOperationTrigger::OperatingSpeedUpdate);
      if (error != 0) {
        return error;
      }
    }
    return modbus->writeRegisterBy32t(
        address, Registers::directDataOperationSpeed, speed);
  }

  uint8_t writeDirectOperationTrigger(uint32_t trigger) {
    return modbus->writeRegisterBy32t(
        address, Registers::directDataOperationTrigger, trigger);
  }

 private:
  rs485_asukiaaa::ModbusRtu::Central *modbus;
  const bool createdModbus;
  uint8_t address;
};

}  // namespace OrientalAZ_asukiaaa
