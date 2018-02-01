#include "msp430g2553.h"
#include <string.h>

/**
 * main.c
 */

void flash_spi_detected(void) {
	int i=0;
	P1OUT = 0;
	for (i=0; i < 6; ++i) {
		P1OUT = ~P1OUT;
		__delay_cycles(0x4fff);
		__delay_cycles(0x4fff);
	}
}

unsigned volatile int delay_a = 1000000000;
unsigned volatile int delay_b = 1000000000;
unsigned int counter_a = 0;
unsigned int counter_b = 0;
volatile int j; //prevent optimization
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
  
	/* led */
	//P1DIR |= BIT0 + BIT5;
	P1DIR |= BIT0;
	__delay_cycles(1000000); //delay to ensure slave is ready to accept from master
	while (P1IN & BIT5);                   // If clock sig from mstr stays low,
	__delay_cycles(1000000);
	                                          // it is not yet in SPI mode
	
	flash_spi_detected();                 // Blink 3 times
	   
	P1SEL = BIT4 + BIT5 + BIT6 + BIT7;
	P1SEL2 = BIT4 + BIT5 + BIT6 + BIT7;
	UCB0CTL1 = UCSWRST;                             //Put state machine in reset
	UCB0CTL0 |= UCCKPH + UCMSB + UCSYNC + UCMODE_0; //3-pin, 8-bit SPI slave
	UCB0CTL1 &= ~UCSWRST;                           //Initialize USCI0 RX interrupt
	IE2 |= UCB0RXIE;
	
	__bis_SR_register(GIE);       // enable interrupts
	
	P2DIR |= BIT0 + BIT1;
	P2OUT |= BIT0 + BIT1;

	//LED delay
	for(;;){
		counter_a++;
		counter_b++;
		if(counter_a==delay_a){
			P2OUT ^= BIT0; //toggle LED 1 and reset counter
		  	counter_a = 0; 
		}
		if(counter_b == delay_b){
		 	P2OUT ^= BIT1; //toggle LED 2 and reset counter
		 	counter_b = 0;
		}
	}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	char value = UCB0RXBUF;
	if(value == 'c'){
		P1OUT ^= BIT0;
		delay_a /= 2; //double speed LED 1
	}
	else if(value == 'd'){
		delay_a *= 2; //half speed LED 1
	}
	else if(value == 'e'){
		 delay_b /= 2; //double speed LED 2
	}
	else if(value == 'f'){ //half speed LED 2
		delay_b *= 2;
	}
}
