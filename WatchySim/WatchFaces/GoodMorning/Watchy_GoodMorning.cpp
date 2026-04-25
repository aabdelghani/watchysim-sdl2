#include "Watchy_GoodMorning.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/goodmorning.h"

WatchyGoodMorning::WatchyGoodMorning() {}

void WatchyGoodMorning::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    drawGoodMorningFace(display, 12, 32, referenceGoodMorning());
}
