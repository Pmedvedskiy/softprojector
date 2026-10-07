#ifndef LISTZOOM_HPP
#define LISTZOOM_HPP

#include <QObject>

class QAbstractButton;
class QAbstractItemView;

// Lets the user size the text of one list with a smaller / bigger button
// pair, one point per click. A size of 0 means "inherit the normal font".
class ListZoom : public QObject
{
    Q_OBJECT
public:
    ListZoom(QAbstractItemView *view, QAbstractButton *smaller, QAbstractButton *bigger,
             QObject *parent = nullptr);

    int fontSize() const;            // 0 = normal font
    void setFontSize(int pointSize); // clamped to 6..40, 0 or less = normal font

private:
    int currentPointSize() const;

    QAbstractItemView *view;
    int size;
};

#endif // LISTZOOM_HPP
