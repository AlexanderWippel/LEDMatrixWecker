#include <LedControl.h>
#include <DS3231.h>

#define CLK_PIN 13
#define DIN_PIN 12
#define CS_PIN 11
#define MAXDEVICES 4

#define SDA_PIN A0
#define SCL_PIN A1

DS3231 rtc(SDA_PIN, SCL_PIN);
Time rtc_zeit;
int zwischenzeit;

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MAXDEVICES);
void setup()
{
  CLKPR = 0x80;
  CLKPR = 0x00;

  rtc.begin();
  rtc.setTime(23,58, 50);

  int devices = lc.getDeviceCount();
  for (int address = 0; address < devices; address++)
  {
    lc.shutdown(address, false);
    lc.setIntensity(address, 2);
    lc.clearDisplay(address);
  };
  rtc_zeit = rtc.getTime();
  zwischenzeit = rtc_zeit.sec;
};

const long long int zahl[] =
{
  0x0018242c34241800,
  0x0038101010181000,
  0x003c041820241800,
  0x0018242010241800,
  0x0020203c24283000,
  0x001824201c043c00,
  0x001824241c043800,
  0x0008081010203c00,
  0x0018242418241800,
  0x0018242038241800
};

const long long int temperatur[] =
{
  0x003c420202423c00,
  0x0000000000060600
};

const long long int prozent = {0x0062640810264600};

void delayausgabe(int device, long long int zahlen)
{
  for (int i = 1; i < 7; i++)
  {
    byte row = (zahlen >> i * 8) & 0xff;
    for (int j = 1; j < 7; j++)
    {
      lc.setLed(device, i, j, bitRead(row, j));
    }
    delay(1);
  }
}

void ausgabe(int device, long long int zahlen)
{
  for (int i = 1; i < 7; i++)
  {
    byte row = (zahlen >> i * 8) & 0xff;
    for (int j = 1; j < 7; j++)
    {
      lc.setLed(device, i, j, bitRead(row, j));
    }
  }
}



boolean bolean = true;
boolean einmal = true;

int i = 0;
int j = 0;
int sek_d = 3;
int sek_r = 0;
int sek_s = 1;
int x = 0;

int sec_MSB = 0;
int sec_LSB = 0;

int min_LSB = 0;
int min_MSB = 0;

int stund_LSB = 0;
int stund_MSB = 0;

int temp_LSB = 0;
int temp_MSB = 0;

void berechnen()
{
  rtc_zeit = rtc.getTime();

  sec_LSB = (rtc_zeit.sec % 10);
  sec_MSB = (rtc_zeit.sec / 10) % 10;

  min_LSB = (rtc_zeit.min % 10);
  min_MSB = (rtc_zeit.min / 10) % 10;

  stund_LSB = (rtc_zeit.hour % 10);
  stund_MSB = (rtc_zeit.hour / 10) % 10;

  temp_LSB = (((int)rtc.getTemp()) % 10);
  temp_MSB = (((int)rtc.getTemp()) / 10) % 10;
}


  
void loop()
{
  rtc_zeit = rtc.getTime();

  while (rtc_zeit.sec != zwischenzeit)
  {
    berechnen();
    
    if (einmal == true)
    {
      //Uhrzeit anzeigen
      ausgabe(0, zahl[min_LSB]);
      ausgabe(1, zahl[min_MSB]);

      ausgabe(2, zahl[stund_LSB]);
      ausgabe(3, zahl[stund_MSB]);

      einmal = false;
    }

    //Sekundenazeige
    if (rtc_zeit.sec <= 7 && rtc_zeit.sec != 0)
    {
      lc.setLed(3, 0, (rtc_zeit.sec % 8), true);
    }
    if (rtc_zeit.sec >= 8 && rtc_zeit.sec <= 15)
    {
      lc.setLed(2, 0, (rtc_zeit.sec % 8), true);
    }
    if (rtc_zeit.sec >= 16 && rtc_zeit.sec <= 23)
    {
      lc.setLed(1, 0, (rtc_zeit.sec % 8), true);
    }
    if (rtc_zeit.sec >= 24 && rtc_zeit.sec <= 30)
    {
      lc.setLed(0, 0, (rtc_zeit.sec % 8), true);
    }


    if (rtc_zeit.sec >= 31 && rtc_zeit.sec <= 37)
    {
      lc.setLed(3, 7, ((rtc_zeit.sec - 30) % 8), true);
    }
    if (rtc_zeit.sec >= 38 && rtc_zeit.sec <= 45)
    {
      lc.setLed(2, 7, ((rtc_zeit.sec - 30) % 8), true);
    }
    if (rtc_zeit.sec >= 46 && rtc_zeit.sec <= 53)
    {
      lc.setLed(1, 7, ((rtc_zeit.sec - 30) % 8), true);
    }
    if (rtc_zeit.sec >= 54 && rtc_zeit.sec <= 60)
    {
      lc.setLed(0, 7, ((rtc_zeit.sec - 30) % 8), true);
    }

    if (rtc_zeit.sec == 14 || rtc_zeit.sec == 34 || rtc_zeit.sec == 54)
    {
      //Uhrzeit anzeigen
      ausgabe(0, zahl[min_LSB]);
      ausgabe(1, zahl[min_MSB]);

      ausgabe(2, zahl[stund_LSB]);
      ausgabe(3, zahl[stund_MSB]);
    }

    //Temperatur anzeigen
    if (rtc_zeit.sec >= 10 && rtc_zeit.sec <= 13 || rtc_zeit.sec >= 30 && rtc_zeit.sec <= 33 || rtc_zeit.sec >= 50 && rtc_zeit.sec <= 53)
    {
      lc.setLed(2, 2, 7, false);
      lc.setLed(2, 5, 7, false);

      ausgabe(0, temperatur[1]);
      ausgabe(1, temperatur[0]);

      ausgabe(2, zahl[temp_LSB]);
      ausgabe(3, zahl[temp_MSB]);
    }

    else
    {
      //Doppelpunkt anzeigen
      if (rtc_zeit.sec % 2 == 1)
      {
        if (bolean == true)
        {
          bolean = false;
        }
        lc.setLed(2, 2, 7, true);
        lc.setLed(2, 5, 7, true);
      }
      else
      {
        if (bolean == false)
        {
          bolean = true;
        }
        lc.setLed(2, 2, 7, false);
        lc.setLed(2, 5, 7, false);
      }
    }
    //Sekundenanzeige zurücksetzen
    if (rtc_zeit.sec == 59)
    {
      delay(660);
      for (j = 0; j < 4 ; j++)
      {
        for (i = 7; i >= 0; i--)
        {
          delay(10);
          lc.setLed(j, 0, i, false);
          lc.setLed(j, 7, i, false);
        }
      }
      berechnen();
      //Uhrzeit anzeigen
      delayausgabe(0, zahl[min_LSB]);
      delayausgabe(1, zahl[min_MSB]);

      delayausgabe(2, zahl[stund_LSB]);
      delayausgabe(3, zahl[stund_MSB]);
    }
    zwischenzeit = rtc_zeit.sec;
  }
}
