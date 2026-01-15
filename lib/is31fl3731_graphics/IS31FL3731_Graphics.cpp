#include "IS31FL3731_Graphics.h"
#include <string.h>
#include <algorithm>

#define abs(x) ((x) < 0 ? -(x) : (x))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

IS31FL3731_Graphics::IS31FL3731_Graphics()
: driver_(nullptr), width_(0), height_(0), frame_(0)
{
}

IS31FL3731_Graphics::~IS31FL3731_Graphics()
{
    if(brightness_cache_ != nullptr)
    {
        delete[] brightness_cache_;
    }
}

bool IS31FL3731_Graphics::Init(const Config& config)
{
    if(config.driver == nullptr)
    {
        return false;
    }

    driver_     = config.driver;
    frame_      = config.frame;
    width_       = driver_->getWidth();
    height_      = driver_->getHeight();
    cache_size_  = width_ * height_;

    brightness_cache_ = new uint8_t[cache_size_];
    memset(brightness_cache_, 0, cache_size_);

    driver_->setFrame(frame_);
    clear();

    return true;
}

void IS31FL3731_Graphics::setPixel(int16_t x, int16_t y, uint8_t brightness)
{
    if(x < 0 || x >= width_ || y < 0 || y >= height_)
    {
        return;
    }

    uint16_t led_num = x + y * width_;
    brightness_cache_[led_num] = brightness;
    
    driver_->drawPixel(x, y, brightness);
}

void IS31FL3731_Graphics::clear()
{
    driver_->clear();
}

void IS31FL3731_Graphics::fill(uint8_t brightness)
{
    driver_->clear();
    
    uint8_t buffer[144];
    for(uint16_t i = 0; i < width_ * height_; i++)
    {
        buffer[i] = brightness;
    }
    
    writeBuffer(buffer, width_ * height_);
}

void IS31FL3731_Graphics::update()
{
    driver_->displayFrame(frame_);
}

void IS31FL3731_Graphics::writeBuffer(uint8_t* buffer, uint16_t size)
{
    uint16_t led_count = width_ * height_;
    if(size > led_count)
    {
        size = led_count;
    }

    uint8_t chunks = (size + 23) / 24;
    
    for(uint8_t chunk = 0; chunk < chunks; chunk++)
    {
        uint8_t cmd[25];
        cmd[0] = 0x24 + chunk * 24;
        memcpy(&cmd[1], &buffer[chunk * 24], 24);
        driver_->setLEDPWM(0, cmd[0], frame_);
    }
}

void IS31FL3731_Graphics::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness)
{
    int16_t dx = abs(x2 - x1);
    int16_t dy = abs(y2 - y1);
    int16_t sx = (x1 < x2) ? 1 : -1;
    int16_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx - dy;

    while(true)
    {
        setPixel(x1, y1, brightness);

        if(x1 == x2 && y1 == y2)
            break;

        int16_t e2 = 2 * err;

        if(e2 > dy)
        {
            err -= dy;
            x1 += sx;
        }

        if(e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void IS31FL3731_Graphics::drawHLine(int16_t x, int16_t y, int16_t w, uint8_t brightness)
{
    if(y < 0 || y >= height_)
        return;

    int16_t x_start = x;
    int16_t x_end = x + w;

    if(x_start < 0)
        x_start = 0;
    if(x_end > width_)
        x_end = width_;

    for(int16_t ix = x_start; ix < x_end; ix++)
    {
        setPixel(ix, y, brightness);
    }
}

void IS31FL3731_Graphics::drawVLine(int16_t x, int16_t y, int16_t h, uint8_t brightness)
{
    if(x < 0 || x >= width_)
        return;

    int16_t y_start = y;
    int16_t y_end = y + h;

    if(y_start < 0)
        y_start = 0;
    if(y_end > height_)
        y_end = height_;

    for(int16_t iy = y_start; iy < y_end; iy++)
    {
        setPixel(x, iy, brightness);
    }
}

void IS31FL3731_Graphics::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t brightness, bool fill)
{
    if(fill)
    {
        for(int16_t iy = y; iy < y + h && iy < height_; iy++)
        {
            drawHLine(x, iy, w, brightness);
        }
    }
    else
    {
        drawLine(x, y, x + w - 1, y, brightness);
        drawLine(x + w - 1, y, x + w - 1, y + h - 1, brightness);
        drawLine(x + w - 1, y + h - 1, x, y + h - 1, brightness);
        drawLine(x, y + h - 1, x, y, brightness);
    }
}

void IS31FL3731_Graphics::drawCircle(int16_t cx, int16_t cy, int16_t radius, uint8_t brightness, bool fill)
{
    int16_t x = 0;
    int16_t y = radius;
    int16_t err = 1 - radius;

    if(fill)
    {
        while(y >= x)
        {
            drawHLine(cx - x, cy - y, 2 * x + 1, brightness);
            drawHLine(cx - x, cy + y, 2 * x + 1, brightness);
            drawHLine(cx - y, cy - x, 2 * y + 1, brightness);
            drawHLine(cx - y, cy + x, 2 * y + 1, brightness);

            if(err < 0)
            {
                err += 2 * x + 3;
            }
            else
            {
                y--;
                err += 2 * (x - y) + 5;
            }
            x++;
        }
    }
    else
    {
        while(y >= x)
        {
            setPixel(cx + x, cy - y, brightness);
            setPixel(cx - x, cy - y, brightness);
            setPixel(cx + x, cy + y, brightness);
            setPixel(cx - x, cy + y, brightness);
            setPixel(cx + y, cy - x, brightness);
            setPixel(cx - y, cy - x, brightness);
            setPixel(cx + y, cy + x, brightness);
            setPixel(cx - y, cy + x, brightness);

            if(err < 0)
            {
                err += 2 * x + 3;
            }
            else
            {
                y--;
                err += 2 * (x - y) + 5;
            }
            x++;
        }
    }
}

void IS31FL3731_Graphics::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness, bool fill)
{
    if(fill)
    {
        int16_t coords[] = {x0, y0, x1, y1, x2, y2};

        int16_t minY = min(min(y0, y1), y2);
        int16_t maxY = max(max(y0, y1), y2);

        for(int16_t y = minY; y <= maxY; y++)
        {
            if(y < 0 || y >= height_)
                continue;

            int16_t xMin = width_ - 1;
            int16_t xMax = 0;

            bool foundEdge = false;

            for(int16_t i = 0; i < 3; i++)
            {
                int16_t j = (i + 1) % 3;
                int16_t y1_i = coords[2 * i + 1];
                int16_t y2_j = coords[2 * j + 1];
                int16_t x1_i = coords[2 * i];
                int16_t x2_j = coords[2 * j];

                if((y >= y1_i && y < y2_j) || (y >= y2_j && y < y1_i))
                {
                    float t = (float)(y - y1_i) / (y2_j - y1_i);
                    int16_t x = x1_i + (int16_t)(t * (x2_j - x1_i));

                    if(x < xMin)
                        xMin = x;
                    if(x > xMax)
                        xMax = x;
                    foundEdge = true;
                }
            }

            if(foundEdge)
            {
                if(xMin < 0)
                    xMin = 0;
                if(xMax >= width_)
                    xMax = width_ - 1;

                drawHLine(xMin, y, xMax - xMin + 1, brightness);
            }
        }
    }
    else
    {
        drawLine(x0, y0, x1, y1, brightness);
        drawLine(x1, y1, x2, y2, brightness);
        drawLine(x2, y2, x0, y0, brightness);
    }
}

void IS31FL3731_Graphics::drawEllipse(int16_t cx, int16_t cy, int16_t rx, int16_t ry, uint8_t brightness, bool fill)
{
    if(rx < 1 || ry < 1)
        return;

    int16_t x = 0;
    int16_t y = ry;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t p = ry2 - (rx2 * ry) + (rx2 / 4);

    while(x * ry2 < y * rx2)
    {
        if(fill)
        {
            drawHLine(cx - x, cy - y, 2 * x + 1, brightness);
            drawHLine(cx - x, cy + y, 2 * x + 1, brightness);
        }
        else
        {
            setPixel(cx + x, cy - y, brightness);
            setPixel(cx - x, cy - y, brightness);
            setPixel(cx + x, cy + y, brightness);
            setPixel(cx - x, cy + y, brightness);
        }

        x++;
        if(p < 0)
        {
            p += 2 * ry2 * x + ry2;
        }
        else
        {
            y--;
            p += 2 * ry2 * x - 2 * rx2 * y + ry2;
        }
    }

    p = ry2 * (x + 0.5) * (x + 0.5) + rx2 * (y - 1) * (y - 1) - rx2 * ry2;

    while(y >= 0)
    {
        if(fill)
        {
            drawHLine(cx - x, cy - y, 2 * x + 1, brightness);
            drawHLine(cx - x, cy + y, 2 * x + 1, brightness);
        }
        else
        {
            setPixel(cx + x, cy - y, brightness);
            setPixel(cx - x, cy - y, brightness);
            setPixel(cx + x, cy + y, brightness);
            setPixel(cx - x, cy + y, brightness);
        }

        y--;
        if(p > 0)
        {
            p += -2 * rx2 * y + rx2;
        }
        else
        {
            x++;
            p += 2 * ry2 * x - 2 * rx2 * y + rx2;
        }
    }
}

void IS31FL3731_Graphics::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t brightness, bool fill)
{
    if(w < 1 || h < 1)
        return;

    if(r < 0)
        r = 0;
    if(r * 2 > w)
        r = w / 2;
    if(r * 2 > h)
        r = h / 2;

    int16_t x1 = x + r;
    int16_t y1 = y + r;
    int16_t x2 = x + w - 1 - r;
    int16_t y2 = y + h - 1 - r;

    if(fill)
    {
        for(int16_t iy = y1; iy <= y2; iy++)
        {
            drawHLine(x1, iy, x2 - x1 + 1, brightness);
        }

        for(int16_t iy = y; iy <= y2; iy++)
        {
            for(int16_t ix = x; ix < x1; ix++)
            {
                int16_t dx = x1 - ix;
                int16_t dy = iy - y1;
                if(dx * dx + dy * dy <= r * r)
                {
                    setPixel(ix, iy, brightness);
                }
            }
        }

        for(int16_t iy = y; iy <= y2; iy++)
        {
            for(int16_t ix = x2 + 1; ix <= x + w - 1; ix++)
            {
                int16_t dx = ix - x2;
                int16_t dy = iy - y1;
                if(dx * dx + dy * dy <= r * r)
                {
                    setPixel(ix, iy, brightness);
                }
            }
        }

        for(int16_t iy = y2 + 1; iy <= y + h - 1; iy++)
        {
            for(int16_t ix = x; ix < x1; ix++)
            {
                int16_t dx = x1 - ix;
                int16_t dy = iy - y2;
                if(dx * dx + dy * dy <= r * r)
                {
                    setPixel(ix, iy, brightness);
                }
            }
        }

        for(int16_t iy = y2 + 1; iy <= y + h - 1; iy++)
        {
            for(int16_t ix = x2 + 1; ix <= x + w - 1; ix++)
            {
                int16_t dx = ix - x2;
                int16_t dy = iy - y2;
                if(dx * dx + dy * dy <= r * r)
                {
                    setPixel(ix, iy, brightness);
                }
            }
        }
    }
    else
    {
        drawLine(x1, y, x2, y, brightness);
        drawLine(x2, y, x2, y2, brightness);
        drawLine(x2, y2, x1, y2, brightness);
        drawLine(x1, y2, x1, y, brightness);

        int16_t cx = x + r;
        int16_t cy = y + r;
        int16_t c_x = 0;
        int16_t c_y = r;
        int32_t p = 1 - r;

        while(c_y >= c_x)
        {
            setPixel(cx + c_x, cy - c_y, brightness);
            setPixel(cx - c_x, cy - c_y, brightness);

            c_x++;
            if(p < 0)
            {
                p += 2 * c_x + 1;
            }
            else
            {
                c_y--;
                p += 2 * (c_x - c_y) + 1;
            }
        }

        cx = x + w - 1 - r;
        cy = y + r;
        c_x = 0;
        c_y = r;
        p = 1 - r;

        while(c_y >= c_x)
        {
            setPixel(cx + c_x, cy - c_y, brightness);
            setPixel(cx - c_x, cy - c_y, brightness);

            c_x++;
            if(p < 0)
            {
                p += 2 * c_x + 1;
            }
            else
            {
                c_y--;
                p += 2 * (c_x - c_y) + 1;
            }
        }

        cx = x + r;
        cy = y + h - 1 - r;
        c_x = 0;
        c_y = r;
        p = 1 - r;

        while(c_y >= c_x)
        {
            setPixel(cx + c_x, cy + c_y, brightness);
            setPixel(cx - c_x, cy + c_y, brightness);

            c_x++;
            if(p < 0)
            {
                p += 2 * c_x + 1;
            }
            else
            {
                c_y--;
                p += 2 * (c_x - c_y) + 1;
            }
        }

        cx = x + w - 1 - r;
        cy = y + h - 1 - r;
        c_x = 0;
        c_y = r;
        p = 1 - r;

        while(c_y >= c_x)
        {
            setPixel(cx + c_x, cy + c_y, brightness);
            setPixel(cx - c_x, cy + c_y, brightness);

            c_x++;
            if(p < 0)
            {
                p += 2 * c_x + 1;
            }
            else
            {
                c_y--;
                p += 2 * (c_x - c_y) + 1;
            }
        }
    }
}

void IS31FL3731_Graphics::fadeAll(uint8_t target, uint8_t step)
{
    bool any_changed;
    
    do
    {
        any_changed = false;
        
        for(uint16_t i = 0; i < cache_size_; i++)
        {
            uint8_t current = brightness_cache_[i];
            
            if(current != target)
            {
                uint8_t next_val;
                if(current < target)
                    next_val = min(target, current + step);
                else
                    next_val = max(target, current - step);
                
                brightness_cache_[i] = next_val;
                
                int16_t x = i % width_;
                int16_t y = i / width_;
                
                setPixel(x, y, next_val);
                any_changed = true;
            }
        }
        
        if(any_changed)
        {
            update();
            System::Delay(10);
        }
    } while(any_changed);
}

void IS31FL3731_Graphics::fadePixel(int16_t x, int16_t y, uint8_t target, uint8_t step)
{
    if(x < 0 || x >= width_ || y < 0 || y >= height_)
        return;

    uint16_t led_num = x + y * width_;
    
    while(brightness_cache_[led_num] != target)
    {
        uint8_t current = brightness_cache_[led_num];
        uint8_t next_val;
        
        if(current < target)
            next_val = min(target, current + step);
        else
            next_val = max(target, current - step);
        
        setPixel(x, y, next_val);
        update();
    }
}
