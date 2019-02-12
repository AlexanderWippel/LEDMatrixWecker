

void setup() 
{

 DDRB=DDRB|(1<<5);
 DDRB=DDRB&~(1<<0);

 PORTB=PORTB|(1<<0);
 PORTB=PORTB|(1<<5);

}
int x=254;

void loop() 
{
  if((PINB|0b11111110) == 0b11111110)
  {
    delay(25);
    if((PINB|0b11111110) == 0b11111110)
    {
      PORTB=PORTB|(1<<5);
    }
  }
  else
  {
    PORTB=PORTB&~(1<<5);
  }
}
