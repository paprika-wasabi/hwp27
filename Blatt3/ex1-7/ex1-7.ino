
#define DEBUG_MODE true

long time;
bool toggle1 = 0;
volatile uint32_t tCount5 = 0;
uint16_t durations[10] = {2000, 1000, 1000, 1000,
                          1000, 1000, 1000, 1000, 1000, 1000};
uint16_t notes[10] = {440, 494, 523, 587, 659, 698, 784, 880, 988, 1047};
volatile uint8_t melodyIdx = 0;
bool settimer2actv = true;

void debugprint(char* const msg) {
  if (DEBUG_MODE) {
    Serial.println(msg);
  }
}


void setTimer1Freq(int freq) {
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
  // debugprint("SetTimer1");
  // Set timer1 in CTC mode with TOP = OCR1A
  TCCR1A = 0;              // Clear TCCR1A register
  TCCR1B = (1 << WGM12);   // Set WGM12 bit for CTC mode
  TCCR1B |= (1 << CS10);   // Set prescaler to 1 (no prescaling)
  uint16_t temp = (16e6) / (freq * 1024) - 1;
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

void setTimer2(bool activate) {
  debugprint("SetTimer2");
  // Serial.print("SetTimer2"); // Correct typo
  if (activate) {
    cli();
    TCCR2A = (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22) | (1 << CS20);
    TCNT2 = 0;
    OCR2A = 255;

    TIMSK2 = 0;
    TIMSK2 |= (1 << TOIE2);

    sei();
  } else {
    cli();
    TCCR2A = 0;
    TCCR2B = 0;
    TIMSK2 = 0;
    sei();
  }

}


void wait(unsigned long ms) {
  unsigned long now = millis();
  while (millis() < now + ms) {}
}

ISR(TIMER2_OVF_vect) {
  // debugprint("Interupt Routine in");
  cli();
  if (tCount5 >= durations[melodyIdx]) {
    tCount5 = 0;
    Serial.print(tCount5);
    Serial.print("  ");
    Serial.println(melodyIdx);
    Serial.println();
    melodyIdx++;
    wait(10);

  }
  tCount5++;
  sei();
  Serial.println(melodyIdx);
  // debugprint("Interupt Routine out");
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

void playMelody() {
  /*
  melodyIdx = 0;
  pinMode(10, OUTPUT);
  setTimer1Freq(notes[melodyIdx]);
  setTimer2(true);
  // setTimer2(settimer2actv);
  while (melodyIdx < 10) {
    setTimer1Freq(notes[melodyIdx]);
    // setTimer2(false);
  }
  // settimer2actv = false;
  setTimer2(false);
  Serial.println(notes[melodyIdx]);
  */
  pinMode(10, OUTPUT);
  Serial.println(melodyIdx);
  setTimer2(true);
  while (melodyIdx < 10) {
    Serial.println(notes[melodyIdx]);
    setTimer1Freq(notes[melodyIdx]);
    delay(durations[melodyIdx]);
    delay(10); // Optional delay between notes
  }
  setTimer2(false);
}

void setup() {
  Serial.begin(9600);
  debugprint("Setup start");
  playMelody();
  debugprint("Setup end");
  /*
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  // debugprint("Timer1");
  setTimer1Freq(notes[melodyIdx]);
  // debugprint("Timer 2");
 
  setTimer2();
  // debugprint("Timer2 out");
  */
}

void loop() {}
