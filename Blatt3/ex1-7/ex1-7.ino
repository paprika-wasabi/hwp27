long time;
bool toggle1 = 0;
volatile uint32_t tCount = 0;
uint16_t durations[10] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200};
uint16_t notes[10] = {400, 494, 523, 587, 659, 700, 784, 880, 988, 1047};
volatile uint8_t melodyIdx;

// ex.1)
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

// Ex.2) Ex.4)
void setTimer1Freq(int freq) {
  pinMode(10, OUTPUT);
  // __________Ex.3: Configuration of Timer 1_______________
  /* 
    set pins as outputs
    pinMode(10, OUTPUT);

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

    ________________________________________________________________
  */  
  cli(); // stop interrupts
  // Set timer1 in CTC mode with TOP = OCR1A
  TCCR1A = 0;              // Clear TCCR1A register
  TCCR1B = (1 << WGM12);   // Set WGM12 bit for CTC mode
  TCCR1B |= (1 << CS10);   // Set prescaler to 1 (no prescaling)
  uint16_t ocr = (F_CPU / (freq * 1024)) - 1;
  
  if ((ocr > ((uint16_t) pow(2, 16))) || (freq > 3000)) {
    PORTB &= ~(1 << 2); // disable pin10 out.
    TCCR1A = 0; // Clear TCCR1A register
    TCCR1B = 0; // Clear TCCR1B register
  }

  OCR1A = ocr;            // Set OCR1A value for desired frequency (1046 Hz)
  // result = 16*10^6) / (1046*1024) - 1.

  // Toggle pin 10 on compare match
  TCCR1A |= (1 << COM1B0);
  sei(); // allow interrupts
  

}

// ex.5) 
void setTimer2(bool activate) {
  cli();

  TCCR2A = 0;
  TCCR2B = (1 << CS22);
  TCNT2 = 0;

  if (activate) {
    TIMSK2 = (1 << TOIE2);
  } else {
    TIMSK2 = 0;
  }

  sei();
}

// ISR for Timer2.
ISR(TIMER2_OVF_vect) {
  tCount++;
  if (tCount >= durations[melodyIdx]) {
    tCount = 0;
    if (++melodyIdx >= 10) {
      setTimer2(false); // deactivate timer2 and timer1.
      setTimer1Freq(3001);
      return;
    }

    setTimer1Freq(notes[melodyIdx]);
  }
}

// ex.6)
void playMelody() {
  melodyIdx = 0;
  setTimer1Freq(notes[melodyIdx]);
  setTimer2(true);
}

void setup() {
  Serial.begin(38400);
  playMelody();
}

void loop() {
  //Serial.println((int)tCount);
}

// part of exercise 1.) which use ISR compare to toggle the pin.
// ISR(TIMER1_COMPA_vect) {
//   if (toggle1) {
//     //digitalWrite(13 , HIGH);
//     PORTB |= (1 << 4);
//     toggle1 = 0;
//   } else {
//     //digitalWrite(13 , LOW);
//     PORTB &= ~(1 << 4);
//     toggle1 = 1;
//   }
// }
