#include "TKeyboardS3.h"
#include <Arduino.h>

const char* TKeyboardS3Class::TAG = "TKeyboardS3";

TKeyboardS3Class TKeyboardS3;

// ---------------------------------------------------------------------------

bool TKeyboardS3Class::begin(BoardConfig cfg)
{
    const board_pin_config_t& p = *_pins;

    if (cfg.enableDisplay) {
        {   // Shared SPI bus (all four displays).
            auto bc        = _bus.config();
            bc.spi_host    = SPI2_HOST;     // FSPI on the ESP32-S3
            bc.spi_mode    = 0;
            bc.freq_write  = 40000000;      // 40 MHz
            bc.freq_read   = 16000000;
            bc.spi_3wire   = false;
            bc.use_lock    = true;
            bc.dma_channel = 0;             // disabled: keeps the shared bus simple
            bc.pin_sclk    = p.spi_sclk;
            bc.pin_mosi    = p.spi_mosi;
            bc.pin_miso    = p.spi_miso;
            bc.pin_dc      = p.disp_dc;
            _bus.config(bc);
        }
        {   // Shared backlight.
            auto lc        = _light.config();
            lc.pin_bl      = p.disp_bl;
            lc.invert      = false;
            lc.freq        = 44100;
            lc.pwm_channel = 7;
            _light.config(lc);
        }

        // One hardware reset for all four GC9107 controllers (shared RST line),
        // done before any panel is initialised so the displays don't reset each
        // other. Each panel is then configured with pin_rst = -1.
        if (p.disp_rst >= 0) {
            pinMode(p.disp_rst, OUTPUT);
            digitalWrite(p.disp_rst, LOW);
            ::delay(10);
            digitalWrite(p.disp_rst, HIGH);
            ::delay(120);
        }

        bool ok = display1.begin(_bus, _light, p.disp_cs[0])
               && display2.begin(_bus, _light, p.disp_cs[1])
               && display3.begin(_bus, _light, p.disp_cs[2])
               && display4.begin(_bus, _light, p.disp_cs[3]);
        if (!ok) {
            TKB_LOGE(TAG, "Display init failed");
            return false;
        }
        setBrightness(255);
    }

    if (cfg.enableKeys) {
        btn1.begin(p.key[0], true, true);  // active-low, internal pullup
        btn2.begin(p.key[1], true, true);
        btn3.begin(p.key[2], true, true);
        btn4.begin(p.key[3], true, true);
    }

    _initialized = true;
    return true;
}

void TKeyboardS3Class::end()
{
    setBrightness(0);
    _initialized = false;
}

void TKeyboardS3Class::update()
{
    uint32_t msec = millis();
    btn1.update(msec);
    btn2.update(msec);
    btn3.update(msec);
    btn4.update(msec);
}

void TKeyboardS3Class::setBrightness(uint8_t brightness)
{
    // All four displays share one backlight; setting it on any display is enough.
    display1.setBrightness(brightness);
}

Display& TKeyboardS3Class::display(uint8_t index)
{
    switch (index) {
        case 0:  return display1;
        case 1:  return display2;
        case 2:  return display3;
        case 3:  return display4;
        default: return display1;
    }
}

button::ButtonSense& TKeyboardS3Class::button(uint8_t index)
{
    switch (index) {
        case 0:  return btn1;
        case 1:  return btn2;
        case 2:  return btn3;
        case 3:  return btn4;
        default: return btn1;
    }
}
