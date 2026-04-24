#include "Watchy_Multiday.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/multiday.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockMultiday;

// Set by --dump path to render the static reference snapshot instead of the
// animated mock — lets pixel-diffs against references/multyday.png ignore
// shuffle/midline/day-dot animation noise.
bool g_multidayStaticDump = false;

WatchyMultiday::WatchyMultiday() {}

void WatchyMultiday::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);
    if (g_multidayStaticDump) {
        drawMultidayFace(display, 12, 32, referenceMultiday());
    } else {
        sim_mockMultiday.tick();
        drawMultidayFace(display, 12, 32, sim_mockMultiday.currentMultiday());
    }
}
