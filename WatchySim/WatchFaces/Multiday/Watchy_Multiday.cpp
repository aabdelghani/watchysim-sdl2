#include "Watchy_Multiday.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/multiday.h"
#include "../../../../watchy-screens/src/fonts/RainyHearts6pt7b.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockMultiday;

WatchyMultiday::WatchyMultiday() {}

void WatchyMultiday::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);

    display.setFont(&rainyhearts6pt7b);

    sim_mockMultiday.tick();
    drawMultidayFace(display, 12, 32, sim_mockMultiday.currentMultiday());
}
