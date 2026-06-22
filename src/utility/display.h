#pragma once
#include <stdint.h>
#include <LovyanGFX.hpp>

// ---------------------------------------------------------------------------
// Display — one physical GC9107 128x128 panel as its own LovyanGFX device.
//
// The four displays on the T-Keyboard-S3 share a single SPI bus (SCLK/MOSI/DC),
// a single backlight and a single reset line; only the chip-select differs.
// Each Display therefore owns its own Panel_GC9107 with its own pin_cs but is
// handed the shared bus and backlight by the manager. LovyanGFX drives each
// display's CS automatically, so drawing on one Display never touches the
// others — exactly the way each key is its own ButtonSense object.
//
// Display inherits lgfx::LGFX_Device, so the full LovyanGFX drawing API is
// available directly:  TKeyboardS3.display1.fillScreen(TFT_RED);
// ---------------------------------------------------------------------------
class Display : public lgfx::LGFX_Device {
    lgfx::Panel_GC9107 _panel_instance;
    bool _initialized = false;

public:
    // bus and light are owned by the manager and shared by all four displays.
    // The hardware reset is performed once by the manager, so the panel is
    // configured with pin_rst = -1 here.
    bool begin(lgfx::Bus_SPI& bus, lgfx::Light_PWM& light, int csPin);

    bool isInitialized() const { return _initialized; }
};
