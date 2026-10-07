#ifndef WHEELGUARD_HPP
#define WHEELGUARD_HPP

#include <QObject>

class QWidget;

// Keeps the mouse wheel from changing combo boxes, spin boxes and sliders
// that happen to be under the pointer, so wheeling over a settings page only
// scrolls the page. Clicking a widget still focuses it for keyboard editing.
class WheelGuard : public QObject
{
    Q_OBJECT
public:
    // Guards every value widget below root that exists at call time.
    static void install(QWidget *root);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    explicit WheelGuard(QObject *parent);
};

#endif // WHEELGUARD_HPP
