#include "daisy_seed.h"
#include "../lib/is31fl3731/is31fl3731.h"
#include "../lib/is31fl3731_graphics/IS31FL3731_Graphics.h"
#include <stdint.h>

using namespace daisy;

DaisySeed  hw;
IS31FL3731 ledmatrix;
IS31FL3731_Graphics display;

const uint32_t DISPLAY_DELAY = 500;

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

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix;
    gfx_cfg.frame = 0;

    if(!display.Init(gfx_cfg))
    {
        return -1;
    }

    display.clear();
    System::Delay(DISPLAY_DELAY);

    int16_t x          = 0;
    int16_t y          = 0;
    uint8_t brightness = 255;

    uint16_t step_count = 0;

    while(1)
    {
        switch(step_count % 5)
        {
            case 0:
                display.drawLine(x, y, x + 15, y + 15, brightness);
                break;
            case 1:
                display.drawRect(2, 2, 13, 6, brightness, false);
                break;
            case 2:
                display.drawCircle(8, 4, 3, brightness);
                break;
            case 3:
                display.fillRect(2, 2, 13, 6, brightness, true);
                break;
            case 4:
                display.drawHLine(x + 3, y + 7, 10, brightness);
                display.drawVLine(x + 8, y, 4, brightness);
                break;
        }

        display.update();
        System::Delay(DISPLAY_DELAY * 2);

        x++;
        if(x >= 16)
        {
            x = 0;
            y++;

            if(y >= 9)
            {
                y = 0;
            }

            if(brightness > 50)
            {
                brightness -= 40;
            }
            else
            {
                brightness = 255;
            }

            step_count++;
    }
}
