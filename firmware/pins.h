#ifndef PINS_H
#define PINS_H

// STATUS LED //

#define LED        RA4
#define LED_TRIS   TRISA4

// RESET BUTTON //

#define BUTTON        RA5
#define BUTTON_TRIS   TRISA5

// ACCELEROMETER //

#define X_TRIS   TRISC3
#define Y_TRIS   TRISC6
#define Z_TRIS   TRISC7

#define X_ANSEL   ANS7
#define Y_ANSEL   ANS8
#define Z_ANSEL   ANS9

#define X_CHANNEL   7
#define Y_CHANNEL   8
#define Z_CHANNEL   9

// FLEX SENSORS //

#define F1_TRIS    TRISA2
#define F2_TRIS    TRISC0
#define F3_TRIS    TRISC1
#define F4_TRIS    TRISC2
#define F5_TRIS    TRISB4

#define F1_ANSEL   ANS2
#define F2_ANSEL   ANS4
#define F3_ANSEL   ANS5
#define F4_ANSEL   ANS6
#define F5_ANSEL   ANS10

#define F1_CHANNEL   2
#define F2_CHANNEL   4
#define F3_CHANNEL   5
#define F4_CHANNEL   6
#define F5_CHANNEL   10

#endif /* PINS_H */
