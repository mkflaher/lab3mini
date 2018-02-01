//Dalton Scorgie + Sean Flaherty
//Servo Slave MSP430
//Receives signals from SPI master and decides whether to move a servo left or right, and drives lights on a LED bar to indicate position of servo.
#include "msp430g2553.h"
#include <string.h>

void flash_spi_detected(void){
	int j = 0;
	P1OUT = 0;
	for(j = 0; j < 6; ++j){
		P1OUT = ~P1OUT;
		__delay_cycles(0x4FFF);
		__delay_cycles(0x4FFF);
	}
}

#define MCU_CLOCK       1000000
#define PWM_FREQUENCY   46

#define SERVO_STEPS     180
#define SERVO_MIN       900
#define SERVO_MAX       2100

unsigned int PWM_PERIOD = (MCU_CLOCK/PWM_FREQUENCY);
unsigned int PWM_DUTY   = 0;

unsigned int servo_stepvalue, servo_stepnow;
unsigned int servo_lut[SERVO_STEPS];
unsigned int i;
unsigned int last;

unsigned int x = 90;
int main(void){
	WDTCTL| = WDTPW + WDTHOLD;   //stop watchdog timer
	
	servo_stepvalue = ((SERVO_MAX - SERVO_MIN)/SERVO_STEPS);
	servo_stepnow = SERVO_MIN;
	
	for(i = 0; i < SERVO_STEPS; i++){
		servo_stepnow += servo_stepvalue;
		servo_lut[i] = servo_stepnow;
	}
	
	//Setup the PWM, etc...
	
	/* led */
	//P1DIR |= BIT0 + BIT5;
	P1DIR |= BIT0;
	__delay_cycles(1000000); //delay to ensure slave is ready to accept from master
	while (P1IN & BIT5);                   // If clock sig from mstr stays low,
	__delay_cycles(1000000);

	flash_spi_detected();                 // Blink 3 times
	
	P1SEL = BIT4 + BIT5 + BIT6 + BIT7;
	P1SEL2 = BIT4 + BIT5 + BIT6 + BIT7;
	UCB0CTL1 = UCSWRST;                             //Put state machine in reset
	UCB0CTL0 |= UCCKPH + UCMSB + UCSYNC + UCMODE_0; //4-pin, 8-bit SPI master
	UCB0CTL1 &= ~UCSWRST;                           //Initialize USCI0 RX interrupt
	IE2 |= UCB0RXIE;
	
	P2DIR |= BIT0 +BIT1 + BIT2 + BIT3 + BIT4;     //set LED ports as output
	P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4);    //clear ports
	TA1CCTL1 = OUTMOD_7;         //TACCR1 reset/set
	TA1CTL = TASSEL_2 + MC_1;    //SMCLK, upmode
	TA1CCR0 = PWM_PERIOD+1;      //PWM period
	TA1CCR1 = servo_lut[x];          //TACCR1 PWM duty cycle
	P2SEL |= BIT1;              //P2.0 = TA1 output*/
	P2SEL2 &= ~BIT1;
	
	__bis_SR_register(GIE);             //Enter LPM4, enable interrupts
	
	//TA0CCR1 = servo_lut[x];      //default position - 90 degrees
	if(x >= 0 && x <= 45){
		P2OUT |= BIT0;                  //LED0 on
		P2OUT &= ~(BIT2 + BIT3 + BIT4);   //all other LEDs off
	}
	else if(x > 45 && x <= 90){
		P2OUT |= BIT2;                  //LED1 on
		P2OUT &= ~(BIT0 + BIT3 + BIT4);   //all other LEDs off
	}
	else if(x > 90 && x <= 135){
		P2OUT |= BIT3;                  //LED2 on
		P2OUT &= ~(BIT0 + BIT2 + BIT4);   //all other LEDs off
	}
	else if(x > 135 && x <= 180){
		P2OUT |= BIT4;                  //LED3 on
		P2OUT &= ~(BIT0 + BIT2 + BIT3);   //all other LEDs off
	}
	else{                               //not in range, all LEDs indicates error
		P2OUT |= BIT0 + BIT2 + BIT3 + BIT4;
	}
	for(;;){
	}
	
	return 0;
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	char value = UCB0RXBUF;
	if (value == 'a'){        //move servo right(clockwise)
		//P1OUT ^= BIT0;
		if(x<180){
			x++;
		}
	}
	else if(value == 'b'){    //move servo left(counter-clockwise)
		if(x>0){
			x--;
		}
	}
        TA1CCR1 = servo_lut[x];

	if(x >= 0 && x <= 45){
        	P2OUT |= BIT0;                  //LED0 on
        	P2OUT &= ~(BIT2 + BIT3 + BIT4);   //all other LEDs off
	}
	else if(x > 45 && x <= 90){
		P2OUT |= BIT2;                  //LED1 on
		P2OUT &= ~(BIT0 + BIT3 + BIT4);   //all other LEDs off
	}
	else if(x > 90 && x <= 135){
		P2OUT |= BIT3;                  //LED2 on
		P2OUT &= ~(BIT0 + BIT2 + BIT4);   //all other LEDs off
	}
	else if(x > 135 && x <= 180){
		P2OUT |= BIT4;                  //LED3 on
		P2OUT &= ~(BIT0 + BIT2 + BIT3);   //all other LEDs off
	}
	else{                               //not in range, all LEDs indicates error
		P2OUT |= BIT0 + BIT2 + BIT3 + BIT4;
	}
	
}
