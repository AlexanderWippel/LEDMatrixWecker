void setup() 
{
  CLKPR=0x80;
  CLKPR=0x00;
  DDRB=DDRB|(1<<5);
  DDRB=DDRB&~(1<<0);

  PORTB=PORTB|(1<<0);
  PORTB=PORTB&~(1<<5);

  TCCR1A=0;
  TCCR1B=0;

  TCCR1B=TCCR1B&~(1<<0);  //stopp
  TCCR1B=TCCR1B&~(1<<2);
  TCCR1B=TCCR1B&~(1<<1);

  TCCR1A=TCCR1A&~(1<<7); //normaler Pin modus
  TCCR1A=TCCR1A&~(1<<6);

  TCCR1A=TCCR1A&~(1<<0);  //CTC Modus
  TCCR1A=TCCR1A&~(1<<1);
  TCCR1B=TCCR1B|(1<<3);
  TCCR1B=TCCR1B&~(1<<4);

  OCR1A=156;  //OCR1A gesetzt
  
  TIMSK1=TIMSK1|(1<<1); //Interrupt lokal aktiviert
  SREG=SREG|(1<<7);     //Interrupt global freigeschaltet
}

int x=0;

ISR(TIMER1_COMPA_vect)
{
     if((PINB|0b11111110) == 0b11111110)
     {
      x++;
      if(x==100)
     {
      PORTB=PORTB^(1<<5);
      
      TCNT1=0;
      
      
      TCCR1B=TCCR1B&~(1<<0); //stopp
      TCCR1B=TCCR1B&~(1<<2);
     }
    }
    else
    {
      x=0;
      TCNT1=0;
      
      TCCR1B=TCCR1B&~(1<<0); //stopp
      TCCR1B=TCCR1B&~(1<<2);
    }  
}

void loop() 
{
  if((PINB|0b11111110) == 0b11111110)
  {
    delay(20);
    if((PINB|0b11111110) == 0b11111110)
    {
      TCCR1B=TCCR1B|(1<<0); //Teiler auf 1024
      TCCR1B=TCCR1B|(1<<2);
    }
  }
}
