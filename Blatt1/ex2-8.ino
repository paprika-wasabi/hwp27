// Include LCD functions:
#include <LiquidCrystal.h>
// Define Constance
#define REF_VOLTAGE 3.3
// LCD Connection
#define R_S 8
#define E   9
#define DB4 10
#define DB5 11
#define DB6 12
#define DB7 13

// LED
#define LED_PIN1 2
#define LED_PIN2 3
#define LED_PIN3 4
#define LED_PIN4 5
#define LED_PIN5 6
#define LED_PIN6 7

// Analogport
#define A0PIN A0
#define A1PIN A1

// LCD Defines
#define NUM_CHAR 20
#define NUM_LINES 4

// Global variables
unsigned int time;
static int S1State = 0;
static bool ButtonPress = false;
static bool ButtonRelease = false;
char* MyColors[] = {"RED", "GREEN", "YELLOW"};
static float Frequenz;
static float Voltage;
// Define the LCD screen
LiquidCrystal lcd(R_S, E, DB4, DB5, DB6, DB7);

void setup() {
  // Define Serial
  Serial.begin(9600);
  delay(1000);
  // LCD has 4 lines with 20 chars
  lcd.begin(NUM_CHAR, NUM_LINES);
}

void S1StateCheck() {
  switch (S1State) {
    case 1:
      Tr1Tuner(2, 3);
      PORTD &= ~(1 << 4);
      PORTD &= ~(1 << 5);
      PORTD &= ~(1 << 6);
      PORTD &= ~(1 << 7);
      break;
    case 2:
      Tr1Tuner(4, 5);
      PORTD &= ~(1 << 2);
      PORTD &= ~(1 << 3);
      PORTD &= ~(1 << 6);
      PORTD &= ~(1 << 7);
      break;
    default:
      Tr1Tuner(6, 7);
      PORTD &= ~(1 << 4);
      PORTD &= ~(1 << 5);
      PORTD &= ~(1 << 2);
      PORTD &= ~(1 << 3);
      break;
  }
}

void Tr1Tuner(uint8_t a, uint8_t b){
  time = millis();
  if (time % (2000 / (int)Frequenz) < (1000 / (int)Frequenz)) {
    PORTD |= (1 << a);
    PORTD |= (1 << b);
  } else {
    PORTD &= ~(1 << a);
    PORTD &= ~(1 << b);
  }
}

void CheckButtonS1Press(){
  if (ButtonPress) {
    ButtonRelease = true;
  }
  if (ButtonPress && ButtonRelease){
    S1State++;
    lcd.clear();
    ButtonPress = false;
    ButtonRelease = false;
    if (S1State > 2) {
      S1State = 0;
    }
  }
}

void loop() {
  S1StateCheck();
  // Set cursor to arbitrary position
  lcd.setCursor(0, 0);
  // Print statements
  float value = analogRead(A0PIN);
  Voltage = (float)(value * REF_VOLTAGE/ 1023.0);
  lcd.print("Analog 0: ");
  lcd.print(Voltage);
  lcd.print("V");
  // Set cursor to next line
  lcd.setCursor(0, 1);
  lcd.print("Analog 1: ");
  int value1 = analogRead(A1PIN);
  // S1 = 0, S2 = 153, S3 = 301, S4 = 431, S5 = 528
  // base_case_value = 652
  switch (value1) {
    case 0:
      lcd.print("S1");
      Serial.println("S1");
      ButtonPress = true;
      break;
    case 153:
      lcd.print("S2");
      Serial.println("S2");
      break;
    case 301:
      lcd.print("S3");
      Serial.println("S3");
      break;
    case 431:
      lcd.print("S4");
      Serial.println("S4");
      break;
    case 528:
      lcd.print("S5");
      Serial.println("S5");
      break;
    case 529:
      lcd.print("S5");
      Serial.println("S5");
      break;
    default:
      lcd.print("Sx");
      CheckButtonS1Press();
      break;
  }
  lcd.setCursor(0, 2);
  lcd.print(MyColors[S1State]);
  lcd.setCursor(0, 3);
  Frequenz = map(Voltage, 0.00, 2.1, 1, 50);
  lcd.print(Frequenz);
  lcd.print("Hz");
  if (Frequenz == 1) {
    lcd.setCursor(6, 3);
    lcd.print(" ");
  }
}

// Test low level ISR.
void toggleTimer(){

  pinMode(7, OUTPUT);
  cli();

  TCCR2A = 0;
  TCCR2B = 0;

  TCCR2B |= (1 << CS22);
  TCCR2A |= (1 << WGM21);

  OCR2A = 156;

  TIMSK2 |= (1 << OCIE2A);

  sei();
}

// Test Low level ISR.
void timerBased(uint8_t val){
  cli();

  TCCR2A = 0;
  TCCR2B = 0;

  TCCR2B |= (1 << CS22);

  OCR2A = val;

  if (val != 255){
    TIMSK2 |= (1 << OCIE2A);
  }
  else {
    TIMSK2 &= ~(1 << OCIE2A);
  }

  if (val != 0) {
    TIMSK2 |= (1 << TOIE2);
  }
  else {
    TIMSK2 &= ~(1 << TOIE2);
  }
  sei();
}

ISR(TIMER2_COMPA_vect){
  //digitalWrite(7, LOW);
  //PORTD &= ~(1 << 7);
  //PIND |= (1 << 7);
}

ISR(TIMER2_OVF_vect){
  //digitalWrite(7, HIGH);
  //PORTD |= (1 << 7);
}
