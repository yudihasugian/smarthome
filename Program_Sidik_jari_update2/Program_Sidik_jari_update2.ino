 
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define DHTPIN 5        // Pin yang di gunakan untuk sensor
#define DHTTYPE DHT11   // Type Sensor DHT11 
#define FANPIN 9       // Pin yang di gunakan untuk kipas
#define BATAS  31.00

LiquidCrystal_I2C lcd(0x3F, 16, 2);
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

DHT dht(DHTPIN, DHTTYPE);

//------------------------------//
int tombol1 = A2;
int relay1 = 8;
int relay2=9;
int eadd = 0;
int getFingerprintIDez();
uint8_t getFingerprintEnroll(int id);

void setup() {
  Serial.begin(9600);
  lcd.begin();
  dht.begin();
  eadd = EEPROM.read(0);
  if (eadd > 200)EEPROM.write(0, 0);

  pinMode(relay1, OUTPUT);
  pinMode(tombol1, INPUT_PULLUP);
  digitalWrite(relay1, LOW);
  
 

  finger.begin(57600);
  if (finger.verifyPassword()) {

  } else {
    while (1);
  }
  eadd = EEPROM.read(0);
  
  
  Serial.println(F("Kontrol Kipas Otomatis"));
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH);
  pinMode(FANPIN,OUTPUT); //mendeklarasikan pin menjadi OUTPUT
}

void loop() {
  lcd.setCursor (0, 0);
  lcd.print(F(" -System Ready- "));

  if (!digitalRead(tombol1)) {
    delay(1000);
    if (!digitalRead(tombol1)) {
      finger.emptyDatabase();
      eadd = 0;
      EEPROM.write(0, eadd);
      lcd.clear();
      delay(15);
      lcd.setCursor(3, 0);
      lcd.print("Sidik Jari");
      lcd.setCursor(1, 1);
      lcd.print("Telah Dihapus");
      delay(2500);
      lcd.clear();
      delay(15);
      goto awal;
    }
    eadd += 1;
    if (eadd > 50)eadd = 0;
    EEPROM.write(0, eadd);
    getFingerprintEnroll(eadd);
    eadd = EEPROM.read(0);
  }
awal:
  getFingerprintIDez();


  
  delay(1000); //Jeda 500ms

  // Mengambil Data Kelembaban
  float h = dht.readHumidity();
  // Mengambil Data Celsius
  float t = dht.readTemperature();
  // Data Fahrenheit
  float f = dht.readTemperature(true);

  // Mengecek Sensor apakah dapat memberikan data 
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Gagal Mengamblil Data Dari Sensor, Periksa Kembali Kabel Sensor"));
    return;
  }

  // Format Fahrenheit 
  float hif = dht.computeHeatIndex(f, h);
  // Format Celsius 
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Kelembaban: "));
  Serial.print(h);
  Serial.print(F("%  Suhu: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Index Panas: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.print(F("°F"));
  Serial.print(F(" Kipas:")); 
  if(t>=BATAS) Serial.println(F(" ON")); else Serial.println(F(" OFF"));

//================================================================  
//Algoritma untuk menyalakan kipas
 if(t>=BATAS) digitalWrite(FANPIN,HIGH); else digitalWrite(FANPIN,LOW); 

 delay(1000);

}
