#pragma once

#ifndef IS31FL3731_GRAPHICS_H
#define IS31FL3731_GRAPHICS_H

#include "daisy_seed.h"
#include "../is31fl3731/is31fl3731.h"
#include <stdint.h>

using namespace daisy;

class IS31FL3731_Graphics
{
  public:
    struct Config
    {
        IS31FL3731* driver;
        uint8_t      frame;
        
        void Defaults()
        {
            driver = nullptr;
            frame  = 0;
        }
    };

    IS31FL3731_Graphics();
    ~IS31FL3731_Graphics();

    bool Init(const Config& config);

    void setPixel(int16_t x, int16_t y, uint8_t brightness);
    void clear();
    void fill(uint8_t brightness);
    void update();

    void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness);
    void drawHLine(int16_t x, int16_t y, int16_t w, uint8_t brightness);
    void drawVLine(int16_t x, int16_t y, int16_t h, uint8_t brightness);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t brightness, bool fill = false);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t brightness);
    void drawCircle(int16_t x, int16_t y, int16_t radius, uint8_t brightness, bool fill = false);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness, bool fill = false);
    void drawEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t brightness, bool fill = false);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t brightness, bool fill = false);

    void fadeAll(uint8_t target, uint8_t step = 10);
    void fadePixel(int16_t x, int16_t y, uint8_t target, uint8_t step = 10);

    uint16_t width() const { return width_; }
    uint16_t height() const { return height_; }

  private:
    IS31FL3731* driver_;
    uint16_t        width_;
    uint16_t        height_;
    uint8_t         frame_;
    uint8_t*        brightness_cache_;
    uint16_t        cache_size_;
    
    void writeBuffer(uint8_t* buffer, uint16_t size);
};

#endif
