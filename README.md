# LILYGO T-Keyboard-S3 Library

An Arduino driver library for the [LILYGO T-Keyboard-S3](https://github.com/Xinyuan-LilyGO/T-Keyboard-S3) (ESP32-S3). It follows the same structure as the T-CameraPlus-S3 library: one unified class with public subsystem members, reached through a global singleton.

- **Four 128×128 GC9107 IPS displays**, each its own LovyanGFX device `display1..display4`. They share one SPI bus and backlight internally, but every display has its own chip-select which LovyanGFX drives automatically — so drawing on one display never affects the others.
- **Four keys** exposed as [ButtonSense](https://github.com/lbuque/ButtonSense) instances `btn1..btn4`, with debounce, click / double-click / hold detection and event callbacks.

Displays and keys are addressed the same way: four independent instances (`display1..display4`, `btn1..btn4`), or by index via `display(0..3)` / `key(0..3)`.

This release targets the Arduino environment only.

## Requirements

- ESP32-S3 board support (Arduino-ESP32 core 2.x or 3.x)
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX)
- [ButtonSense](https://github.com/lbuque/ButtonSense)
- Board settings matching the upstream project: 16 MB flash, OPI PSRAM, USB-CDC on boot

## Installation

Copy this folder into your Arduino `libraries/` directory, then install LovyanGFX and ButtonSense from the Library Manager. For PlatformIO, the dependencies are declared in `library.json`.

## Quick start

```cpp
#include <TKeyboardS3.h>

void setup() {
    TKeyboardS3.begin();                              // displays + keys

    TKeyboardS3.display1.fillScreen(TFT_BLACK);        // draws only on display 1
    TKeyboardS3.display1.setTextColor(TFT_YELLOW);
    TKeyboardS3.display1.drawString("Hi", 40, 56);

    TKeyboardS3.display2.fillScreen(TFT_BLUE);         // display 2, independently
}

void loop() {
    TKeyboardS3.update();                             // poll the four keys
    if (TKeyboardS3.btn1.wasClicked()) {
        // KEY1 single click
    }
}
```

`TKeyboardS3` is the global singleton (an instance of `TKeyboardS3Class`), so there is no object to construct.

## Displays

Each of the four panels is its own LovyanGFX device — `TKeyboardS3.display1 … display4` (or `TKeyboardS3.display(0..3)`). They share one SPI bus (SCLK 47, MOSI 48, DC 45), one reset (38) and one backlight (39) internally, but each `Display` owns its own `Panel_GC9107` with its own chip-select (CS1 12, CS2 13, CS3 14, CS4 21). LovyanGFX drives each display's CS automatically, so you just draw:

```cpp
TKeyboardS3.display3.fillScreen(TFT_RED);
TKeyboardS3.display3.drawString("hello", 0, 0);
```

`Display` inherits `lgfx::LGFX_Device`, so the entire LovyanGFX drawing API (`fillScreen`, `drawString`, `pushImage`, `setFont`, …) is available directly. Backlight is shared by all four displays; set it with `TKeyboardS3.setBrightness(0..255)`. (Internally the shared SPI bus runs without DMA to keep multi-device access simple.)

## Buttons

The four keys are `button::ButtonSense` objects: `TKeyboardS3.btn1 … btn4` (or `TKeyboardS3.key(0..3)`). Call `TKeyboardS3.update()` once per `loop()` to poll them, then query state:

- Levels/edges: `isPressed()`, `wasPressed()`, `wasReleased()`, `isHolding()`.
- Gestures: `wasClicked()`, `wasHold()`, `wasSingleClicked()`, `wasDoubleClicked()`, `wasDecideClickCount()` / `getClickCount()`.
- Events: `onEvent(cb)` with a `void(ButtonSense&, Event)` callback.
- Tuning: `setDebounceThresh()`, `setHoldThresh()`, `setHoldRepeatMs()`.

See the ButtonSense library for the full API.

## API summary

```
TKeyboardS3.begin()                     // init displays + keys
TKeyboardS3.begin(BoardConfig)          // selectively enable subsystems
TKeyboardS3.end()
TKeyboardS3.update()                    // poll btn1..btn4 (call in loop)
TKeyboardS3.display1 .. display4          // Display : lgfx::LGFX_Device
TKeyboardS3.display(0..3) -> Display&     // index accessor
TKeyboardS3.btn1 .. btn4                // button::ButtonSense
TKeyboardS3.key(0..3) -> ButtonSense&   // index accessor
TKeyboardS3.setBrightness(0..255)       // shared backlight
TKeyboardS3.isInitialized()
TKeyboardS3.pins()                      // board_pin_config_t
```

## Examples

- **Basic/Display** — random graphics drawn across all four displays.
- **Basic/Button** — each display shows its key's press count and last event; events also printed over Serial.
- **Advanced/VirtualScreen** — treat the four panels as one 512×128 surface.

## Pin map

| Function | GPIO | | Function | GPIO |
|---|---|---|---|---|
| LCD CS1 | 12 | | LCD DC | 45 |
| LCD CS2 | 13 | | LCD SCLK | 47 |
| LCD CS3 | 14 | | LCD MOSI | 48 |
| LCD CS4 | 21 | | LCD RST | 38 |
| KEY1 | 10 | | LCD BL | 39 |
| KEY2 | 9 | | KEY3 | 46 |
| KEY4 | 3 | | WS2812B | 11 |

The WS2812B RGB LEDs are not driven by this library.

## License

MIT.
