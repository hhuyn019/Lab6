/*	Author: hhuyn019
 *  Partner(s) Name: Badr Asi
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include <avr/interrupt.h>
#endif

volatile unsigned char TimerFlag = 0;

//Internal Variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0; //Current internal count of 1ms ticks

void TimerOn() {
	TCCR1B = 0x0B; //bit3=0: CTC mode (clear timer on compare)
	// AVR output compare register OCR1A
	OCR1A = 125; //Timer interrupt will be generated when TCNT1 == OCR1A
	//AVR timer interrupt mask register
	TIMSK1 = 0x02; //bit1: OCIE1A -- enables compare match interrupt
	//initialize avr counter

	TCNT1 = 0;

	_avr_timer_cntcurr= _avr_timer_M;
	//TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // ox80: 10000000
}

void TimerOff() {
	TCCR1B = 0x00; //bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

//In our approach, the C programmer does not touch this ISR, but rather TimerISR ()
ISR(TIMER1_COMPA_vect) {
	//CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; //Count down to 0 rather than up to TOP
	if(_avr_timer_cntcurr == 0) { //results in a more efficient compare
		TimerISR(); //Call th ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

//Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}




void main() {
   	DDRC = 0xFF;
	PORTC = 0x00;
	TimerSet(125);
	TimerOn();
	unsigned char tmpB = 0x00;
	while(1) {
		tmpB = ~tmpB; 
		PORTC = tmpB;
		while (!TimerFlag);
		TimerFlag = 0;
	}
}
