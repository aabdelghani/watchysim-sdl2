#include "Watchy_GoodMorning.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/goodmorning.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockGoodMorning;

bool g_goodMorningStaticDump = false;

WatchyGoodMorning::WatchyGoodMorning() {}

void WatchyGoodMorning::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    if (g_goodMorningStaticDump) {
        drawGoodMorningFace(display, 12, 32, referenceGoodMorning());
    } else {
        sim_mockGoodMorning.tick();
        drawGoodMorningFace(display, 12, 32, sim_mockGoodMorning.currentGoodMorning());
    }
}
