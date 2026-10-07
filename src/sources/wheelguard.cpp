#include "../headers/wheelguard.hpp"

#include <QAbstractSpinBox>
#include <QComboBox>
#include <QEvent>
#include <QSlider>
#include <QWidget>

WheelGuard::WheelGuard(QObject *parent)
    : QObject(parent)
{
}

void WheelGuard::install(QWidget *root)
{
    WheelGuard *guard = new WheelGuard(root);
    const auto guardAll = [guard](const auto &widgets) {
        for (QWidget *w : widgets)
        {
            // Focus by click or Tab only; wheel focus would let the next
            // wheel tick land in the widget after all.
            w->setFocusPolicy(Qt::StrongFocus);
            w->installEventFilter(guard);
        }
    };
    guardAll(root->findChildren<QComboBox *>());
    guardAll(root->findChildren<QAbstractSpinBox *>());
    guardAll(root->findChildren<QSlider *>());
}

bool WheelGuard::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Wheel)
    {
        // The widget never sees the wheel. Leaving the event unaccepted makes
        // QApplication carry it on up the parent chain (it does so for the
        // spontaneous events real mice produce), so the enclosing scroll
        // area scrolls as if the pointer were over empty space.
        event->ignore();
        return true;
    }
    return QObject::eventFilter(watched, event);
}
