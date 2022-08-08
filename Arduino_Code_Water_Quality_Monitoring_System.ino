//Prateeek
//www.prateeks.in

#include <stdlib.h>
#include <SoftwareSerial.h>
#include<LiquidCrystal_I2C.h>
#include<Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SoftwareSerial nodemcu(2, 3);
LiquidCrystal_I2C lcd(0x3F, 20, 4);
#define SensorPin 0          //pH meter Analog output to Arduino Analog Input 0
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10], temp;

int sensorPin = A1;
float volt;
float ntu;

int f;
float val;
char buff2[10];
String valueString = "";
String Value = "";

void setup()
{
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("     Welcome TO");
  lcd.setCursor(0, 1);
  lcd.print("    Water Quality");
  lcd.setCursor(0, 2);
  lcd.print("  Monitoring System");
  delay(4000);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("xxxxxxxxxxxx");
  lcd.setCursor(5, 2);
  lcd.print("xxxxxxxxxxxx");
  delay(3000);
  lcd.clear();
  pinMode(13, OUTPUT);
  sensors.begin();
  Serial.begin(9600);
  nodemcu.begin(9600);
}
void loop()
{
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(SensorPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;                  //convert the millivolt into pH value

  Value =  dtostrf(phValue, 4, 2, buff2);  //4 is mininum width, 6 is precision
  valueString = valueString + Value + ",";
  Serial.println(valueString);

  sensors.requestTemperatures();
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");
  Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  Serial.print((char)176);//shows degrees character
  Serial.println("F");
  lcd.setCursor(0, 2);
  lcd.print("Temperature:=");
  lcd.setCursor(13, 2);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor(19, 2);
  lcd.print("C");
  lcd.setCursor(0, 3);
  lcd.print("Temperature:=");
  lcd.setCursor(13, 3);
  lcd.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  lcd.setCursor(19, 3);
  lcd.print("F");
  lcd.setCursor(2, 1);
  lcd.print("PH Value =");
  lcd.setCursor(14, 1);
  lcd.print(valueString);
  nodemcu.println(valueString);
  valueString = "";
  delay(1000);

  volt = 0;
  for (int i = 0; i < 800; i++)
  {
    volt += ((float)analogRead(sensorPin) / 1023) * 5;
  }
  volt = volt / 800;
  volt = round_to_dp(volt, 2);
  if (volt < 2.5) {
    ntu = 3000;
  } else {
    ntu = -1120.4 * square(volt) + 5742.3 * volt - 4353.8;
  }

  lcd.setCursor(3, 0);
  lcd.print(ntu);
  lcd.print(" NTU");

}

float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
