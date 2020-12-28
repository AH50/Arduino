#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ESP8266WiFi.h>
#include "Gsender.h"



#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME  "test"
#define AIO_KEY       "test"
#define WLAN_SSID       "test"  // can't be longer than 32 characters!
#define WLAN_PASS       "test"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TemperaturaPokoju");
Adafruit_MQTT_Publish wilgotnosc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/WilgotnoscPokoju");
Adafruit_MQTT_Publish OtwartyZamknietyPokoj = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/OtwartyZamknietyPokoj");
Adafruit_MQTT_Publish uruchomionoESP8266 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/UruchomionoESP8266");





Adafruit_BME280 bme; // I2C

int buttonPin = 12;
int period = 60000;
unsigned long time_now = 0;
bool otwarte  = true;
bool flagaWlaczenia = true;
int buttonValue;
bool sendMaill = false;
bool email = false;


void setup() {
  Serial.begin(9600);

  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  pinMode(buttonPin, INPUT_PULLUP);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  while (!Serial);   // time to get serial running
  Serial.println(F("BME280 test"));

  unsigned status;

  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  Serial.println("-- Default Test --");

  Serial.println();
  
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X2,  // temperature
                  Adafruit_BME280::SAMPLING_X16, // pressure
                  Adafruit_BME280::SAMPLING_X1,  // humidity
                  Adafruit_BME280::FILTER_X16,
                  Adafruit_BME280::STANDBY_MS_0_5 );

}


void loop() {

  if (millis() - time_now > period) {
    time_now = millis();
    printAndSendValues();

    if (otwarte == true) {
      OtwartyZamknietyPokoj.publish("Drzwi zostaly otwarte!!!");
      otwarte = false;
      if (sendMaill == true && email == false)
      {
        sendMaill = false;
        email = true;

        Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
        String subject = "Drzwi zostały otwarte!";
        if (gsender->Subject(subject)->Send("test2@gmail.com", "Drzwi zostały otwarte!")) {
          Serial.println("Message send.");
        } else {
          Serial.print("Error sending message: ");
          Serial.println(gsender->getError());
        }
      }
    }
  }

  buttonValue = digitalRead(buttonPin);
  if (buttonValue == LOW) {
    otwarte = true ;
    sendMaill = true;
  }
  else {
    email = false;
  }
}


void printAndSendValues() {

  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");


  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
  MQTT_connect();
  temperatura.publish(bme.readTemperature());
  wilgotnosc.publish(bme.readHumidity());
  if (flagaWlaczenia == true) {
    uruchomionoESP8266.publish("Uruchomiono ESP8266");
    flagaWlaczenia = false;
  }

}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
