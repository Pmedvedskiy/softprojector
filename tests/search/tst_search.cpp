// Search tests. The Qt 5 -> Qt 6 migration replaced QRegExp with
// QRegularExpression, whose \w, \W and \b are ASCII-only unless
// UseUnicodePropertiesOption is set. These tests pin the behaviour for
// non-Latin (Cyrillic) text, which is what the church uses.

#include <QtTest>
#include <QtSql>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextDocument>
#include <QTextLayout>

#include "song.hpp"        // clean()
#include "bible.hpp"
#include "biblewidget.hpp"

namespace {

const QString kjvId = "1";
const QString rstId = "2";

struct FixtureVerse { const char *bible; int book; int chapter; int verse; const char *text; };

// Genesis 1:1-5 from the Russian Synodal translation, plus Genesis 1:1 KJV.
const FixtureVerse verses[] = {
    { "2", 1, 1, 1, "В начале сотворил Бог небо и землю." },
    { "2", 1, 1, 2, "Земля же была безвидна и пуста, и тьма над бездною, и Дух Божий носился над водою." },
    { "2", 1, 1, 3, "И сказал Бог: да будет свет. И стал свет." },
    { "2", 1, 1, 4, "И увидел Бог свет, что он хорош, и отделил Бог свет от тьмы." },
    { "2", 1, 1, 5, "И назвал Бог свет днем, а тьму ночью. И был вечер, и было утро: день один." },
    { "1", 1, 1, 1, "In the beginning God created the heaven and the earth." },
};

QRegularExpression::PatternOptions searchOptions()
{
    return QRegularExpression::CaseInsensitiveOption
         | QRegularExpression::UseUnicodePropertiesOption;
}

} // namespace

class SearchTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void cleanKeepsCyrillicLetters();
    void cleanStripsPunctuation();

    void bibleAllWordsSearchMatchesCyrillic();
    void bibleAllWordsSearchMatchesLatin();

    void highlighterMarksCyrillicWholeWord();

    void bibleWidgetSearch_data();
    void bibleWidgetSearch();
};

void SearchTest::initTestCase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    QVERIFY2(db.open(), qPrintable(db.lastError().text()));

    QSqlQuery q;
    QVERIFY2(q.exec("CREATE TABLE BibleBooks (bible_id INTEGER, id INTEGER, book_name TEXT, chapter_count INTEGER DEFAULT 0)"),
             qPrintable(q.lastError().text()));
    QVERIFY2(q.exec("CREATE TABLE BibleVerse (verse_id TEXT, bible_id TEXT, book TEXT, chapter INTEGER, verse INTEGER, verse_text TEXT)"),
             qPrintable(q.lastError().text()));
    QVERIFY(q.exec("INSERT INTO BibleBooks VALUES (2, 1, 'Бытие', 50)"));
    QVERIFY(q.exec("INSERT INTO BibleBooks VALUES (1, 1, 'Genesis', 50)"));

    for (const FixtureVerse &v : verses) {
        q.prepare("INSERT INTO BibleVerse VALUES (?, ?, ?, ?, ?, ?)");
        q.addBindValue(QString("B%1C%2V%3").arg(v.book, 3, 10, QChar('0'))
                                            .arg(v.chapter, 3, 10, QChar('0'))
                                            .arg(v.verse, 3, 10, QChar('0')));
        q.addBindValue(QString(v.bible));
        q.addBindValue(QString::number(v.book));
        q.addBindValue(v.chapter);
        q.addBindValue(v.verse);
        q.addBindValue(QString::fromUtf8(v.text) + "\n");
        QVERIFY2(q.exec(), qPrintable(q.lastError().text()));
    }
}

void SearchTest::cleanKeepsCyrillicLetters()
{
    QCOMPARE(clean("В начале, сотворил Бог!"), QString("В начале сотворил Бог"));
}

void SearchTest::cleanStripsPunctuation()
{
    QCOMPARE(clean("In the beginning, God!"), QString("In the beginning God"));
}

void SearchTest::bibleAllWordsSearchMatchesCyrillic()
{
    Bible bible;
    QString id = rstId;
    bible.setBiblesId(id);
    bible.loadOperatorBible();

    // "Contains All Words" hands searchBible an any-word pattern and lets it
    // check each word on its own.
    QRegularExpression anyWord("\\b(Бог|свет)\\b", searchOptions());
    QList<BibleSearch> results = bible.searchBible(true, anyWord);

    QCOMPARE(results.count(), 3); // verses 3, 4 and 5 contain both words
    QVERIFY(results.first().verse_text.startsWith("Бытие 1:3 "));
}

void SearchTest::bibleAllWordsSearchMatchesLatin()
{
    Bible bible;
    QString id = kjvId;
    bible.setBiblesId(id);
    bible.loadOperatorBible();

    QRegularExpression anyWord("\\b(God|heaven)\\b", searchOptions());
    QCOMPARE(bible.searchBible(true, anyWord).count(), 1);
}

void SearchTest::highlighterMarksCyrillicWholeWord()
{
    // The widget hands the search pattern to HighlightSearch so matches are
    // painted red in the results list.
    QTextDocument doc;
    HighlightSearch highlighter(&doc);
    highlighter.setHighlightText("\\bБог\\b");
    doc.setPlainText("Бытие 1:1 В начале сотворил Бог небо и землю.");
    highlighter.rehighlight();

    QList<QTextLayout::FormatRange> marks = doc.firstBlock().layout()->formats();
    QCOMPARE(marks.count(), 1);
    QCOMPARE(marks.first().start, doc.toPlainText().indexOf("Бог"));
    QCOMPARE(marks.first().length, 3);
}

void SearchTest::bibleWidgetSearch_data()
{
    // Search type indexes follow comboBoxSearchType in biblewidget.ui:
    // 0 Contains Phrase, 1 Contains Word Phrase, 2 Verse Begins,
    // 3 Contains Any Word, 4 Contains All Words.
    QTest::addColumn<int>("type");
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("expectedCount");
    QTest::addColumn<QString>("firstResultPrefix");

    QTest::newRow("phrase")                      << 0 << "сотворил Бог" << 1 << "Бытие 1:1 ";
    QTest::newRow("phrase matches inside words") << 0 << "тьм"          << 3 << "Бытие 1:2 ";
    QTest::newRow("whole word")                  << 1 << "тьма"         << 1 << "Бытие 1:2 ";
    QTest::newRow("whole word rejects fragment") << 1 << "тьм"          << 0 << "";
    QTest::newRow("whole word ignores case")     << 1 << "бог"          << 4 << "Бытие 1:1 ";
    QTest::newRow("verse begins")                << 2 << "И сказал"     << 1 << "Бытие 1:3 ";
    QTest::newRow("any word")                    << 3 << "землю тьма"   << 2 << "Бытие 1:1 ";
    QTest::newRow("all words")                   << 4 << "Бог свет"     << 3 << "Бытие 1:3 ";
}

void SearchTest::bibleWidgetSearch()
{
    QFETCH(int, type);
    QFETCH(QString, text);
    QFETCH(int, expectedCount);
    QFETCH(QString, firstResultPrefix);

    BibleWidget widget;
    QString id = rstId;
    widget.bible.setBiblesId(id);
    widget.bible.loadOperatorBible();

    QLineEdit *searchBox = widget.findChild<QLineEdit *>("search_ef");
    QComboBox *searchType = widget.findChild<QComboBox *>("comboBoxSearchType");
    QComboBox *searchRange = widget.findChild<QComboBox *>("comboBoxSearchRange");
    QPushButton *searchButton = widget.findChild<QPushButton *>("search_button");
    QListWidget *results = widget.findChild<QListWidget *>("search_results_list");
    QVERIFY(searchBox && searchType && searchRange && searchButton && results);

    searchRange->setCurrentIndex(0); // Entire Bible
    searchType->setCurrentIndex(type);
    searchBox->setText(text);
    searchButton->click();

    QCOMPARE(results->count(), expectedCount);
    if (!firstResultPrefix.isEmpty())
        QVERIFY2(results->item(0)->text().startsWith(firstResultPrefix),
                 qPrintable(results->item(0)->text()));
}

QTEST_MAIN(SearchTest)
#include "tst_search.moc"
