#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// LILYGO T-Keyboard-S3 pin map (ESP32-S3).
//
// The board carries four GC9107 128x128 IPS displays that share one SPI bus
// (MOSI/SCLK/DC/RST/BL) and are selected by four individual chip-select lines,
// plus four mechanical keys (active LOW). Pin names mirror the schematic
// labels used by the official firmware.
//
// A single hardware revision is supported, so there is one pin table.
// ---------------------------------------------------------------------------
typedef struct {
    // ---------- Shared SPI bus (all four displays) ----------
    int spi_mosi;
    int spi_sclk;
    int spi_miso;   // not connected on this board (-1)

    // ---------- Display GC9107 (4 x 128x128 IPS) ----------
    int disp_dc;
    int disp_rst;   // shared reset
    int disp_bl;    // shared backlight (PWM)
    int disp_cs[4]; // per-display chip select (display 0..3)

    // ---------- Keys (active LOW) ----------
    int key[4];     // KEY1..KEY4

    // ---------- WS2812B RGB (not driven by this library) ----------
    int ws2812b;
} board_pin_config_t;

// N085-1212TBWIG06-C08 panels + keys.
static const board_pin_config_t BOARD_PIN_CONFIG = {
    .spi_mosi = 48,
    .spi_sclk = 47,
    .spi_miso = -1,

    .disp_dc  = 45,
    .disp_rst = 38,
    .disp_bl  = 39,
    .disp_cs  = { 12, 13, 14, 21 },   // CS1, CS2, CS3, CS4

    .key      = { 10, 9, 46, 3 },     // KEY1, KEY2, KEY3, KEY4

    .ws2812b  = 11,
};

#ifdef __cplusplus
}
#endif
