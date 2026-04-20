#include "Watchy_Multiday.h"

// Pull in the portable face code and mock-data module from the
// watchy-screens repo (sibling checkout).
#include "../../../../watchy-screens/src/faces/multiday.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"
#include "../DOS/Px437_IBM_BIOS5pt7b.h"

static MockState sim_mockMultiday;

WatchyMultiday::WatchyMultiday() {}

void WatchyMultiday::drawWatchFace() {
    // Clear to white (face renders in black on white; border will be dithered later).
    display.fillScreen(GxEPD_WHITE);

    display.setFont(&Px437_IBM_BIOS5pt7b);

    sim_mockMultiday.tick();
    drawMultidayFace(display, 12, 32, sim_mockMultiday.currentMultiday());
}
