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

// Display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define BUFFER_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8)

// Pin definitions
#define SSD1306_CS_LAT     LATCbits.LATC0  // RC0 lat for CS
#define SSD1306_DATA_LAT   LATCbits.LATC1  // RC1 lat for DataOut (SDO1)
#define SSD1306_DC_LAT     LATCbits.LATC2  // RC2 lat for D/C
#define SSD1306_CLK_LAT    LATCbits.LATC3  // RC3 lat for CLK (SCK1)
#define SSD1306_RST_LAT    LATCbits.LATC4  // RC4 lat for RST

#define SSD1306_CS_TRIS    TRISCbits.TRISC0 // RC0 tris for CS
#define SSD1306_DATA_TRIS  TRISCbits.TRISC1 // RC1 tris for DataOut (SDO1)
#define SSD1306_DC_TRIS    TRISCbits.TRISC2 // RC2 tris for D/C
#define SSD1306_CLK_TRIS   TRISCbits.TRISC3 // RC3 tris for CLK (SCK1)
#define SSD1306_RST_TRIS   TRISCbits.TRISC4 // RC4 tris for RST

// SSD1306 command definitions
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_DEACTIVATE_SCROLL 0x2E

// Display buffer
uint8_t display_buffer[BUFFER_SIZE];

// SPI Write function
void spi_write(uint8_t data) {
    SSP1BUF = data;              // Load data into buffer
    while (!SSP1STATbits.BF);    // Wait for transmission complete
}

// Send command list to SSD1306
void display_send_command_list(const uint8_t *commands, uint8_t count) {
    SSD1306_DC_LAT = 0;          // Command mode
    SSD1306_CS_LAT = 0;          // Select chip
    while (count--) {
        spi_write(*(commands++));
    }
    SSD1306_CS_LAT = 1;          // Deselect chip
}

// Send single command to SSD1306
void display_send_command(uint8_t command) {
    SSD1306_DC_LAT = 0;          // Command mode
    SSD1306_CS_LAT = 0;          // Select chip
    spi_write(command);
    SSD1306_CS_LAT = 1;          // Deselect chip
}

// Update display with buffer contents
void display_update(void) {
    SSD1306_DC_LAT = 0;          // Command mode
    SSD1306_CS_LAT = 0;          // Select chip
    const uint8_t init_commands[] = {
        SSD1306_PAGEADDR,
        0,                       // Page start
        0xFF,                    // Page end
        SSD1306_COLUMNADDR,
        0                        // Column start
    };
    display_send_command_list(init_commands, sizeof(init_commands));
    display_send_command(SCREEN_WIDTH - 1); // Column end

    SSD1306_DC_LAT = 1;          // Data mode
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        spi_write(display_buffer[i]);
    }
    SSD1306_CS_LAT = 1;          // Deselect chip
}
void* memset (void *buf, unsigned char ch, size_t count) 
{
  register unsigned char *ret = buf;

  while (count--)
    {
      *(unsigned char *)ret = ch;
      ++ret;
    }

  return buf;
}

// Clear display buffer
void display_clear(void) {
    memset(display_buffer, 0, BUFFER_SIZE);
}

// Draw pixel in buffer
void display_draw_pixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    uint16_t index = x + (y / 8) * SCREEN_WIDTH;
    uint8_t bit = 1 << (y & 7);
    if (color) {
        display_buffer[index] |= bit;
    } else {
        display_buffer[index] &= ~bit;
    }
}

// Draw test pattern (top half white, bottom half black)
void display_test_pattern(void) {
    display_clear();
    for (int16_t x = 0; x < SCREEN_WIDTH; x++) {
        for (int16_t y = 0; y < SCREEN_HEIGHT; y++) {
            display_draw_pixel(x, y, y < SCREEN_HEIGHT / 2);
        }
    }
    display_update();
}

void display_test_pattern2(void) {
    display_clear();
    for (int16_t x = 0; x < SCREEN_WIDTH; x++) {
        for (int16_t y = 0; y < SCREEN_HEIGHT; y++) {
            display_draw_pixel(x, y, y > SCREEN_HEIGHT / 2);
        }
    }
    display_update();
}

// SPI Initialization
void spi_init(void) {
    ANSELC = 0x00;               // Disable analog functions on PORTC
    SSD1306_CS_TRIS = 0;         // RC0 output (CS)
    SSD1306_DC_TRIS = 0;         // RC2 output (DC)
    SSD1306_RST_TRIS = 0;        // RC4 output (RST)
    SSD1306_CLK_TRIS = 0;        // RC3 output (SCK1)
    SSD1306_DATA_TRIS = 0;       // RC1 output (SDO1)

    // PPS configuration
    RC0PPS = 0x00;               // RC0 = LATxy (CS)
    RC1PPS = 0x19;               // RC1 = SDO1
    RC2PPS = 0x00;               // RC2 = LATxy (D/C)
    RC3PPS = 0x18;               // RC3 = SCK1
    RC4PPS = 0x00;               // RC4 = LATxy (RST)

    // SPI configuration for Mode 0 (CPOL=0, CPHA=0)
    SSP1STAT = 0x40;             // SMP=0, CKE=1 (Mode 0)
    SSP1CON1 = 0x20;             // SSPEN=1, SSPM=0000 (Fosc/4)
    SSP1ADD = 0x00;              // Clock divider (Fosc/4 = 250 kHz at 1 MHz)
}

// Display Initialization
void display_init(void) {
    // Reset sequence
    SSD1306_RST_LAT = 1;
    __delay_ms(1);
    SSD1306_RST_LAT = 0;
    __delay_ms(10);
    SSD1306_RST_LAT = 1;

    SSD1306_CS_LAT = 1;          // Deselect chip initially

    // SSD1306 initialization sequence
    const uint8_t init1[] = {
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV,
        0x80,
        SSD1306_SETMULTIPLEX
    };
    display_send_command_list(init1, sizeof(init1));
    display_send_command(SCREEN_HEIGHT - 1);

    const uint8_t init2[] = {
        SSD1306_SETDISPLAYOFFSET,
        0x00,
        SSD1306_SETSTARTLINE,
        SSD1306_CHARGEPUMP
    };
    display_send_command_list(init2, sizeof(init2));
    display_send_command(0x14);

    const uint8_t init3[] = {
        SSD1306_MEMORYMODE,
        0x00,
        SSD1306_SEGREMAP | 0x1,
        SSD1306_COMSCANDEC
    };
    display_send_command_list(init3, sizeof(init3));

    display_send_command(SSD1306_SETCOMPINS);
    display_send_command(0x02);
    display_send_command(SSD1306_SETCONTRAST);
    display_send_command(0x8F);

    display_send_command(SSD1306_SETPRECHARGE);
    display_send_command(0xF1);

    const uint8_t init5[] = {
        SSD1306_SETVCOMDETECT,
        0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY,
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON
    };
    display_send_command_list(init5, sizeof(init5));
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

    __delay_ms(1000); //Delay to ensure the display is ready 

    spi_init();
    display_init();

    while (1) {        
        display_test_pattern();
        __delay_ms(1000);
        display_test_pattern2();
    }
    
    return;
}
