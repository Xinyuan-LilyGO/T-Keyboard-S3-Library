#pragma once

#include <Arduino.h>
#include "board/board_config.h"
#include "utility/display.h"
#include <ButtonSense.h>

// Logging macro (Arduino only — this library targets the Arduino environment).
#define TKB_LOGI(tag, fmt, ...) Serial.printf("[%s] " fmt "\n", tag, ##__VA_ARGS__)
#define TKB_LOGE(tag, fmt, ...) Serial.printf("[%s] ERROR: " fmt "\n", tag, ##__VA_ARGS__)

// Selectively enable/disable subsystems passed to TKeyboardS3.begin().
struct BoardConfig {
    bool enableDisplay = true;
    bool enableKeys    = true;
};

// ---------------------------------------------------------------------------
// TKeyboardS3Class — unified driver for the LILYGO T-Keyboard-S3.
//
// The four displays and four keys are each exposed as independent instances,
// reached through the global singleton `TKeyboardS3`:
//
//     TKeyboardS3.begin();
//     TKeyboardS3.display1.fillScreen(TFT_RED);   // draws only on display 1
//     TKeyboardS3.display2.drawString("hi", 0, 0);
//
//     void loop() {
//         TKeyboardS3.update();                   // poll the four keys
//         if (TKeyboardS3.btn1.wasClicked()) { ... }
//     }
//
// Each display is a full lgfx::LGFX_Device with its own chip-select, driven
// automatically by LovyanGFX; the four displays share one SPI bus and one
// backlight internally.
// ---------------------------------------------------------------------------
class TKeyboardS3Class {
public:
    static constexpr uint8_t DISPLAY_COUNT = 4;
    static constexpr uint8_t KEY_COUNT    = 4;

    // Public subsystems — access as TKeyboardS3.display1 / .btn1 ...
    Display               display1;   // CS1 — GPIO12
    Display               display2;   // CS2 — GPIO13
    Display               display3;   // CS3 — GPIO14
    Display               display4;   // CS4 — GPIO21

    button::ButtonSense  btn1;      // KEY1 — GPIO10, active-low
    button::ButtonSense  btn2;      // KEY2 — GPIO9,  active-low
    button::ButtonSense  btn3;      // KEY3 — GPIO46, active-low
    button::ButtonSense  btn4;      // KEY4 — GPIO3,  active-low

    bool begin(void) { BoardConfig cfg; return begin(cfg); }
    bool begin(BoardConfig cfg);

    void end();

    // Poll loop — call from loop(). Updates all four key state machines.
    void update();

    // Index accessors (0..3).
    Display&              display(uint8_t index);   // 0 == display1 ... 3 == display4
    button::ButtonSense& button(uint8_t index);    // 0 == btn1 ...    3 == btn4

    // Backlight is shared by all four displays; 0..255.
    void setBrightness(uint8_t brightness);

    static inline void delay(uint32_t msec) { ::delay(msec); }

    bool isInitialized() const { return _initialized; }
    const board_pin_config_t& pins() const { return *_pins; }

private:
    // Shared across all four displays.
    lgfx::Bus_SPI   _bus;
    lgfx::Light_PWM _light;

    const board_pin_config_t* _pins        = &BOARD_PIN_CONFIG;
    bool                      _initialized = false;
    static const char*        TAG;
};

// Global singleton instance.
extern TKeyboardS3Class TKeyboardS3;
