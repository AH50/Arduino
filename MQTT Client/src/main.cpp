#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_NeoPixel.h>
#include <OneWire.h>
#include <DallasTemperature.h>


Adafruit_BMP280 bmp; // I2C
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, D3, NEO_GRB + NEO_KHZ800);

const char* ssid = "SSID";
const char* password = "password";
const char* mqtt_server = "ip_mqtt_serwer";
int sensorValue;
float brightness;
int ilosc_led=8;
boolean manual=true;
char r[3];
char g[3];
char b[3];
byte R,G,B;
char lastColor[8]="#FFFFFF";
long lastMsg = 0;
char msg[50];
OneWire oneWire(D6);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';

   for (int i = 0; i < length; i++) {
    msg[i]=payload[i];
  }
 Serial.println(msg);

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    client.publish("outLedStatus", "led on");
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else if((char)payload[0] == '0'){
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    client.publish("outLedStatus", "led off");
  }
  else if((char)payload[0] == '#')
  {
    if(strcmp(msg,"#000000")!=0)
    {
      strcpy(lastColor, msg);
    }
     strncpy(r, msg+1,2);
     strncpy(g, msg+3,2);
     strncpy(b, msg+5,2);
     R=(int)strtol(r, NULL, 16);
     G=(int)strtol(g, NULL, 16);
     B=(int)strtol(b, NULL, 16);
    for(int i=0;i<ilosc_led;i++)
    {
      strip.setPixelColor(i,R,G,B);
      strip.show();
    }
    client.publish("LedColor",msg);
    }
    else if((char)payload[0] == 'm')
    {
      manual=true;
    }
     else if((char)payload[0] == 'a')
    {
      manual=false;
    }
memset(msg, 0, 50);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xeeee), HEX);                                                                                                                                                                                                                                                                                                                                             Serial.println("JHJH");//JH znajdź w tej lini buga
    // Attempt to connect
    if (client.connect(clientId.c_str(),"login_to_mqtt_serwer(mosquitto)","password")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("inTopic");
      client.subscribe("inLedColor");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);
  pinMode(12,OUTPUT);
  sensors.begin();

  Serial.begin(115200);
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

   bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  strip.begin();
for(int k=0;k<ilosc_led;k++)
 {
   strip.setPixelColor(k,0, 0, 0,0);
   strip.setBrightness(10);
   strip.show();
 }
  setup_wifi();
  client.setServer(mqtt_server, 5555);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    sensorValue = analogRead(A0);
    brightness = sensorValue * (100 / 1023.0);
    client.publish("Oswietlenie", String(brightness).c_str());
    client.publish("Temperatura", String(bmp.readTemperature()).c_str());
    client.publish("Cisnienie", String(bmp.readPressure()/100+208/8.3).c_str());
    client.publish("Wysokosc", String(bmp.readAltitude(1010)).c_str());
    sensors.requestTemperatures();
    client.publish("tem", String( sensors.getTempCByIndex(0)).c_str());

    if(brightness<30 && manual==false){
      client.publish("inLedColor",lastColor);
    }
    else if(manual==false){
      client.publish("inLedColor","#000000");
      }
  }
}
