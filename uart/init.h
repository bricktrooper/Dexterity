#ifndef INIT_H
#define INIT_H

#include <xc.h>
#include <pic16f690.h>

#pragma config FOSC = INTRCIO
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF

void init_hardware(void);
void startup_indicator(void);

#endif /* INIT_H */
