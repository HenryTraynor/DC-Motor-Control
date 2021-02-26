#include <Arduino.h>
#include <LiquidCrystal.h>

const int rs=13, en=12, d4=8, d5=7, d6=4, d7=5;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
volatile byte reading = 0;
const int opticalPin = 2;
volatile byte state = LOW;
volatile int currentPos=0;
volatile int oldPos;
boolean isClockwise=false;
const int in1=10, in2=11;
const int enA1=14, enA2=15;
//enA1=A0
//enA2=A1

void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  lcd.print("hello");
  pinMode(opticalPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(opticalPin), opticalSensor_ISRb, FALLING);
  pinMode(enA1, OUTPUT);
  pinMode(enA2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(opticalPin, INPUT);
  delay(1000);
  lcd.setCursor(0,0);
  lcd.print("Current Position");
  lcd.setCursor(0,1);
  lcd.print(0);
  Serial.println("Input (n) an integer ranging from -9 to 9. A positive input will turn the motor clockwise, a negative counterclockwise.");
  Serial.println("Angular displacement is found via theta=|n|*90. The current position of the motor will be displayed on the LCD screen.");
  Serial.println("-----------------------------------------------------------------------------------------------------------------------");
}
 
int integerValue=0;  // Max value is 65535
char incomingByte;
int userCommand=0;
int midCommand=0;
 
void loop() {
  if (Serial.available() > 0) {   // something came across serial
    integerValue = 0;         // throw away previous integerValue
    while(1) {            // force into a loop until 'n' is received
      incomingByte = Serial.read();
      if (incomingByte == '\n') break;   // exit the while(1), we're done receiving
      if (incomingByte == -1) continue;  // if no characters are in the buffer read() returns -1
      integerValue *= 10;  // shift left 1 decimal place
      // convert ASCII to integer, add, and shift left 1 decimal place
      integerValue = ((incomingByte - 48) + integerValue);
    }
    midCommand=integerValue;
    if(midCommand<0) {
      userCommand=midCommand+30;
      userCommand*=-1;
    }
    else userCommand=midCommand;   

    //userCommand is now the integer put in by the user: -9<=userCommand<=some number that is less than 60,000 but too much to be of worry
    moveBy(userCommand);
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print(currentPos);
  }
}

void opticalSensor_ISRb() {
  if (isClockwise) currentPos++;
  else currentPos--;
}

void turnCW() {
  enable();
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  isClockwise=true;
  
}

void turnCCW() {
  enable();
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  isClockwise=false;
}

void enable() {
  digitalWrite(enA1, 50);
  digitalWrite(enA2, 50);
}

void brake() {
  digitalWrite(enA1, LOW);
  digitalWrite(enA2, LOW);
}

void moveBy(int i) {
  int newPosition=0;
  i=userCommand;
  if(i > 0) {
    newPosition=currentPos+i;
    while(currentPos<newPosition) {
      turnCW();
    }
    brake();
    currentPos=newPosition;
    newPosition=0;
    Serial.print("User Input: ");
    Serial.println(userCommand);
    Serial.print("Now moving ");
    Serial.print(userCommand*90);
    Serial.println(" degrees clockwise.");
    Serial.println("-----------------------------------------------------------------------------------------------------------------------");
  }
  if(i < 0) {
    newPosition=currentPos+i;
    while(currentPos>newPosition) {
      turnCCW();
    }
    brake();
    currentPos=newPosition;
    newPosition=0;
    Serial.print("User Input: ");
    Serial.println(userCommand);
    Serial.print("Now moving ");
    Serial.print(userCommand*-90);
    Serial.println(" degrees counterclockwise.");
    Serial.println("-----------------------------------------------------------------------------------------------------------------------");
  }
}
