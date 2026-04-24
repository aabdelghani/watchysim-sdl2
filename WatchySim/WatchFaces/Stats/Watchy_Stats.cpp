#include "Watchy_Stats.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/stats.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockStats;

bool g_statsStaticDump = false;

WatchyStats::WatchyStats() {}

void WatchyStats::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    if (g_statsStaticDump) {
        drawStatsFace(display, 12, 32, referenceStats());
    } else {
        sim_mockStats.tick();
        drawStatsFace(display, 12, 32, sim_mockStats.currentStats());
    }
}
