# IS31FL3731 Graphics Library

A grayscale graphics library for IS31FL3731 LED matrix displays with support for advanced 2D shapes and fading effects.

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
- `drawTriangle(x0, y0, x1, y1, x2, y2, brightness, fill)` - Outline or filled triangle
- `drawEllipse(x, y, rx, ry, brightness, fill)` - Outline or filled ellipse
- `drawRoundRect(x, y, w, h, r, brightness, fill)` - Outline or filled rounded rectangle

### Fading Effects
- `fadeAll(target, step)` - Smoothly fade entire display to target brightness
- `fadePixel(x, y, target, step)` - Smoothly fade single LED to target brightness

## Usage

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

    I2CHandle i2c_handle;
    // Initialize I2C...
    
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

## Performance Notes

- **Automatic buffering**: Shape operations (lines, circles, rectangles, etc.) automatically buffer internally for 10x faster rendering
- **Direct pixel access**: `setPixel()` calls are direct I2C writes (~0.2ms per pixel)
- **Bulk writes**: Buffered shapes use 6 I2C transactions (~3ms total vs ~29ms for 144 individual pixels)
- **Brightness caching**: `fadeAll()` and `fadePixel()` use internal brightness cache for smooth transitions

## Supported Displays

- IS31FL3731 (16x9 matrix)
- IS31FL3731_Wing (15x7 matrix with remapped coordinates)

## Implementation Notes

- All shape methods use optimized algorithms (Bresenham's for lines/circles, scan-line fills)
- Filled shapes use horizontal scan algorithms
- Memory usage: 144-byte temporary buffer for shape rendering + 144-byte brightness cache for fading
- Compatible with both IS31FL3731 and IS31FL3731_Wing variants
