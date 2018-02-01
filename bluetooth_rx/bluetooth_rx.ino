//Sean Flaherty
//ECE 3334
//Master MCU receiving UART data from bluetooth and transmitting I2C to two slave MCUs.
//Program based on UART transmitter/receiver code by Enrico Garante on EmbeddedRelated.com
#include "msp430g2553.h"
#include <string.h>
 
#define TXD BIT2
#define RXD BIT1
#define MOSI BIT7
#define MISO BIT6
#define SCLK BIT5

//in this program we don't need MISO or slave select - just clock and 
 
const char string[] = { "$$$" };
char rx[200] = {""};
unsigned int i; // tx counter
unsigned int j; // rx counter

void SPI_TX(void){
	
	for(int x; x<=j; x++){
		UCB0TXBUF = rx[x]; //send string index by index
		__delay_cycles(100); // 100 khz divider => 10 us per bit => 80 us per byte
		//best practice is to ensure UCB0TXIFG is ready
	}
}

int main(void){
	WDTCTL = WDTPW + WDTHOLD; // Stop WDT
	
	DCOCTL = 0; // Select lowest DCOx and MODx settings
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
	DCOCTL = CALDCO_1MHZ;
	
	i = 0;
	
	P2DIR |= 0xFF; // All P2.x outputs
	P2OUT &= 0x00; // All P2.x reset

	P1DIR |= BIT0; //red LED for testing purposes
	P1SEL |= RXD + TXD + MOSI + MISO + SCLK; // setup pins for UART and SPI
	P1SEL2 |= RXD + TXD + MOSI + MISO + SCLK; // "" ""
	P1OUT &= 0x00;

	//UART setup
	UCA0CTL1 |= UCSSEL_2; // SMCLK
	UCA0BR0 = 0x08; // 1MHz 115200
	UCA0BR1 = 0x00; // 1MHz 115200
	UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5
	UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

	//SPI setup
	UCB0CTL1 = UCSWRST;
	UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC; // 3-pin active-low slave, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2; // SMCLK
	UCB0BR0 = 10; //100khz divider
	UCB0BR1 = 0; //second byte of divider (0)
	UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

	UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	__bis_SR_register(GIE); // Enter LPM0 w/ int until Byte RXed

	while (1)
   	{ }
}

//we're only RXing over uart, so no need for this tx vector
/*#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
	UCA0TXBUF = string[i++]; // TX next character 
	if (i == sizeof(string) - 1){ // TX over? 
		UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt 
		__delay_cycles(500000);
	}
} */
  
#pragma vector=USCIAB0RX_VECTOR 
__interrupt void USCI0RX_ISR(void) 
{ 
	if (UCA0RXBUF != '\r') // not carriage return?
	{
		P1OUT ^= BIT0;
		rx[j] = UCA0RXBUF;
		j++;
	}
	else //carriage return
	{
	//P1OUT |= BIT0; //test cr received
		UCA0MCTL = 0; //turn off modulation for SPI
		SPI_TX(); //send the string before clearing
	//P1OUT |= BIT0; //test SPI transmitted
		j = 0;
		for(int x=0; x<200;x++){
			rx[x]='\0'; //clear string
		}
		UCA0MCTL = UCBRS2 + UCBRS0; //turn modulation back on when done
	}
}
