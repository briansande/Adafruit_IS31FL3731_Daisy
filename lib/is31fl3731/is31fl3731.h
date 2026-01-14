#ifndef IS31FL3731_H
#define IS31FL3731_H

#include "daisy_seed.h"
#include <stdint.h>

using namespace daisy;

#define ISSI_ADDR_DEFAULT 0x74

#define ISSI_REG_CONFIG 0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME 0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B

class IS31FL3731
{
  public:
    IS31FL3731(uint8_t x = 16, uint8_t y = 9);
    ~IS31FL3731();

    bool begin(uint8_t    addr       = ISSI_ADDR_DEFAULT,
               I2CHandle* i2c_handle = nullptr);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void clear(void);

    void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
    void audioSync(bool sync);
    void setFrame(uint8_t b);
    void displayFrame(uint8_t frame);

    uint8_t getWidth() const { return width_; }
    uint8_t getHeight() const { return height_; }

  protected:
    bool    selectBank(uint8_t bank);
    bool    writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
    uint8_t readRegister8(uint8_t bank, uint8_t reg);
    uint8_t _frame;

  private:
    uint8_t    width_;
    uint8_t    height_;
    uint8_t    i2c_addr_;
    I2CHandle* i2c_handle_;
    I2CHandle  internal_i2c_handle_;
};

class IS31FL3731_Wing : public IS31FL3731
{
  public:
    IS31FL3731_Wing(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
};

#endif
