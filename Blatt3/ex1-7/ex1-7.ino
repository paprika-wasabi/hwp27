
long time;
bool toggle1 = 0;
volatile uint32_t tCount5 = 0;



void setup() {
  setTimer1Freq(300);
  setTimer2();
  pinMode(10, OUTPUT);
}

void setTimer1Freq(int freq) {
   
  // //set pins as outputs
  // pinMode(10, OUTPUT);

  // cli();//stop interrupts

  // //set timer1 interrupt at 1Hz
  // TCCR1A = 0;// set entire TCCR1A register to 0
  // TCCR1B = 0;// same for TCCR1B
  // TCNT1  = 0;//initialize counter value to 0
  // // set compare match register for 1hz increments
  // OCR1A = 14;// = (16*10^6) / (1046*1024) - 1 (must be <65536)
  // // turn on CTC mode
  // TCCR1B |= (1 << WGM12);
  // // Set CS12 and CS10 bits for 1024 prescaler
  // TCCR1B |= (1 << CS12) | (1 << CS10);  
  // // enable timer compare interrupt
  // TIMSK1 |= (1 << OCIE1A);

  // sei();//allow interrupts

  //________________________________________________________________
  cli(); // stop interrupts

  // Set timer1 in CTC mode with TOP = OCR1A
  TCCR1A = 0;              // Clear TCCR1A register
  TCCR1B = (1 << WGM12);   // Set WGM12 bit for CTC mode
  TCCR1B |= (1 << CS10);   // Set prescaler to 1 (no prescaling)
  uint16_t temp = (16 * pow(10,6)) / (freq * 1024) - 1;
  if (temp > ((uint16_t) pow(2, 16))) {
    PORTB &= ~(1 << 4);
    TCCR1A = 0; // Clear TCCR1A register
    TCCR1B = 0; // Clear TCCR1B register
  }
  OCR1A = temp;            // Set OCR1A value for desired frequency (1046 Hz)
  // 14 = (16*10^6) / (1046*1024) - 1.

  // Toggle pin 10 on compare match
  TCCR1A |= (1 << COM1B0);

  sei(); // allow interrupts

}


void setTimer2() {
  cli();
  TCCR2A = 0;             // set entire TCCR2A register to 0
  TCCR2B = 0;             // set entire TCCR2B register to 0
  TCNT2  = 0;             // initialize counter value to 0
  OCR2A = 249;            // set compare match register for 1 ms interrupt
  TCCR2B |= (1 << WGM21);  // turn on CTC mode
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024
  TIMSK2 |= (1 << OCIE2A);

sei();
Serial.print(1234);
}

ISR(TIMER2_COMPA_vect) {
  tCount5++;
  /*if (toggle1) {
     //digitalWrite(13 , HIGH);
    PORTB |= (1 << 4);
     toggle1 = 0;
     tCount5++;
   } else {
  //digitalWrite(13 , LOW);
     PORTB &= ~(1 << 4);
     toggle1 = 1;
     tCount5++;
   }
*/}


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
  Serial.println(tCount5);
}

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
