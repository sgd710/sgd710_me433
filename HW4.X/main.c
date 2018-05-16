#include "NU32.h"
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include <math.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging **(1x)**
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch **(1x)**
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_4 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_1 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module
/////////////////////////////////////////////////////////////////////////

#define CS LATBbits.LATB7       // chip select pin

float tri(int j);
float sin(int i);
void setVoltage(char channel, int voltage);
void spi_init();
unsigned char spi_io(unsigned char o);

int main(){
    spi_init();
    while(1){
      int i = 0, j = 99;
      float sine, triangle;
      while (i < 1000){
        //sin wave
        sine = sin(i);
        setVoltage(0, sine);

        //triangle wave
        if (i <= 100) {
          triangle = tri(i);
        }
        else if (i <= 200) {
          triangle = tri(j);
          j--;
        }
        else if (i <= 300) {
          triangle = tri(i - 200);
          j = 99;
        }
        else if (i <= 400) {
          triangle = tri(j);
          j--;
        }
        else if (i <= 500) {
          triangle = tri(i - 400);
          j = 99;
        }
        else if (i <= 600) {
          triangle = tri(j);
          j--;
        }
        else if (i <= 700) {
          triangle = tri(i - 600);
          j = 99;
        }
        else if (i <= 800) {
          triangle = tri(j);
          j--;
        }
        else if (i <= 900) {
          triangle = tri(i - 800);
          j = 99;
        }
        else if (i <= 998) {
          triangle = tri(j);
          j--;
        }
        setVoltage(1,triangle);
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 24000) {
          ;
        }  
        i++;
      }
    }
}


//////////FUNCTIONS/////////

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}


void setVoltage(char channel, int voltage){
  unsigned short data = 0;
  data = data | 0b0111000000000000;
  data = (voltage << 2) | data;
  data = (channel << 15) | data;

  CS = 0;
  SPI_io(data >> 8);  //send bits 8-15
  SPI_io(data); //send bits 0-7
  CS = 1;
}

float sin(int i){
  float v = 511.5 * (pi/1000)
}

float tri(int j){
  float v;
  v = 1023/100 * j;
}

// initialize spi1 and the ram module
void spi_init() {
  // set up the chip select pin as an output
  // the chip select pin is used by the sram to indicate
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)

  RPA1Rbits.RPA1R = 0b0011; //set SDO1 to A1
  TRISBbits.TRISB7 = 0; //set SS1 to B7
  CS = 1;
  SPI1CON = 0;
  SPI1BUF;
  SPI1BRG = 2;
  SPI1STATbits.SPIROV = 0;
  SPI1CONbits.CKE = 1;
  SPI1CONbits.MSTEN = 1;
  SPI1CONbits.ON = 1;
}
