#define OKeyKurz    0
#define OKeyLange   1
#define plusKey     2
#define minusKey    3

#define weckerOff   false
#define weckerOn    true

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
          //Wecker aus Symbol
          break;

        case weckerOff:
          weckerZustand = true;
          //Wecker ein Symbol
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
