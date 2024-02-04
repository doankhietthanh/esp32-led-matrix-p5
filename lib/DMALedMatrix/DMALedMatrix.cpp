#include "DMALedMatrix.h"

DMALedMatrix::DMALedMatrix()
{
    HUB75_I2S_CFG mxconfig(
        PANEL_RES_X, // module width
        PANEL_RES_Y, // module height
        PANEL_CHAIN  // Chain length
    );

    uint16_t myBLACK = dma_display->color565(0, 0, 0);
    uint16_t myWHITE = dma_display->color565(255, 255, 255);
    uint16_t myRED = dma_display->color565(255, 0, 0);
    uint16_t myGREEN = dma_display->color565(0, 255, 0);
    uint16_t myBLUE = dma_display->color565(0, 0, 255);

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setBrightness8(90); // 0-255
    dma_display->clearScreen();
    dma_display->fillScreen(myWHITE);
}

void DMALedMatrix::testDrawShape()
{
    // fix the screen with green
    dma_display->fillRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(0, 15, 0));
    delay(500);

    // draw a box in yellow
    dma_display->drawRect(0, 0, dma_display->width(), dma_display->height(), dma_display->color444(15, 15, 0));
    delay(500);

    // draw an 'X' in red
    dma_display->drawLine(0, 0, dma_display->width() - 1, dma_display->height() - 1, dma_display->color444(15, 0, 0));
    dma_display->drawLine(dma_display->width() - 1, 0, 0, dma_display->height() - 1, dma_display->color444(15, 0, 0));
    delay(500);

    // draw a blue circle
    dma_display->drawCircle(10, 10, 10, dma_display->color444(0, 0, 15));
    delay(500);

    // fill a violet circle
    dma_display->fillCircle(40, 21, 10, dma_display->color444(15, 0, 15));
    delay(500);

    // fill the screen with 'black'
    dma_display->fillScreen(dma_display->color444(0, 0, 0));
};

void DMALedMatrix::testDrawText(int colorWheelOffset)
{
    // draw text with a rotating colour
    dma_display->setTextSize(1);     // size 1 == 8 pixels high
    dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

    dma_display->setCursor(5, 0); // start at top left, with 8 pixel of spacing
    uint8_t w = 0;
    const char *str = "ESP32 DMA";
    for (w = 0; w < strlen(str); w++)
    {
        dma_display->setTextColor(this->colorWheel((w * 32) + colorWheelOffset));
        dma_display->print(str[w]);
    }

    dma_display->println();
    dma_display->print(" ");
    for (w = 9; w < 18; w++)
    {
        dma_display->setTextColor(this->colorWheel((w * 32) + colorWheelOffset));
        dma_display->print("*");
    }

    dma_display->println();

    dma_display->setTextColor(dma_display->color444(15, 15, 15));
    dma_display->println("LED MATRIX!");

    // print each letter with a fixed rainbow color
    dma_display->setTextColor(dma_display->color444(0, 8, 15));
    dma_display->print('3');
    dma_display->setTextColor(dma_display->color444(15, 4, 0));
    dma_display->print('2');
    dma_display->setTextColor(dma_display->color444(15, 15, 0));
    dma_display->print('x');
    dma_display->setTextColor(dma_display->color444(8, 15, 0));
    dma_display->print('6');
    dma_display->setTextColor(dma_display->color444(8, 0, 15));
    dma_display->print('4');

    // Jump a half character
    dma_display->setCursor(34, 24);
    dma_display->setTextColor(dma_display->color444(0, 15, 15));
    dma_display->print("*");
    dma_display->setTextColor(dma_display->color444(15, 0, 0));
    dma_display->print('R');
    dma_display->setTextColor(dma_display->color444(0, 15, 0));
    dma_display->print('G');
    dma_display->setTextColor(dma_display->color444(0, 0, 15));
    dma_display->print("B");
    dma_display->setTextColor(dma_display->color444(15, 0, 8));
    dma_display->println("*");
};

uint16_t DMALedMatrix::colorWheel(uint8_t pos)
{
    if (pos < 85)
    {
        return dma_display->color565(pos * 3, 255 - pos * 3, 0);
    }
    else if (pos < 170)
    {
        pos -= 85;
        return dma_display->color565(255 - pos * 3, 0, pos * 3);
    }
    else
    {
        pos -= 170;
        return dma_display->color565(0, pos * 3, 255 - pos * 3);
    }
}

void DMALedMatrix::drawText(uint8_t x, uint8_t y, const char *text, const char *hexColor, int size)
{
    dma_display->setTextSize(size);
    dma_display->setTextWrap(false);
    dma_display->setCursor(x, y);
    dma_display->setTextColor(this->convertHexToRGBColor(hexColor));
    dma_display->print(text);
}

void DMALedMatrix::drawTextWrap(uint8_t x, uint8_t y, const char *text, const char *hexColor, int size)
{
    dma_display->setTextSize(size);
    dma_display->setTextWrap(true);
    dma_display->setCursor(x, y);
    dma_display->setTextColor(this->convertHexToRGBColor(hexColor));
    dma_display->print(text);
}

void DMALedMatrix::drawColorWheelText(uint8_t x, uint8_t y, const char *text, int size, int colorWheelOffset)
{
    dma_display->setTextSize(size);
    dma_display->setTextWrap(false);
    dma_display->setCursor(x, y);
    for (uint8_t w = 0; w < strlen(text); w++)
    {
        dma_display->setTextColor(this->colorWheel((w * 32) + colorWheelOffset));
        dma_display->print(text[w]);
    }
}

void DMALedMatrix::drawXbm565(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const char *xbm, uint16_t color)
{
    if (width % 8 != 0)
    {
        width = ((width / 8) + 1) * 8;
    }
    for (int i = 0; i < width * height / 8; i++)
    {
        unsigned char charColumn = pgm_read_byte(xbm + i);
        for (int j = 0; j < 8; j++)
        {
            int targetX = (i * 8 + j) % width + x;
            int targetY = (8 * i / (width)) + y;
            if (bitRead(charColumn, j))
            {
                dma_display->drawPixel(targetX, targetY, color);
            }
        }
    }
}

void DMALedMatrix::clearScreen()
{
    dma_display->clearScreen();
}

uint16_t DMALedMatrix::convertHexToRGBColor(const char *hexColor)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    sscanf(hexColor, "#%02x%02x%02x", &red, &green, &blue);
    return dma_display->color565(red, green, blue);
}

void DMALedMatrix::drawGifs(String gifDir)
{
    root = FILESYSTEM.open(gifDir);
    if (root)
    {
        gifFile = root.openNextFile();
        while (gifFile)
        {
            if (!gifFile.isDirectory()) // play it
            {

                // C-strings... urghh...
                memset(filePath, 0x0, sizeof(filePath));
                strcpy(filePath, gifFile.path());

                // Show it.
                ShowGIF(filePath);
            }
            gifFile.close();
            gifFile = root.openNextFile();
        }
        root.close();
    } // root

    delay(1000); // pause before restarting
}

void DMALedMatrix::drawGif(char *filename)
{
    ShowGIF(filename);
}

void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y, iWidth;

    iWidth = pDraw->iWidth;
    if (iWidth > MATRIX_WIDTH)
        iWidth = MATRIX_WIDTH;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        int x, iCount;
        pEnd = s + pDraw->iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < pDraw->iWidth)
        {
            c = ucTransparent - 1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                }
                else // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            }           // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                for (int xOffset = 0; xOffset < iCount; xOffset++)
                {
                    dma_display->drawPixel(x + xOffset, y, usTemp[xOffset]); // 565 Color Format
                }
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent)
                    iCount++;
                else
                    s--;
            }
            if (iCount)
            {
                x += iCount; // skip these
                iCount = 0;
            }
        }
    }
    else // does not have transparency
    {
        s = pDraw->pPixels;
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        for (x = 0; x < pDraw->iWidth; x++)
        {
            dma_display->drawPixel(x, y, usPalette[*s++]); // color 565
        }
    }
} /* GIFDraw() */

void *GIFOpenFile(const char *fname, int32_t *pSize)
{
    Serial.print("Playing gif: ");
    Serial.println(fname);
    f = FILESYSTEM.open(fname);
    if (f)
    {
        *pSize = f.size();
        return (void *)&f;
    }
    return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
    File *f = static_cast<File *>(pHandle);
    if (f != NULL)
        f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
        iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
        return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
    int i = micros();
    File *f = static_cast<File *>(pFile->fHandle);
    f->seek(iPosition);
    pFile->iPos = (int32_t)f->position();
    i = micros() - i;
    //  Serial.printf("Seek time = %d us\n", i);
    return pFile->iPos;
} /* GIFSeekFile() */

void ShowGIF(char *name)
{
    start_tick = millis();

    if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
    {
        x_offset = (MATRIX_WIDTH - gif.getCanvasWidth()) / 2;
        if (x_offset < 0)
            x_offset = 0;
        y_offset = (MATRIX_HEIGHT - gif.getCanvasHeight()) / 2;
        if (y_offset < 0)
            y_offset = 0;
        Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
        Serial.flush();
        while (gif.playFrame(true, NULL))
        {
            if ((millis() - start_tick) > 8000)
            { // we'll get bored after about 8 seconds of the same looping gif
                break;
            }
        }
        gif.close();
    }

} /* ShowGIF() */
