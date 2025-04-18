/*
 * File:   main.c
 * Author: brj
 *
 * Created on April 15, 2025, 3:34 PM
 */

// PIC16F18345 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = OFF    // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = 110   // Power-up default value for COSC bits (LFINTOSC (31kHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = OFF       // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#pragma config LPBOREN = OFF    // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled)
#pragma config BORV = LOW       // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF      // Debugger enable bit (Background debugger disabled)

// CONFIG3
#pragma config WRT = OFF        // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)

// CONFIG4
#pragma config CP = OFF         // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF        // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 1000000
#define SSD1306_CS_LAT     LATCbits.LATC0 // RC0 lat for CS
#define SSD1306_DATA_LAT   LATCbits.LATC1 // RC1 lat for DataOut (SDO1)
#define SSD1306_DC_LAT     LATCbits.LATC2 // RC2 lat for D/C
#define SSD1306_CLK_LAT    LATCbits.LATC3 // RC3 lat for CLK (SCK1)
#define SSD1306_RST_LAT    LATCbits.LATC4 // RC4 lat for RST

#define SSD1306_CS_TRIS    TRISCbits.TRISC0 // RC0 tris for CS
#define SSD1306_DATA       TRISCbits.TRISC1 // RC1 tris for RST
#define SSD1306_DC_TRIS    TRISCbits.TRISC2 // RC2 tris for D/C
#define SSD1306_CLK_TRIS   TRISCbits.TRISC3 // RC3 tris for RST
#define SSD1306_RST_TRIS   TRISCbits.TRISC4 // RC4 tris for RST

void SPI_Init(void) {
    ANSELC = 0x00; // Disable analog functions on PORTC
    SSD1306_CS_TRIS = 0;  // RC0 output (CS)
    SSD1306_DC_TRIS = 0;  // RC2 output (DC)
    SSD1306_RST_TRIS = 0; // RC4 output (RST)
    SSD1306_CLK_TRIS = 0; // RC3 output (SCK1)
    SSD1306_DATA = 0;     // RC1 output (SDO1)

    SSP1STAT = 0b00000000; // CKE=1, SMP=0
    
    // Output PPS - page 163
    RC0PPS = 0b00000; // RC0 = LATxy (CS)
    RC1PPS = 0b11001; // RC1 = SDO1 (DataOut)
    RC2PPS = 0b00000; // RC2 = LATxy (D/C)
    RC3PPS = 0b11000; // RC3 = SCK1 (CLK)
    RC4PPS = 0b00000; // RC4 = LATxy (RST)
    
    //Page 361
    SSP1ADD = 0b0000; //Clock divider for SCLK to nothing. Its divided by 4, so output is 250khz   
    SSP1CONbits.SSPM = 0b0000; //Set SPI master mode    
    SSP1CON1bits.SSPEN = 1; //Enable SPI mode
}

void SSD1306_Reset(void) {
    SSD1306_RST_LAT = 0;
    __delay_ms(10);
    SSD1306_RST_LAT = 1;
    __delay_ms(10);
}

void SPI_Write(uint8_t data) {
    SSP1BUF = data;
    while (!SSP1STATbits.BF);
}

void SSD1306_Command(uint8_t cmd) {
    SSD1306_DC_LAT = 0;
    SSD1306_CS_LAT = 0;
    SPI_Write(cmd);
    SSD1306_CS_LAT = 1;
}

void SSD1306_Data(uint8_t data) {
    SSD1306_DC_LAT = 1;
    SSD1306_CS_LAT = 0;
    SPI_Write(data);
    SSD1306_CS_LAT = 1;
}

void SSD1306_Init(void) {
    uint8_t height = 32;

    SSD1306_Reset();

    SSD1306_Command(0xAE); // Display OFF
    SSD1306_Command(0xD5); // Set display clock
    SSD1306_Command(0x80);

    SSD1306_Command(0xA8); // Set multiplex ratio
    SSD1306_Command(height - 1); // 0x1F

    SSD1306_Command(0xD3); // Display offset
    SSD1306_Command(0x00);
    SSD1306_Command(0x40); // Start line = 0

    SSD1306_Command(0x8D); // Charge pump
    SSD1306_Command(0x14);

    SSD1306_Command(0x20); // Memory mode
    SSD1306_Command(0x00); // Horizontal addressing

    SSD1306_Command(0xA1); // Segment remap
    SSD1306_Command(0xC8); // COM scan direction

    SSD1306_Command(0xDA); // COM pins config
    SSD1306_Command(0x02); // For 128x32

    SSD1306_Command(0x81); // Contrast
    SSD1306_Command(0x8F);

    SSD1306_Command(0xD9); // Pre-charge
    SSD1306_Command(0xF1);

    SSD1306_Command(0xDB); // VCOMH
    SSD1306_Command(0x40);

    SSD1306_Command(0xA4); // Resume to RAM content
    SSD1306_Command(0xA6); // Normal display

    SSD1306_Command(0xAF); // Display ON
}

void SSD1306_Clear(void) {
    uint8_t pages = 4;

    SSD1306_Command(0x21); // Column address
    SSD1306_Command(0x00);
    SSD1306_Command(0x7F); // 127

    SSD1306_Command(0x22); // Page address
    SSD1306_Command(0x00);
    SSD1306_Command(pages - 1); // 0x03

    for (uint16_t i = 0; i < 128 * pages; i++) {
        SSD1306_Data(0x00);
    }
}

void SSD1306_Test(void) {
    uint8_t pages = 4;

    SSD1306_Command(0x21);
    SSD1306_Command(0x00);
    SSD1306_Command(0x7F);

    SSD1306_Command(0x22);
    SSD1306_Command(0x00);
    SSD1306_Command(pages - 1);

    for (uint16_t i = 0; i < 128 * pages; i++) {
        SSD1306_Data((i & 1) ? 0xFF : 0x00);
    }
}

void main(void) {
	//Setting up internal clock

	OSCCON1bits.NOSC = 0b110; // LFINTOSC
    OSCCON1bits.NDIV = 0b0000; // No divider (1:1)
    OSCFRQbits.HFFRQ = 0b000;

    // Allow clock switch to proceed
    OSCCON3bits.CSWHOLD = 0;

    // Wait for the oscillator to be ready
    while (!OSCCON3bits.ORDY);

    SPI_Init();
    SSD1306_Init();
    
    while(1){
        __delay_ms(10);
        SSD1306_Clear();
        SSD1306_Test();
    }
    
    return;
}
