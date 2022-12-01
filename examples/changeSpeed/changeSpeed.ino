#include <OrientalAZ_asukiaaa.hpp>

#define PIN_DE 5
#define PIN_RE PIN_DE
#define SERIAL_RS485 Serial2
#define ADDRESS_MOTOR 1
#define BAUDRATE 115200

OrientalAZ_asukiaaa::Core stepMotor(&SERIAL_RS485, ADDRESS_MOTOR, PIN_DE,
                                    PIN_RE);

void setup() {
  Serial.begin(115200);
  stepMotor.begin(BAUDRATE);
}

void loop() {
  Serial.println("at " + String(millis()));
  stepMotor.writeDirectOperatonChangeRate(5000);
  stepMotor.writeDirectOperatonStopRate(5000);
  stepMotor.writeDirectOperatonType(
      OrientalAZ_asukiaaa::DirectOperationType::ContinuousOperationBySpeed);
  stepMotor.writeDirectOperationSpeed(1000);
  delay(3000);
  // stepMotor.writeDirectOperationSpeed(stepMotor.maxSpeed, false);
  stepMotor.writeDirectOperationSpeed(5000, false);
  delay(3000);
  stepMotor.writeDirectOperationSpeed(1000, false);
  delay(3000);
  stepMotor.writeDirectOperationSpeed(-1000, false);
  delay(3000);
}
