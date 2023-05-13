
long time;
bool toggle1 = 0;

void setup() {
  setTimer1Freq();
}

void setTimer1Freq(){
   
  //set pins as outputs
  pinMode(12, OUTPUT);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 14;// = (16*10^6) / (1046*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
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
}

ISR(TIMER1_COMPA_vect) {
  if (toggle1) {
    //digitalWrite(13 , HIGH);
    PORTB |= (1 << 4);
    toggle1 = 0;
  } else {
    //digitalWrite(13 , LOW);
    PORTB &= ~(1 << 4);
    toggle1 = 1;
  }
}
