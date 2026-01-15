# IS31FL3731 Graphics Library

A grayscale graphics library for IS31FL3731 LED matrix displays with support for advanced 2D shapes, fading effects, and multi-panel support.

## Features

### Core Operations
- `setPixel(x, y, brightness)` - Set individual LED brightness (0-255)
- `clear()` - Clear entire display to 0
- `fill(brightness)` - Fill entire display to specific brightness
- `update()` - Send buffered changes to hardware

### Shape Primitives
- `drawLine(x1, y1, x2, y2, brightness)` - Bresenham's line algorithm
- `drawHLine(x, y, w, brightness)` - Optimized horizontal line
- `drawVLine(x, y, h, brightness)` - Optimized vertical line
- `drawRect(x, y, w, h, brightness, fill)` - Outline or filled rectangle
- `drawCircle(x, y, radius, brightness, fill)` - Outline or filled circle
- `drawTriangle(x0, y0, x1, y1, x2, y2, brightness, fill)` - Outline or filled triangle with scan-line fill
- `drawEllipse(x, y, rx, ry, brightness, fill)` - Outline or filled ellipse using midpoint algorithm with 4-way symmetry
- `drawRoundRect(x, y, w, h, r, brightness, fill)` - Outline or filled rounded rectangle with circular corner arcs

### Fading Effects
- `fadeAll(target, step)` - Smoothly fade entire display to target brightness
- `fadePixel(x, y, target, step)` - Smoothly fade single LED to target brightness

### Multi-Panel Support
- Create multiple IS31FL3731 instances with different I2C addresses
- Synchronize graphics across multiple displays
- Pattern effects spanning chained panels

## Usage

### Basic Setup

```cpp
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

    display.clear();

    // Draw shapes
    display.drawLine(0, 0, 15, 8, 200);
    display.drawCircle(8, 4, 3, 150, true);
    display.drawRect(2, 2, 13, 6, 100, false);

    display.update();
}
```

### Multi-Panel Setup

```cpp
IS31FL3731 ledmatrix[3];
IS31FL3731_Graphics display[3];
const uint8_t ADDRESSES[3] = {0x74, 0x75, 0x76};

for(uint8_t i = 0; i < 3; i++)
{
    if(!ledmatrix[i].begin(ADDRESSES[i], &i2c_handle))
    {
        continue;
    }

    IS31FL3731_Graphics::Config gfx_cfg;
    gfx_cfg.driver = &ledmatrix[i];
    gfx_cfg.frame = 0;
    display[i].Init(gfx_cfg);
}
```

## Examples

### basic_demo.cpp

Demonstrates core graphics operations and user-managed brightness array for trail effects.

Features:
- Manual brightness array for pixel-level control
- Bouncing ball animation with fading trail
- Frame switching (frame 0 and 1)
- Shows the pattern for creating custom fading animations

Build: `make`
(or build with individual examples using custom target configuration)

### filled_shapes_demo.cpp

Shows all advanced shape primitives with both outline and filled variants.

Features:
- Triangles (outline and filled)
- Ellipses (outline and filled)
- Rounded rectangles (outline and filled)
- Each shape displayed for 1 second
- Demonstrates scan-line fill algorithms

Build: `make` (update CPP_SOURCES in Makefile)

### fading_animation_demo.cpp

Comprehensive demonstration of fading capabilities and animation techniques.

Features:
- **Trail Effect**: User-managed brightness array creates smooth pixel trails
  - Fade amount: 40 (controls trail length)
  - Bouncing ball with trail following motion
- **Global Fade**: fadeAll() with smooth transitions
  - Fade from full brightness to zero
  - Configurable step size (15 for smooth fade)
- **Shape Fading**: Fade shapes between brightness levels
  - Circle fades from 255 to 100
  - Shows fadeAll vs manual brightness management
- **Best Practices Demonstrated**:
  - Use brightness arrays for trail effects with manual control
  - Use fadeAll() for global brightness changes
  - Track current brightness when not using built-in cache
  - Multiple animation phases in a single demo

Build: `make` (update CPP_SOURCES in Makefile)

### chained_matrices_demo.cpp

Shows multi-panel support with 3 IS31FL3731 instances at different I2C addresses.

Features:
- **Multiple Panels**: 3 IS31FL3731 instances at addresses 0x74, 0x75, 0x76
  - Each panel uses separate display instance
  - All panels share same I2C bus
- **Wave Animation**: Sine wave spanning all panels
  - Phase shift between panels creates continuous wave
  - Dynamic brightness based on wave position
- **Pattern Effects**:
  - Bouncing line with phase offset
  - Expanding/contracting circles
  - Moving rectangle across panels
- **Synchronized Fading**: Global fade across all panels
  - fadeAll() called on each display instance
  - Creates unified visual effect

Build: `make` (update CPP_SOURCES in Makefile)

## API Reference

### Core Operations

#### `void setPixel(int16_t x, int16_t y, uint8_t brightness)`
- Set individual LED brightness
- Parameters: `x` (0-15), `y` (0-8), `brightness` (0-255)
- Direct I2C write, ~0.2ms per pixel
- Bounds checked: out-of-bounds pixels ignored

#### `void clear()`
- Clear entire display to brightness 0
- Uses buffered writes for performance

#### `void fill(uint8_t brightness)`
- Fill entire display to specific brightness
- Uses buffered writes for performance

#### `void update()`
- Send all buffered changes to hardware
- Required after shape operations
- Not required for setPixel() calls (direct write)

### Shape Primitives

#### `void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness)`
- Draw line using Bresenham's algorithm
- Automatically buffered, 6 I2C transactions

#### `void drawHLine(int16_t x, int16_t y, int16_t w, uint8_t brightness)`
- Optimized horizontal line
- Faster than generic drawLine for horizontal lines

#### `void drawVLine(int16_t x, int16_t y, int16_t h, uint8_t brightness)`
- Optimized vertical line
- Faster than generic drawLine for vertical lines

#### `void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t brightness, bool fill = false)`
- Draw rectangle
- If `fill=true`, draws filled rectangle
- Uses scan-line fill algorithm

#### `void drawCircle(int16_t x, int16_t y, int16_t radius, uint8_t brightness, bool fill = false)`
- Draw circle using midpoint algorithm
- 8-way symmetry for performance
- Scan-line fill for filled circles

#### `void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t brightness, bool fill = false)`
- Draw triangle with three vertices
- Scan-line fill algorithm for filled triangles
- Handles any triangle orientation

#### `void drawEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry, uint8_t brightness, bool fill = false)`
- Draw ellipse using midpoint algorithm
- 4-way symmetry for performance
- Supports different horizontal (rx) and vertical (ry) radii
- Scan-line fill for filled ellipses

#### `void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t brightness, bool fill = false)`
- Draw rectangle with rounded corners
- Corner radius `r` creates circular arcs
- Lines + circular arcs algorithm
- Fill uses scan-line approach

### Fading Effects

#### `void fadeAll(uint8_t target, uint8_t step = 10)`
- Smoothly fade entire display to target brightness
- Uses internal brightness cache for smooth transitions
- Parameters:
  - `target`: Desired brightness (0-255)
  - `step`: Brightness change per iteration (default 10)
- Blocks until fade complete
- Calls update() automatically each step
- Best for global brightness changes

#### `void fadePixel(int16_t x, int16_t y, uint8_t target, uint8_t step = 10)`
- Smoothly fade single LED to target brightness
- Uses internal brightness cache
- Parameters:
  - `x`, `y`: Pixel coordinates
  - `target`: Desired brightness (0-255)
  - `step`: Brightness change per iteration (default 10)
- Blocks until fade complete
- Calls update() automatically each step
- Best for single-pixel animations

### Utility Methods

#### `uint16_t width() const`
- Returns display width (16 pixels)

#### `uint16_t height() const`
- Returns display height (9 pixels)

## Fading Effects Guide

### Fading Modes

#### 1. User-Managed Brightness Array (Trail Effects)
Best for: Custom animations, trails, particle effects

```cpp
uint8_t brightness[16][9] = {0};
const uint8_t FADE_AMOUNT = 40;

while(1)
{
    // Fade all pixels
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

    // Set active pixel
    brightness[x][y] = 255;
    display.setPixel(x, y, 255);
    display.update();
}
```

**Pros**:
- Full control over fade behavior per pixel
- Can implement complex fade curves
- Works well for custom trail effects
- No blocking on fade operations

**Cons**:
- More code
- Must manage brightness state manually
- More CPU intensive

#### 2. Global Fade (fadeAll)
Best for: Simple brightness transitions, on/off effects

```cpp
display.fill(255);
display.update();

// Fade to zero
display.fadeAll(0, 15);
```

**Pros**:
- Simple API, minimal code
- Automatic brightness tracking
- Smooth transitions
- Blocking (easy to sequence)

**Cons**:
- Affects all pixels equally
- Blocking until fade complete
- Less control over per-pixel behavior

#### 3. Single-Pixel Fade (fadePixel)
Best for: Highlight animations, individual LED effects

```cpp
display.drawCircle(8, 4, 3, 255, true);
display.update();

// Fade circle to dimmer brightness
for(int16_t angle = 0; angle < 360; angle += 10)
{
    int16_t x = 8 + (int16_t)(3 * cos(angle * 3.14159 / 180));
    int16_t y = 4 + (int16_t)(3 * sin(angle * 3.14159 / 180));
    display.fadePixel(x, y, 100, 5);
}
```

**Pros**:
- Target specific pixels
- Automatic brightness tracking
- Smooth transitions
- Easy to use

**Cons**:
- Only affects one pixel at a time
- Blocking until fade complete
- Less efficient for multiple pixels

### Best Practices

1. **For trail effects**: Use user-managed brightness array
   - Gives fine-grained control over fade curves
   - Non-blocking, good for complex animations
   - Example: fading_animation_demo.cpp demo_phase 0

2. **For simple brightness changes**: Use fadeAll()
   - Minimal code, automatic caching
   - Example: fading_animation_demo.cpp demo_phase 1, 3, 5

3. **For single-pixel animations**: Use fadePixel()
   - Simple API for individual LEDs
   - Good for highlight effects

4. **Track brightness externally when not using built-in cache**
   - The graphics library's brightness cache is private
   - Use a separate variable to track current brightness when needed
   - Example: `current_brightness` variable in fading_animation_demo.cpp

5. **Combine fading modes for complex effects**
   - Use brightness array for trails + fadeAll for global transitions
   - Creates layered animations

## Performance Notes

- **Automatic buffering**: Shape operations (lines, circles, rectangles, etc.) automatically buffer internally for 10x faster rendering
- **Direct pixel access**: `setPixel()` calls are direct I2C writes (~0.2ms per pixel)
- **Bulk writes**: Buffered shapes use 6 I2C transactions (~3ms total vs ~29ms for 144 individual pixels)
- **Brightness caching**: `fadeAll()` and `fadePixel()` use internal brightness cache for smooth transitions

## Supported Displays

- IS31FL3731 (16x9 matrix)
- IS31FL3731_Wing (15x7 matrix with remapped coordinates)

## I2C Addressing

The IS31FL3731 supports multiple I2C addresses for multi-panel setups:
- Default: 0x74
- Can be configured to: 0x75, 0x76, 0x77 (using address pins)
- Up to 4 panels on a single I2C bus

See chained_matrices_demo.cpp for multi-panel implementation.

## Implementation Notes

- All shape methods use optimized algorithms (Bresenham's for lines/circles, scan-line fills)
- Filled shapes use horizontal scan algorithms
- Memory usage: 144-byte temporary buffer for shape rendering + 144-byte brightness cache for fading
- Compatible with both IS31FL3731 and IS31FL3731_Wing variants
- Thread safety: Not thread-safe (single-threaded embedded environment)
