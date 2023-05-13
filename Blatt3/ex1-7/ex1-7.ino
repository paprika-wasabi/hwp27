
long time;

void setup() {
  delay(3000);
  Serial.begin(2000000);
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << WGM12); // Turn on CTC mode.
  TCCR1B |= (1 << CS10) | (1 << CS11) | (1 << CS12);
  OCR1A = 255;
  OCR1B = 255;

  TIMSK1 |= (1 << OCIE1A);
  TIMSK1 |= (1 << OCIE1B);

  Serial.println("Setup done");
}

void setTimer1Freq(){

}

void setPin13(bool param) {
  if (param) {
    PORTB |= (1 << 5);
  } else {
    PORTB &= ~(1 << 5);
  }
}

void Blink() {
  time = millis();
  if ((time % 2000) < 1000) {
    setPin13(1);
  } else {
    setPin13(0);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Blink();

}

uint8_t counter = 0;
ISR(TIMER1_COMPA_vect) {
  counter++;
  if (counter >= 30) {
    counter = 0;
    PORTB |= (1 << 4);
  }
}
