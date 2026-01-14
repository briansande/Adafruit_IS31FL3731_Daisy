# IS31FL3731 LED Driver for Daisy Seed

A portable C++ driver for the Adafruit IS31FL3731 16x9 Charlieplexed PWM LED matrix driver, ported from the Adafruit Arduino library for use with the Daisy Seed platform.

## Overview

The IS31FL3731 is an I2C-controlled LED driver that can control up to 144 LEDs (16x9 grid) with individual PWM brightness control (0-255). This driver provides full control of the chip including:
- All 8 frame buffers for animation
- Individual LED brightness control
- Frame switching for smooth animations
- Audio synchronization support
- Complete register-level access

## Hardware Requirements

- **Daisy Seed** or compatible Daisy board
- **IS31FL3731 LED driver breakout board** (e.g., Adafruit 2946)
- **LED matrix** (optional, for 16x9 grid)
- **I2C connections**: SDA, SCL, VCC, GND

## Features

- Full API compatibility with Arduino version
- Support for all 8 frame buffers (0-7)
- Individual PWM control for each LED (0-255)
- Frame selection and display switching
- Audio sync mode support
- Built-in FeatherWing support (15x7 variant)
- Configurable I2C address (supports 4 addresses: 0x74-0x77)
- Daisy Seed I2C integration

## Installation

### Quick Start

1. Copy the `lib/is31fl3731/` directory into your project's lib folder
2. Include the header: `#include "lib/is31fl3731/is31fl3731.h"`
3. Link the driver source file in your Makefile
4. Build and flash!

### Manual Installation

1. Create a `lib/` directory in your project if it doesn't exist
2. Copy the entire `is31fl3731` folder to `lib/is31fl3731/`
3. Add the driver to your Makefile:

```makefile
CPP_SOURCES = your_main.cpp \
              lib/is31fl3731/is31fl3731.cpp
```

4. Include in your code:
```cpp
#include "lib/is31fl3731/is31fl3731.h"
```

## Wiring

### Daisy Seed I2C Pins

| IS31FL3731 Pin | Daisy Seed I2C_1 | Daisy Seed I2C_4 |
|----------------|------------------|------------------|
| SCL            | D11 (PB8)        | D13 (PB10)      |
| SDA            | D12 (PB9)        | D14 (PB11)      |
| VCC            | 5V or 3.3V      | 5V or 3.3V      |
| GND            | GND              | GND              |

**Note**: The breakout board includes built-in 20K pull-up resistors to VCC.

### Daisy Patch SM I2C Pins

| IS31FL3731 Pin | Daisy Patch SM |
|----------------|---------------|
| SCL            | B7            |
| SDA            | B8            |
| VCC            | 5V or 3.3V   |
| GND            | GND           |

### I2C Address Selection

The IS31FL3731 supports up to 4 devices on the same I2C bus:

| Soldered Jumper | I2C Address |
|-----------------|-------------|
| None (default)  | 0x74        |
| 0x75           | 0x75        |
| 0x76           | 0x76        |
| 0x77           | 0x77        |

Solder the corresponding jumper on the breakout board to change the address.

## Usage

### Basic Setup

```cpp
#include "daisy_seed.h"
#include "lib/is31fl3731/is31fl3731.h"

using namespace daisy;

DaisySeed hw;
IS31FL3731 ledmatrix;

int main(void) {
    hw.Init();

    // Configure I2C
    I2CHandle::Config i2c_conf;
    i2c_conf.periph = I2CHandle::Config::Peripheral::I2C_1;
    i2c_conf.mode = I2CHandle::Config::Mode::I2C_MASTER;
    i2c_conf.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_conf.pin_config.scl = {DSY_GPIOB, 8};   // D11 on Daisy Seed
    i2c_conf.pin_config.sda = {DSY_GPIOB, 9};   // D12 on Daisy Seed

    I2CHandle i2c_handle;
    i2c_handle.Init(i2c_conf);

    // Initialize the driver
    ledmatrix.begin(0x74, &i2c_handle);

    // Use the driver...
}
```

### Using Internal I2C Handle (Simpler)

The driver can create its own I2C handle if you don't need to share it:

```cpp
IS31FL3731 ledmatrix;

int main(void) {
    hw.Init();

    // Driver will create I2C_1 with default pins
    ledmatrix.begin(0x74);

    // Use the driver...
}
```

### Drawing Pixels

```cpp
// Set a single LED to full brightness
ledmatrix.drawPixel(0, 0, 255);

// Set a pixel to half brightness
ledmatrix.drawPixel(5, 5, 128);

// Turn off a pixel
ledmatrix.drawPixel(10, 3, 0);

// Parameters: (x, y, brightness)
// x: 0-15 (horizontal position)
// y: 0-8 (vertical position)
// brightness: 0-255 (PWM value)
```

### Clear Display

```cpp
// Turn off all LEDs in the current frame
ledmatrix.clear();
```

### Using Multiple Frames for Animation

```cpp
// Set up animation frames
ledmatrix.setFrame(0);
ledmatrix.drawPixel(0, 0, 255);  // Frame 0: top-left LED on

ledmatrix.setFrame(1);
ledmatrix.drawPixel(1, 1, 255);  // Frame 1: different LED on

// Display frame 0
ledmatrix.displayFrame(0);
System::Delay(500);

// Switch to frame 1
ledmatrix.displayFrame(1);
System::Delay(500);

// Parameters: (frame_number)
// frame_number: 0-7
```

### Direct LED PWM Control

```cpp
// Set specific LED number to brightness value (bypasses x/y mapping)
ledmatrix.setLEDPWM(0, 255);     // LED #0 at full brightness
ledmatrix.setLEDPWM(143, 128);    // Last LED at half brightness

// Parameters: (led_number, brightness, frame)
// led_number: 0-143
// brightness: 0-255
// frame: 0-7 (default: 0)
```

### Audio Sync

```cpp
// Enable audio sync (modulates brightness with audio input)
ledmatrix.audioSync(true);

// Disable audio sync
ledmatrix.audioSync(false);
```

### FeatherWing (15x7 Matrix)

For the Adafruit FeatherWing (15x7 LED matrix):

```cpp
IS31FL3731_Wing wing;

// Initialize and use same as IS31FL3731
wing.begin(0x74, &i2c_handle);
wing.drawPixel(7, 3, 255);
```

The Wing class handles the different pixel mapping for the 15x7 grid.

## API Reference

### Constructor

```cpp
IS31FL3731(uint8_t width = 16, uint8_t height = 9);
```
Creates an IS31FL3731 driver object. Default dimensions are 16x9.

### Initialization

```cpp
bool begin(uint8_t addr = ISSI_ADDR_DEFAULT, I2CHandle* i2c_handle = nullptr);
```
Initializes the driver.

**Parameters:**
- `addr`: I2C address (default: 0x74)
- `i2c_handle`: Pointer to existing I2C handle (optional). If nullptr, creates internal I2C handle.

**Returns:** `true` on success, `false` on failure.

### Drawing

```cpp
void drawPixel(int16_t x, int16_t y, uint16_t color);
```
Sets a single LED's brightness.

**Parameters:**
- `x`: X position (0-15)
- `y`: Y position (0-8)
- `color`: Brightness (0-255)

```cpp
void clear(void);
```
Turns off all LEDs in the current frame.

### Frame Control

```cpp
void setFrame(uint8_t frame);
```
Sets the current frame for drawing operations.

**Parameters:**
- `frame`: Frame number (0-7)

```cpp
void displayFrame(uint8_t frame);
```
Tells the chip which frame to display.

**Parameters:**
- `frame`: Frame number (0-7)

### Low-Level Control

```cpp
void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank = 0);
```
Sets LED brightness by LED number (bypasses x/y mapping).

**Parameters:**
- `lednum`: LED number (0-143)
- `pwm`: Brightness (0-255)
- `bank`: Frame/bank number (0-7, default: 0)

```cpp
void audioSync(bool sync);
```
Enables/disables audio sync mode.

**Parameters:**
- `sync`: `true` to enable, `false` to disable

### Protected Methods

```cpp
bool writeRegister8(uint8_t bank, uint8_t reg, uint8_t data);
uint8_t readRegister8(uint8_t bank, uint8_t reg);
```
Low-level register access for advanced users.

## Examples

### Example 1: Basic Demo (examples/basic_demo.cpp)

A simple example that scans through all LEDs with increasing brightness and demonstrates frame switching.

```bash
make
make deploy
```

### Example 2: Simple Animation

```cpp
// Fill display with gradient
for (uint8_t y = 0; y < 9; y++) {
    for (uint8_t x = 0; x < 16; x++) {
        uint8_t brightness = (x + y) * 8;
        if (brightness > 255) brightness = 255;
        ledmatrix.drawPixel(x, y, brightness);
    }
}
```

### Example 3: Blink Animation

```cpp
// Create two frames
ledmatrix.setFrame(0);
ledmatrix.clear();  // All off

ledmatrix.setFrame(1);
ledmatrix.fill(255);  // All on (if you implement fill())

// Blink between them
while(true) {
    ledmatrix.displayFrame(0);
    System::Delay(500);
    ledmatrix.displayFrame(1);
    System::Delay(500);
}
```

## Building

```bash
# Build the example
make

# Flash to Daisy Seed
make deploy

# Clean build
make clean
```

## Troubleshooting

### LEDs not lighting up
- Check I2C wiring (SDA, SCL, VCC, GND)
- Verify I2C address is correct (default: 0x74)
- Ensure power supply can provide enough current (all LEDs at max may need external power)
- Check that `begin()` returns `true`

### Communication failures
- Try slower I2C speed (I2C_100KHZ instead of I2C_400KHZ)
- Check for proper pull-up resistors (20K built into breakout)
- Ensure correct pin assignments for your Daisy board

### Incorrect LED positions
- Verify you're using the correct class (IS31FL3731 vs IS31FL3731_Wing)
- Check your LED matrix is properly seated in the header

### Build errors
- Ensure libDaisy is properly set up
- Check that paths in Makefile are correct
- Verify all required files are in place

## Advanced Features

### Register Access

For advanced users, you can access the IS31FL3731 registers directly:

```cpp
// Read configuration register
uint8_t config = ledmatrix.readRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG);

// Write custom configuration
ledmatrix.writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, custom_value);
```

### Bank Selection

The chip has multiple memory banks:
- **Function Registers (0x0B)**: Configuration, shutdown, picture frame, audio sync
- **Frame Banks (0-7)**: LED PWM data for each frame

Use `selectBank()` to switch between them manually, or let the driver handle it automatically.

## Power Considerations

- Operating voltage: 2.7V - 5.5V
- **Warning**: All 144 LEDs at full brightness can draw significant current
- For full brightness operation, consider external power supply
- Daisy Seed USB provides limited current (~500mA max)

## Comparison with Arduino Library

This driver maintains API compatibility with the Adafruit Arduino library:

| Arduino Function | Daisy Function | Notes |
|-----------------|----------------|-------|
| `begin(addr)` | `begin(addr, i2c_handle)` | Daisy version accepts I2C handle |
| `drawPixel(x, y, color)` | `drawPixel(x, y, color)` | Identical |
| `clear()` | `clear()` | Identical |
| `setFrame(frame)` | `setFrame(frame)` | Identical |
| `displayFrame(frame)` | `displayFrame(frame)` | Identical |
| `setLEDPWM(lednum, pwm, bank)` | `setLEDPWM(lednum, pwm, bank)` | Identical |
| `audioSync(sync)` | `audioSync(sync)` | Identical |

## Resources

- [Adafruit IS31FL3731 Library](https://github.com/adafruit/Adafruit_IS31FL3731)
- [IS31FL3731 Datasheet](https://www.issi.com/WW/pdf/31FL3731.pdf)
- [Adafruit Learning Guide](https://learn.adafruit.com/i31fl3731-16x9-charliplexed-pwm-led-driver)
- [libDaisy Documentation](https://daisy.audio/)
- [Daisy I2C Tutorial](https://daisy.audio/tutorials/_a9_Getting-Started-I2C/)

## License

Ported from Adafruit's MIT-licensed Arduino library. This driver is also released under the MIT License.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Credits

- Original Arduino library: Adafruit Industries
- Daisy port: Adapted for libDaisy platform
- IS31FL3731 Chip: Integrated Silicon Solution Inc.
