#include "msp430.h"
#include "delay.h"
#include "ds18b20.h"

int ds18b20_init()
{
    // first we set DQ output and high
    P2DIR |= BIT0;
    P2OUT |= BIT0;
    delay_us(100);

    // then we pull down for 480us ~ 960us
    P2OUT &= ~BIT0;
    delay_us(750);

    // set DQ input and pullup
    ds18b20_set_input();

    // wait for ds18b20 to pull down
    int dq;
    int dq_readcnt = 0;
    do
    {
        dq = (P2IN & BIT0);
        dq_readcnt++;
        delay_us(1);
    } while (dq && dq_readcnt < 200);
    if (dq_readcnt == 200)  // ds18b20 didn't pull down
    {
        return 0;
    }

    // now we wait for it to release
    dq_readcnt = 0;
    do
    {
        dq = (P2IN & BIT0);
        dq_readcnt++;
        delay_us(1);
    } while (!dq && dq_readcnt < 200);
    if (dq_readcnt == 200)  // ds18b20 didn't release
    {
        return 0;
    }
    delay_us(500);
    P2DIR |= BIT0;
    P2OUT |= BIT0;
    return 1;
}

void ds18b20_set_input()
{
    P2DIR &= ~BIT0;
    P2REN |= BIT0;
    P2OUT |= BIT0;
}

int ds18b20_readbit()
{
    // master pull down for at least 1us
    P2DIR |= BIT0;
    P2OUT &= ~BIT0;
    delay_us(2);

    // master set input and pull up
    // then delay for a short while
    ds18b20_set_input();
    delay_us(5);

    int rdata = (P2IN & BIT0);
    delay_us(60);
    return (rdata != 0);
}

int ds18b20_readbyte()
{
    int rbyte = 0;
    int i = 0;
    for (i = 0; i < 8; i++)
    {
        int tmp = ds18b20_readbit();
        rbyte |= (tmp << i);        // read LSB first
        delay_us(1);                // t_rec between reads
    }
    return rbyte;
}

void ds18b20_writebyte(int wbyte)
{
    // set output high first
    P2DIR |= BIT0;
    P2OUT |= BIT0;

    int i = 0;
    for (i = 0; i < 8; i++)
    {
        int wbit = (wbyte >> i) & 0x01;
        if (wbit)                   // write bit 1
        {
            P2OUT &= ~BIT0;
            delay_us(2);
            P2OUT |= BIT0;
            delay_us(60);
        }
        else                        // write bit 0
        {
            P2OUT &= ~BIT0;
            delay_us(60);
            P2OUT |= BIT0;
            delay_us(2);
        }
        delay_us(1);                // t_rec between reads
    }
}

void ds18b20_convert2temp(int rdata, char *tempstr)
{
    int tmp_integer = rdata >> 4;
    int tmp_point = (rdata & 0xf) * 625;
    tempstr[0] = (tmp_integer / 10) % 10 + '0';
    tempstr[1] = tmp_integer % 10 + '0';
    tempstr[2] = '.';
    tempstr[3] = tmp_point / 1000 + '0';
    tempstr[4] = (tmp_point / 100) % 10 + '0';
    tempstr[5] = (tmp_point / 10) % 10 + '0';
    tempstr[6] = tmp_point % 10 + '0';
    tempstr[7] = '\0';
}
