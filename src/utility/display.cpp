#include "display.h"

bool Display::begin(lgfx::Bus_SPI& bus, lgfx::Light_PWM& light, int csPin)
{
    _panel_instance.setBus(&bus);

    {   // GC9107 panel — this display's own chip-select.
        auto cfg             = _panel_instance.config();
        cfg.pin_cs           = csPin;    // LovyanGFX drives CS automatically
        cfg.pin_rst          = -1;       // shared RST handled once by the manager
        cfg.pin_busy         = -1;
        cfg.memory_width     = 128;
        cfg.memory_height    = 128;
        cfg.panel_width      = 128;
        cfg.panel_height     = 128;
        cfg.offset_x         = 0;        // matches upstream GC9107 config
        cfg.offset_y         = 32;
        cfg.offset_rotation  = 0;
        cfg.dummy_read_pixel = 8;
        cfg.dummy_read_bits  = 1;
        cfg.readable         = false;
        cfg.invert           = false;     // IPS panel
        cfg.rgb_order        = false;
        cfg.dlen_16bit       = false;
        cfg.bus_shared       = true;     // bus is shared with the other displays
        _panel_instance.config(cfg);
    }

    _panel_instance.setLight(&light);
    setPanel(&_panel_instance);

    if (!init()) return false;
    setRotation(0);
    fillScreen(0x0000);
    _initialized = true;
    return true;
}
