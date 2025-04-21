// Setup for the ESP32 S3 with ILI9341 display
// Note SPI DMA with ESP32 S3 is not currently supported
#define USER_SETUP_ID 70
// See SetupX_Template.h for all options available
#define ILI9341_2_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

                    // Typical board default pins - change to match your board
#define TFT_CS   10 //     10 (HSPI) or 39 (VSPI) CS 
#define TFT_MOSI 11 //     11 (HSPI) or 35 (VSPI) D
#define TFT_SCLK 12 //     12 (HSPI) or 36 (VSPI) CLK
#define TFT_MISO 13 //     13 (HSPI) or 37 (VSPI) Q

// Use pins in range 0-31
#define TFT_DC    7            // Data/Command control pin
#define TFT_RST   6            // Reset pin (could connect to RST pin)
#define TFT_BL   15            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define TOUCH_CS -1            // Optional for touch screen

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF

#define SMOOTH_FONT

// The ESP32 integrates 4 SPI peripherals: SPI0, SPI1, SPI2 (commonly referred to as HSPI), 
// and SPI3 (commonly referred to as VSPI). SP0 and SP1 are used internally to communicate
// with the built-in flash memory, and you should not use them for other tasks. Uncomment
// the following line to use the HSPI port (SPI2) instead of the default VSPI port (SPI3).
#define USE_HSPI_PORT

#define SPI_FREQUENCY  40000000   // Maximum for ILI9341

#define SPI_READ_FREQUENCY  20000000

#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

#define TFT_INVERSION_ON       // Enable TFT inversion