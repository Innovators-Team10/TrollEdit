#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

#include "block.h"
#include "text_item.h"
#include "doc_block.h"

class DocBlock;

class Arrow : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 4 };

    Arrow(DocBlock *startItem, Block *endItem,
          Block *parent = 0, QGraphicsScene *scene = 0);

    int type() const
    { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color) { myColor = color; }
    DocBlock *startItem() const { return myStartItem; }
    Block *endItem() const { return myEndItem; }

public slots:
    void updatePosition();
    void updateVisibility(bool flag);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

private:
    DocBlock *myStartItem;
    Block *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
};

#endif
