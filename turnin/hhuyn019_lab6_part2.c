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
#define Press (~PINA & 0x01)

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

enum States {Init, LED1, LED2, LED3, Wait, Reset} State;

int main(void) {
	DDRA = 0x00;
	PORTA = 0xFF;
   	DDRC = 0xFF;
	PORTC = 0x00;
	TimerSet(300);
	TimerOn();
	unsigned char button = 0x00;
	unsigned char tmpB = 0x00;
	unsigned char currPos = 0x01;
	State = Init;
	while(1) { 
		Tick();
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

void Tick() {
	switch(State) {
		case Init:
			PORTC = 0x00;
			State = LED1;
			break;
		case LED1:
			if(Press == 0x01) {
				State = Wait;
				break;
			} else {
				State = LED2;
				break;
			}
		case LED2:
			if(Press == 0x01) {
				State = Wait;
				break;
			} else {
				State = LED3;
				break;
			}
		case LED3:
			if(Press == 0x01) {
				State = Wait;
				break;
			} else {
				State = LED1;
				break;
			}
		case Wait:
			if(Press == 0x01) {
				State = Wait;
				break;
			} else {
				State = Reset;
				break;
			}
		case Reset:
			if(Press == 0x01) {
				State = LED1;
				break;
			} else {
				State = Reset;
				break;
			}
		default:
			break;
	}

	switch(State) {
		case Init:
			break;
		case LED1:
			PORTC = 0x01;
			break;
		case LED2:
			PORTC = 0x02;
			break;
		case LED3:
			PORTC = 0x04;
			break;
		case Wait:
			break;
		case Reset:
			break;
		default:
			break;
	}
}		

