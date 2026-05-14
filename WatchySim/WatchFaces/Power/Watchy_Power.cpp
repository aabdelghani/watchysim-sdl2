#include "Watchy_Power.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/power.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockPower;

WatchyPower::WatchyPower() {}

void WatchyPower::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    sim_mockPower.tick();
    drawPowerFace(display, 12, 32, sim_mockPower.currentPower());
}
