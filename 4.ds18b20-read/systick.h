#ifndef SYSTICK_H

#define SYSTICK_H
// about the default CPU frequency, see
// https://stackoverflow.com/questions/34258254/msp430-delay-cycles-is-slow-mclk-might-be-slow
// in order to make delay more accurate, set CPU frequency to 25MHz
#define CPU_Freq            ((double)25000000)
#define delay_us(x)         __delay_cycles((long)(CPU_Freq * (double)x / 1000000.0))
#define delay_ms(x)         __delay_cycles((long)(CPU_Freq * (double)x / 1000.0))

#endif
