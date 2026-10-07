// Operator-window layout: the right-hand Bible column (chapter_preview_list)
// gets its own text size, changed one point at a time with two buttons next
// to "Go Live", and layout state (zoom size, splitter positions) survives a
// restart through the window-layout settings.

#include <QtTest>
#include <QtSql>
#include <QFontInfo>
#include <QListWidget>
#include <QSplitter>
#include <QToolButton>

#include "biblewidget.hpp"
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

struct Widgets
{
    QListWidget *list;
    QToolButton *plus;
    QToolButton *minus;
};

Widgets find(BibleWidget &w)
{
    return { w.findChild<QListWidget *>("chapter_preview_list"),
             w.findChild<QToolButton *>("btnVerseTextBigger"),
             w.findChild<QToolButton *>("btnVerseTextSmaller") };
}

int listPointSize(const Widgets &ui)
{
    return QFontInfo(ui.list->font()).pointSize();
}

} // namespace

void LayoutTest::plusRaisesFontByOnePoint()
{
    BibleWidget w;
    Widgets ui = find(w);
    QVERIFY(ui.list && ui.plus && ui.minus);
    int before = listPointSize(ui);

    ui.plus->click();

    QCOMPARE(listPointSize(ui), before + 1);
    QCOMPARE(w.getVerseListFontSize(), before + 1);
}

void LayoutTest::minusLowersFontByOnePoint()
{
    BibleWidget w;
    Widgets ui = find(w);
    QVERIFY(ui.list && ui.plus && ui.minus);
    int before = listPointSize(ui);

    ui.minus->click();

    QCOMPARE(listPointSize(ui), before - 1);
    QCOMPARE(w.getVerseListFontSize(), before - 1);
}

void LayoutTest::plusStopsAtLargestSize()
{
    BibleWidget w;
    Widgets ui = find(w);
    w.setVerseListFontSize(40);

    ui.plus->click();

    QCOMPARE(listPointSize(ui), 40);
    QCOMPARE(w.getVerseListFontSize(), 40);
}

void LayoutTest::minusStopsAtSmallestSize()
{
    BibleWidget w;
    Widgets ui = find(w);
    w.setVerseListFontSize(6);

    ui.minus->click();

    QCOMPARE(listPointSize(ui), 6);
    QCOMPARE(w.getVerseListFontSize(), 6);
}

void LayoutTest::setterAppliesSizeToList()
{
    BibleWidget w;
    Widgets ui = find(w);

    w.setVerseListFontSize(18);

    QCOMPARE(listPointSize(ui), 18);
    QCOMPARE(w.getVerseListFontSize(), 18);
}

void LayoutTest::zeroRestoresDefaultFont()
{
    BibleWidget w;
    Widgets ui = find(w);
    int defaultSize = listPointSize(ui);
    w.setVerseListFontSize(18);

    w.setVerseListFontSize(0);

    QCOMPARE(listPointSize(ui), defaultSize);
    QCOMPARE(w.getVerseListFontSize(), 0);
}

void LayoutTest::sizeSurvivesSettingsRoundTrip()
{
    Settings saved;
    saved.spMain.bibleVerseListFontSize = 18;
    saved.saveNewSettings();

    Settings loaded;
    loaded.loadSettings();

    QCOMPARE(loaded.spMain.bibleVerseListFontSize, 18);
}

void LayoutTest::missingKeyMeansDefaultSize()
{
    // A database written before this setting existed has no key for it.
    QSqlQuery q;
    QVERIFY(q.exec("INSERT INTO Settings VALUES ('spMain', 'isWindowMaximized = false')"));

    Settings loaded;
    loaded.loadSettings();

    QCOMPARE(loaded.spMain.bibleVerseListFontSize, 0);
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
