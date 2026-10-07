#include "../headers/listzoom.hpp"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QFontInfo>

namespace {
const int minPointSize = 6;
const int maxPointSize = 40;
}

ListZoom::ListZoom(QAbstractItemView *view, QAbstractButton *smaller, QAbstractButton *bigger,
                   QObject *parent)
    : QObject(parent), view(view), size(0)
{
    connect(smaller, &QAbstractButton::clicked, this, [this] { setFontSize(currentPointSize() - 1); });
    connect(bigger, &QAbstractButton::clicked, this, [this] { setFontSize(currentPointSize() + 1); });
}

int ListZoom::fontSize() const
{
    return size;
}

void ListZoom::setFontSize(int pointSize)
{
    QFont f; // attributes left unset keep inheriting from the parent font
    if (pointSize > 0)
    {
        size = qBound(minPointSize, pointSize, maxPointSize);
        f.setPointSize(size);
    }
    else
        size = 0;
    view->setFont(f);
}

int ListZoom::currentPointSize() const
{
    return QFontInfo(view->font()).pointSize();
}
