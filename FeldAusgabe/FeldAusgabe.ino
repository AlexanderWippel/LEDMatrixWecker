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

boolean doppelpunktBoolean = true;
boolean ersterWeckerDurchlauf = true;
boolean tasteKurzGedrueckt = false;
boolean tasteLangeGedrueckt = false;

//Wecker Menü
boolean weckerzustand = false;

//Für schleifen
int x = 0;
int i = 0;
int j = 0;

//Display Aufteilung
int min_LSB = 0;
int min_MSB = 0;

int stund_LSB = 0;
int stund_MSB = 0;

int temp_LSB = 0;
int temp_MSB = 0;


//Zeichen für LED-Matrix

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

const long long int wecker_aus[] = {0xc0201090909020c0, 0x0304080b08080403};

const long long int wecker_ein[] =
{
  0x0000804040408000,
  0xc0241292929224c0,
  0x0324484b48482403,
  0x0001020202010000
};

const long long int weckerWeckzeitEinstellen[] =
{
  0x000c1ef0f01e0c00,
  0xc0241292929224c0,
  0x0324484b48482403,
  0x0030780f0f783000
};

const long long int weckerUhrzeitEinstellen[] =
{
  0x000c1ef0f01e0c00,
  0xc0201097979020c0,
  0x030408ebe8080403,
  0x0030780f0f783000
};

DS3231 rtc(SDA_PIN, SCL_PIN);

LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MAXDEVICES);

boolean ledtest = LOW;

void setup()
{

  CLKPR = 0x80; //ClockChangeEnable
  CLKPR = 0x00; //CLK=16Mhz

  //Input und Outputbechaltung
  DDRB = DDRB | (1 << 5);
  DDRB = DDRB & ~(1 << 0);

  PORTB = PORTB | (1 << 0);
  PORTB = PORTB & ~(1 << 5);

  DDRD &= ~(1 << 2);
  PORTD |= (1 << 2);

  DDRD &= ~(1 << 3);
  PORTD |= (1 << 3);

  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, ledtest);

  //Externen Interrupt am PIN3(PD3) wenn eine fallende Flanke auftritt
  EICRA = 0;
  EICRA |= (1 << 3);
  EIMSK = 0;
  EIMSK |= (1 << 1);

  TCCR1A = 0; //Register auf 0 setzen weil es sonst undefiniert währe
  TCCR1B = 0; //Register auf 0 setzen weil es sonst undefiniert währe
  TCNT1 = 0;  //Register auf 0 setzen weil es sonst undefiniert währe

  TCCR2A = 0; //Register auf 0 setzen weil es sonst undefiniert währe
  TCNT2 = 0;  //Register auf 0 setzen weil es sonst undefiniert währe
  TCCR2B = 0; //Register auf 0 setzen weil es sonst undefiniert währe
  TIMSK2 = 0; //Register auf 0 setzen weil es sonst undefiniert währe
  
//Timer 1 stopp
  TCCR1B = TCCR1B & ~(1 << 0); 
  TCCR1B = TCCR1B & ~(1 << 2);  
  TCCR1B = TCCR1B & ~(1 << 1);

  TCCR1A = TCCR1A & ~(1 << 7); //normaler Pin modus
  TCCR1A = TCCR1A & ~(1 << 6);

  TCCR1A = TCCR1A & ~(1 << 0); //CTC Modus
  TCCR1A = TCCR1A & ~(1 << 1);
  TCCR1B = TCCR1B | (1 << 3);
  TCCR1B = TCCR1B & ~(1 << 4);

  OCR1A = 15600; //Timer2-COMPA-Interrupt alle 1s
  OCR2A = 156;   //Timer2-COMPA-Interrupt alle 10ms

  TIMSK1 = TIMSK1 | (1 << 1); //Interrupt lokal aktiviert
  TIMSK2 = TIMSK2 | (1 << 1); //Timer2 COMPA-Interrupt enable
  SREG = SREG | (1 << 7);    //Interrupt global freigeschaltet


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

  if ((PIND | 0b11110111) == 0b11110111)
  {
    delay(15);
    if ((PIND | 0b11110111) == 0b11110111)
    {
      //TCCR1B = TCCR1B | (1 << 0); //Teiler auf 1024
      //TCCR1B = TCCR1B | (1 << 2);
      TCCR2B = 7; //Timer2 mit Takt-Teiler 1024 starten
    }
  }
}

ISR(TIMER1_COMPA_vect)
{
  Serial.println("im Timer 1");
  TCNT1 = 0;
  TCCR1B = TCCR1B & ~(1 << 0); //Timer 1 stopp
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

      TCNT1 = 0; //Zählerstand des Timer1 zurücksetzen
      TCNT2 = 0; //Zählerstand des Timer2 zurücksetzen

      TCCR2B = 0;//Timer2 stoppen
      tasteLangeGedrueckt = true;
    }
  }
  else
  {
    x = 0;

    TCNT1 = 0;  //Zählerstand des Timer1 zurücksetzen
    TCNT2 = 0;  //Zählerstand des Timer2 zurücksetzen

    TCCR2B = 0; //Timer2 stoppen

    //TCCR1B = TCCR1B | (1 << 0); //Teiler auf 1024
    //TCCR1B = TCCR1B | (1 << 2);

    tasteKurzGedrueckt = true;
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

void uhrzeitBerechnen()
{
  rtc_zeit = rtc.getTime();

  min_LSB = (rtc_zeit.min % 10);
  min_MSB = (rtc_zeit.min / 10) % 10;

  stund_LSB = (rtc_zeit.hour % 10);
  stund_MSB = (rtc_zeit.hour / 10) % 10;

  temp_LSB = (((int)rtc.getTemp()) % 10);
  temp_MSB = (((int)rtc.getTemp()) / 10) % 10;
}

void uhrzeitAnzeigen()
{
  ausgabe(0, zahl[min_LSB]);
  ausgabe(1, zahl[min_MSB]);

  ausgabe(2, zahl[stund_LSB]);
  ausgabe(3, zahl[stund_MSB]);
}

void temperaturAnzeigen()
{
  ausgabe(0, temperatur[1]);
  ausgabe(1, temperatur[0]);

  ausgabe(2, zahl[temp_LSB]);
  ausgabe(3, zahl[temp_MSB]);
}

void sekundenAnzeigen()
{
  if (rtc_zeit.sec <= 7 && rtc_zeit.sec >= 1)
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

void sekundenNachholen()
{
  int durchzaehlSekunde;

  for (durchzaehlSekunde = 1; durchzaehlSekunde <= rtc_zeit.sec; durchzaehlSekunde ++)
  {
    if (durchzaehlSekunde <= 7 && durchzaehlSekunde >= 1)
    {
      lc.setLed(3, 0, (durchzaehlSekunde % 8), true);
    }
    if (durchzaehlSekunde >= 8 && durchzaehlSekunde <= 15)
    {
      lc.setLed(2, 0, (durchzaehlSekunde % 8), true);
    }
    if (durchzaehlSekunde >= 16 && durchzaehlSekunde <= 23)
    {
      lc.setLed(1, 0, (durchzaehlSekunde % 8), true);
    }
    if (durchzaehlSekunde >= 24 && durchzaehlSekunde <= 30)
    {
      lc.setLed(0, 0, (durchzaehlSekunde % 8), true);
    }


    if (durchzaehlSekunde >= 31 && durchzaehlSekunde <= 37)
    {
      lc.setLed(3, 7, ((durchzaehlSekunde - 30) % 8), true);
    }
    if (durchzaehlSekunde >= 38 && durchzaehlSekunde <= 45)
    {
      lc.setLed(2, 7, ((durchzaehlSekunde - 30) % 8), true);
    }
    if (durchzaehlSekunde >= 46 && durchzaehlSekunde <= 53)
    {
      lc.setLed(1, 7, ((durchzaehlSekunde - 30) % 8), true);
    }
    if (durchzaehlSekunde >= 54 && durchzaehlSekunde <= 60)
    {
      lc.setLed(0, 7, ((durchzaehlSekunde - 30) % 8), true);
    }

  }

}

void sekundenanzeigeZuruecksetzen()
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
  uhrzeitBerechnen();
  uhrzeitAnzeigen();
}

void doppelpunktAnzeigen()
{
  if (rtc_zeit.sec % 2 == 1)
  {
    if (doppelpunktBoolean == true)
    {
      doppelpunktBoolean = false;
    }
    lc.setLed(2, 2, 7, true);
    lc.setLed(2, 5, 7, true);
  }

  else
  {
    if (doppelpunktBoolean == false)
    {
      doppelpunktBoolean = true;
    }
    lc.setLed(2, 2, 7, false);
    lc.setLed(2, 5, 7, false);
  }
}

void kurzerTastendruck()
{
  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  weckerzustand ^= true;            //Wecker ein/aus

  if (weckerzustand == true) {      //Weckersymbol ein
    ganzausgabe(3, wecker_ein[0]);
    ganzausgabe(2, wecker_ein[1]);
    ganzausgabe(1, wecker_ein[2]);
    ganzausgabe(0, wecker_ein[3]);
  } else {                          //Weckersymbol aus
    ganzausgabe(2, wecker_aus[0]);
    ganzausgabe(1, wecker_aus[1]);
  }

  delay(2000);

  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  //Sofortige überschribung des geclearten Displays mit der Temp/Uhranzeige
  rtc_zeit = rtc.getTime();
  if (rtc_zeit.sec >= 10 && rtc_zeit.sec <= 13 || rtc_zeit.sec >= 30 && rtc_zeit.sec <= 33 || rtc_zeit.sec >= 50 && rtc_zeit.sec <= 53)
  {
    lc.setLed(2, 2, 7, false);
    lc.setLed(2, 5, 7, false);

    temperaturAnzeigen();
  }
  else
  {
    uhrzeitAnzeigen();
  }

  sekundenNachholen();      //Alle Sekunden neu darstellen
  tasteKurzGedrueckt = false;
}

void langerTastendruck()
{
  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  ganzausgabe(3, weckerWeckzeitEinstellen[0]);
  ganzausgabe(2, weckerWeckzeitEinstellen[1]);
  ganzausgabe(1, weckerWeckzeitEinstellen[2]);
  ganzausgabe(0, weckerWeckzeitEinstellen[3]);

  delay(2000);

  lc.clearDisplay(3);
  lc.clearDisplay(2);
  lc.clearDisplay(1);
  lc.clearDisplay(0);

  //Sofortige überschribung des geclearten Displays mit der Temp/Uhranzeige
  rtc_zeit = rtc.getTime();
  if (rtc_zeit.sec >= 10 && rtc_zeit.sec <= 13 || rtc_zeit.sec >= 30 && rtc_zeit.sec <= 33 || rtc_zeit.sec >= 50 && rtc_zeit.sec <= 53)
  {
    lc.setLed(2, 2, 7, false);
    lc.setLed(2, 5, 7, false);

    temperaturAnzeigen();
  }
  else
  {
    uhrzeitAnzeigen();
  }

  sekundenNachholen();      //Alle Sekunden neu darstellen
  tasteLangeGedrueckt = false;
}

void loop()
{
  if (tasteKurzGedrueckt == true) //In der Funktion kurzerTastendruck sind delays. Deswegen wird die Funktion im main ausgeführt
  {
    kurzerTastendruck();
  }

  if (tasteLangeGedrueckt == true)
  {
    langerTastendruck();
  }

  rtc_zeit = rtc.getTime();

  while (rtc_zeit.sec != zwischenzeit)  //Bei einer änderung der Zeit wird while ausgeführt
  {
    uhrzeitBerechnen();

    if (ersterWeckerDurchlauf == true)
    {
      uhrzeitAnzeigen();
      ersterWeckerDurchlauf = false;
    }

    sekundenAnzeigen();

    if (rtc_zeit.sec == 14 || rtc_zeit.sec == 34 || rtc_zeit.sec == 54)   //Temperauranzeige überdecken
    {
      uhrzeitAnzeigen();
    }
    else if (rtc_zeit.sec >= 10 && rtc_zeit.sec <= 13 || rtc_zeit.sec >= 30 && rtc_zeit.sec <= 33 || rtc_zeit.sec >= 50 && rtc_zeit.sec <= 53)
    {
      lc.setLed(2, 2, 7, false);
      lc.setLed(2, 5, 7, false);

      temperaturAnzeigen();
    }
    else
    {
      doppelpunktAnzeigen();
    }

    if (rtc_zeit.sec == 59)
    {
      sekundenanzeigeZuruecksetzen();
    }
    zwischenzeit = rtc_zeit.sec;
  }
}
