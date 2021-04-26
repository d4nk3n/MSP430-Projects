#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "include/HAL_Dogs102x6.h"
#include "include/HAL_AppUart.h"

#define contrastSetpointAddress 0x1880
#define brightnessSetpointAddress 0x1882

/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // Configure Dogs102x6 ports
    P5OUT &= ~(BIT6 + BIT7); // LCD_C/D, LCD_RST
    P5DIR |= BIT6 + BIT7;
    P7OUT &= ~(BIT4 + BIT6); // LCD_CS, LCD_BL_EN
    P7DIR |= BIT4 + BIT6;
    P4OUT &= ~(BIT1 + BIT3); // SIMO, SCK
    P4DIR &= ~BIT2;          // SOMI pin is input
    P4DIR |= BIT1 + BIT3;

    // Set up LCD
    Dogs102x6_init();
    Dogs102x6_backlightInit();
    uint8_t contrast = *((unsigned char *)contrastSetpointAddress);
    uint8_t brightness = *((unsigned char *)brightnessSetpointAddress);

    if (contrast == 0xFF) // Contrast not programed in Flash Yet
        // Set Default Contrast
        contrast = 11;

    if (brightness == 0xFF) // Brightness not programed in Flash Yet
        // Set Default Brightness
        brightness = 1;

    Dogs102x6_setBacklight(brightness);
    Dogs102x6_setContrast(contrast);
    Dogs102x6_clearScreen();

    AppUart_init();

    int row = 0;
    int col = 0;
    while (1)
    {
        char a = AppUart_getChar();
        AppUart_putChar(a);
        if (a != '\n')    // check for line feed character
        {
            if (a == '\x7f') // backspace in ascii
            {
                if (col < 6)
                {
                    col = 96;
                    row = (row == 0) ? 7 : row - 1;
                }
                else
                {
                    col -= 6;
                }
                Dogs102x6_charDraw(row, col, ' ', DOGS102x6_DRAW_NORMAL);
            }
            //check for carriage return '/r' (ignore if found)
            else if (a != '\r')
            {
                Dogs102x6_charDraw(row, col, a, DOGS102x6_DRAW_NORMAL);
                //Update location
                col += 6;
                //Text wrapping
                if (col >= 102)
                {
                    col = 0;
                    row = (row < 7) ? row + 1 : 0;
                }
            }
        }
        // handle line feed character
        else
        {
            if (row < 7)
                row++;
            else
                row = 0;
            col = 0;
        }
    }
}
include/