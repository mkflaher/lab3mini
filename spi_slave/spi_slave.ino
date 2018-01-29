//Sean Flaherty
//MSP430G2553 SPI code by D. Dang - https://gist.github.com/chrismeyersfsu/3326502
//SPI 4-pin slave mode - allows for slave-select pins for the master to send information to multiple slaves.
//Code is modified to change from 3- to 4-pin slave, as well as use UCSI_B0 instead of USCI_A0
//Each slave MCU should use excerpts of this program.
//P1.4 - Slave select
//P1.5 - SCLK
//P1.6 - MISO (not doing anything in this example)
//P1.7 - MOSI
#include "msp430g2553.h"
#include <string.h>

char cmdbuf[20];
char cmd_index=0;

void flash_spi_detected(void) {
    int i=0;
    P1OUT = 0;
    for (i=0; i < 6; ++i) {
        P1OUT = ~P1OUT;
        __delay_cycles(0x4fff);
        __delay_cycles(0x4fff);
    }
}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
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
  UCB0CTL1 = UCSWRST;                       // **Put state machine in reset**
  UCB0CTL0 |= UCMSB + UCSYNC + UCCKPH + UCMODE_2;               // 4-pin, 8-bit SPI master
  UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

  __bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, enable interrupts
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
    /*
  if (UCB0STAT & UCOE) {
    P1OUT |= BIT0;
  }
  */
  char value = UCB0RXBUF;
  if (value == 'a'){
    P1OUT ^= BIT0;
  }
  else if(value == 'b'){
    //move servo left
  }
  if (value == '\n') {
    P1OUT ^= BIT0; //using this to test that slave receives character
    cmd_index = 0;
  }

}
