#ifndef INIT_H
#define INIT_H

#pragma config FOSC = INTRCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

#define PIC_TEST_BOARD

void init_hardware(void);

#ifdef PIC_TEST_BOARD
void startup_indicator(void);
#endif

#endif /* INIT_H */
