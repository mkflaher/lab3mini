//Code for master from Argenox tutorial
#include "msp430g2553.h"

volatile char received_ch = 0;
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT

    P1OUT |= BIT3 + BIT4; //chip selects
    P1DIR |= BIT3 + BIT4;

    P1SEL = BIT6 | BIT7 | BIT5; //P1.6: MISO; P1.7: MOSI; P1.5: CLK
    P1SEL2 = BIT6 | BIT7 | BIT5;

    UCB0CTL1 = UCSWRST;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCMODE_2 + UCSYNC; // 4-pin active-low slave, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0BR0 |= 0x02; // /2
    UCB0BR1 = 0;
    //UCB0MCTL = 0; // No modulation
    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

    P1OUT &= (~BIT4); // Select Device

    while (!(IFG2 & UCA0TXIFG)); // USCI_A0 TX buffer ready?
    UCB0TXBUF = 'a'; // Send 0xAA over SPI to Slave

    P1OUT |= (BIT4); // Unselect Device
}
