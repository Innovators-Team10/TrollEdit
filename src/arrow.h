/**
 * arrow.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class Arrow and it's funtions and identifiers
 *
 */
#ifndef ARROW_H
#define ARROW_H

#include <QObject>
#include <QGraphicsLineItem>

class Block;
class DocBlock;
class BlockGroup;

class Arrow : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    enum { Type = UserType + 4 };

    Arrow(DocBlock *startItem, Block *endItem, BlockGroup *parentGroup);
    ~Arrow();

    int type() const { return Type; }
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setColor(const QColor &color) { myColor = color; }
    DocBlock *startItem() const { return myStartItem; }
    Block *endItem() const { return myEndItem; }

public slots:
    void updateVisibility(bool flag);
    void deleteLater();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

private:
    QPointF startPoint();
    QPointF midPoint();
    QPointF endPoint();

    DocBlock *myStartItem;
    Block *myEndItem;
    QColor myColor;
    QPolygonF arrowHead;
    QLineF line1, line2, cornerLine1, cornerLine2;
};

#endif
