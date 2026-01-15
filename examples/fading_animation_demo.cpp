#include "daisy_seed.h"
#include "../lib/is31fl3731/is31fl3731.h"
#include "../lib/is31fl3731_graphics/IS31FL3731_Graphics.h"

using namespace daisy;

DaisySeed hw;
IS31FL3731 ledmatrix;
IS31FL3731_Graphics display;

int main(void)
{
    hw.Init();

    I2CHandle::Config i2c_conf;
    i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
    i2c_conf.mode = I2CHandle::Config::Mode::I2C_MASTER;
    i2c_conf.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_conf.pin_config.scl = {DSY_GPIOB, 8};
    i2c_conf.pin_config.sda = {DSY_GPIOB, 9};

    I2CHandle i2c_handle;
    if(i2c_handle.Init(i2c_conf) != I2CHandle::Result::OK)
    {
        return -1;
    }

    if(!ledmatrix.begin(0x74, &i2c_handle))
    {
        return -1;
    }

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix;
    gfx_cfg.frame = 0;
    display.Init(gfx_cfg);

    uint8_t brightness[16][9] = {0};
    int16_t x = 8;
    int16_t y = 4;
    int16_t dx = 1;
    int16_t dy = 1;
    const uint8_t FADE_AMOUNT = 40;
    const uint32_t ANIMATION_DELAY = 30;
    uint32_t demo_phase = 0;
    uint32_t phase_counter = 0;
    uint8_t current_brightness = 0;

    while(1)
    {
        System::Delay(ANIMATION_DELAY);

        if(demo_phase == 0)
        {
            for(int16_t py = 0; py < 9; py++)
            {
                for(int16_t px = 0; px < 16; px++)
                {
                    if(brightness[px][py] > FADE_AMOUNT)
                    {
                        brightness[px][py] -= FADE_AMOUNT;
                    }
                    else
                    {
                        brightness[px][py] = 0;
                    }
                    display.setPixel(px, py, brightness[px][py]);
                }
            }

            brightness[x][y] = 255;
            display.setPixel(x, y, 255);
            display.update();

            x += dx;
            y += dy;
            if(x <= 0 || x >= 15)
            {
                dx = -dx;
            }
            if(y <= 0 || y >= 8)
            {
                dy = -dy;
            }

            phase_counter++;
            if(phase_counter > 200)
            {
                demo_phase = 1;
                phase_counter = 0;
                display.fill(255);
                display.update();
                current_brightness = 255;
            }
        }
        else if(demo_phase == 1)
        {
            display.fadeAll(0, 15);
            display.update();
            System::Delay(ANIMATION_DELAY);

            if(current_brightness > 15)
            {
                current_brightness -= 15;
            }
            else
            {
                current_brightness = 0;
                demo_phase = 2;
                phase_counter = 0;

                for(int16_t py = 0; py < 9; py++)
                {
                    for(int16_t px = 0; px < 16; px++)
                    {
                        brightness[px][py] = 0;
                    }
                }
            }
        }
        else if(demo_phase == 2)
        {
            for(int16_t py = 0; py < 9; py++)
            {
                for(int16_t px = 0; px < 16; px++)
                {
                    if(brightness[px][py] > FADE_AMOUNT)
                    {
                        brightness[px][py] -= FADE_AMOUNT;
                    }
                    else
                    {
                        brightness[px][py] = 0;
                    }
                    display.setPixel(px, py, brightness[px][py]);
                }
            }

            x = 8 + (int16_t)(4 * System::GetNow() / 1000);
            if(x > 15) x = 0;

            brightness[x][4] = 255;
            display.setPixel(x, 4, 255);
            display.update();

            phase_counter++;
            if(phase_counter > 200)
            {
                demo_phase = 3;
                phase_counter = 0;
                display.fill(128);
                display.update();
                current_brightness = 128;
            }
        }
        else if(demo_phase == 3)
        {
            display.fadeAll(0, 5);
            display.update();
            System::Delay(ANIMATION_DELAY);

            if(current_brightness > 5)
            {
                current_brightness -= 5;
            }
            else
            {
                current_brightness = 0;
                demo_phase = 4;
                phase_counter = 0;
                display.drawCircle(8, 4, 3, 255, true);
                display.update();
            }
        }
        else if(demo_phase == 4)
        {
            display.fadeAll(100, 8);
            display.update();
            System::Delay(ANIMATION_DELAY * 2);

            phase_counter++;
            if(phase_counter > 50)
            {
                demo_phase = 5;
                phase_counter = 0;
            }
        }
        else if(demo_phase == 5)
        {
            display.fadeAll(0, 10);
            display.update();
            System::Delay(ANIMATION_DELAY);

            if(current_brightness > 10)
            {
                current_brightness -= 10;
            }
            else
            {
                current_brightness = 0;
                demo_phase = 0;
                phase_counter = 0;
                x = 8;
                y = 4;
                dx = 1;
                dy = 1;
            }
        }
    }
}
