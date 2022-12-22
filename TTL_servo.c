/************************************************************************
 * 
 * This firmware detect a voltage change on certain pin ( here is PB4).
 * Then set pulse width of two PWM waveforms according to the state 
 * of the pin. 
 *
 * The pulse width change could be potential used to control a servo
 * motor. 
 *
 * The voltage HIGH on this pin move servo to one position
 * voltage LOW move servo to another position perpendicular
 * to the first position. 
 *
 * License: CC BY-SA 4.0
 * Author: Dapeng Zhang
 *
 ************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#define INPUT_BIT 1 << PB4	// arduino digital pin 12, PCINT4
#define OUTPUT_BIT 1 << PB2	// arduino digital pin 10, OC1B
#define LED 1 << PB5		// arduino digital pin 13
void init_timer1(void);
void init_timer2(void);
unsigned short per_par = 40000;
unsigned short ini_par = 2000;

int main(void)
{
	DDRB &= ~INPUT_BIT;	// set INPUT_BIT as input
	DDRB |= LED;		// set LED output
	PORTB |= INPUT_BIT;	// enable pull-up resistor
	PCICR |= (1 << PCIE0);	// enable pin change interrupt for PCINT7..0
	PCMSK0 |= (1 << PCINT4);// set PCINT4 enable
	init_timer1();
	init_timer2();
	sei();		// enable interrupts

	while(1){
	}
	return 0;
}
ISR(PCINT0_vect)
{
	if ( PINB & INPUT_BIT ){
		OCR2A = 35;
		OCR1B = 4000;
		PORTB |= LED; 	// LED indicator on
	}
	else{
		OCR2A = 12;
		OCR1B = 2000;
		PORTB &= ~LED;	// LED indicator off
	}
}
void init_timer1(void)
{
	// prescaler = 8, clock = 2000Khz, count = 40K, freq = 50 hz
	// 0.75ms = 1500 count and 2.25ms = 4500 count

	// Set to Fast PWM mode, mode 15 of timer 1, different timer has different modes
	TCCR1A |= (0b11 << WGM10); 	// set the 1:0 bit of TCCR1A
	TCCR1B |= (0b11 << WGM12); 	// set the 4:3 bit of TCCR1B

	TCCR1A |= (0b10 << COM1B0); 	// set D10 at 0x00, clear it at OCR1A

	// Set pulse period and width count number
	OCR1A = per_par;		// set pulse period to be 20 ms (50Hz)
	OCR1B = ini_par;		// initial pulse for 90 degree of servo

	TCCR1B |= (1 << CS11);		// Set prescalar = 8 and start counter

	DDRB |= OUTPUT_BIT;		// set OUTPUT_BIT as output for waveform output
}
void init_timer2(void)
{
	TCCR2A |= (0b11 << WGM20);	// fast PWM, mode 3 of timer 2
	TCCR2A |= (0b10 << COM2A0);	// set PB3 at 0x00, clear it at OCR2A
	TCCR2B |= (0b111 << CS20);	// set prescaler = 1024 and start timer
	DDRB |= (1 << PB3); 		// set output, arduino digital pin 11
}
