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

    uint32_t demo_time = System::GetNow();
    int demo_mode = 0;
    const uint8_t BRIGHTNESS = 200;
    int ball_x = 0;
    int ball_y = 4;
    int ball_dir = 1;
    uint8_t fade_counter = 0;

    while(1)
    {
        uint32_t current_time = System::GetNow();
        
        if(current_time - demo_time > 3000)
        {
            demo_time = current_time;
            demo_mode = (demo_mode + 1) % 8;
            display.clear();
            display.update();
            ball_x = 0;
            fade_counter = 0;
        }

        switch(demo_mode)
        {
            case 0:
                display.drawLine(0, 0, 15, 8, BRIGHTNESS);
                display.drawLine(15, 0, 0, 8, BRIGHTNESS);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 1:
                display.drawRect(2, 2, 12, 5, BRIGHTNESS, false);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 2:
                display.drawRect(3, 2, 10, 5, BRIGHTNESS, true);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 3:
                display.drawCircle(8, 4, 3, BRIGHTNESS, false);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 4:
                display.drawCircle(8, 4, 3, BRIGHTNESS, true);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 5:
                display.drawTriangle(8, 1, 1, 7, 15, 7, BRIGHTNESS, false);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 6:
                display.drawEllipse(8, 4, 6, 3, BRIGHTNESS, false);
                display.update();
                System::Delay(UPDATE_DELAY);
                break;

            case 7:
                display.clear();
                
                fade_counter += 8;
                uint8_t brightness = (uint8_t)((fade_counter > 127) ? (255 - fade_counter) : fade_counter);
                
                ball_x += ball_dir;
                if(ball_x >= 15 || ball_x <= 0)
                {
                    ball_dir *= -1;
                }
                
                display.setPixel(ball_x, ball_y, brightness);
                display.setPixel(ball_x + 1, ball_y, brightness);
                display.setPixel(ball_x, ball_y + 1, brightness);
                display.setPixel(ball_x + 1, ball_y + 1, brightness);
                
                display.update();
                System::Delay(UPDATE_DELAY);
                break;
        }
    }
}
