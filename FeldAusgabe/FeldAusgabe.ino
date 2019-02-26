#include <LedControl.h>
#include <DS3231.h>

#define CLK_PIN 13
#define DIN_PIN 12
#define CS_PIN 11
#define MAXDEVICES 4

#define SDA_PIN A0
#define SCL_PIN A1

Time rtc_zeit;
int zwischenzeit;

boolean bolean = true;
boolean einmal = true;
boolean isr = false;

int x = 0;
int i = 0;
int j = 0;

int min_LSB = 0;
int min_MSB = 0;

int stund_LSB = 0;
int stund_MSB = 0;

int temp_LSB = 0;
int temp_MSB = 0;

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
//define Menüzeichen aus uhr feld machen
const long long int wecker_aus[] = {0xc0201090909020c0, 0x0304080b08080403};

const long long int wecker_ein[] =
{
  0x0000804040408000,
  0xc0241292929224c0,
  0x0324484b48482403,
  0x0001020202010000
};

DS3231 rtc(SDA_PIN, SCL_PIN);

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MAXDEVICES);

boolean ledtest = LOW;

void setup()
{

  CLKPR = 0x80;
  CLKPR = 0x00;

  DDRB = DDRB | (1 << 5);
  DDRB = DDRB & ~(1 << 0);

  PORTB = PORTB | (1 << 0);
  PORTB = PORTB & ~(1 << 5);

  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, ledtest);

  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  DDRD &= ~(1 << 3);
  PORTD |= (1 << 3);

  EICRA = 0;
  EICRA|=(1<<3);
  EIMSK = 0;    //Externen Interrupt am PIN3(PD3) wenn eine fallende Flanke auftritt
  EIMSK |= (1 << 1);

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR2A = 0;
  TCNT2 = 0;
  TCCR2B = 0;


  TIMSK2 = 0;

  SMCR = 0;
  SMCR |= (1 << 0);

  TCCR1B = TCCR1B & ~(1 << 0); //stopp
  TCCR1B = TCCR1B & ~(1 << 2);
  TCCR1B = TCCR1B & ~(1 << 1);

  TCCR1A = TCCR1A & ~(1 << 7); //normaler Pin modus
  TCCR1A = TCCR1A & ~(1 << 6);

  TCCR1A = TCCR1A & ~(1 << 0); //CTC Modus
  TCCR1A = TCCR1A & ~(1 << 1);
  TCCR1B = TCCR1B | (1 << 3);
  TCCR1B = TCCR1B & ~(1 << 4);

  OCR1A = 15600; //OCR1A gesetzt
  OCR2A = 156;

  TIMSK1 = TIMSK1 | (1 << 1); //Interrupt lokal aktiviert
  TIMSK2 = TIMSK2 | (1 << 1);
  SREG = SREG | (1 << 7); //Interrupt global freigeschaltet


  rtc.begin();
  rtc.setTime(12, 10, 0);

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


ISR(INT1_vect)  //endlos drinnen wenn taster gedrückt bleibt
{
  Serial.println("im INT1");
  SMCR = 0;

  if ((PIND | 0b11110111) == 0b11110111)
  {
    delay(15);
    if ((PIND | 0b11110111) == 0b11110111)
    {
      //TCCR1B = TCCR1B | (1 << 0); //Teiler auf 1024
      //TCCR1B = TCCR1B | (1 << 2);
      TCCR2B = 7;
    }
  }
}

ISR(TIMER1_COMPA_vect)
{
  Serial.println("im Timer 1");
  TCNT1 = 0;
  TCCR1B = TCCR1B & ~(1 << 0); //stopp
  TCCR1B = TCCR1B & ~(1 << 2);
  TCCR1B = TCCR1B & ~(1 << 1);

};


ISR(TIMER2_COMPA_vect)  
{
  Serial.println("im Timer 2");
  if ((PIND | 0b11110111) == 0b11110111)
  {
    x++;
    if (x == 100)
    {
      PORTB = PORTB ^ (1 << 5);

      TCNT1 = 0;
      TCNT2 = 0;

      TCCR2B = 0;
    }
  }
  else
  {
    x = 0;

    TCNT1 = 0;
    TCNT2 = 0;

    TCCR2B = 0;

    //TCCR1B = TCCR1B | (1 << 0); //Teiler auf 1024
    //TCCR1B = TCCR1B | (1 << 2);

    isr = true;
  }
};



void ganzausgabe(int device, long long int zahlen)
{
  for (int i = 0; i < 8; i++)
  {
    byte row = (zahlen >> i * 8) & 0xff;
    for (int j = 0; j < 8; j++)
    {
      lc.setLed(device, i, j, bitRead(row, j));
    }
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

void berechnen()
{
  rtc_zeit = rtc.getTime();

  min_LSB = (rtc_zeit.min % 10);
  min_MSB = (rtc_zeit.min / 10) % 10;

  stund_LSB = (rtc_zeit.hour % 10);
  stund_MSB = (rtc_zeit.hour / 10) % 10;

  temp_LSB = (((int)rtc.getTemp()) % 10);
  temp_MSB = (((int)rtc.getTemp()) / 10) % 10;
}

void uhrzeitanzeigen()
{
  ausgabe(0, zahl[min_LSB]);
  ausgabe(1, zahl[min_MSB]);

  ausgabe(2, zahl[stund_LSB]);
  ausgabe(3, zahl[stund_MSB]);
}

void temperaturanzeigen()
{
  ausgabe(0, temperatur[1]);
  ausgabe(1, temperatur[0]);

  ausgabe(2, zahl[temp_LSB]);
  ausgabe(3, zahl[temp_MSB]);
}

void sekundenanzeigen()
{
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
}

void sekundennachholen()
{
  int sek = 0;
  int y = 0;
  int anzeige = 3;
  rtc_zeit = rtc.getTime();

  sek = rtc_zeit.sec;

 
}

void sekundenanzeige_zuruecksetzen()
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
  uhrzeitanzeigen();
}

void doppelpunkt_anzeigen()
{
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

void kurzertastendruck()
{
  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  ganzausgabe(3, wecker_ein[0]);
  ganzausgabe(2, wecker_ein[1]);
  ganzausgabe(1, wecker_ein[2]);
  ganzausgabe(0, wecker_ein[3]);

  delay(2000);

  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  uhrzeitanzeigen();

  isr = false;
}

void loop()
{
  if (isr == true)
  {
    kurzertastendruck();
    sekundennachholen();
  }

  rtc_zeit = rtc.getTime();

  while (rtc_zeit.sec != zwischenzeit)
  {
    berechnen();

    if (einmal == true)
    {
      uhrzeitanzeigen();
      einmal = false;
    }

    sekundenanzeigen();

    if (rtc_zeit.sec == 14 || rtc_zeit.sec == 34 || rtc_zeit.sec == 54)
    {
      uhrzeitanzeigen();
    }

    else if (rtc_zeit.sec >= 10 && rtc_zeit.sec <= 13 || rtc_zeit.sec >= 30 && rtc_zeit.sec <= 33 || rtc_zeit.sec >= 50 && rtc_zeit.sec <= 53)
    {
      lc.setLed(2, 2, 7, false);
      lc.setLed(2, 5, 7, false);

      temperaturanzeigen();
    }

    else
    {
      //Doppelpunkt anzeigen
      doppelpunkt_anzeigen();
    }
    //Sekundenanzeige zurücksetzen
    if (rtc_zeit.sec == 59)
    {
      sekundenanzeige_zuruecksetzen();
    }
    zwischenzeit = rtc_zeit.sec;
  }
}
