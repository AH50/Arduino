#include <Arduino.h>

#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)
Adafruit_BMP280 bmp; // I2C

int chipSelect = 4;
File file;

void setup() {
Serial.begin(9600);

if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

   bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

pinMode(chipSelect,OUTPUT);

if(!SD.begin(chipSelect))
  {
    Serial.println("Could not initialize SD card!");
  }
  if(SD.exists("dane.txt"))
  {
    Serial.println("File exist!");
  }
  else {
    Serial.println("File not exist!");
    file=SD.open("dane.txt",FILE_WRITE);
   if(file)
  {
    file.print("Temperatura[C]    ");
    file.print("Cisnienie[hPa]    ");
    file.println("Wysokosc[m]      ");
   }
  else
  {
    Serial.println("Could not open file(writing)");
  }
  file.close();
  }
}
void loop() {

  file=SD.open("dane.txt",FILE_WRITE);
  if(file)
  {
    file.print(bmp.readTemperature());
    file.print("  ");
    file.print(bmp.readPressure()/100+346/8.3);
    file.print("  ");
    file.println(bmp.readAltitude(1030));
    Serial.println("Zapis danych do karty SD");
  }
  else
  {
    Serial.println("Could not open file(writing)");
  }
file.close();
delay(30000);
}
