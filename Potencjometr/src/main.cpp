#include <Arduino.h>

float napiecie = 0;//Wartość przeliczona na napięcie w V

void setup() {
  Serial.begin(9600);//Uruchomienie komunikacji przez USART
   pinMode(8, OUTPUT);
}

void loop() {
  napiecie = analogRead(A5) * (5.0/1024.0); //Przeliczenie wartości na napięcie
  Serial.println(napiecie);//Wysyłamy zmierzone napięcie
  delay(300);//Czekamy, aby wygodniej odczytywać wyniki

  if(napiecie>3)
  {
    digitalWrite(8, HIGH);   // włącznie LED
  }
  else
  {
    digitalWrite(8, LOW);    // wyłączenie LED
  }
}
