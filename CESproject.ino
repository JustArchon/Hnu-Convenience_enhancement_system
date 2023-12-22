#include <pitches.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h> //시리얼통신 라이브러리 호출
#include <Servo.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int lightPin= A0;
int TempePin = A1;
Servo servo;
Servo servo2;
int servoPin = 8;
int servo2Pin = 9;
int buzzer[] = {2093,2349,2637,2793,3136,3520,3951,4186};
int buzzerPin= 10;
int blueTx=5;   //Tx (보내는핀 설정)at
int blueRx=4;   //Rx (받는핀 설정)
char message;
SoftwareSerial bluetoothSerial(blueTx, blueRx);  //시리얼 통신을 위한 객체선언
int pirPinA = 2;
int pirPinB = 3;
int pirStateA = 0;
int pirStateB = 0;
int EbuttonO = 6;
int EbuttonI = 7;
int homeled = 13;
int boilerled = 11;
int airconled = 12;
int ElevatepirState = 0;
int HomepirState = 0;
boolean Elevate1f = false;
boolean LEDsystem = true;
boolean HomeSystem = false;
boolean HomeDoor = false;
boolean Hometempcontrol = true;
boolean DisplayEnable = false;
boolean Elevate1fbuzzer = false;
boolean Homeopenbuzzer = false;
boolean Homeopendisplay = false;
boolean Homedoortimeout = false;
boolean Airconstate = false;
boolean Boilerstate = false;
int IntNoA = 0;
int IntNoB = 1;
double DSthistime = 0;
double DRthistime = 0;
int airconC = 0;
int boilerC = 0;
 
void setup() 
{
  pinMode(pirPinA, INPUT);
  pinMode(pirPinB, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(EbuttonI, INPUT_PULLUP);
  pinMode(EbuttonO, INPUT_PULLUP);
  pinMode(homeled, OUTPUT);
  pinMode(boilerled, OUTPUT);
  pinMode(airconled, OUTPUT);
  servo.attach(servoPin);
  servo2.attach(servo2Pin);
  attachInterrupt(IntNoA, Elevate1Fpir, CHANGE);
  attachInterrupt(IntNoB, HomeDoorpir, CHANGE);
  servo2.write(0);
  Serial.begin(9600);   //시리얼모니터
  bluetoothSerial.begin(9600); //블루투스 시리얼
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
}
void loop(){
  int light = analogRead(lightPin);
  delay(100);
  int sensorValue= analogRead(TempePin);
  float temperature = sensorValue/ 9.31;
  //블루투스
  if(bluetoothSerial.available())
  {
    char bt;
    bt = bluetoothSerial.read(); // a= 에어컨키기,b= 에어컨끄기
    if(bt == 'a'){ // 에어컨키기
      digitalWrite(airconled, HIGH);
      Airconstate = true;
    }
    if(bt == 'b'){ // 에어컨끄기
      digitalWrite(airconled, LOW);
      Airconstate = false;
    }
    if(bt == 'c'){ // 보일러키기
      digitalWrite(boilerled, HIGH);
      Boilerstate = true;
    }
    if(bt == 'd'){ // 보일러 끄기
      digitalWrite(boilerled, LOW);
      Boilerstate = false;
    }
    if(bt == 'e'){ // 조명 키기
      digitalWrite(homeled, HIGH);
    }    
    if(bt == 'f'){ // 조명 끄기
      digitalWrite(homeled, LOW);
    }
    if(bt == 'g'){ // 온도 표시
      if (HomeSystem == true) {
        lcd.begin();
        lcd.setCursor(1,0);
        lcd.print("Temperature");
        lcd.setCursor(4,1);
        lcd.print(temperature);
        lcd.setCursor(8,1);
        lcd.print("C");
        lcd.backlight();
        DisplayEnable = true;
        DSthistime = millis();
      }
    }
    if(bt == 'h'){ // 사용자에게 온도알리기
      int sensorValue= analogRead(TempePin);
      float temperature = sensorValue/ 9.31;
      bluetoothSerial.println(temperature);
    }
    if(bt == 'i'){ // 화면에 에어컨, 보일러상태띄우기
      if (HomeSystem == true) {
        lcd.begin();
        lcd.setCursor(0,0);
        lcd.print("Aircon State: ");
        lcd.setCursor(13,0);
        if (Airconstate == true) {
          lcd.print("ON");
        }else{
          lcd.print("OFF");
        }
        lcd.setCursor(0,1);
        lcd.print("Boiler State: ");
        lcd.setCursor(13,1);
        if (Boilerstate == true) {
          lcd.print("ON");
        }else{
          lcd.print("OFF");
        }
        lcd.backlight();
        DisplayEnable = true;
        DSthistime = millis();
      }
    }
    if(bt == 'j'){ // 홈시스템에서 자동온도제어 키기
      Hometempcontrol = true;
    }
    if(bt == 'k'){ // 홈시스템에서 자동온도제어 끄기
      Hometempcontrol = false;
    }
    if(bt == 'l'){ // 홈시스템에게 에어컨상태체크
      if(Airconstate == true){
        bluetoothSerial.println("ON");
      }else{
        bluetoothSerial.println("OFF");
      }
    }
    if(bt == 'm'){ // 홈시스템에게 보일러상태체크
      if(Boilerstate == true){
        bluetoothSerial.println("ON");
      }else{
        bluetoothSerial.println("OFF");
      }
    }
    if(bt == 'n'){ // 홈시스템 상태체크
      if(HomeSystem == true){
        bluetoothSerial.println("ON");
      }else{
        bluetoothSerial.println("OFF");
      }
    }
    if(bt == 'y'){ // 홈서비스 켜기
      HomeSystem = true;
    }
    if(bt == 'z'){ // 홈서비스 끄기
      HomeSystem = false;
    }
  }
//홈서비스
  if (HomeSystem == true) {
    if (Hometempcontrol == true){
        if (temperature >= 7){
          airconC = airconC + 1;
        }else{
          airconC = 0;
        }
        if(temperature <= 5){
          boilerC = boilerC + 1;
        }else{
          boilerC = 0;
        }
      delay(2000);
    }
    // 조도센서에 의한 불켜기 (홈 서비스 활성화시)
    if (analogRead(lightPin) < 400) {
    digitalWrite(homeled, HIGH);
    }else{
    digitalWrite(homeled, LOW);
    }
    // 온도센서에 의한 에어컨켜기 (홈 서비스 활성화시)
    if (airconC >= 3 && Hometempcontrol == true){
      digitalWrite(airconled, HIGH);
      Airconstate = true;
    }else if(Hometempcontrol == false){
    }else{
      digitalWrite(airconled, LOW);
      Airconstate = false;
    }
    // 온도센서에 의한 보일러켜기 (홈 서비스 활성화시)
    if (boilerC >= 3 && Hometempcontrol == true){
      digitalWrite(boilerled, HIGH);
      Boilerstate = true;
    }else if(Hometempcontrol == false){
    }else{
      digitalWrite(boilerled, LOW);
      Boilerstate = false;
    }
    // 디스플레이 타임아웃
    if (DisplayEnable == true && millis() - DSthistime >= 10000){
      lcd.noBacklight();
      lcd.noDisplay();
      }
  }else{}
// 부저시스템
  if(Elevate1fbuzzer == true){
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[0], 500);
      delay(200);
      tone(buzzerPin, buzzer[1], 500);
      delay(200);
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[3], 500);
      delay(200);
      Elevate1fbuzzer = false;
  }
  if(Homeopenbuzzer == true){
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[0], 500);
      delay(200);
      tone(buzzerPin, buzzer[1], 500);
      delay(200);
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[3], 500);
      delay(200);
      tone(buzzerPin, buzzer[4], 500);
      delay(200);
      tone(buzzerPin, buzzer[3], 500);
      delay(400);
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[2], 500);
      delay(200);
      tone(buzzerPin, buzzer[1], 500);
      delay(200);
      tone(buzzerPin, buzzer[0], 500);
      delay(200);
      tone(buzzerPin, buzzer[1], 500);
      delay(200);
      Homeopenbuzzer = false;
  }
// 웰컴 유어하우스!
  if(Homeopendisplay == true){
    lcd.begin();
    lcd.setCursor(0, 0);
    lcd.print("Welcome Your Home!");
    lcd.backlight();
    for (int position = 0; position < 16; position++) {
      lcd.scrollDisplayLeft();
      delay(150);
    }
    for (int position = 0; position < 35; position++) {
      lcd.scrollDisplayLeft();
      delay(150);
    }
  }
  if(Homeopendisplay == true && millis() - DSthistime >= 10000){
    Homeopendisplay = false;
    lcd.noBacklight();
    lcd.noDisplay();
  }
// 도어 타임아웃
  if(Homedoortimeout == true && millis() - DRthistime >= 8000){
    servo2.write(0);
    Homedoortimeout = false;
  }
    
// 엘리베이터 버튼
  if (digitalRead(EbuttonI) == LOW && Elevate1f == true ) {
      tone(buzzerPin, buzzer[2], 500);
      delay(300);
      tone(buzzerPin, buzzer[0], 500);
      delay(300);
      servo.write(120);
      Elevate1f = false;
      delay(300);
      }
   if (digitalRead(EbuttonO) == LOW && Elevate1f == true ) {
      tone(buzzerPin, buzzer[0], 500);
      delay(300);
      tone(buzzerPin, buzzer[2], 500);
      delay(300);
      servo.write(120);
      Elevate1f = false;
      delay(300);
      }
}

void Elevate1Fpir(){
  int ElevatepirState = digitalRead(pirPinA);
  if (Elevate1f) {
    Serial.println("elevate 1f");
  } else {
      Elevate1fbuzzer = true;
      tone(buzzerPin, buzzer[2], 500);
      servo.write(0);
      Elevate1f = true;
  } 
}
void HomeDoorpir(){
 int HomepirState = digitalRead(pirPinB);
 if (HomeDoor == false) {
 servo2.write(180);
 Homeopenbuzzer = true;
 Homeopendisplay = true;
 Homedoortimeout = true;
 DRthistime = millis();
 DSthistime = millis();
 HomeDoor = true;
 HomeSystem = true;
 }
}
