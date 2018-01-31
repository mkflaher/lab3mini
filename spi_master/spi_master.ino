//Code for master from Argenox tutorial
#include "msp430g2553.h"

volatile char received_ch = 0;
unsigned char data;
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT
	
	P1OUT |= BIT3 + BIT4; //chip selects
	P1DIR |= BIT3 + BIT4 + BIT0;
	
	P1SEL = BIT6 | BIT7 | BIT5; //P1.6: MISO; P1.7: MOSI; P1.5: CLK
	P1SEL2 = BIT6 | BIT7 | BIT5;

	__delay_cycles(2000000);
	DCOCTL = 0;
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
 
	UCB0CTL1 = UCSWRST;
	UCB0CTL0 |=  UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC; // 3-pin active-low slave, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 = 10; // /2
	UCB0BR1 = 0;
	UCA0MCTL = 0; // No modulation
	UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
	
	while(1){
		__delay_cycles(1000000);
	UCB0TXBUF = 'a';
		//while (!(IFG2 & UCB0TXIFG)); // USCI_A0 TX buffer ready?
 	P1OUT ^= BIT0;
	}
}
