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

    I2CHandle i2c_handle;
    I2CHandle::Config i2c_cfg;
    i2c_cfg.periph = I2CHandle::Config::Peripheral::I2C_1;
    i2c_cfg.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_cfg.mode = I2CHandle::Config::Mode::I2C_MASTER;
    i2c_cfg.pin_config.scl = {DSY_GPIOB, 8};
    i2c_cfg.pin_config.sda = {DSY_GPIOB, 9};
    i2c_handle.Init(i2c_cfg);

    IS31FL3731::Config matrix_cfg;
    matrix_cfg.i2c = &i2c_handle;
    matrix_cfg.address = 0x74;
    ledmatrix.Init(matrix_cfg);

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix;
    gfx_cfg.frame = 0;
    display.Init(gfx_cfg);

    display.clear();

    while(1)
    {
        display.clear();

        display.drawTriangle(2, 1, 7, 8, 13, 1, 255, false);
        display.update();
        System::Delay(1000);

        display.clear();

        display.drawTriangle(2, 1, 7, 8, 13, 1, 200, true);
        display.update();
        System::Delay(1000);

        display.clear();

        display.drawEllipse(8, 4, 5, 3, 255, false);
        display.update();
        System::Delay(1000);

        display.clear();

        display.drawEllipse(8, 4, 5, 3, 180, true);
        display.update();
        System::Delay(1000);

        display.clear();

        display.drawRoundRect(2, 1, 12, 6, 2, 255, false);
        display.update();
        System::Delay(1000);

        display.clear();

        display.drawRoundRect(2, 1, 12, 6, 2, 200, true);
        display.update();
        System::Delay(1000);
    }
}
