#include <string.h>
#include "msp430.h"
#include "ds18b20.h"
#include "systick.h"
#include "HAL_AppUart.h"
#include "HAL_UCS.h"
#include "HAL_PMM.h"

char send_str[20];
char temp_str[10];

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	                    // stop watchdog timer
    P1DIR |= BIT0;                                  // set P1.0 LED
    P1OUT &= ~BIT0;
    SetVCore(3);                                    // Set Vcore to 3 for 25MHz speed
    Init_FLL_Settle(25000, 762);                    // Set system clock to max (25MHz)

    AppUart_init();                                 // initialize UART
    AppUart_sendString("Read start, every 5s\n");
    TA0CCTL0 = CCIE;                                // CCR0 interrupt enabled
    TA0CCR0 = 20480;                                // Freq(XT1CLK) = 32768 / 8
    TA0CTL = TASSEL_1 + MC_1 + TACLR + ID__8;       // ACLK, upmode, clear TAR, divider 8
    __bis_SR_register(LPM0_bits + GIE);             // Enter LPM0, enable interrupts
    __no_operation();                               // For debugger
}

// Timer0 A0 interrupt service routine
// every 5s, P1.0 LED indicates the system is working
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    P1OUT |= BIT0;
    ds18b20_init();
    ds18b20_writebyte(0xcc);    // skip
    ds18b20_writebyte(0x44);    // convert T
    P1OUT &= ~BIT0;

    delay_ms(770);

    P1OUT |= BIT0;
    ds18b20_init();
    ds18b20_writebyte(0xcc);    // skip
    ds18b20_writebyte(0xbe);    // READ SCRATCHPAD
    unsigned int temp_l = ds18b20_readbyte();
    unsigned int temp_h = ds18b20_readbyte();
    unsigned int tmp = (temp_h << 8) + temp_l;
    ds18b20_convert2temp(tmp, temp_str);
    strcpy(send_str, "Temp = ");
    strcat(send_str, temp_str);
    strcat(send_str, "\r\n");
    AppUart_sendString(send_str);
    P1OUT &= ~BIT0;
}
