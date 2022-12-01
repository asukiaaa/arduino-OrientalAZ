#include <OrientalAZ_asukiaaa.hpp>

#define PIN_DE 5
#define PIN_RE PIN_DE
#define SERIAL_RS485 Serial2
#define ADDRESS_MOTOR 1
#define BAUDRATE 115200

OrientalAZ_asukiaaa::Core stepMotor(&SERIAL_RS485, ADDRESS_MOTOR, PIN_DE, PIN_RE);

void setup() {
  Serial.begin(115200);
  stepMotor.begin(BAUDRATE);
}

void loop() {
  Serial.println("at " + String(millis()));
  stepMotor.writeChangeRate(10000);
  stepMotor.writeStopRate(10000);
  stepMotor.writeSpeed(5000);
  stepMotor.writeCommandForward();
  delay(2000);
  stepMotor.writeCommandStop();
  delay(2000);
  stepMotor.writeCommandReverse();
  delay(2000);
  stepMotor.writeCommandStop();
  delay(2000);
}
