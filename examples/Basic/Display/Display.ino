/*
 * Display — exercises the four GC9107 128×128 displays via LovyanGFX.
 *
 * Each display is its own LovyanGFX device (display1..display4), so the same
 * draw helper can be fed any of them through a base pointer — no manual
 * chip-select switching. Mirrors the T-CameraPlus-S3 random-draw demo,
 * fanned out across all four panels.
 */
#include <TKeyboardS3.h>

void setup(void)
{
    Serial.begin(115200);
    TKeyboardS3.begin();
    TKeyboardS3.setBrightness(255);

    for (int i = 0; i < TKeyboardS3Class::DISPLAY_COUNT; i++) {
        TKeyboardS3.display(i).fillScreen(TFT_BLACK);
    }
}

// Drawing helper — accepts any LovyanGFX device pointer, sizing itself to the
// panel it is given (mirrors the M5Unified / T-CameraPlus-S3 pattern).
void draw_function(LovyanGFX* gfx)
{
    int x = rand() % gfx->width();
    int y = rand() % gfx->height();
    int r = (gfx->width() >> 4) + 2;
    uint16_t c = rand();
    gfx->fillRect(x - r, y - r, r * 2, r * 2, c);
}

void loop(void)
{
    // Draw on each of the four displays every pass.
    for (int i = 0; i < TKeyboardS3Class::DISPLAY_COUNT; i++) {
        Display& s = TKeyboardS3.display(i);

        // A random filled circle...
        int x = rand() % s.width();
        int y = rand() % s.height();
        int r = (s.width() >> 4) + 2;
        s.fillCircle(x, y, r, rand());

        // ...and a random rectangle via the generic helper.
        draw_function(&s);
    }

    delay(1);
}
