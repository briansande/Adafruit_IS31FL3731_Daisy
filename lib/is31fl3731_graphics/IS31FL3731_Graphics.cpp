#include "is31fl3731_graphics/IS31FL3731_Graphics.h"
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

    uint16_t total_pixels = width_ * height_;
    uint8_t* temp_buffer = new uint8_t[total_pixels];
    uint16_t buffer_idx = 0;

    auto draw_buf_pixel = [&](int16_t bx, int16_t by) {
        if(bx >= 0 && bx < width_ && by >= 0 && by < height_ && buffer_idx < total_pixels)
        {
            temp_buffer[buffer_idx++] = brightness;
        }
    };

    while(y > 0)
    {
        draw_buf_pixel(cx + x, cy + y);
        draw_buf_pixel(cx - x, cy + y);

        if(err < 0)
        {
            x++;
            err += 2 * y + 1;
        }
        else
        {
            y--;
            err -= 2 * x + 1;
        }
    }

    draw_buf_pixel(cx + radius, cy);

    if(fill)
    {
        y--;
        while(x > y)
        {
            int16_t scan_start = x - y + 1;
            for(int16_t iy = cy - y; iy <= cy + y; iy++)
            {
                for(int16_t ix = cx - scan_start; ix <= cx + scan_start; ix++)
                {
                    draw_buf_pixel(ix, iy);
                }
            }

            if(err < 0)
            {
                x++;
                err += 2 * y + 1;
            }
            else
            {
                y--;
                err -= 2 * x + 1;
            }
        }
    }

    if(buffer_idx > 0)
    {
        writeBuffer(temp_buffer, buffer_idx);
    }

    delete[] temp_buffer;
}

void IS31FL3731_Graphics::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness, bool fill)
{
    if(fill)
    {
        int16_t a, b, y, last;

        a = y1 - y0;
        b = y2 - y0;

        int16_t dx01 = x1 - x0;
        int16_t dx02 = x0 - x2;
        int16_t dx12 = x2 - x1;

        int16_t dy01 = y1 - y0;
        int16_t dy02 = y0 - y2;
        int16_t dy12 = y2 - y1;

        int16_t sa = 0;
        int16_t sb = 0;

        bool c = false;

        if(y1 < y0)
        {
            if(y1 < y2)
                for(int16_t x = x0; x <= x1; x++)
                {
                    y = x - x0;
                    setPixel(x, y0 + y, brightness);
                }
            else
                for(int16_t x = x0; x <= x2; x++)
                {
                    y = x - x0;
                    setPixel(x, y0 + y, brightness);
                }
            c = true;
        }

        if(!c)
        {
            if(y0 < y1)
            {
                if(b * a <= dy01)
                {
                    for(int16_t x = x0; x <= x2; x++)
                    {
                        setPixel(x, y0 + (a * x) / dx01, brightness);
                    }
                }
                else
                {
                    for(int16_t x = x0; x <= x1; x++)
                    {
                        setPixel(x, y0 + (b * x) / dy01, brightness);
                    }
                }
            }
            else
            {
                if(b * a <= dy01)
                {
                    for(int16_t x = x0; x <= x2; x++)
                    {
                        setPixel(x, y0 + (a * x) / dy01, brightness);
                    }
                }
                else
                {
                    for(int16_t x = x0; x <= x1; x++)
                    {
                        setPixel(x, y0 + (b * x) / dy01, brightness);
                    }
                }
            }
        }

        int16_t p;
        int16_t t;
        int16_t q;
        int16_t d;

        if(c)
        {
            d = a - b;
            if(b > a)
            {
                p = a + 1;
                t = b;
                q = b + d / p;
                for(int16_t x = x2; x >= x1; x--)
                {
                    setPixel(x, y1 + (t * (x - x1) / d, brightness);
                }
            }
            else
            {
                p = b + 1;
                t = b;
                q = b + d / p;
                for(int16_t x = x1; x <= x2; x++)
                {
                    setPixel(x, y1 + (t * (x - x1) / d, brightness);
                }
            }
        }
        else
        {
            d = b - a;
            if(b > a)
            {
                p = a + 1;
                t = a;
                q = a + d / p;
                for(int16_t x = x0; x <= x2; x++)
                {
                    setPixel(x, y0 + (t * (x - x0)) / d, brightness);
                }
            }
            else
            {
                p = a + 1;
                t = a;
                q = a + d / p;
                for(int16_t x = x1; x <= x0; x--)
                {
                    setPixel(x, y0 + (t * (x - x0)) / d, brightness);
                }
            }
        }

        if(c)
        {
            while(p < abs(b - a))
        {
            p++;
            if(b < a)
            {
                for(int16_t x = x2; x > x1; x--)
                {
                    setPixel(x, y1 + (t * (x - x1)) / d, brightness);
                }
            }
            else
            {
                for(int16_t x = x1; x < x2; x++)
                {
                    setPixel(x, y1 + (t * (x - x1)) / d, brightness);
                }
            }
            q -= d;
            t -= q;
            if(t < 0)
            {
                t += p;
                q += p;
            }
            else
            {
                t -= p;
                q -= p;
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
    uint16_t total_pixels = width_ * height_;
    uint8_t* temp_buffer = new uint8_t[total_pixels];
    uint16_t buffer_idx = 0;

    auto draw_buf_pixel = [&](int16_t bx, int16_t by) {
        if(bx >= 0 && bx < width_ && by >= 0 && by < height_ && buffer_idx < total_pixels)
        {
            temp_buffer[buffer_idx++] = brightness;
        }
    };

    int16_t x0 = rx;
    int16_t y0 = 0;

    int16_t x1 = 0;
    int16_t y1 = ry;

    int16_t x2 = 0;
    int16_t y2 = 2 * ry - 1;

    int32_t d = 4 * (ry * ry) - (rx * rx);

    while(y1 < y2)
    {
        draw_buf_pixel(cx + x0, cy - y0);
        draw_buf_pixel(cx - x0, cy - y0);
        draw_buf_pixel(cx + x0, cy + y0);
        draw_buf_pixel(cx - x0, cy - y0);

        x0++;

        if(d >= 0)
        {
            d += 2 * rx * (x1 - x0 + 1) + (ry * ry);
            y1++;
        }
        else
        {
            d -= 2 * ry * (y1 - y0 + 1) + (rx * rx);
            x0--;
        }
    }

    if(fill)
    {
        int16_t y = ry;
        int32_t d = (ry * ry) - (rx * rx);

        while(y > 0)
        {
            int16_t x = 0;

            draw_buf_pixel(cx + rx, cy - y);
            draw_buf_pixel(cx - rx, cy - y);

            int32_t e = (rx * rx) - (ry * ry);

            while(2 * rx * y <= d)
            {
                draw_buf_pixel(cx + x, cy - y);
                draw_buf_pixel(cx - x, cy - y);

                x++;
                e += 2 * ry * (1 + y);
            }

            draw_buf_pixel(cx + x, cy - y);
            draw_buf_pixel(cx - x, cy - y);

            d += (2 * rx + (2 * ry + 1) * (1 - y));
            y--;
        }
    }

    if(buffer_idx > 0)
    {
        writeBuffer(temp_buffer, buffer_idx);
    }

    delete[] temp_buffer;
}

void IS31FL3731_Graphics::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t brightness, bool fill)
{
    if(r * 2 > w)
        r = w / 2;
    if(r * 2 > h)
        r = h / 2;

    uint16_t total_pixels = width_ * height_;
    uint8_t* temp_buffer = new uint8_t[total_pixels];
    uint16_t buffer_idx = 0;

    auto draw_buf_pixel = [&](int16_t bx, int16_t by) {
        if(bx >= 0 && bx < width_ && by >= 0 && by < height_ && buffer_idx < total_pixels)
        {
            temp_buffer[buffer_idx++] = brightness;
        }
    };

    int16_t x1 = x + r;
    int16_t y1 = y + r;
    int16_t x2 = x + w - 1 - r;
    int16_t y2 = y + h - 1 - r;

    if(fill)
    {
        drawRect(x + 1, y + 1, w - 2, h - 2, brightness, true);

        drawLine(x1, y, x2, y, brightness);
        drawRect(x1 + 1, y + 1, x2 - x1, y2 - y, brightness, true);
        drawLine(x, y1, x, y2, brightness);
        drawRect(x + 1, y1 + 1, w - 2, y2 - y1, brightness, true);
    }
    else
    {
        drawLine(x1, y, x2, y, brightness);
        drawLine(x2, y, x, y2, brightness);
        drawRect(x1 + 1, y + 1, x2 - x1, y2 - y, brightness, true);
        drawLine(x, y1, x, y2, brightness);
        drawRect(x + 1, y1 + 1, w - 2, y2 - y1, brightness, true);
    }

    drawCircle(x + r, y + r, r, brightness, false);
    drawCircle(x + w - 1 - r, y + r, r, brightness, false);
    drawCircle(x + r, y + h - 1 - r, r, brightness, false);
    drawCircle(x + w - 1 - r, y + h - 1 - r, r, brightness, false);

    if(buffer_idx > 0)
    {
        writeBuffer(temp_buffer, buffer_idx);
    }

    delete[] temp_buffer;
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
    uint8_t current = driver_->getLEDPWM(led_num, frame_);
    
    if(current != target)
    {
        uint8_t next_val;
        if(current < target)
            next_val = min(target, current + step);
        else
            next_val = max(target, current - step);
        
        setPixel(x, y, next_val);
        update();
    }
}
