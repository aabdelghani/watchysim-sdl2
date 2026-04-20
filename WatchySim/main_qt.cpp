#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QImage>
#include <QRegularExpression>
#include <QDateTime>

#include <ctime>
#include <memory>

#include "WatchFaces/7_SEG/Watchy_7_SEG.h"
#include "WatchFaces/AnalogGabel/Watchy_AnalogGabel.h"
#include "WatchFaces/DOS/Watchy_DOS.h"
#include "WatchFaces/DrawTest/Watchy_Draw_Test.h"
#include "WatchFaces/MacPaint/Watchy_MacPaint.h"
#include "WatchFaces/Mario/Watchy_Mario.h"
#include "WatchFaces/Niobe/niobe.h"
#include "WatchFaces/Pokemon/Watchy_Pokemon.h"
#include "WatchFaces/PowerShell/Watchy_PowerShell.h"
#include "WatchFaces/Scene/Watchy_scene.h"
// Note: niobe.h declares class `Niobe`; Watchy_scene.h declares class `Scene`.
#include "WatchFaces/Tetris/Watchy_Tetris.h"

static const int BG_W = 676;
static const int BG_H = 676;
static const int SCREEN_OFFSET_X = 241;
static const int SCREEN_OFFSET_Y = 198;

class WatchyView : public QWidget {
public:
    WatchyView(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(BG_W, BG_H);
        QString bgPath = QCoreApplication::applicationDirPath() + "/../WatchySim/background.gif";
        if (!background.load(bgPath)) {
            background.load("WatchySim/background.gif");
        }
    }

    void setWatchy(Watchy *w) { watchy = w; }

    void redraw() {
        if (!watchy) return;
        watchy->showWatchFace();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter p(this);
        if (!background.isNull()) {
            p.drawPixmap(0, 0, background);
        } else {
            p.fillRect(rect(), Qt::black);
        }
        if (watchy) {
            QImage img(watchy->display.getFramebuffer(),
                       DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_WIDTH * 3,
                       QImage::Format_RGB888);
            p.drawImage(SCREEN_OFFSET_X, SCREEN_OFFSET_Y, img);
        }
    }

private:
    QPixmap background;
    Watchy *watchy = nullptr;
};

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        loadFace(new Watchy7SEG());

        view = new WatchyView(this);
        setCentralWidget(view);
        view->setWatchy(watchy.get());

        buildMenus();
        applyCurrentSystemTime();
        view->redraw();

        setWindowTitle("WatchySim");

        auto *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            time_t t = time(nullptr);
            struct tm tm_now; localtime_r(&t, &tm_now);
            int newMinute = tm_now.tm_min;
            watchy->setTime(tm_now);
            if (newMinute != lastMinute) {
                lastMinute = newMinute;
                view->redraw();
            }
        });
        timer->start(1000);
    }

private:
    void loadFace(Watchy *next) {
        watchy.reset(next);
    }

    void swapFace(Watchy *next) {
        loadFace(next);
        view->setWatchy(watchy.get());
        applyCurrentSystemTime();
        view->redraw();
    }

    void applyCurrentSystemTime() {
        time_t t = time(nullptr);
        struct tm tm_now; localtime_r(&t, &tm_now);
        lastMinute = tm_now.tm_min;
        watchy->setTime(tm_now);
    }

    void buildMenus() {
        auto *mb = menuBar();

        auto *mTime = mb->addMenu("&Time");
        mTime->addAction("Current time", this, [this]() {
            watchy->resetTime();
            view->redraw();
        });
        mTime->addAction("Short format (01/01/2011 01:01)", this, [this]() {
            setCustomTime(2011, 4, 1, 1, 1, 1, 0);
        });
        mTime->addAction("Long format (08/31/2099 18:33)", this, [this]() {
            setCustomTime(2099, 8, 31, 18, 33, 1, 3);
        });
        mTime->addAction("Custom...", this, [this]() {
            bool ok;
            QString input = QInputDialog::getText(this, "Set time",
                "Enter date/time (MM/DD/YYYY HH:MM):",
                QLineEdit::Normal, "03/14/2021 16:20", &ok);
            if (!ok) return;
            QRegularExpression re(R"((\d{2})/(\d{2})/(\d{4}) (\d{2}):(\d{2}))");
            auto match = re.match(input);
            if (!match.hasMatch()) {
                QMessageBox::warning(this, "Could not understand date",
                    "Please enter a date in the form of 'MM/DD/YYYY HH:MM'");
                return;
            }
            int mo = match.captured(1).toInt();
            int d  = match.captured(2).toInt();
            int y  = match.captured(3).toInt();
            int h  = match.captured(4).toInt();
            int mi = match.captured(5).toInt();
            QDate date(y, mo, d);
            int wday = date.dayOfWeek() % 7; // Qt: Mon=1..Sun=7; tm_wday: Sun=0..Sat=6
            setCustomTime(y, mo, d, h, mi, 0, wday);
        });

        auto addRadio = [&](QMenu *m, const QString &label, std::function<void()> fn) {
            auto *a = m->addAction(label, this, fn);
            a->setCheckable(false);
            return a;
        };

        auto *mBattery = mb->addMenu("&Battery");
        addRadio(mBattery, "Dead (0.0V)",   [this]{ watchy->setBatteryVoltage(0.0f);  view->redraw(); });
        addRadio(mBattery, "Low (3.7V)",    [this]{ watchy->setBatteryVoltage(3.7f);  view->redraw(); });
        addRadio(mBattery, "Medium (3.81V)",[this]{ watchy->setBatteryVoltage(3.81f); view->redraw(); });
        addRadio(mBattery, "High (3.96V)",  [this]{ watchy->setBatteryVoltage(3.96f); view->redraw(); });
        addRadio(mBattery, "Max (4.2V)",    [this]{ watchy->setBatteryVoltage(4.2f);  view->redraw(); });

        auto *mBt = mb->addMenu("Bl&uetooth");
        addRadio(mBt, "On",  [this]{ watchy->setBluetooth(true);  view->redraw(); });
        addRadio(mBt, "Off", [this]{ watchy->setBluetooth(false); view->redraw(); });

        auto *mWifi = mb->addMenu("Wi&Fi");
        addRadio(mWifi, "On", [this]{
            watchy->setWifi(true);
            watchy->setWeatherExternal(true);
            if (watchy->getTemperature() == WATCHY_INTERNAL_TEMP) {
                watchy->setTemperature(WATCHY_DEFAULT_TEMP);
            }
            view->redraw();
        });
        addRadio(mWifi, "Off", [this]{
            watchy->setWifi(false);
            watchy->setWeatherCode(800);
            watchy->setWeatherExternal(false);
            watchy->setTemperature(WATCHY_INTERNAL_TEMP);
            view->redraw();
        });

        auto *mSteps = mb->addMenu("&Steps");
        addRadio(mSteps, "None (0)",         [this]{ watchy->setSteps(0);     view->redraw(); });
        addRadio(mSteps, "Lazy (12)",        [this]{ watchy->setSteps(12);    view->redraw(); });
        addRadio(mSteps, "Regular (5280)",   [this]{ watchy->setSteps(5280);  view->redraw(); });
        addRadio(mSteps, "Athlete (52769)",  [this]{ watchy->setSteps(52769); view->redraw(); });

        auto *mWeather = mb->addMenu("&Weather");
        auto addWx = [&](const QString &label, int code) {
            mWeather->addAction(label, this, [this, code]() {
                watchy->setWeatherCode(code);
                watchy->setWeatherExternal(true);
                view->redraw();
            });
        };
        addWx("Cloudy (802)",         802);
        addWx("Few Clouds (801)",     801);
        addWx("Clear (800)",          800);
        addWx("Atmosphere (750)",     750);
        addWx("Snow (650)",           650);
        addWx("Rain (550)",           550);
        addWx("Drizzle (350)",        350);
        addWx("Thunderstorm (250)",   250);
        addWx("Other (150)",          150);

        auto *mTemp = mb->addMenu("Te&mperature");
        addRadio(mTemp, "Celsius",    [this]{ watchy->setTemperatureUnitMetric(true);  view->redraw(); });
        addRadio(mTemp, "Fahrenheit", [this]{ watchy->setTemperatureUnitMetric(false); view->redraw(); });
        mTemp->addSeparator();
        auto addT = [&](const QString &label, int t) {
            mTemp->addAction(label, this, [this, t]() {
                watchy->setTemperature(t);
                watchy->setWeatherExternal(true);
                view->redraw();
            });
        };
        addT("Canada (-45)", -45);
        addT("Chilly (7)",     7);
        addT("Warm (15)",     15);
        addT("Inferno (40)",  40);

        auto *mFace = mb->addMenu("&Watch Face");
        auto addFace = [&](const QString &label, std::function<Watchy*()> make) {
            mFace->addAction(label, this, [this, make]() { swapFace(make()); });
        };
        addFace("7_SEG",         []{ return new Watchy7SEG(); });
        addFace("AnalogGabel",   []{ return new WatchyAnalogGabel(); });
        addFace("DOS",           []{ return new WatchyDOS(); });
        addFace("DrawTest",      []{ return new WatchyDrawTest(); });
        addFace("MacPaint",      []{ return new WatchyMacPaint(); });
        addFace("Mario",         []{ return new WatchyMario(); });
        addFace("Niobe",         []{ return new Niobe(); });
        addFace("Pokemon",       []{ return new WatchyPokemon(); });
        addFace("PowerShell",    []{ return new WatchyPowerShell(); });
        addFace("Scene",         []{ return new Scene(); });
        addFace("Tetris",        []{ return new WatchyTetris(); });

        auto *mTools = mb->addMenu("T&ools");
        mTools->addAction("Screenshot...", this, [this]() {
            QString path = QFileDialog::getSaveFileName(this, "Save screenshot",
                "watchy.png", "PNG Images (*.png)");
            if (path.isEmpty()) return;
            QImage img(watchy->display.getFramebuffer(),
                       DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_WIDTH * 3,
                       QImage::Format_RGB888);
            img.save(path, "PNG");
        });
    }

    void setCustomTime(int y, int mo, int d, int h, int mi, int s, int wday) {
        struct tm t{};
        t.tm_year = y - 1900;
        t.tm_mon  = mo;
        t.tm_mday = d;
        t.tm_hour = h;
        t.tm_min  = mi;
        t.tm_sec  = s;
        t.tm_wday = wday;
        watchy->setTime(t);
        view->redraw();
    }

    std::unique_ptr<Watchy> watchy;
    WatchyView *view = nullptr;
    int lastMinute = -1;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
