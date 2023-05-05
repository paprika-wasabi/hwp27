unsigned long delayStart;
unsigned long duration;
bool delayRunning = false;

void setup() {
  delay(3000);
  Serial.begin(2000000);
  // put your setup code here, to run once:
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // drive(true, 1000, 100);
  // turn(true, 1000, 100);
  // turn(false, 1000, 100);
}

void setMotor(bool forward, uint16_t speed, int motor) {
  switch (motor) {
    case 0: // motor A left.
      if (forward) {
        analogWrite(D2, speed);
        analogWrite(D1, 0);
      } else {
        analogWrite(D2, 0);
        analogWrite(D1, speed);
      }
      break;
    case 1: // motor B right.
      if (forward) {
        analogWrite(D5, speed);
        analogWrite(D6, 0);
      } else {
        analogWrite(D5, 0);
        analogWrite(D6, speed);
      }
      break;
    default:
      break;
  }
}


void drive(bool forward, uint32_t time, uint16_t speed) {
  delayRunning = true;
  delayStart = millis();
  while (delayRunning) {
    if ((millis() - delayStart) > time) {
      delayRunning = false;
      break;
    }
    setMotor(forward, speed, 0);
    setMotor(forward, speed, 1);
  }
  setMotor(forward, 0, 0);
  setMotor(forward, 0, 1);
}

void turn(bool left, uint32_t time, uint16_t speed) {
  delayRunning = true;
  delayStart = millis();
  while (delayRunning) {
    if ((millis() - delayStart) > time) {
      delayRunning = false;
      break;
    }
    if (left) {
      setMotor(false, speed, 0);
      setMotor(true, speed, 1);
    } else {
      setMotor(true, speed, 0);
      setMotor(false, speed, 1);
    }
  }
  setMotor(true, 0, 0);
  setMotor(true, 0, 1);
}

long measureDistance(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
  delay(0.01);
  digitalWrite(pin, LOW);
  pinMode(pin, INPUT);
  duration = pulseIn(pin, HIGH, 30000);
  if (duration == 0) {
    return -1;
  }

  return microsecondsToCentimeters(duration);
}

long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimetre.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}

void loop() {
  Serial.println(measureDistance(D8));
  delay(500);
}
