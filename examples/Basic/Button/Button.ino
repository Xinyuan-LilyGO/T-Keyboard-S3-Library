/*
 * Button — read the four keys and mirror their state on the four displays.
 *
 * Each display shows its key's running press count and the name of the most
 * recent event (press / release / click / double / hold). The same events are
 * also printed to the Serial Monitor at 115200 baud. Uses the display() and
 * button() index accessors so both subsystems are addressed the same way.
 */
#include <TKeyboardS3.h>

static constexpr uint8_t N = TKeyboardS3Class::KEY_COUNT;   // == DISPLAY_COUNT (4)

static uint32_t    pressCount[N] = {0};
static const char* lastEvent[N]  = {"-", "-", "-", "-"};

// Redraw one display from that key's current count + last-event state.
void drawKey(uint8_t i)
{
    Display& s = TKeyboardS3.display(i);
    s.fillScreen(TFT_BLACK);

    s.setTextColor(TFT_WHITE);
    s.setTextSize(2);
    s.setCursor(8, 10);
    s.printf("KEY%d", i + 1);

    s.setTextColor(TFT_CYAN);
    s.setCursor(8, 48);
    s.printf("x %lu", (unsigned long)pressCount[i]);   // total presses

    s.setTextColor(TFT_YELLOW);
    s.setTextSize(1);
    s.setCursor(8, 92);
    s.print("last:");
    s.setCursor(8, 106);
    s.print(lastEvent[i]);                             // name of last event
}

void setup(void)
{
    Serial.begin(115200);
    TKeyboardS3.begin();
    for (uint8_t i = 0; i < N; ++i) drawKey(i);
}

void loop(void)
{
    TKeyboardS3.delay(1);
    TKeyboardS3.update();   // polls btn1..btn4

    for (uint8_t i = 0; i < N; ++i) {
        button::ButtonSense& b = TKeyboardS3.button(i);

        // Capture the most recent event this pass; counting only real presses.
        const char* ev = nullptr;
        if (b.wasPressed())       { ev = "press"; pressCount[i]++; }
        if (b.wasReleased())        ev = "release";
        if (b.wasClicked())         ev = "click";
        if (b.wasDoubleClicked())   ev = "double";
        if (b.wasHold())            ev = "hold";

        if (ev) {
            lastEvent[i] = ev;
            Serial.printf("KEY%d: %s (total %lu)\n",
                          i + 1, ev, (unsigned long)pressCount[i]);
            drawKey(i);     // refresh only the display that changed
        }
    }
}
