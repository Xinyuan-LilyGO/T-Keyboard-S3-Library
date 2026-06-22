/*
 * VirtualScreen — treat the four 128x128 panels as one 512x128 display.
 *
 * Everything is drawn onto a single off-screen 512x128 LGFX_Sprite living in
 * PSRAM. On each frame the canvas is sliced into four 128-pixel-wide columns
 * and each column is blitted to its physical panel, so you can draw across the
 * whole 512x128 surface with the normal LovyanGFX API.
 *
 * The trick: pushSprite() with a negative x offset. A panel is only 128px wide,
 * so pushing the canvas at x = -i*128 clips everything except the i-th column
 * onto display i.
 *
 * Note: there are physical bezels between the panels, so the image is logically
 * continuous but has visible seams. If the panels read right-to-left on your
 * unit, replace `i` with `(SCREENS - 1 - i)` in flushCanvas().
 */
#include <TKeyboardS3.h>

static constexpr int SCREENS = TKeyboardS3Class::DISPLAY_COUNT; // 4
static constexpr int PANEL_W = 128;
static constexpr int PANEL_H = 128;
static constexpr int VW      = PANEL_W * SCREENS;              // 512 virtual width
static constexpr int VH      = PANEL_H;                        // 128 virtual height

// One big virtual framebuffer. Draw on this as if it were a single 512x128 display.
static LGFX_Sprite canvas;

// Blit each 128px-wide slice of the canvas to its physical panel.
void flushCanvas()
{
    for (int i = 0; i < SCREENS; i++) {
        canvas.pushSprite(&TKeyboardS3.display(i), -i * PANEL_W, 0);
    }
}

void setup(void)
{
    Serial.begin(115200);
    TKeyboardS3.begin();
    TKeyboardS3.setBrightness(255);

    canvas.setPsram(true);          // 512x128x2 = 128 KB -> keep it in PSRAM
    canvas.setColorDepth(16);
    if (!canvas.createSprite(VW, VH)) {
        TKB_LOGE("VScreen", "createSprite(%d,%d) failed", VW, VH);
        while (true) { delay(1000); }
    }

    canvas.setTextColor(TFT_YELLOW);
    canvas.setTextSize(4);
    canvas.setTextDatum(middle_left);
}

void loop(void)
{
    static const char* msg = "  T-Keyboard-S3  *  one 512x128 virtual display  ";
    static int scroll = VW;                 // start off the right edge

    int textW = canvas.textWidth(msg);

    canvas.fillScreen(TFT_BLACK);
    canvas.drawString(msg, scroll, VH / 2); // single draw spanning all four panels
    flushCanvas();

    if (--scroll < -textW) { scroll = VW; } // wrap around
    delay(10);
}
