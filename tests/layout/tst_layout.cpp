// Operator-window layout: the live chapter panel (listShow at the far right
// of the main window) gets its own text size, changed one point at a time with
// two buttons in its title row, and layout state (zoom size, splitter
// positions) survives a restart through the window-layout settings.

#include <QtTest>
#include <QtSql>
#include <QFontInfo>
#include <QListWidget>
#include <QSplitter>
#include <QToolButton>

#include "biblewidget.hpp"
#include "listzoom.hpp"
#include "settings.hpp"

class LayoutTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

    void plusRaisesFontByOnePoint();
    void minusLowersFontByOnePoint();
    void plusStopsAtLargestSize();
    void minusStopsAtSmallestSize();
    void setterAppliesSizeToList();
    void zeroRestoresDefaultFont();

    void sizeSurvivesSettingsRoundTrip();
    void missingKeyMeansDefaultSize();

    void columnWidthsSurviveSettingsRoundTrip();
};

void LayoutTest::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    QVERIFY2(db.open(), qPrintable(db.lastError().text()));
}

void LayoutTest::init()
{
    QSqlQuery q;
    QVERIFY2(q.exec("DROP TABLE IF EXISTS Settings"), qPrintable(q.lastError().text()));
    QVERIFY2(q.exec("CREATE TABLE Settings (type TEXT, sets TEXT)"), qPrintable(q.lastError().text()));
}

namespace {

// A list with its two zoom buttons, wired the way the main window wires listShow.
struct ZoomedList
{
    QListWidget list;
    QToolButton minus;
    QToolButton plus;
    ListZoom zoom{&list, &minus, &plus};

    int pointSize() const { return QFontInfo(list.font()).pointSize(); }
};

} // namespace

void LayoutTest::plusRaisesFontByOnePoint()
{
    ZoomedList z;
    int before = z.pointSize();

    z.plus.click();

    QCOMPARE(z.pointSize(), before + 1);
    QCOMPARE(z.zoom.fontSize(), before + 1);
}

void LayoutTest::minusLowersFontByOnePoint()
{
    ZoomedList z;
    int before = z.pointSize();

    z.minus.click();

    QCOMPARE(z.pointSize(), before - 1);
    QCOMPARE(z.zoom.fontSize(), before - 1);
}

void LayoutTest::plusStopsAtLargestSize()
{
    ZoomedList z;
    z.zoom.setFontSize(40);

    z.plus.click();

    QCOMPARE(z.pointSize(), 40);
    QCOMPARE(z.zoom.fontSize(), 40);
}

void LayoutTest::minusStopsAtSmallestSize()
{
    ZoomedList z;
    z.zoom.setFontSize(6);

    z.minus.click();

    QCOMPARE(z.pointSize(), 6);
    QCOMPARE(z.zoom.fontSize(), 6);
}

void LayoutTest::setterAppliesSizeToList()
{
    ZoomedList z;

    z.zoom.setFontSize(18);

    QCOMPARE(z.pointSize(), 18);
    QCOMPARE(z.zoom.fontSize(), 18);
}

void LayoutTest::zeroRestoresDefaultFont()
{
    ZoomedList z;
    int defaultSize = z.pointSize();
    z.zoom.setFontSize(18);

    z.zoom.setFontSize(0);

    QCOMPARE(z.pointSize(), defaultSize);
    QCOMPARE(z.zoom.fontSize(), 0);
}

void LayoutTest::sizeSurvivesSettingsRoundTrip()
{
    Settings saved;
    saved.spMain.showListFontSize = 18;
    saved.saveNewSettings();

    Settings loaded;
    loaded.loadSettings();

    QCOMPARE(loaded.spMain.showListFontSize, 18);
}

void LayoutTest::missingKeyMeansDefaultSize()
{
    // A database written before this setting existed has no key for it.
    QSqlQuery q;
    QVERIFY(q.exec("INSERT INTO Settings VALUES ('spMain', 'isWindowMaximized = false')"));

    Settings loaded;
    loaded.loadSettings();

    QCOMPARE(loaded.spMain.showListFontSize, 0);
}

void LayoutTest::columnWidthsSurviveSettingsRoundTrip()
{
    // Book | Chapter | Verse column widths are a QSplitter state stored as hex
    // text in the spMain settings block.
    BibleWidget before;
    QSplitter *columns = before.findChild<QSplitter *>("columns_splitter");
    QVERIFY(columns);
    columns->setSizes(QList<int>() << 150 << 80 << 400);
    QByteArray state = before.getColumnsSplitterState();
    QVERIFY(!state.isEmpty());

    Settings saved;
    saved.spMain.bibleColumnsSplitter = state;
    saved.saveNewSettings();

    Settings loaded;
    loaded.loadSettings();
    QCOMPARE(loaded.spMain.bibleColumnsSplitter, state);

    BibleWidget after;
    after.setColumnsSplitterState(loaded.spMain.bibleColumnsSplitter);
    QCOMPARE(after.findChild<QSplitter *>("columns_splitter")->sizes(), columns->sizes());
}

QTEST_MAIN(LayoutTest)
#include "tst_layout.moc"
