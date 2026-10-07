# Tests for the operator-window layout: the verse-list zoom buttons in the
# Bible widget and the persistence of layout state (zoom size, splitter
# positions) through the window-layout settings.
#
# Build out of source (tests/build is git-ignored):
#   mkdir -p tests/build/layout && cd tests/build/layout
#   ~/Qt/6.8.3/gcc_64/bin/qmake6 ../../layout/layout.pro && make
#   QT_QPA_PLATFORM=offscreen ./tst_layout

TARGET = tst_layout
include(../common.pri)
SOURCES += tst_layout.cpp
