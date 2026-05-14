#include "Watchy_Biosync.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/biosync.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockBiosync;

bool g_biosyncStaticDump = false;

WatchyBiosync::WatchyBiosync() {}

void WatchyBiosync::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    BiosyncData d;
    if (g_biosyncStaticDump) {
        d = BiosyncData{45, 10, 13};   // matches references/biosync.png scene
    } else {
        sim_mockBiosync.tick();
        d = sim_mockBiosync.currentBiosync();
    }
    drawBiosyncFace(display, 12, 32, d);
}
