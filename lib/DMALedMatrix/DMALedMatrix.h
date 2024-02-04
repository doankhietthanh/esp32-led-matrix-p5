#ifndef DMA_LED_MATRIX_H
#define DMA_LED_MATRIX_H

#include <SPIFFS.h>
#include <AnimatedGIF.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#define FILESYSTEM SPIFFS
#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.
#define PANEL_CHAIN 1  // Total number of panels chained one to another

static MatrixPanel_I2S_DMA *dma_display = nullptr;
static AnimatedGIF gif;
static File f;
static int x_offset, y_offset;
static unsigned long start_tick = 0;
static char filePath[256] = {0};
static File root, gifFile;

class DMALedMatrix
{
private:
public:
    DMALedMatrix();
    void testDrawShape();
    void testDrawText(int colorWheelOffset);
    uint16_t colorWheel(uint8_t pos);
    void drawText(uint8_t x, uint8_t y, const char *text, const char *hexColor, int size);
    void drawTextWrap(uint8_t x, uint8_t y, const char *text, const char *hexColor, int size);
    void drawColorWheelText(uint8_t x, uint8_t y, const char *text, int size, int colorWheelOffset);
    void drawXbm565(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const char *xbm, uint16_t color = 0xffff);
    void drawGifs(String gifDir);
    void drawGif(char *filename);
    void clearScreen();
    uint16_t convertHexToRGBColor(const char *hexColor);
};

void GIFDraw(GIFDRAW *pDraw);
void *GIFOpenFile(const char *fname, int32_t *pSize);
void GIFCloseFile(void *pHandle);
int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
void ShowGIF(char *name);

#endif