#include "Watchy_Hourly.h"

#include "../../../../watchy-screens/src/faces/frame.h"
#include "../../../../watchy-screens/src/faces/hourly.h"
#include "../../../../watchy-screens/src/mock/mock_data.h"

static MockState sim_mockHourly;

// Render one fixed scene (instead of cycling through the animated
// mock) when --dump Hourly is invoked, so pixel diffs aren't drowned
// out by scene rotation.
bool g_hourlyStaticDump = false;

// Render the static chrome plus a 6×6 diamond at every kMinuteDot[m]
// position simultaneously when --dump-all Hourly is invoked. Used to
// visually verify all 60 diamond positions in a single screenshot.
bool g_hourlyAllDotsDump = false;

WatchyHourly::WatchyHourly() {}

void WatchyHourly::drawWatchFace() {
    display.fillScreen(GxEPD_WHITE);
    drawWatchyChrome(display);

    HourlyData scene;
    if (g_hourlyStaticDump || g_hourlyAllDotsDump) {
        scene = HourlyData{47, 10, 30, 4, 5, 6 /*SUN*/, 87, 0};
    } else {
        sim_mockHourly.tick();
        scene = sim_mockHourly.currentHourly();
    }
    drawHourlyFace(display, 12, 32, scene);

    if (g_hourlyAllDotsDump) {
        const uint16_t BLACK = 0x0000;
        constexpr uint8_t kBlob[6] = {0x30, 0x78, 0xFC, 0xFC, 0x78, 0x30};
        const int ox = 12, oy = 32;
        for (int m = 0; m < 60; ++m) {
            const int cx = kMinuteDot[m][0];
            const int cy = kMinuteDot[m][1];
            for (int r = 0; r < 6; ++r) {
                for (int c = 0; c < 6; ++c) {
                    if (kBlob[r] & (0x80 >> c))
                        display.drawPixel(ox + cx - 3 + c,
                                          oy + cy - 3 + r, BLACK);
                }
            }
        }
    }
}
