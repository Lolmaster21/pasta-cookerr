/*
  Project: Pasta Auto Boiler
  Desc: Controls the burner to boil pasta without overflowing.
  Authors: Keegan Dang and Lukas Henriquez
  Last Updated: 12/2/24
  Credit: Mathias Munk Hansen for DS18B20 Library https://github.com/matmunk/DS18B20/blob/master/README.md
*/
 
// Libraries
#include <Servo.h> // Servo Library
#include <DS18B20.h> // Water Temperature Probe Library
#include <DHT.h>; // Humidity Sensor Library
#include <LiquidCrystal_I2C.h> // LCD control library
 
// Pins
#define DHTPIN 2 // DHT11
#define DSPIN 4 // DS18B20
#define DIALPIN 3 // Micro Servo
#define FANPIN A0 // Fan PWM only
 
// Objects
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display. Does not use specified pins.
DHT dht(DHTPIN, DHT11); // Humidity Sensor
DS18B20 ds(DSPIN); // Temperature Probe
Servo dial; // Servo
 
// Constants: Found while testing
const int servoMax = 135; // Burner on Low
const int servoMin = 0; // Burner on High
const int humidityThreshold = 40; // OverBoiling
const int temperatureThreshold = 90; // Boiling
const int fanSpeed = 255;
 
// Functions
void setDial(bool burnerHigh) { // Set burner to low or High
  if (burnerHigh) {
  dial.write(servoMin); delay(20);  
  } else {
  dial.write(servoMax); delay(20);
  }
}
 
void setup()
{
  lcd.init(); // Set up LCD to be used.
  lcd.clear();
  lcd.backlight();  
  dht.begin();
  Serial.begin(9600);
  Serial.println("start");
  dial.attach(DIALPIN);
  pinMode(FANPIN, OUTPUT); // Fan PWM
  setDial(servoMin); // Set Servo to 0
}
 
void loop() {
  // Local Variables
  static unsigned long time; // Debug System time
  static float temperature;
  static float humidity;
  static bool highHumidity;
  static bool highTemperature;
 
  time = millis();
  Serial.print("System Time: "); Serial.println(time); // Debug timing
 
  // Read Sensors
  while (ds.selectNext()) {temperature = ds.getTempC();} // temperature
  humidity = dht.readHumidity(); // humidity
 
  // Control Logic
  highTemperature = temperature > temperatureThreshold;
  highHumidity = humidity > humidityThreshold;
 
  if (!highTemperature) { // Below boiling
    setDial(1); // Burner on High
    analogWrite(FANPIN, 0);
  }
  else if (highHumidity) { // Over Boiling
    setDial(0); // Burner on Low
    analogWrite(FANPIN, fanSpeed);
  }
  else { // Boiling
    setDial(1); // Burner on High
    analogWrite(FANPIN, fanSpeed);
  }
 
  // Liquid Crystal Display
 
  // Check if failed reading
  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print("Failed");
    lcd.setCursor(0, 1);
    lcd.print("Read");
    return;
  }
 
  // Display humidity on the first line
  lcd.setCursor(0, 0);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%   ");
 
  // Display temperature in Celsius on second line
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C   ");
}
 
