#include <ctime>
#include <cstring>
#include "Watchy.h"

#ifndef _MAX_INT_DIG
#define _MAX_INT_DIG    32
#endif

Watchy::Watchy() {
    resetTime();
}

void Watchy::drawWatchFace() {
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(5, 53 + 60);
    if (currentTime.Hour < 10) {
        display.print("0");
    }
    display.print(currentTime.Hour);
    display.print(":");
    if (currentTime.Minute < 10) {
        display.print("0");
    }
    display.println(currentTime.Minute);
}

void Watchy::init() {}

void Watchy::showWatchFace()
{
    drawWatchFace();
}

float Watchy::getBatteryVoltage()
{
    return currentVoltage;
}

void Watchy::setBatteryVoltage(float voltage)
{
    currentVoltage = voltage;
}

void Watchy::setBluetooth(bool enabled)
{
    BLE_CONFIGURED = enabled;
}

void Watchy::setWifi(bool enabled)
{
    WIFI_CONFIGURED = enabled;
}

void Watchy::setSteps(uint32_t stepCount)
{
    sensor.setSteps(stepCount);
}

void Watchy::setWeatherCode(int16_t weatherConditionCode)
{
    currentWeather.weatherConditionCode = weatherConditionCode;
}

void Watchy::setWeatherExternal(bool external)
{
    currentWeather.external = external;
}

void Watchy::setTemperatureUnitMetric(bool isMetric)
{
    if (currentWeather.isMetric != isMetric)
    {
        currentWeather.isMetric = isMetric;
        if (isMetric)
        {
            currentWeather.temperature = (int8_t)((currentWeather.temperature - 32) * 5.0 / 9.0);
        }
        else
        {
            currentWeather.temperature = (int8_t)(currentWeather.temperature * 9.0 / 5.0 + 32.0);
        }
    }
}

void Watchy::setTemperature(int8_t temperature)
{
    if (!currentWeather.isMetric) {
        temperature = (int8_t) (temperature * 9.0 / 5.0 + 32.0);
    }

    currentWeather.temperature = temperature;
}

int8_t Watchy::getTemperature()
{
    return currentWeather.temperature;
}

void Watchy::setTime(tm newTime)
{
    currentTime.Wday = newTime.tm_wday + 1;
    currentTime.Day = newTime.tm_mday;
    currentTime.Month = newTime.tm_mon + 1;
    currentTime.Year = newTime.tm_year + 1900 - 1970;
    currentTime.Hour = newTime.tm_hour;
    currentTime.Minute = newTime.tm_min;
    currentTime.Second = newTime.tm_sec;
}

void Watchy::resetTime()
{
    time_t curr_time;
    curr_time = time(NULL);

    struct tm tm_local;
    localtime_r(&curr_time, &tm_local);
    currentTime.Wday = tm_local.tm_wday + 1;
    currentTime.Day = tm_local.tm_mday;
    currentTime.Month = tm_local.tm_mon + 1;
    currentTime.Year = tm_local.tm_year + 1900 - 1970;
    currentTime.Hour = tm_local.tm_hour;
    currentTime.Minute = tm_local.tm_min;
    currentTime.Second = tm_local.tm_sec;
}

weatherData Watchy::getWeatherData()
{
    return currentWeather;
}

const char* Watchy::dayShortStr(uint8_t day)
{
    const char *dayStr[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    return dayStr[day - 1];
}

const char* Watchy::dayStr(uint8_t day)
{
    const char *dayStr[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
    return dayStr[day - 1];
}

const char* Watchy::monthShortStr(uint8_t month)
{
    const char *monthShortStr[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return monthShortStr[month - 1];
}

// ─── DisplaySim ──────────────────────────────────────────────────────────────

DisplaySim::DisplaySim()
    : currentX(0), currentY(0), currentFontColor(GxEPD_WHITE), currentFont(nullptr)
{
    std::memset(pixels, 0, sizeof(pixels));
}

void DisplaySim::putPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || y < 0 || x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) return;
    uint8_t v = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
    size_t off = ((size_t)y * DISPLAY_WIDTH + (size_t)x) * 3;
    pixels[off + 0] = v;
    pixels[off + 1] = v;
    pixels[off + 2] = v;
}

void DisplaySim::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color)
{
    drawBitmapRaw(x, y, bitmap, w, h, color, false);
}

void DisplaySim::drawFontBitmap(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color)
{
    drawBitmapRaw(x, y, bitmap, w, h, color, true);
}

void DisplaySim::drawBitmapRaw(int16_t x, int16_t y, const uint8_t *bitmap, uint16_t w, uint16_t h, uint16_t color, bool rasterMode)
{
    int bitmapOffset = 0;
    if (!rasterMode)
    {
        bitmapOffset--;
    }

    int bitOffset = 128;

    for (int currentY = 0; currentY < h; currentY++)
    {
        for (int currentX = 0; currentX < w; currentX++, bitOffset = bitOffset >> 1)
        {
            if (
                (rasterMode && (bitOffset == 0)) ||
                (!rasterMode && ((currentX % 8) == 0)))
            {
                bitOffset = 128;
                bitmapOffset++;
            }

            if (bitmap[bitmapOffset] & bitOffset)
            {
                putPixel(x + currentX, y + currentY, color);
            }
        }
    }
}

void DisplaySim::fillScreen(uint16_t color)
{
    uint8_t v = (color == GxEPD_BLACK) ? 0x00 : 0xFF;
    std::memset(pixels, v, sizeof(pixels));
}

void DisplaySim::setTextColor(uint16_t color)
{
    currentFontColor = color;
}

void DisplaySim::setFont(const GFXfont *f)
{
    currentFont = f;
}

void DisplaySim::setCursor(int16_t x, int16_t y)
{
    currentX = x;
    currentY = y;
}

void DisplaySim::println(String text)
{
    println(text.c_str());
}

void DisplaySim::println(const char* text)
{
    print(text);

    currentY += currentFont->yAdvance;
    currentX = 0;
}

void DisplaySim::println(char text)
{
    print(text);

    currentY += currentFont->yAdvance;
    currentX = 0;
}

void DisplaySim::println(uint8_t number)
{
    char buffer[_MAX_INT_DIG];
    _itoa_s<_MAX_INT_DIG>(number, buffer, 10);
    println(buffer);
}

void DisplaySim::println(uint32_t number)
{
    char buffer[_MAX_INT_DIG];
    _itoa_s<_MAX_INT_DIG>(number, buffer, 10);
    println(buffer);
}

void DisplaySim::println(int32_t number)
{
    char buffer[_MAX_INT_DIG];
    _itoa_s<_MAX_INT_DIG>(number, buffer, 10);
    println(buffer);
}

void DisplaySim::print(String text)
{
    print(text.c_str());
}

void DisplaySim::print(const char* text)
{
    for (size_t currentChar = 0; currentChar < strlen(text); currentChar++)
    {
        char itemToPrint = text[currentChar];
        print(itemToPrint);
    }
}

void DisplaySim::print(char text)
{
    int fontOffset = text - currentFont->first;

    GFXglyph currentGlyph = currentFont->glyph[fontOffset];
    int bitmapOffset = currentGlyph.bitmapOffset;

    drawFontBitmap(currentX + currentGlyph.xOffset, currentY + currentGlyph.yOffset,
        &currentFont->bitmap[bitmapOffset], currentGlyph.width, currentGlyph.height, currentFontColor);
    currentX += currentGlyph.xAdvance;
}

void DisplaySim::print(uint8_t number)
{
    char buffer[_MAX_INT_DIG];
    _itoa_s<_MAX_INT_DIG>(number, buffer, 10);
    print(buffer);
}

void DisplaySim::print(uint32_t number)
{
    char buffer[_MAX_INT_DIG];
    _itoa_s<_MAX_INT_DIG>(number, buffer, 10);
    print(buffer);
}

void DisplaySim::charBounds(unsigned char c, int16_t *x, int16_t *y,
    int16_t *minx, int16_t *miny, int16_t *maxx,
    int16_t *maxy) {

    if (currentFont) {

        if (c == '\n') {
            *x = 0;
            *y += currentFont->yAdvance;
        }
        else if (c != '\r') {
            uint8_t first = currentFont->first, last = currentFont->last;
            if ((c >= first) && (c <= last)) {
                GFXglyph glyph = currentFont->glyph[c - first];
                uint8_t gw = glyph.width, gh = glyph.height, xa = glyph.xAdvance;
                int8_t xo = glyph.xOffset, yo = glyph.yOffset;
                if (wrap && ((*x + (((int16_t)xo + gw))) > _width)) {
                    *x = 0;
                    *y += (uint8_t) currentFont->yAdvance;
                }
                int16_t tsx = (int16_t)1, tsy = (int16_t)1,
                    x1 = *x + xo * tsx, y1 = *y + yo * tsy, x2 = x1 + gw * tsx - 1,
                    y2 = y1 + gh * tsy - 1;
                if (x1 < *minx)
                    *minx = x1;
                if (y1 < *miny)
                    *miny = y1;
                if (x2 > *maxx)
                    *maxx = x2;
                if (y2 > *maxy)
                    *maxy = y2;
                *x += xa * tsx;
            }
        }

    }
    else {

        if (c == '\n') {
            *x = 0;
            *y += 8;
        }
        else if (c != '\r') {
            if (wrap && ((*x + 6) > _width)) {
                *x = 0;
                *y += 8;
            }
            int x2 = *x + 6 - 1,
                y2 = *y + 8 - 1;
            if (x2 > *maxx)
                *maxx = x2;
            if (y2 > *maxy)
                *maxy = y2;
            if (*x < *minx)
                *minx = *x;
            if (*y < *miny)
                *miny = *y;
            *x += 6;
        }
    }
}

void DisplaySim::getTextBounds(String str, int16_t x, int16_t y,
    int16_t *x1, int16_t *y1, uint16_t *w,
    uint16_t *h)
{
    getTextBounds(str.c_str(), x, y, x1, y1, w, h);
}

void DisplaySim::getTextBounds(const char *str, int16_t x, int16_t y,
    int16_t *x1, int16_t *y1, uint16_t *w,
    uint16_t *h) {

    uint8_t c;
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1;

    *x1 = x;
    *y1 = y;
    *w = *h = 0;

    while ((c = *str++)) {
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }

    if (maxx >= minx) {
        *x1 = minx;
        *w = maxx - minx + 1;
    }
    if (maxy >= miny) {
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}

void DisplaySim::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    putPixel(x, y, color);
}

void DisplaySim::writePixel(int16_t x, int16_t y, uint16_t color) {
    drawPixel(x, y, color);
}

void DisplaySim::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    int16_t dx = std::abs(x1 - x0);
    int16_t dy = -std::abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    while (true) {
        putPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void DisplaySim::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    for (int16_t i = 0; i < h; i++) putPixel(x, y + i, color);
}

void DisplaySim::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    for (int16_t i = 0; i < w; i++) putPixel(x + i, y, color);
}

void DisplaySim::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    drawFastVLine(x, y, h, color);
}

void DisplaySim::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    drawFastHLine(x, y, w, color);
}

void DisplaySim::startWrite() {}
void DisplaySim::endWrite()   {}

void DisplaySim::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    writeFastHLine(x, y + h - 1, w, color);
    writeFastVLine(x, y, h, color);
    writeFastVLine(x + w - 1, y, h, color);
    endWrite();
}

void DisplaySim::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    startWrite();
    for (int16_t i = x; i < x + w; i++) {
        writeFastVLine(i, y, h, color);
    }
    endWrite();
}

void DisplaySim::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    fillRect(x, y, w, h, color);
}

void DisplaySim::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    startWrite();
    writePixel(x0, y0 + r, color);
    writePixel(x0, y0 - r, color);
    writePixel(x0 + r, y0, color);
    writePixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}

void DisplaySim::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
}

void DisplaySim::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    startWrite();
    writeFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
    endWrite();
}

void DisplaySim::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {

    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    int16_t px = x;
    int16_t py = y;

    delta++;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (x < (y + 1)) {
            if (corners & 1)
                writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
            if (corners & 2)
                writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
        }
        if (y != py) {
            if (corners & 1)
                writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
            if (corners & 2)
                writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
            py = y;
        }
        px = x;
    }
}

void DisplaySim::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2;
    if (r > max_radius)
        r = max_radius;
    startWrite();
    writeFastHLine(x + r, y, w - 2 * r, color);
    writeFastHLine(x + r, y + h - 1, w - 2 * r, color);
    writeFastVLine(x, y + r, h - 2 * r, color);
    writeFastVLine(x + w - 1, y + r, h - 2 * r, color);
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
    endWrite();
}

void DisplaySim::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2;
    if (r > max_radius)
        r = max_radius;
    startWrite();
    writeFillRect(x + r, y, w - 2 * r, h, color);
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
    endWrite();
}

void DisplaySim::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

void DisplaySim::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

    int16_t a, b, y, last;

    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    startWrite();
    if (y0 == y2) {
        a = b = x0;
        if (x1 < a)
            a = x1;
        else if (x1 > b)
            b = x1;
        if (x2 < a)
            a = x2;
        else if (x2 > b)
            b = x2;
        writeFastHLine(a, y0, b - a + 1, color);
        endWrite();
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
        dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    if (y1 == y2)
        last = y1;
    else
        last = y1 - 1;

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }

    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b)
            _swap_int16_t(a, b);
        writeFastHLine(a, y, b - a + 1, color);
    }
    endWrite();
}


uint32_t SensorSim::getCounter()
{
    return stepCount;
}

void SensorSim::resetStepCounter()
{
    stepCount = 0;
}

void SensorSim::setSteps(uint32_t stepCount)
{
    this->stepCount = stepCount;
}
