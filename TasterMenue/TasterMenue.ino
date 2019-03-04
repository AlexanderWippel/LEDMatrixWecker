#define OKeyKurz    0
#define OKeyLange   1
#define plusKey     2
#define minusKey    3

#define weckerOff   false
#define weckerOn    true

const long long int uhr = {0x3c4281b98989423c};

const long long int weckwellenrechtslinks[]=
{
  0x0012242424140200,
  0x0040282424244800
};

void setup() {


}
 
int eingabe = OKeyLange;
boolean weckerZustand = false;

void loop() {

  //Warte auf Eingabe - Weckerzustand

  switch (eingabe) {
    case OKeyKurz:  //Wecker ein- oder ausschalten

      switch (weckerZustand) {
        case weckerOn:
          weckerZustand = false;
          ausgabe(1, uhr);
          break;

        case weckerOff:
          weckerZustand = true;
          ausgabe(1, weckwellenrechtslinks[0]);
          ausgabe(1, weckwellenrechtslinks[1]);
          ausgabe(1, uhr);
          break;
      }
      break;

    case OKeyLange: //Weckzeit einstellen
      //Weckzeit einstellen Symbol
      //beginn Stunden blinken
      switch (eingabe) {
        case plusKey:
          //Stunde + 1
          break;

        case minusKey:
          //Stunde - 1
          break;

        case OKeyKurz:
          //ende Stunden blinken
          //beginn Minuten blinken

          switch (eingabe) {
            case plusKey:
              //Minute + 1
              break;

            case minusKey:
              //Minute -1
              break;

            case OKeyKurz:
              //zurück zum start
              break;
          }

          break;

        case OKeyLange:
          switch (eingabe) {
            case plusKey:
              //Stunde + 1
              break;

            case minusKey:
              //Stunde - 1
              break;

            case OKeyKurz:
              //ende Stunden blinken
              //beginn Minuten blinken

              switch (eingabe) {
                case plusKey:
                  //Minute + 1
                  break;

                case minusKey:
                  //Minute -1
                  break;

                case OKeyKurz:
                  //zurück zum start
                  break;
                case OKeyLange:
                  break;
              }
              break;
          }
          break;
      }
      break;
  }

}
