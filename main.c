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
#define SSD1306_DATA_TRIS  TRISCbits.TRISC1 // RC1 tris for RST
#define SSD1306_DC_TRIS    TRISCbits.TRISC2 // RC2 tris for D/C
#define SSD1306_CLK_TRIS   TRISCbits.TRISC3 // RC3 tris for RST
#define SSD1306_RST_TRIS   TRISCbits.TRISC4 // RC4 tris for RST

// SSD1306 Command Definitions (from Adafruit_SSD1306.h)
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x01
#define SSD1306_SWITCHCAPVCC 0x02
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E

// Display dimensions
#define WIDTH 128
#define HEIGHT 32
#define PAGES (HEIGHT / 8) // 4 pages for 128x32
#define BUFFER_SIZE (WIDTH * PAGES) // 128 * 4 = 512 bytes

// Display buffer
uint8_t display_buffer[BUFFER_SIZE];

void SPI_Init(void) {
    ANSELC = 0x00; // Disable analog functions on PORTC
    SSD1306_CS_TRIS = 0;  // RC0 output (CS)
    SSD1306_DC_TRIS = 0;  // RC2 output (DC)
    SSD1306_RST_TRIS = 0; // RC4 output (RST)
    SSD1306_CLK_TRIS = 0; // RC3 output (SCK1)
    SSD1306_DATA_TRIS = 0;     // RC1 output (SDO1)

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
    SSD1306_DC_LAT = 0;    // Command mode
    SSD1306_CS_LAT = 0;    // Select device
    SPI_Write(cmd);
    SSD1306_CS_LAT = 1;    // Deselect device
}

void SSD1306_Data(uint8_t data) {
    SSD1306_DC_LAT = 1;    // Data mode
    SSD1306_CS_LAT = 0;    // Select device
    SPI_Write(data);
    SSD1306_CS_LAT = 1;    // Deselect device
}

void SSD1306_CommandList(const uint8_t *commands, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        SSD1306_Command(commands[i]);
    }
}

void SSD1306_Init(void) {
    uint8_t vccstate = SSD1306_SWITCHCAPVCC; // Internal charge pump
    uint8_t comPins = 0x02; // For 128x32
    uint8_t contrast = 0x8F;

    SSD1306_Reset();

    // Initialization sequence adapted from Adafruit_SSD1306.cpp
    const uint8_t init1[] = {
        SSD1306_DISPLAYOFF,         // 0xAE
        SSD1306_SETDISPLAYCLOCKDIV, // 0xD5
        0x80,                       // Suggested ratio 0x80
        SSD1306_SETMULTIPLEX        // 0xA8
    };
    SSD1306_CommandList(init1, sizeof(init1));
    SSD1306_Command(HEIGHT - 1); // 31 for 128x32

    const uint8_t init2[] = {
        SSD1306_SETDISPLAYOFFSET,   // 0xD3
        0x00,                       // No offset
        SSD1306_SETSTARTLINE | 0x0, // Line #0
        SSD1306_CHARGEPUMP          // 0x8D
    };
    SSD1306_CommandList(init2, sizeof(init2));
    SSD1306_Command(vccstate == SSD1306_EXTERNALVCC ? 0x10 : 0x14); // 0x14 for internal VCC

    const uint8_t init3[] = {
        SSD1306_MEMORYMODE,   // 0x20
        0x00,                 // Horizontal addressing mode
        SSD1306_SEGREMAP | 0x1, // Segment remap
        SSD1306_COMSCANDEC    // COM scan direction
    };
    SSD1306_CommandList(init3, sizeof(init3));

    SSD1306_Command(SSD1306_SETCOMPINS);
    SSD1306_Command(comPins); // 0x02 for 128x32
    SSD1306_Command(SSD1306_SETCONTRAST);
    SSD1306_Command(contrast); // 0x8F

    SSD1306_Command(SSD1306_SETPRECHARGE); // 0xD9
    SSD1306_Command(vccstate == SSD1306_EXTERNALVCC ? 0x22 : 0xF1); // 0xF1 for internal VCC

    const uint8_t init5[] = {
        SSD1306_SETVCOMDETECT,      // 0xDB
        0x40,
        SSD1306_DISPLAYALLON_RESUME, // 0xA4
        SSD1306_NORMALDISPLAY,      // 0xA6
        SSD1306_DEACTIVATE_SCROLL,
        SSD1306_DISPLAYON           // Turn on display
    };
    SSD1306_CommandList(init5, sizeof(init5));
}

void SSD1306_ClearBuffer(void) {
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        display_buffer[i] = 0x00; // All black
    }
}

void SSD1306_FillBuffer(uint8_t value) {
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        display_buffer[i] = value; // Fill with specified value (e.g., 0xFF for all white)
    }
}

void SSD1306_DrawPixel(int16_t x, int16_t y, uint8_t color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return; // Out of bounds
    }

    uint16_t index = x + (y / 8) * WIDTH;
    uint8_t bit = 1 << (y & 7);

    if (color) {
        display_buffer[index] |= bit;  // Set pixel (white)
    } else {
        display_buffer[index] &= ~bit; // Clear pixel (black)
    }
}

void SSD1306_Display(void) {
    // Set column and page address range
    SSD1306_Command(SSD1306_COLUMNADDR);
    SSD1306_Command(0);         // Column start (0)
    SSD1306_Command(WIDTH - 1); // Column end (127)

    SSD1306_Command(SSD1306_PAGEADDR);
    SSD1306_Command(0);         // Page start (0)
    SSD1306_Command(PAGES - 1); // Page end (3)

    // Send buffer to display
    SSD1306_DC_LAT = 1;    // Data mode
    SSD1306_CS_LAT = 0;    // Select device
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        SPI_Write(display_buffer[i]);
    }
    SSD1306_CS_LAT = 1;    // Deselect device
}

void SSD1306_TestPattern(void) {
    SSD1306_ClearBuffer();
    // Draw a simple pattern: top half white, bottom half black
    for (int16_t x = 0; x < WIDTH; x++) {
        for (int16_t y = 0; y < HEIGHT; y++) {
            if (y < HEIGHT / 2) {
                SSD1306_DrawPixel(x, y, 1); // Top half white
            } else {
                SSD1306_DrawPixel(x, y, 0); // Bottom half black
            }
        }
    }
    SSD1306_Display();
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
    SSD1306_ClearBuffer();
    
    while(1) {
        // Test 1: Top half white, bottom half black
        SSD1306_TestPattern();
        __delay_ms(2000); // Display for 2 seconds

        // Test 2: All white
        SSD1306_FillBuffer(0xFF);
        SSD1306_Display();
        __delay_ms(2000); // Display for 2 seconds

        // Test 3: All black
        SSD1306_ClearBuffer();
        SSD1306_Display();
        __delay_ms(2000); // Display for 2 seconds
    }
    
    return;
}
