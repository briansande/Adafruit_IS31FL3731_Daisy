#include "daisy_seed.h"
#include "../lib/is31fl3731/is31fl3731.h"
#include "../lib/is31fl3731_graphics/IS31FL3731_Graphics.h"
#include <math.h>

using namespace daisy;

DaisySeed hw;
IS31FL3731 ledmatrix[3];
IS31FL3731_Graphics display[3];

const uint8_t ADDRESSES[3] = {0x74, 0x75, 0x76};

void setupMatrix(uint8_t idx, I2CHandle* i2c_handle)
{
    if(!ledmatrix[idx].begin(ADDRESSES[idx], i2c_handle))
    {
        return;
    }

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix[idx];
    gfx_cfg.frame = 0;
    display[idx].Init(gfx_cfg);
}

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

    for(uint8_t i = 0; i < 3; i++)
    {
        setupMatrix(i, &i2c_handle);
    }

    for(uint8_t i = 0; i < 3; i++)
    {
        display[i].clear();
        display[i].update();
    }

    uint32_t tick = 0;
    uint32_t demo_phase = 0;
    uint32_t phase_counter = 0;

    while(1)
    {
        System::Delay(30);

        if(demo_phase == 0)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].clear();

                for(int16_t x = 0; x < 16; x++)
                {
                    int16_t y = 4 + (int16_t)(3 * System::GetNow() / 1000 + i * 2);
                    y = ((y % 9) + 9) % 9;

                    uint8_t brightness = 200 - abs(x - 8) * 15;
                    display[i].setPixel(x, y, brightness);
                }

                display[i].update();
            }

            phase_counter++;
            if(phase_counter > 150)
            {
                demo_phase = 1;
                phase_counter = 0;
            }
        }
        else if(demo_phase == 1)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].clear();

                int16_t center_x = 8;
                int16_t center_y = 4;
                int16_t offset = (int16_t)(4 * System::GetNow() / 1000);
                offset = ((offset % 5) + 5) % 5;

                display[i].drawCircle(center_x, center_y, offset, 180 + i * 30, false);
                display[i].update();
            }

            phase_counter++;
            if(phase_counter > 100)
            {
                demo_phase = 2;
                phase_counter = 0;
            }
        }
        else if(demo_phase == 2)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].clear();

                int16_t wave_offset = (int16_t)(System::GetNow() / 50 + i * 3);

                for(int16_t x = 0; x < 16; x++)
                {
                    int16_t y = 4 + (int16_t)(2 * sin((x + wave_offset) * 3.14159 / 8));
                    y = ((y % 9) + 9) % 9;

                    uint8_t brightness = 150 + (int16_t)(50 * sin((x + wave_offset) * 3.14159 / 8));
                    display[i].setPixel(x, y, brightness);
                }

                display[i].update();
            }

            phase_counter++;
            if(phase_counter > 150)
            {
                demo_phase = 3;
                phase_counter = 0;
            }
        }
        else if(demo_phase == 3)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].clear();

                int16_t box_x = 2;
                int16_t box_y = 2;
                int16_t box_w = 12;
                int16_t box_h = 5;

                if(phase_counter < 50)
                {
                    box_x += phase_counter / 5;
                }
                else if(phase_counter < 100)
                {
                    box_x = 12 - (phase_counter - 50) / 5;
                }
                else
                {
                    phase_counter = 0;
                    demo_phase = 4;
                    break;
                }

                display[i].drawRect(box_x, box_y, box_w, box_h, 200 + i * 30, true);
                display[i].update();
            }

            phase_counter++;
        }
        else if(demo_phase == 4)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].fadeAll(0, 10);
                display[i].update();
                System::Delay(30);
            }

            phase_counter++;
            if(phase_counter > 30)
            {
                demo_phase = 5;
                phase_counter = 0;

                for(uint8_t i = 0; i < 3; i++)
                {
                    display[i].fill(255);
                    display[i].update();
                }
            }
        }
        else if(demo_phase == 5)
        {
            for(uint8_t i = 0; i < 3; i++)
            {
                display[i].fadeAll(0, 15);
                display[i].update();
                System::Delay(50);
            }

            phase_counter++;
            if(phase_counter > 20)
            {
                demo_phase = 0;
                phase_counter = 0;
            }
        }

        tick++;
    }
}
