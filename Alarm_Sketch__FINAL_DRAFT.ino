

/*
:Project:Allarm_Clock
:Author: Tiziano Bianchettin
:Date: 10/02/2017
:Revision: 2
:License: Public Domain
thanks to:
  http://arduinoenonsolo.blogspot.it/2012/12/orologio-con-arduino-e-il-ds1307.html
  http://www.mauroalfieri.it/
  http://www.danielealberti.it/
  http://www.maffucci.it/
  My electronics laboratory professor "Perito Carli"  
*/
//************libraries**************//
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <pitches.h>

//************************************//
LiquidCrystal_I2C lcd(0x27,16,2); // Display  I2C 20 x 4
RTC_DS1307 RTC;

//************Button*****************//
int P1=5; // Button SET MENU'
int P2=4; // Button +
int P3=3; // Button -
int P4=2; // SWITCH Alarm
int P5=12; //Temp/Hum Toggle

//**************Alarm***************//
#define buzzer 11
#define DHTPin 7  //  Input pin for temp and humidity sensor
#define DHTTYPE DHT11  //Used to create an object (DHTTYPE) of the DHT11 temp sensor (I think?) 

DHT dht(DHTPin, DHTTYPE);  //Sets up the sensor
//************Variables**************//
int hourupg;
int minupg;
int yearupg;
int monthupg;
int dayupg;
int menu =0;
int setAll =0;

uint8_t alarmHours = 0, alarmMinutes = 0;  // Holds the current alarm time

//array for holding the "song" the  speaker will play when alarm goes off
int notes[] = {
  NOTE_A4, NOTE_E3, NOTE_A4, 0,
  NOTE_A4, NOTE_E3, NOTE_A4, 0
};

//array for holding the "duration"of each note in then song when alarm goes off
int times[] = {
  250, 250, 250, 250,
  250, 250, 250, 250
};
//pin numbers for shift register
const int SER = 8;
const int LATCH = 9;
const int CLK = 10;
int seq [15]={1, 2, 4, 8, 16, 32, 64, 128, 64, 32, 16, 8, 4, 2, 1};

//variables to hold temp and humidity 
int humidity = 0;
int temperature = 0;

void setup()
{

  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(P1,INPUT); // https://www.arduino.cc/en/Tutorial/InputPullupSerial
  pinMode(P2,INPUT);
  pinMode(P3,INPUT);
  pinMode(P4,INPUT);
  pinMode (P5, INPUT);
  pinMode(buzzer, OUTPUT); // Set buzzer as an output
  pinMode (SER, OUTPUT);
  pinMode (LATCH, OUTPUT);
  pinMode (CLK, OUTPUT);
  printAllOff();
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // Set the date and time at compile time
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // RTC.adjust(DateTime(__DATE__, __TIME__)); //removing "//" to adjust the time
    // The default display shows the date and time
  int menu=0;
}
 
void loop()
{ 

// check if you press the SET button and increase the menu index
  if(digitalRead(P1)== HIGH) 
  {
   menu=menu+1;
  }
  if((digitalRead(P2)== HIGH)&&(digitalRead(P3)== HIGH))                                                                                                                                                                     
  {
    
    DisplaySetHourAll();
    DisplaySetMinuteAll();
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("ALARM");
    lcd.setCursor(5,1);
    lcd.print(alarmHours, DEC);
    lcd.print(":");
    lcd.print(alarmMinutes, DEC);
    delay(1000);
    lcd.clear();
  }
// in which subroutine should we go?
  if (menu==0)
    {
     DisplayDateTime(); // void DisplayDateTime
     Alarm(); // Alarm control
          }
  if (menu==1)
    {
    DisplaySetHour();
    }
  if (menu==2)
    {
    DisplaySetMinute();
    }
  if (menu==3)
    {
    DisplaySetYear();
    }
  if (menu==4)
    {
    DisplaySetMonth();
    }
  if (menu==5)
    {
    DisplaySetDay();
    }
  if (menu==6)
    {
    StoreAgg(); 
    delay(500);
    menu=0;
    }
    delay(100);
    DisplayTempHum();
}

void DisplayDateTime ()
{
// We show the current date and time
  DateTime now = RTC.now();

  lcd.setCursor(0, 0);
  lcd.print("Hr: ");
  
  if (now.hour()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.hour(), DEC);
  hourupg=now.hour();
  lcd.print(":");
  if (now.minute()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  minupg=now.minute();
  lcd.print(":");
  if (now.second()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);

  
  lcd.setCursor(0, 1);
  lcd.print("Dt: ");
  if (now.month()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.month(), DEC);
  monthupg=now.month();
  lcd.print("/");
  if (now.day()<=9)
  {
    lcd.print("0");
  }
  lcd.print(now.day(), DEC);
  dayupg=now.day();
  lcd.print("/");
  lcd.print(now.year(), DEC);
  yearupg=now.year();
 

}

void DisplaySetHour()
{
// time setting
  lcd.clear();
  DateTime now = RTC.now();
  if(digitalRead(P2)==HIGH)
  {
    if(hourupg==23)
    {
      hourupg=0;
    }
    else
    {
      hourupg=hourupg+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if(hourupg==0)
    {
      hourupg=23;
    }
    else
    {
      hourupg=hourupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Hour:");
  lcd.setCursor(0,1);
  lcd.print(hourupg,DEC);
  delay(200);
}

void DisplaySetMinute()
{
// Setting the minutes
  lcd.clear();
  if(digitalRead(P2)==HIGH)
  {
    if (minupg==59)
    {
      minupg=0;
    }
    else
    {
      minupg=minupg+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if (minupg==0)
    {
      minupg=59;
    }
    else
    {
      minupg=minupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Minutes:");
  lcd.setCursor(0,1);
  lcd.print(minupg,DEC);
  delay(200);
}
  
void DisplaySetYear()
{
// setting the year
  lcd.clear();
  if(digitalRead(P2)==HIGH)
  {    
    yearupg=yearupg+1;
  }
   if(digitalRead(P3)==HIGH)
  {
    yearupg=yearupg-1;
  }
  lcd.setCursor(0,0);
  lcd.print("Set Year:");
  lcd.setCursor(0,1);
  lcd.print(yearupg,DEC);
  delay(200);
}

void DisplaySetMonth()
{
// Setting the month
  lcd.clear();
  if(digitalRead(P2)==HIGH)
  {
    if (monthupg==12)
    {
      monthupg=1;
    }
    else
    {
      monthupg=monthupg+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if (monthupg==1)
    {
      monthupg=12;
    }
    else
    {
      monthupg=monthupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Month:");
  lcd.setCursor(0,1);
  lcd.print(monthupg,DEC);
  delay(200);
}

void DisplaySetDay()
{
// Setting the day
  lcd.clear();
  if(digitalRead(P2)==HIGH)
  {
    if (dayupg==31)
    {
      dayupg=1;
    }
    else
    {
      dayupg=dayupg+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if (dayupg==1)
    {
      dayupg=31;
    }
    else
    {
      dayupg=dayupg-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set Day:");
  lcd.setCursor(11,0);
  lcd.print(dayupg,DEC);
  delay(200);
}

void StoreAgg()
{
// Variable saving
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SAVING IN");
  lcd.setCursor(0,1);
  lcd.print("PROGRESS");
  RTC.adjust(DateTime(yearupg,monthupg,dayupg,hourupg,minupg,0));
  delay(200);
}



void DisplayTempHum()
{
  while (digitalRead(P5)==HIGH){
  int temperature, humidity;
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  char tempF[6]; 
  char humF[6];
  dtostrf(temperature, 2, 0, tempF);
  dtostrf(humidity, 2, 0, humF);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.setCursor (6,0); 
  lcd.print(tempF);
  lcd.print((char)223);
  lcd.print("C ");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humF);
  lcd.print("%");
}
}

void DisplaySetHourAll()// Setting the alarm minutes
{
  while(digitalRead(P1)==LOW){

  lcd.clear();

  if(digitalRead(P2)==HIGH)
  {
    if(alarmHours==23)
    {
      alarmHours=0;
    }
    else
    {
      alarmHours=alarmHours+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if(alarmHours==0)
    {
      alarmHours=23;
    }
    else
    {
      alarmHours=alarmHours-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set HOUR Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmHours,DEC);
  delay(200);
 }
 delay(200);
}

void DisplaySetMinuteAll()// Setting the alarm minutes
 {
  while(digitalRead(P1)==LOW){ 

  lcd.clear();
  if(digitalRead(P2)==HIGH)
  {
    if (alarmMinutes==59)
    {
      alarmMinutes=0;
    }
    else
    {
      alarmMinutes=alarmMinutes+1;
    }
  }
   if(digitalRead(P3)==HIGH)
  {
    if (alarmMinutes==0)
    {
      alarmMinutes=59;
    }
    else
    {
      alarmMinutes=alarmMinutes-1;
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Set MIN. Alarm:");
  lcd.setCursor(0,1);
  lcd.print(alarmMinutes,DEC);
  delay(200);
 }
 delay(200);
}
void printAllOn(){
 lcd.setCursor(0,3);
 lcd.print("Alarm:   ");

  
  
  if (alarmHours <= 9)
  {
    lcd.print("0");
  }
  lcd.print(alarmHours, DEC);
  
  lcd.print(":");
  if (alarmMinutes <= 9)
  {
    lcd.print("0");
  }
  lcd.print(alarmMinutes, DEC); 

}
void printAllOff() {
  //lcd.setCursor(0, 3);
 //lcd.print("Alarm: Off  ");  
}
void Alarm(){
   if(digitalRead(P4)== HIGH)
  {
   setAll=setAll+1;
  }
  if (setAll==0)
    {
     printAllOff();
     noTone (buzzer);
     }
  if (setAll==1)
    {

     printAllOn();    
  
     DateTime now = RTC.now();
     if ( now.hour() == alarmHours && now.minute() == alarmMinutes )
        {
         lcd.noBacklight();
         DateTime now = RTC.now();
        for (int i = 0; i < 8; i++){
        tone(buzzer, notes[i], times[i]);
        delay (times[i]);}
        for (int i = 0; i > 8; i--)
        {
        tone(buzzer, notes[i], times[i]);
        delay (times[i]);}
        for (int i=0; i<15; i++){
        digitalWrite (LATCH, LOW);
        shiftOut (SER, CLK, MSBFIRST, seq[i]);
        digitalWrite (LATCH, HIGH);
        delay (100);
}
}
         lcd.backlight();
        }
    else{
         noTone (buzzer);
        }
     
  if (setAll==2)
    {
     setAll=0;
    }
    delay(200);
}
