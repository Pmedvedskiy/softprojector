# Tests for WheelGuard: inside the Settings dialog the mouse wheel must scroll
# the page, never change the combo box, spin box or slider under the pointer.
#
# Build out of source (tests/build is git-ignored):
#   mkdir -p tests/build/wheelguard && cd tests/build/wheelguard
#   ~/Qt/6.8.3/gcc_64/bin/qmake6 ../../wheelguard/wheelguard.pro && make
#   QT_QPA_PLATFORM=offscreen ./tst_wheelguard

QT += core gui widgets testlib
CONFIG += console testcase
CONFIG -= app_bundle
TARGET = tst_wheelguard

SP = $$PWD/../../src
INCLUDEPATH += $$SP/headers
HEADERS += $$SP/headers/wheelguard.hpp
SOURCES += tst_wheelguard.cpp $$SP/sources/wheelguard.cpp
