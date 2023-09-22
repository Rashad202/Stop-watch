/*
 * main.c
 *
 *  Created on: 23 Sep 2023
 *      Author: Rashad
 */



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



unsigned char seconds=0 , minutes=0 , hours=0;



void SevenSegment_init(void);
void SevenSegment_Display(void);
void Timer1_init(void);
void Reset (void);
void Pause(void);
void Resume (void);



int main ()
{
	SevenSegment_init();
	sei();
	Reset ();
	Pause();
	Resume ();
	Timer1_init();

	while (1)
	{
		SevenSegment_Display();
	}
}



void SevenSegment_init(void)
{
	//INITIALIZE FIRST 4-PINS IN PORTC
	DDRC |= 0x0F;
	//INITIALIZE FIRST 6-PINS IN PORTA
	DDRA |= 0x3F;
	//TURN OFF PORTC
	PORTC =0;
}



void Timer1_init(void)
{
	TCNT1 = 0;

	/* TICK TIME = 1024us
	 * WE WANT 1 SEC
	 * 1/1024 =976 , 976 IS COMP VALUE
	 */

	OCR1A = 976; // COMP VALUE



	TCCR1A |= (1<<FOC1A);
	TCCR1B |= (1<<CS10) | (1<<CS12) | (1<<WGM12);
	TIMSK |= (1<<OCIE1A);
}



void Reset (void)
{
	DDRD &= ~(1<<PD2);
	PORTD |= (1<<PD2);
	GICR |= (1<<INT0);
	MCUCR |= (1<<ISC01);
}




void Pause(void)
{
	GICR |= (1<<INT1);
	MCUCR |= (1<<ISC10) | (1<<ISC11);
}



void Resume (void)
{
	DDRB &= ~(1<<PB2);
	PORTB |= (1<<PB2);
	GICR |= (1<<INT2);
	MCUCSR &= ~(1<<ISC2);
}



void SevenSegment_Display(void)
{
	// DISPLAY FIRST DIGIT IN SECONDS
	PORTA = (1<<PA0);
	PORTC = seconds%10;

	_delay_ms(1);

	// DISPLAY SECOND DIGIT IN SECONDS
	PORTA = (1<<PA1);
	PORTC = seconds/10;

	_delay_ms(1);

	// DISPLAY FIRST DIGIT IN MINUTES
	PORTA = (1<<PA2);
	PORTC = minutes%10;

	_delay_ms(1);

	// DISPLAY SECOND DIGIT IN MINUTES
	PORTA = (1<<PA3);
	PORTC = minutes/10;

	_delay_ms(1);

	// DISPLAY FIRST DIGIT IN HOURS
	PORTA = (1<<PA4);
	PORTC = hours%10;

	_delay_ms(1);

	// DISPLAY SECOND DIGIT IN H
	PORTA = (1<<PA5);
	PORTC = hours/10;

	_delay_ms(1);
}



ISR (TIMER1_COMPA_vect)
{
	seconds++;

	if (seconds == 60)
	{
		minutes++;
		seconds=0;
	}

	if (minutes == 60)
	{
		hours++;
		minutes=0;
	}
}



// RESET BUTTOM
ISR (INT0_vect)
{
	hours = 0;
	minutes = 0;
	seconds = 0;
}


// PAUSE BUTTOM
ISR (INT1_vect)
{
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS12);
}



// RESUME BUTTOM
ISR (INT2_vect)
{
	TCCR1B |= (1 << CS10) | (1 << CS12);
}
