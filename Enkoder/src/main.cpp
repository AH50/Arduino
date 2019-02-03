#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

unsigned long time = 0;
double ilosc_impulsow = 0;
int ilosc_led=8;
int tmp=0;
int r=0,g=255,b=0;

void minus(void);
void plus(void);
void led_on(double);
void led_off(double);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, 8, NEO_GRB + NEO_KHZ800);

void setup()
{
 pinMode(2,INPUT); // CLK
 pinMode(3,INPUT); // DT
 pinMode(4,INPUT); // SW

 attachInterrupt(0, plus, LOW);
 attachInterrupt(1, minus, LOW);

 strip.begin();

for(int k=0;k<ilosc_led;k++)
 {
   strip.setPixelColor(k,0, 0, 0,0);
   strip.setBrightness(10);
   strip.show();
 }
 time = millis();
}

void led_on(double n)
{
  if(n>ilosc_led) ilosc_impulsow=n=ilosc_led;
  else if(n<0) ilosc_impulsow=n=0;
    for(int i=0;i<n;i++)
    {
      strip.setPixelColor(i, r, g, b);
      strip.show();
    }
  led_off(ilosc_led-n);
}

void led_off(double m)
{
 for(int j=ilosc_led;j>=ilosc_led-m ;j--)
   {
    strip.setPixelColor(j, 0, 0, 0);
    strip.show();
   }
}

void minus()
{
 if ((millis() - time) > 3)
 {
   ilosc_impulsow--;
 }
 time = millis();
}

void plus()
{
 if ((millis() - time) > 3)
 {
   ilosc_impulsow++ ;
 }
 time = millis();
}

void loop()
{
  if(tmp!=ilosc_impulsow) //usunięcie nadmiarowego uruchamiania się funkcji led_on()
  {
    led_on(ilosc_impulsow);
    tmp=ilosc_impulsow;
  }

  if (digitalRead(4)==LOW)
  {
    delay(115);
     if (digitalRead(4)==LOW)
    {
      if (r==255)
      {
        r=0;
        g=255;
      }
      else if(g==255)
        {
          g=0;
          b=255;
          }
        else if (b==255)
        {
          b=0;
          r=255;
        }
      led_on(ilosc_impulsow);
    }
  }
}
