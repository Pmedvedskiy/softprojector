# Tests for the operator-side text search: clean(), Bible::searchBible and
# the BibleWidget search box, run headless against an in-memory SQLite Bible.
#
# Build out of source (tests/build is git-ignored):
#   mkdir -p tests/build/search && cd tests/build/search
#   ~/Qt/6.8.3/gcc_64/bin/qmake6 ../../search/search.pro && make
#   QT_QPA_PLATFORM=offscreen ./tst_search

TARGET = tst_search
include(../common.pri)
SOURCES += tst_search.cpp
