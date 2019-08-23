#include <Wire.h> 
#include<EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <DHT.h>
#include <DS3231.h>
#include <Servo.h>
#define DHTPIN 50
#define DHTTYPE DHT11   
LiquidCrystal_I2C lcd_green(0x27,16,2);
LiquidCrystal_I2C lcd_blue(0x3F,16,2);
Servo myservo;
DS3231  rtc(SDA, SCL);
Time t;
DHT dht(DHTPIN, DHTTYPE);
//----------------------keypad-----------------------------------------
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//----------------------variables--------------------------------------
boolean turn=LOW;
boolean led_sate=LOW;
int ldr_value,soil_moist,soil_temp,rain_water,soilMoist,h,sm,soilTemp,ldrValue,lv,mValue,dValue,eepmoist,eepdl;
unsigned long currentMilis,previousMilis,pcm,ppm,lcm,lpm;
//------------------------SETUP-------------------------------
void setup() {
  lcd_green.init();
  lcd_blue.init();
  dht.begin();
  lcd_green.backlight();
  lcd_blue.backlight(); 
  rtc.begin();
  pinMode(A0,INPUT);//LDR pin
  pinMode(A1,INPUT);//SOIL moist pin
  pinMode(A2,INPUT);//SOIL TEMP
  pinMode(A3,INPUT);//RAIN WATER
  pinMode(11,OUTPUT);//led out
  pinMode(33,OUTPUT);//pump out
  myservo.attach(49);//servo
  myservo.write(0);
  
  eepmoist=map(EEPROM.read(10),0,100,1023,0);
  eepdl= 6+EEPROM.read(11);
 
 
}
//************************LOOP****************************
void loop() {
  
lcd_green.setCursor(0,0);
  lcd_green.print("S.M | S.T | D.L");
  lcd_green.setCursor(4,1);
  lcd_green.print("|");
  lcd_green.setCursor(10,1);
  lcd_green.print("|");
  lcd_blue.setCursor(9,0);
  lcd_blue.print("|Humidi");
  lcd_blue.setCursor(9,1);
  lcd_blue.print("|");
  
//-----------------------------------------KEYPAD
char customKey = customKeypad.getKey();
    
  
  if (customKey){
    switch(customKey)
    {
  case 'A':
    settings();
    break;
    case 'B':
    information();
    break;
}

    
    }


  
  
  t = rtc.getTime();
  int current_hour=t.hour;
//------------------------------------------LDR/LIGHT 
  ldr_value= analogRead(A0);
  if(current_hour>=6 && current_hour<=eepdl){
    if(ldr_value>=650){
      led_sate=HIGH;
      }
      else{
        led_sate=LOW;
        }
    }
    else{
      led_sate=LOW;
      }
      digitalWrite(11,led_sate);
//-------------------------------------------PUMP
  currentMilis=millis();
  if(currentMilis-previousMilis>30000){
    digitalWrite(33,LOW);
    previousMilis=currentMilis;
    soil_moist= analogRead(A1);
    if (soil_moist>eepmoist){
      pump();
      } 
    }
    
 //---------------------------------------SHADE
 rain_water=analogRead(A3);

 if(rain_water>=150 || ldr_value<=100){
  if(turn==LOW){
  shadefall();
  }
   turn=HIGH;
  }
  if(rain_water<=10){
    if(turn==HIGH){
    shadeup();
    }
    turn=LOW;
    }
  
  //------------------------------------------LCD/PRINTING STUFF
  soilMoist= analogRead(A1);
  soilTemp=analogRead(A2)*0.48;
  ldrValue= analogRead(A0);
  lcm=millis();
  if(lcm-lpm>=5000){
    lpm=lcm;
      
  sm=map(soilMoist,0,1023,100,0);
  lcd_green.setCursor(0,1);
  lcd_green.print(sm);
  lcd_green.setCursor(2,1);
   lcd_green.print("%");

   
   lcd_green.setCursor(5,1);
  lcd_green.print(soilTemp);
  lcd_green.setCursor(7,1);
   lcd_green.print("'C");

   
   lv=map(ldr_value,0,1023,100,0);
   lcd_green.setCursor(12,1);
  lcd_green.print(lv);
  lcd_green.setCursor(14,1);
   lcd_green.print("%");
   
    }
  
  lcd_blue.setCursor(0,0);
  lcd_blue.print(rtc.getDOWStr());
   lcd_blue.setCursor(0,1);
   lcd_blue.print(rtc.getTimeStr());
   h = dht.readHumidity();
   lcd_blue.setCursor(11,1);
   lcd_blue.print(h-20);
   lcd_blue.setCursor(13,1);
   lcd_blue.print("%");

 delay(10);
}


//********************************EXTRA FUNCTIONS********************************

void pump(){
  digitalWrite(33,HIGH);
  delay(2000);
  digitalWrite(33,LOW);
  delay(500);
  }

void shadefall(){
   for (int pos = 0; pos <= 160; pos++) { 
    myservo.write(pos);           
    delay(1);
                         
  }
  }

void shadeup(){
  for (int pos = 160; pos >= 0; pos--) { 
    myservo.write(pos);             
    delay(1);                      
  }
  }



  void settings(){
    mValue=0;
    dValue=0;
    lcd_green.clear();
    lcd_blue.clear();
    lcd_green.print("  SETTINGS MODE");
    delay(2000);
    lcd_green.setCursor(0,1);
    lcd_green.print("Moist Value:-");//----------Moist
    lcd_blue.setCursor(0,1);
    lcd_blue.print("Range: 10 to 99");
    while(mValue<10){
      char GV1 = customKeypad.getKey();
      delay(10);
  if (GV1){
      mValue=(mValue*10)+(GV1-'0');
      if(mValue<10){
      lcd_blue.setCursor(0,0);
      lcd_blue.print(GV1);}
      else{
        lcd_blue.setCursor(1,0);
      lcd_blue.print(GV1);
        }
      delay(5); 
           }
      }
      delay(1000);

      EEPROM.write(10,mValue);//set EPPROM

      
    lcd_green.setCursor(0,1);
    lcd_green.print("                ");
    lcd_blue.setCursor(0,0);
    lcd_blue.print("                ");
    lcd_blue.setCursor(0,1);
    lcd_blue.print("                ");
    delay(500);
    lcd_green.setCursor(0,1);
    lcd_green.print("Day Light time:");//-------dayLight
    lcd_blue.setCursor(0,1);
    lcd_blue.print("Range: 1 to 9");
    while(dValue<=0){
      char GV2 = customKeypad.getKey();
      delay(10);
  if (GV2){
    lcd_blue.setCursor(0,0);
    lcd_blue.print(GV2);
    dValue=(dValue*10)+(GV2-'0');
      delay(5); 
           }
      }
      delay(1000);

      EEPROM.write(11,dValue);//set EPPROM
    
lcd_blue.clear();
lcd_green.clear();
delay(500);
lcd_green.setCursor(1,1);
lcd_green.print("..VALUE TAKEN..");
lcd_blue.setCursor(0,0);
lcd_blue.print("Moisture: ");
lcd_blue.setCursor(10,0);
lcd_blue.print(mValue);
lcd_blue.setCursor(0,1);
lcd_blue.print("Day Light: ");
lcd_blue.setCursor(11,1);
lcd_blue.print(dValue);

    delay(4000);
    lcd_blue.clear();
    lcd_green.clear();
    
    delay(500);
    lcd_green.setCursor(0,0);
    lcd_green.print("To take effect");
    lcd_green.setCursor(0,1);
    lcd_green.print("Please restart.");
delay(5000);
    lcd_blue.clear();
    lcd_green.clear();
delay(1000);
    }




    void information(){
      lcd_blue.clear();
    lcd_green.clear();

    lcd_green.setCursor(5,1);
lcd_green.print("INF.");
      lcd_blue.setCursor(0,0);
lcd_blue.print("Moisture: ");
lcd_blue.setCursor(10,0);
lcd_blue.print(EEPROM.read(10));
lcd_blue.setCursor(0,1);
lcd_blue.print("Day Light: ");
      lcd_blue.setCursor(11,1);
lcd_blue.print(EEPROM.read(11));
delay(5000);
lcd_blue.clear();
    lcd_green.clear();
    delay(500);
      }
