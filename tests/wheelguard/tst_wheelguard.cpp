// WheelGuard keeps the mouse wheel from editing value widgets while the user
// is scrolling a settings page: the wheel event is passed on to the enclosing
// scroll area instead.

#include <QtTest>
#include <QtTest/qtestspontaneevent.h>
#include <QComboBox>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>

#include "wheelguard.hpp"

namespace {

// A scrollable page, taller than its viewport, with one of each value widget.
struct Page
{
    QScrollArea area;
    QComboBox *combo;
    QSpinBox *spin;
    QSlider *slider;

    Page()
    {
        QWidget *content = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(content);
        combo = new QComboBox;
        combo->addItems({"one", "two", "three"});
        spin = new QSpinBox;
        spin->setRange(0, 100);
        spin->setValue(50);
        slider = new QSlider(Qt::Horizontal);
        slider->setRange(0, 100);
        slider->setValue(50);
        layout->addWidget(combo);
        layout->addWidget(spin);
        layout->addWidget(slider);
        layout->addStretch();
        content->setMinimumHeight(2500);
        area.setWidgetResizable(true);
        area.setWidget(content);
        area.resize(300, 200);
        area.show();
    }
};

void wheelDown(QWidget *target)
{
    QPoint pos = target->rect().center();
    QWheelEvent event(pos, target->mapToGlobal(pos), QPoint(), QPoint(0, -120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    // Only spontaneous (window-system) wheel events propagate to parent
    // widgets. Mark it like a real one and hand it to notify() directly,
    // the way QTest::mouseClick does: sendEvent() would clear the flag.
    QSpontaneKeyEvent::setSpontaneous(&event);
    qApp->notify(target, &event);
}

} // namespace

class WheelGuardTest : public QObject
{
    Q_OBJECT

private slots:
    void wheelChangesValuesWithoutGuard();
    void wheelLeavesComboBoxAlone();
    void wheelLeavesSpinBoxAlone();
    void wheelLeavesSliderAlone();
    void wheelScrollsThePageInstead();
    void hoverDoesNotFocusValueWidgets();
};

void WheelGuardTest::wheelChangesValuesWithoutGuard()
{
    // Proves the synthetic wheel event does what a real one does.
    Page page;
    QVERIFY(QTest::qWaitForWindowExposed(&page.area));
    wheelDown(page.combo);
    QCOMPARE(page.combo->currentIndex(), 1);
}

void WheelGuardTest::wheelLeavesComboBoxAlone()
{
    Page page;
    QVERIFY(QTest::qWaitForWindowExposed(&page.area));
    WheelGuard::install(&page.area);
    wheelDown(page.combo);
    QCOMPARE(page.combo->currentIndex(), 0);
}

void WheelGuardTest::wheelLeavesSpinBoxAlone()
{
    Page page;
    QVERIFY(QTest::qWaitForWindowExposed(&page.area));
    WheelGuard::install(&page.area);
    wheelDown(page.spin);
    QCOMPARE(page.spin->value(), 50);
}

void WheelGuardTest::wheelLeavesSliderAlone()
{
    Page page;
    QVERIFY(QTest::qWaitForWindowExposed(&page.area));
    WheelGuard::install(&page.area);
    wheelDown(page.slider);
    QCOMPARE(page.slider->value(), 50);
}

void WheelGuardTest::wheelScrollsThePageInstead()
{
    Page page;
    QVERIFY(QTest::qWaitForWindowExposed(&page.area));
    WheelGuard::install(&page.area);
    QVERIFY2(page.area.verticalScrollBar()->maximum() > 0, "fixture page is not scrollable");
    QCOMPARE(page.area.verticalScrollBar()->value(), 0);
    wheelDown(page.combo);
    QVERIFY2(page.area.verticalScrollBar()->value() > 0, "wheel event did not reach the scroll area");
}

void WheelGuardTest::hoverDoesNotFocusValueWidgets()
{
    // Wheel focus would let the next wheel tick land in the widget; clicking
    // must still focus it so it stays usable from the keyboard.
    Page page;
    WheelGuard::install(&page.area);
    QCOMPARE(page.combo->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(page.spin->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(page.slider->focusPolicy(), Qt::StrongFocus);
}

QTEST_MAIN(WheelGuardTest)
#include "tst_wheelguard.moc"
