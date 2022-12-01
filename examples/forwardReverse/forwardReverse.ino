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

// ref HM-60252J.pdf 284p
void loop() {
  Serial.println("at " + String(millis()));
  stepMotor.writeSpeed(5000);
  stepMotor.writeCommandForward();
  delay(1000);
  stepMotor.writeCommandStop();
  delay(1000);
  // stepMotor.writeCommandReverse();
  stepMotor.writeSpeed(-5000);
  stepMotor.writeCommandForward();
  delay(1000);
  stepMotor.writeCommandStop();
  delay(1000);
}
