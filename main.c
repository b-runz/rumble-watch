#include <xc.h>

// CONFIG BITS (already provided — kept for completeness)
#pragma config FEXTOSC = OFF  // FEXTOSC External Oscillator mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = 110   // Power-up default value for COSC bits (LFINTOSC (31kHz))
#pragma config CLKOUTEN = OFF // Clock Out Enable bit (CLKOUT function is disabled; I/O or oscillator function on OSC2)
#pragma config CSWEN = ON     // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON     // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config MCLRE = ON    // Master Clear Enable bit (MCLR/VPP pin function is MCLR; Weak pull-up enabled)
#pragma config PWRTE = OFF   // Power-up Timer Enable bit (PWRT disabled)
#pragma config WDTE = OFF    // Watchdog Timer Enable bits (WDT disabled; SWDTEN is ignored)
#pragma config LPBOREN = OFF // Low-power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = OFF   // Brown-out Reset Enable bits (Brown-out Reset disabled)
#pragma config BORV = LOW    // Brown-out Reset Voltage selection bit (Brown-out voltage (Vbor) set to 2.45V)
#pragma config PPS1WAY = ON  // PPSLOCK bit One-Way Set Enable bit (The PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON   // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a Reset)
#pragma config DEBUG = OFF   // Debugger enable bit (Background debugger disabled)

// CONFIG3
#pragma config WRT = OFF // User NVM self-write protection bits (Write protection off)
#pragma config LVP = ON  // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored.)

// CONFIG4
#pragma config CP = OFF  // User NVM Program Memory Code Protection bit (User NVM code protection disabled)
#pragma config CPD = OFF // Data NVM Memory Code Protection bit (Data NVM code protection disabled)

#define _XTAL_FREQ 1000000UL

// SSD1306 Pins
#define CS_LAT LATCbits.LATC0
#define DC_LAT LATCbits.LATC2
#define RST_LAT LATCbits.LATC4

#define CS_TRIS TRISCbits.TRISC0
#define DC_TRIS TRISCbits.TRISC2
#define RST_TRIS TRISCbits.TRISC4

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define BUFFER_SIZE 512

#define SSD1306_BLACK 0 ///< Draw 'off' pixels
#define SSD1306_WHITE 1 ///< Draw 'on' pixels

#define SSD1306_MEMORYMODE 0x20          ///< See datasheet
#define SSD1306_COLUMNADDR 0x21          ///< See datasheet
#define SSD1306_PAGEADDR 0x22            ///< See datasheet
#define SSD1306_SETCONTRAST 0x81         ///< See datasheet
#define SSD1306_CHARGEPUMP 0x8D          ///< See datasheet
#define SSD1306_SEGREMAP 0xA0            ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON 0xA5        ///< Not currently used
#define SSD1306_NORMALDISPLAY 0xA6       ///< See datasheet
#define SSD1306_INVERTDISPLAY 0xA7       ///< See datasheet
#define SSD1306_SETMULTIPLEX 0xA8        ///< See datasheet
#define SSD1306_DISPLAYOFF 0xAE          ///< See datasheet
#define SSD1306_DISPLAYON 0xAF           ///< See datasheet
#define SSD1306_COMSCANINC 0xC0          ///< Not currently used
#define SSD1306_COMSCANDEC 0xC8          ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET 0xD3    ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5  ///< See datasheet
#define SSD1306_SETPRECHARGE 0xD9        ///< See datasheet
#define SSD1306_SETCOMPINS 0xDA          ///< See datasheet
#define SSD1306_SETVCOMDETECT 0xDB       ///< See datasheet

#define SSD1306_SETLOWCOLUMN 0x00  ///< Not currently used
#define SSD1306_SETHIGHCOLUMN 0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE 0x40  ///< See datasheet

#define SSD1306_EXTERNALVCC 0x01  ///< External display voltage source
#define SSD1306_SWITCHCAPVCC 0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26              ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27               ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A  ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E                    ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F                      ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3             ///< Set scroll range

void spi_init(void)
{
    ANSELC = 0x00;
    ANSELCbits.ANSC1 = 0; // RC1 (SDO) digital mode

    // PPS Mapping
    RC1PPS = 0x19; // SDO1 -> RC1
    RC3PPS = 0x18; // SCK1 -> RC3

    TRISCbits.TRISC1 = 0; // SDO1 output
    TRISCbits.TRISC3 = 0; // SCK1 output

    SSP1STAT = 0x40; // CKE = 1 (Mode 0)
    SSP1CON1 = 0x20; // SSPEN = 1, SPI Master, Fosc/4
    SSP1ADD = 0x00;  // No additional divider

    // Set pin directions
    DC_TRIS = 0;
    CS_TRIS = 0;
    RST_TRIS = 0;
}

void spi_send(uint8_t cmd)
{
    SSP1BUF = cmd;
    while (!SSP1STATbits.BF)
        ; // Wait for transmission to complete
}

uint8_t ssd1306_buffer[512];

void ssd1306_clear_display()
{
    for (uint16_t i = 0; i < BUFFER_SIZE; i++)
    {
        ssd1306_buffer[i] = 0x00;
    }
}

void ssd1306_begin(void)
{
    CS_LAT = 1; // Set Chip-select high

    RST_LAT = 1;
    __delay_ms(1);
    RST_LAT = 0;
    __delay_ms(10);
    RST_LAT = 1;

    CS_LAT = 0;

    DC_LAT = 0;                           // Set to command mode
    spi_send(SSD1306_DISPLAYOFF);         // 0xAE
    spi_send(SSD1306_SETDISPLAYCLOCKDIV); // 0xD5
    spi_send(0x80);                       // Suggested ratio
    spi_send(SSD1306_SETMULTIPLEX);       // 0xA8

    spi_send(0x1F); // For 128x32 display (0x1F = 31)

    spi_send(SSD1306_SETDISPLAYOFFSET);    // 0xD3
    spi_send(0x00);                        // No offset
    spi_send(SSD1306_SETSTARTLINE | 0x00); // 0x40
    spi_send(SSD1306_CHARGEPUMP);          // 0x8D

    spi_send(0x14); // Enable charge pump (internal VCC)

    spi_send(SSD1306_MEMORYMODE);     // 0x20
    spi_send(0x00);                   // Horizontal addressing mode
    spi_send(SSD1306_SEGREMAP | 0x1); // 0xA1 – column address 127 is mapped to SEG
    spi_send(SSD1306_COMSCANDEC);     // 0xC8 – scan from COM[N-1] to COM0

    DC_LAT = 0; // Set to command mode

    spi_send(SSD1306_SETCOMPINS);  // 0xDA
    spi_send(0x02);                // For 128x32
    spi_send(SSD1306_SETCONTRAST); // 0x81
    spi_send(0x8F);                // Contrast value

    spi_send(SSD1306_SETPRECHARGE); // 0xD9
    spi_send(0xF1);                 // Pre-charge period

    spi_send(SSD1306_SETVCOMDETECT);       // 0xDB
    spi_send(0x40);                        // VCOMH deselect level
    spi_send(SSD1306_DISPLAYALLON_RESUME); // 0xA4
    spi_send(SSD1306_NORMALDISPLAY);       // 0xA6
    spi_send(SSD1306_DEACTIVATE_SCROLL);   // 0x2E
    spi_send(SSD1306_DISPLAYON);           // 0xAF

    CS_LAT = 1;
    ssd1306_clear_display();
}

void ssd1306_draw_pixel(int16_t x, int16_t y, uint16_t color)
{
    if ((x >= 0) && (x < SCREEN_WIDTH) && (y >= 0) && (y < SCREEN_HEIGHT))
    {
        uint16_t index = x + (y / 8) * SCREEN_WIDTH;
        uint8_t bit = 1 << (y & 7);
        switch (color)
        {
        case SSD1306_WHITE:
            ssd1306_buffer[index] |= bit;
            break;
        case SSD1306_BLACK:
            ssd1306_buffer[index] &= ~bit;
            break;
        }
    }
}

void ssd1306_display(void)
{
    CS_LAT = 0;
    DC_LAT = 0;

    spi_send(SSD1306_PAGEADDR);
    spi_send(0);
    spi_send(0xFF);
    spi_send(SSD1306_COLUMNADDR);
    spi_send(0);

    spi_send(SCREEN_WIDTH - 1);

    uint16_t count = SCREEN_WIDTH * ((SCREEN_HEIGHT + 7) / 8);
    uint8_t *ptr = ssd1306_buffer;

    DC_LAT = 1;

    while (count--)
    {
        spi_send(*ptr++);
    }

    CS_LAT = 1;
}

void displayTestPattern(int8_t flip)
{
    ssd1306_clear_display();
    for (int16_t x = 0; x < SCREEN_WIDTH; x++)
    {
        if (flip == 1)
        {
            for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
            {
                ssd1306_draw_pixel(x, y, y < (SCREEN_HEIGHT / 2) ? SSD1306_WHITE : SSD1306_BLACK);
            }
        }
        else
        {
            for (int16_t y = 0; y < SCREEN_HEIGHT; y++)
            {
                ssd1306_draw_pixel(x, y, y > (SCREEN_HEIGHT / 2) ? SSD1306_WHITE : SSD1306_BLACK);
            }
        }
    }
    ssd1306_display();
}

void main(void)
{
    OSCCON1bits.NOSC = 0b110;  // LFINTOSC
    OSCCON1bits.NDIV = 0b0000; // No divider (1:1)
    OSCFRQbits.HFFRQ = 0b000;

    // Allow clock switch to proceed
    OSCCON3bits.CSWHOLD = 0;

    while (!OSCCON3bits.ORDY)
        ; // Wait for oscillator ready

    __delay_ms(500);
    spi_init();
    ssd1306_begin();
    int8_t flip = 0;

    while (1)
    {
        displayTestPattern(flip);
        __delay_ms(1000);
        flip ^= 1;
    }
}
