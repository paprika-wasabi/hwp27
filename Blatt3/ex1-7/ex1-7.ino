#include <Arduino.h>
#define DEBUG_MODE true

long time;
bool toggle1 = 0;
volatile uint32_t tCount5;
uint16_t durations[10] = {2000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
uint16_t notes[10] = {440, 494, 523, 587, 659, 698, 784, 880, 988, 1047};
volatile uint8_t melodyIdx;
volatile  uint8_t dur = 0;
// new additions
volatile uint8_t *timer2_pin_port;
volatile uint8_t timer2_pin_mask;


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
  debugprint("SetTimer1 in");
  // Set timer1 in CTC mode with TOP = OCR1A
  TCCR1A = 0;              // Clear TCCR1A register
  TCCR1B = (1 << WGM12);   // Set WGM12 bit for CTC mode
  TCCR1B |= (1 << CS10);   // Set prescaler to 1 (no prescaling)
  uint16_t ocr = (16e6) / (freq * 1024) - 1;
  // uint32_t ocr = F_CPU / freq / 2 - 1;
  
  if (ocr > ((uint16_t) pow(2, 16))) {
  // if (ocr > 0xffff) {
    ocr = F_CPU / freq / 2 / 64 - 1;
    PORTB &= ~(1 << 4);
    TCCR1A = 0; // Clear TCCR1A register
    TCCR1B = 0; // Clear TCCR1B register
  }
  OCR1A = ocr;            // Set OCR1A value for desired frequency (1046 Hz)
  // 14 = (16*10^6) / (1046*1024) - 1.

  // Toggle pin 10 on compare match
  TCCR1A |= (1 << COM1B0);
  debugprint("SetTimer1 out");
  sei(); // allow interrupts
  

}

void setTimer2(bool activate) {
  debugprint("SetTimer2");
  
  if (activate) {
    cli();
    // Reset the initial drivers
    TCCR2A = 0;
    TCCR2B = 0;
    // Enable CTC
    TCCR2A = (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << WGM22) | (1 << CS20);
    TCNT2 = 0;
    
    // Set Frequency so that it interrupts every 1 ms
    OCR2A = 255;
    // timer2_pin_port = portOutputRegister(digitalPinToPort(10));
    // timer2_pin_mask = digitalPinToBitMask(10);
    TIMSK2 = 0;
    TIMSK2 |= (1 << TOIE2);

    sei();
  } else {
    cli();
    // TCCR2A = 0;
    // TCCR2B = 0;
    // TIMSK2 = 0;
    // finish interrupts
    bitWrite(TIMSK2, OCIE2A, 0); // disable interrupt
    TCCR2A = (1 << WGM20);
    TCCR2B = (TCCR2B & 0b11111000) | (1 << CS22);
    OCR2A = 0;
    sei();
  }
  debugprint("SetTimer2 out");
}

/*
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
*/

// simple waiting function
void wait(unsigned long ms) {
  unsigned long now = millis();
  while (millis() < now + ms) {}
}

ISR(TIMER2_OVF_vect) {
  // debugprint("Interupt Routine in ");
  cli();
  if (durations[melodyIdx] > 0) {
    dur = 2 * notes[melodyIdx] * durations[melodyIdx] / 1000;
  } else {
    dur = -1;
  }
  // dur = (8e6) / (durations[melodyIdx]) - 1;
  Serial.print(dur);
  Serial.print(" ");
  Serial.print(melodyIdx);
  Serial.print(" ");
  Serial.println(tCount5);
  if (tCount5 >= dur) {
    melodyIdx++;
    tCount5 = 0;
  }
  tCount5++;
  Serial.println(melodyIdx);
  sei();
  
  
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
  tCount5 = 0;
  
  Serial.println(tCount5);
  while (melodyIdx < 10) {
    dur = (8e6) / (durations[melodyIdx]) - 1;
    setTimer2(true);
    Serial.println(notes[melodyIdx]);
    setTimer1Freq(notes[melodyIdx]);
    Serial.println();
    // wait(durations[melodyIdx]);
    // wait(10); // Optional delay between notes
    setTimer2(false);
  }
  
  setTimer2(false);
  Serial.println(melodyIdx);
  setTimer1Freq(0);
}

void setup() {
  Serial.begin(38400);
  melodyIdx = 0;

  //Serial.println(melodyIdx);
  
  // debugprint("Setup start");
  pinMode(10, OUTPUT);
  playMelody();
  // debugprint("Setup end");
  // tone(10, 440, 100);
  
  // debugprint("Timer1");
  // setTimer1Freq(notes[melodyIdx]);
  // debugprint("Timer 2");
 
  setTimer2(true);
  // debugprint("Timer2 out");
}

void loop() {
  // Serial.println(tCount5);
  // wait(1000);
}
