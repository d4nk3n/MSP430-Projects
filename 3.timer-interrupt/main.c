#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                         // Stop WDT
    UCSCTL4 = (UCSCTL4 & ~(SELA_7)) | (SELA__XT1CLK); // Use XT1CLK to set ACLK
    P1DIR |= 0x01;                                    // P1.0 output
    TA0CCTL0 = CCIE;                                  // CCR0 interrupt enabled
    TA0CCR0 = 32768;                                  // Freq(XT1CLK) = 32768
    TA0CTL = TASSEL_1 + MC_1 + TACLR;                 // SMCLK, upmode, clear TAR
    __bis_SR_register(LPM0_bits + GIE);               // Enter LPM0, enable interrupts
    __no_operation();                                 // For debugger
}

// Timer0 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    P1OUT ^= 0x01;                                    // Toggle P1.0
}
