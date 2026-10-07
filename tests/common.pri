# Shared build settings for the test projects: compile the real Bible widget
# and everything it links against, so tests drive production code headless.
# Include from a test .pro after setting TARGET; the .pro adds its own
# tst_*.cpp to SOURCES.

QT += core gui widgets sql testlib
CONFIG += console testcase
CONFIG -= app_bundle

SP = $$PWD/../src
INCLUDEPATH += $$SP/headers

HEADERS += \
    $$SP/headers/biblewidget.hpp \
    $$SP/headers/highlight.hpp \
    $$SP/headers/bible.hpp \
    $$SP/headers/song.hpp \
    $$SP/headers/spfunctions.hpp \
    $$SP/headers/settings.hpp

SOURCES += \
    $$SP/sources/biblewidget.cpp \
    $$SP/sources/bible.cpp \
    $$SP/sources/highlight.cpp \
    $$SP/sources/song.cpp \
    $$SP/sources/spfunctions.cpp \
    $$SP/sources/settings.cpp

FORMS += $$SP/ui/biblewidget.ui
