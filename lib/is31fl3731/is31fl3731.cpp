#include "is31fl3731.h"
#include <string.h>
#include <unistd.h>

#define swap_int16_t(a, b) \
    do                     \
    {                      \
        int16_t t = a;     \
        a         = b;     \
        b         = t;     \
    } while(0)

IS31FL3731::IS31FL3731(uint8_t x, uint8_t y)
: width_(x), height_(y), i2c_handle_(nullptr)
{
}

IS31FL3731::~IS31FL3731() {}

bool IS31FL3731::begin(uint8_t addr, I2CHandle* i2c_handle)
{
    i2c_addr_ = addr;

    if(i2c_handle != nullptr)
    {
        i2c_handle_ = i2c_handle;
    }
    else
    {
        I2CHandle::Config i2c_conf;
        i2c_conf.periph         = I2CHandle::Config::Peripheral::I2C_1;
        i2c_conf.mode           = I2CHandle::Config::Mode::I2C_MASTER;
        i2c_conf.speed          = I2CHandle::Config::Speed::I2C_400KHZ;
        i2c_conf.pin_config.scl = {DSY_GPIOB, 8};
        i2c_conf.pin_config.sda = {DSY_GPIOB, 9};

        if(internal_i2c_handle_.Init(i2c_conf) != I2CHandle::Result::OK)
        {
            return false;
        }
        i2c_handle_ = &internal_i2c_handle_;
    }

    _frame = 0;

    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
    System::Delay(10);
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

    writeRegister8(
        ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    displayFrame(_frame);

    clear();

    for(uint8_t f = 0; f < 8; f++)
    {
        for(uint8_t i = 0; i <= 0x11; i++)
        {
            writeRegister8(f, i, 0xff);
        }
    }

    audioSync(false);

    return true;
}

void IS31FL3731::clear(void)
{
    selectBank(_frame);
    uint8_t erasebuf[25];

    memset(erasebuf, 0, 25);

    for(uint8_t i = 0; i < 6; i++)
    {
        erasebuf[0] = 0x24 + i * 24;
        i2c_handle_->TransmitBlocking(i2c_addr_, erasebuf, 25, 1000);
    }
}

void IS31FL3731::setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank)
{
    if(lednum >= 144)
    {
        return;
    }
    writeRegister8(bank, 0x24 + lednum, pwm);
}

void IS31FL3731::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= (int16_t)width_))
    {
        return;
    }
    if((y < 0) || (y >= (int16_t)height_))
    {
        return;
    }
    if(color > 255)
    {
        color = 255;
    }

    setLEDPWM(x + y * width_, color, _frame);
}

IS31FL3731_Wing::IS31FL3731_Wing(void) : IS31FL3731(15, 7) {}

void IS31FL3731_Wing::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if((x < 0) || (x >= 16))
    {
        return;
    }
    if((y < 0) || (y >= 7))
    {
        return;
    }
    if(color > 255)
    {
        color = 255;
    }

    if(x > 7)
    {
        x = 15 - x;
        y += 8;
    }
    else
    {
        y = 7 - y;
    }

    swap_int16_t(x, y);

    setLEDPWM(x + y * 16, color, _frame);
}

void IS31FL3731::setFrame(uint8_t frame)
{
    _frame = frame;
}

void IS31FL3731::displayFrame(uint8_t frame)
{
    if(frame > 7)
    {
        frame = 0;
    }
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame);
}

bool IS31FL3731::selectBank(uint8_t bank)
{
    uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    return i2c_handle_->TransmitBlocking(i2c_addr_, cmd, 2, 1000)
           == I2CHandle::Result::OK;
}

void IS31FL3731::audioSync(bool sync)
{
    if(sync)
    {
        writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
    }
    else
    {
        writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
    }
}

bool IS31FL3731::writeRegister8(uint8_t bank, uint8_t reg, uint8_t data)
{
    selectBank(bank);
    uint8_t cmd[2] = {reg, data};
    return i2c_handle_->TransmitBlocking(i2c_addr_, cmd, 2, 1000)
           == I2CHandle::Result::OK;
}

uint8_t IS31FL3731::readRegister8(uint8_t bank, uint8_t reg)
{
    uint8_t val = 0xFF;
    selectBank(bank);
    i2c_handle_->ReadDataAtAddress(i2c_addr_, reg, 1, &val, 1, 1000);
    return val;
}
