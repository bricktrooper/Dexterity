#include <xc.h>
#include <pic16f690.h>

#include "pins.h"
#include "init.h"

#ifdef PIC_TEST_BOARD
#define OSC_TUNE    0b00000   // factory-calibrated frequency (original PIC)
#else
#define OSC_TUNE    0b10000   // minimum frequency (new PICs)
#endif

#define LED_DELAY     75000

void init_hardware(void)
{
	// ANALOGUE (1) / DIGITAL (0) //

	ANSEL  = 0b00000000;   // AN7, AN6, AN5, AN4, AN3,  AN2,  AN1, AN0
	ANSELH = 0b00000000;   // N/C, N/C, N/C, N/C, AN11, AN10, AN9, AN8

	// INPUT (1) / OUTPUT (0) //

	TRISA = 0b00000000;    // N/C, N/C, RA5, RA4, RA3,  RA2,  RA1, RA0
	TRISB = 0b00000000;    // RB7, RB6, RB5, RB4, N/C,  N/C,  N/C, N/C
	TRISC = 0b00000000;    // RC7, RC6, RC5, RC4, RC3,  RC2,  RC1, RC0

	// INITIALIZE IO REGISTERS //

	PORTA = 0b00000000;
	PORTB = 0b00000000;
	PORTC = 0b00000000;

	// OSCILLATOR FREQUENCY //

	OSCCONbits.IRCF = 0b110;      // Fosc = 4MHz (~3.6864MHz)
	OSCCONbits.SCS = 1;           // use internal oscillator as system clock
	OSCTUNEbits.TUN = OSC_TUNE;   // tune internal oscillator frequency

#ifdef PIC_TEST_BOARD
	TRISAbits.TRISA4 = 1;   // RA4 button input
	TRISAbits.TRISA5 = 1;   // RA5 button input
#else
	TRISAbits.TRISA4 = 0;   // RA4 status LED
	TRISAbits.TRISA5 = 1;   // RA5 reset button
#endif

	init_indicator();       // flash status LEDs
}

void init_indicator(void)
{
#ifdef PIC_TEST_BOARD
	unsigned char LEDs = 0x0;
	PORTC = 0x0;

	for (int i = 0; i < 8; i++)
	{
		LEDs |= 0x1 << i;
		PORTC = LEDs;
		_delay(LED_DELAY);
	}

	for (int i = 0; i < 3; i++)
	{
		PORTC = 0x0;
		_delay(LED_DELAY);
		PORTC = 0xFF;
		_delay(LED_DELAY);
	}

	PORTC = 0x0;
#else
	for (int i = 0; i < 3; i++)
	{
		LED = 1;
		_delay(LED_DELAY);
		LED = 0;
		_delay(LED_DELAY);
	}
#endif
}
