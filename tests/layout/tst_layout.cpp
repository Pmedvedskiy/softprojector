// Operator-window layout: splitter positions survive a restart through the
// window-layout settings.

#include <QtTest>
#include <QtSql>
#include <QSplitter>

#include "biblewidget.hpp"
#include "settings.hpp"

class LayoutTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();

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
