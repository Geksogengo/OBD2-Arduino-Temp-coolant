#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>      // 
#include <Wire.h> 
//#include "SparkFunBME280.h" //áèáëèîòåêà äàò÷èêà BME280
#define RxD 7                //Arduino pin connected to Tx of HC-05
#define TxD 8                //Arduino pin connected to Rx of HC-05
SoftwareSerial blueToothSerial(RxD,TxD);
LiquidCrystal_I2C lcd(0x27, 8,2);  // Set the LCD I2C address
//BME280 mySensorB; //Uses I2C address 0x76 (jumper closed)

const int redpin = 7;
const int greenpin = 8;
const int bluepin = 9;
int p = 3; //объявляем переменную с номером пина, на который мы подключили пьезоэлемент
byte inData;
char inChar;
byte inData2;
char inChar2;
String BuildINString2="";
String DisplayString2="";
String WorkingString2="";
String BuildINString="";
String DisplayString="";
String WorkingString="";
long DisplayValue;
long A;
unsigned long delayTime;
long cont = 0;
float temperature(NAN);

byte temp[8] = {
    B00100,
    B01010,
    B01010,
    B01010,
    B01010,
    B10001,
    B10001,
    B01110,
};

byte hu[8] = {
    B01110,
    B01010,
    B10001,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
};

byte c[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000,
};


void setup() {
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
 // mySensorB.setI2CAddress(0x76); //Connect to a second sensor
 // if(mySensorB.beginI2C() == false) Serial.println("Sensor B connect failed"); 
   
  pinMode(p, INPUT); //объявляем пин как выход.
  pinMode(redpin, INPUT);
  pinMode(greenpin, INPUT);
  pinMode(bluepin, INPUT);
  lcd.init();          // Инициализация LCD           
  lcd.backlight();     // Включаем подсветку дисплея
  lcd.setCursor(0, 0);
  lcd.print("Welcome!"); //Display Welcome Message
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("........");
  delay(2000);
  lcd.clear();

//********************************************************************
//Added new code to check if ELM327 responded
  Retry:
  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("connect");

  
  //Set up Serial communication at 9600 baud
  //Serial.begin(9600);   //initialize Serial  
  //BTSerial.begin(38400);
  Serial.begin(9600);
  Serial.println("ATZ");
  lcd.setCursor(0, 0);
  lcd.print("ATZ");
  delay(2000);
  ReadData();
   

                                             // If used substring(1,4)=="ATZ" needed a space before ATZ in Serial Monitor and it did not work
  if (BuildINString.substring(1,3)=="TZ")    // MIATA RESPONSE TO ATZ IS ATZ[[[ELM327 V1.5  OR AT LEAST THAT IS WHAT ARDUINO HAS IN THE BUFFER
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome");
      lcd.setCursor(9, 0);
      //lcd.print(BuildINString);   //Echo response to screen "Welcome  ELM327"
      lcd.setCursor(0, 1);
      lcd.print("  OK");
      delay(1000);
      tone (p, 500); //включаем на 500 Гц
      delay(100); //ждем 100 Мс
      tone(p, 0); //включаем на 1000 Гц
      delay(100); //ждем 100 Мс
      tone(p, 500); //включаем на 1000 Гц
      delay(100); //ждем 100 Мс
      tone(p, 0); //включаем на 1000 Гц
      pinMode(p, INPUT); //объявляем пин как вход.
      lcd.clear();
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Error:");
      lcd.setCursor(0, 1);
      lcd.print("no connection");
      delay(1500);
      lcd.clear();
      goto Retry;
      
    }

   Serial.println("0100");          // Works with This  only atz
   lcd.setCursor(0, 0);
   lcd.print("initialzing"); //Initialize BUS 
   lcd.clear();
   delay(2000);
   ReadData();
   lcd.setCursor(0, 0);            //Added 12-10-2016
   lcd.print("  OK");  //Added 12-10-2016
   lcd.clear();
    // pinMode(redpin, INPUT_PULLUP);
  pinMode(greenpin, OUTPUT);
  //pinMode(bluepin, INPUT_PULLUP);
  digitalWrite(greenpin, HIGH);
  delay(300);
  digitalWrite(greenpin, LOW);
  delay(300);
  digitalWrite(greenpin, HIGH);
  delay(300);
  digitalWrite(greenpin, LOW);
  delay(300);
  digitalWrite(greenpin, HIGH);
  delay(300);
  digitalWrite(greenpin, LOW);
  delay(300);
  pinMode(greenpin, INPUT);
   delay(500); 
   lcd.setCursor(0, 1);
   lcd.print("protocol");  
   Serial.println("ATSP0"); // !!!!!!!!!!!!!!! Установка протокола обмена данными: ATSP0 - автоматически.
   delay(1000);
   lcd.clear();
}

void loop() {
 // temperature = mySensorB.readTempC();
  // Added to display Coolant Temp
  //resets the received string to NULL  Without it it repeated last string.
  BuildINString = "";  
  Serial.println("0105");// Send Coolant PID request 0105
  //Serial.println("01 05\r");
  //Serial.flush();         //Not sure if it's needed*********************************************
  delay(500);
  //Receive complete string from the serial buffer
  ReadData();  //replaced below code BuildINString.substring
  
  // Parse the received string, retrieve only the hex value for temperature  Example: 32 is the 11 and 12th character
  // 0105[41 05 32   Correct value reading back  10 degrees. (11,13 or even 11,14 works)
  WorkingString = BuildINString.substring(11,13);
  //Serial.println(BuildINString.substring(0,186));
  //Serial.println(WorkingString); 
  delay(100);
   

   A = strtol(WorkingString.c_str(),NULL,16);  //convert hex to decimnal

   DisplayValue = A;
   DisplayString = String(DisplayValue - 40); //+ " C            ";  // Subtract 40 from decimal to get the right temperature
   lcd.clear();
   lcd.createChar(0,temp);
   lcd.createChar(2,c);
   //lcd.clear();
   lcd.setCursor(0, 0); 
   lcd.print(char(0)); //lcd.print(char(4));
   lcd.print(" ");
   lcd.print(DisplayString);
   //lcd.print(" ");
   lcd.print(char(2)); 
   //lcd.print("C ");
   //lcd.setCursor(0, 1);
   //lcd.print(char(0));
   //lcd.print("COOLANT"); 
   //lcd.print(char(4));
   //lcd.print(String(temperature));
   //lcd.print(char(2));
  // delay(100);

//Check if if over 100 C ******************************************************
int B;
B = DisplayString.toInt();  //Convert String to Integer  .toInt()


   if (B >= 97){     //Check if if over 100 C
    
// ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.display();   //For I2C use lcd.backlight
    delay(50);
    lcd.noDisplay(); //For I2C use lcd.noBacklight
    delay(50);
  }
  lcd.display(); // finish with backlight on   //For I2C use lcd.backlight
  tone (p, 500); //включаем на 500 Гц
  delay(10); //ждем 100 Мс
  tone(p, 2000); //включаем на 1000 Гц
  delay(10); //ждем 100 Мс
  tone(p, 500); //включаем на 1000 Гц
  delay(10); //ждем 100 Мс
  tone (p, 500); //включаем на 500 Гц
  delay(10); //ждем 100 Мс
  tone(p, 2000); //включаем на 1000 Гц
  delay(10); //ждем 100 Мс
  tone (p, 500); //включаем на 500 Гц
  delay(100); //ждем 100 Мс
  tone(p, 2000); //включаем на 1000 Гц
  delay(50); //ждем 100 Мс
  pinMode(p, INPUT); //объявляем пин как вход.
  pinMode(redpin, OUTPUT);
  //pinMode(greenpin, INPUT_PULLUP);
  //pinMode(bluepin, INPUT_PULLUP);
  digitalWrite(redpin, HIGH);
  delay(50);
  digitalWrite(redpin, LOW);
  delay(50);
  digitalWrite(redpin, HIGH);
  delay(50);
  pinMode(redpin, INPUT);
   }
  if (B>=-20 && B<=40) //(a==5 && b>5)
  {for(int i=0;i<3;i++)
  { 
  tone (p, 300); //включаем на 300 Гц
  delay(10); //ждем 100 Мс
  tone(p, 300); //включаем на 300 Гц
  delay(10); //ждем 100 Мс
  tone(p, 300); //включаем на 300 Гц
  delay(10); //ждем 100 Мс
  tone (p, 300); //включаем на 300 Гц
  delay(10); //ждем 100 Мс
  pinMode(p, INPUT); //объявляем пин как вход.
  delay(10); //ждем 100\0 Мс
  pinMode(bluepin, OUTPUT);
  digitalWrite(bluepin, HIGH);
  delay(50);
  digitalWrite(bluepin, LOW);
  delay(50);
  digitalWrite(bluepin, HIGH);
  delay(50);
  pinMode(bluepin, INPUT);
  
 }}
  VOLT();
}
void VOLT() {
  BuildINString2 = "";  
  Serial.println("ATRV");// Send Coolant PID request 0105
  delay(100);
  ReadData2();  //replaced below code BuildINString.substring
  WorkingString2 = BuildINString2.substring(5,9);
  //Serial.println(WorkingString2);
  //delay(100);
  // lcd.clear();
   lcd.createChar(1,hu);
   lcd.setCursor(0, 1);
   lcd.print(char(1)); //lcd.print(char(1));
   lcd.print(" ");
   lcd.print(WorkingString2);
   lcd.print(" "); 
   lcd.print("V");
   delay(2000);
}
//Read Data Function ***********************************************************
void ReadData()
{
BuildINString="";  
  while(Serial.available() > 0)
  {
    inData=0;
    inChar=0;
    inData = Serial.read();
    inChar=char(inData);
    BuildINString = BuildINString + inChar;
  }
}
void ReadData2()
{
  BuildINString2="";  
  while(Serial.available() > 0)
  {
    inData2=0;
    inChar2=0;
    inData2 = Serial.read();
    inChar2=char(inData2);
    BuildINString2 = BuildINString2 + inChar2;
  }
}
