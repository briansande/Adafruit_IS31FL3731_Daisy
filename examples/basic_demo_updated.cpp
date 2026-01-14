#include "daisy_seed.h"
#include "../lib/is31fl3731/is31fl3731.h"
#include "../lib/is31fl3731_graphics/IS31FL3731_Graphics.h"
#include <stdint.h>

using namespace daisy;

DaisySeed  hw;
IS31FL3731 ledmatrix;
IS31FL3731_Graphics display;

const uint32_t UPDATE_DELAY = 50;

int main(void)
{
    hw.Init();

    I2CHandle::Config i2c_conf;
    i2c_conf.periph         = I2CHandle::Config::Peripheral::I2C_1;
    i2c_conf.mode           = I2CHandle::Config::Mode::I2C_MASTER;
    i2c_conf.speed          = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_conf.pin_config.scl = {DSY_GPIOB, 8};
    i2c_conf.pin_config.sda = {DSY_GPIOB, 9};

    I2CHandle i2c_handle;
    if(i2c_handle.Init(i2c_conf) != I2CHandle::Result::OK)
    {
        return -1;
    }

    if(!ledmatrix.begin(ISSI_ADDR_DEFAULT, &i2c_handle))
    {
        return -1;
    }

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix;
    gfx_cfg.frame = 0;

    if(!display.Init(gfx_cfg))
    {
        return -1;
    }

    display.clear();
    System::Delay(100);

    int16_t x          = 0;
    int16_t y          = 0;
    uint8_t frame      = 0;
    bool    use_frame2 = false;

    const uint8_t FADE_AMOUNT = 60;

    while(1)
    {
        System::Delay(UPDATE_DELAY);

        for(int16_t py = 0; py < 9; py++)
        {
            for(int16_t px = 0; px < 16; px++)
            {
                if(display.width() * display.height() > 0)
                {
                    display.setPixel(px, py, 0);
                }
            }
        }

        display.setPixel(x, y, 255);
        display.update();

        x++;
        if(x >= 16)
        {
            x = 0;
            y++;
            if(y >= 9)
            {
                y = 0;
                use_frame2 = !use_frame2;

                if(use_frame2)
                {
                    frame = 1;
                }
                else
                {
                    frame = 0;
                }

                display.setFrame(frame);
                display.clear();

                for(int16_t py = 0; py < 9; py++)
                {
                    for(int16_t px = 0; px < 16; px++)
                    {
                        brightness[px][py] = 0;
                    }
                }
            }
        }
    }
}
