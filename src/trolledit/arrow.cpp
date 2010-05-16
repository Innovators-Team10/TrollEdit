#include <QtGui>
#include <cmath>

#include "arrow.h"
#include "block.h"
#include "doc_block.h"
#include "block_group.h"

const qreal Pi = 3.14;

Arrow::Arrow(DocBlock *startItem, Block *endItem, BlockGroup *parentGroup)
    : QGraphicsLineItem(parentGroup)
{
    Q_ASSERT(startItem != 0);Q_ASSERT(endItem != 0);Q_ASSERT(parentGroup != 0);
    myStartItem = startItem;
    myEndItem = endItem;
    myColor = Qt::black;
    setPen(QPen(myColor, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(10);
    connect(myStartItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(myEndItem, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(myEndItem, SIGNAL(visibilityChanged(bool)), this, SLOT(updateVisibility(bool)));

    line1 = QLineF(startPoint(), midPoint());
    line2 = QLineF(midPoint(), endPoint());
}

Arrow::~Arrow()
{
    myStartItem = 0;
    myEndItem = 0;
}

QRectF Arrow::boundingRect() const
{
    qreal extra = (pen().width() + 20) / 2.0;

    QRectF rect1 = QRectF(line1.p1(), QSizeF(line1.dx(), line1.dy()));
    rect1 = rect1.normalized().adjusted(-extra, -extra, extra, extra);

    QRectF rect2 = QRectF(line2.p1(), QSizeF(line2.dx(), line2.dy()));
    rect2 = rect2.normalized().adjusted(-extra, -extra, extra, extra);
    return rect2.united(rect1);
}

QPainterPath Arrow::shape() const
{
    QPainterPath path; //= QGraphicsLineItem::shape();
    path.moveTo(line1.p1());
    path.lineTo(line1.p2());
    path.lineTo(line2.p2());

    QPainterPathStroker ps;
    ps.setCapStyle(pen().capStyle());
    ps.setWidth(pen().widthF());
    ps.setJoinStyle(pen().joinStyle());
    ps.setMiterLimit(pen().miterLimit());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);

    p.addPolygon(arrowHead);
    return p;
}

void Arrow::updateVisibility(bool flag)
{
    setVisible(flag);
    myStartItem->setVisible(flag);
}

void Arrow::deleteLater()
{
    setVisible(false);  // otherwise paint would be called before arrow is actually deleted
    QObject::deleteLater();
}

QPointF Arrow::startPoint() // TODO, if start->right < end->left ...
{
    return mapFromItem(myStartItem,
                       0,
                       myStartItem->idealSize().height()/2);
}

QPointF Arrow::midPoint() // TODO
{
    qreal y = mapFromItem(myEndItem,
                          0,
                          myEndItem->idealSize().height()/2).y();
    qreal x = mapFromItem(myStartItem,
                          -40,
                          0).x();
    return QPointF(x, y);
}

QPointF Arrow::endPoint() // TODO
{
    return mapFromItem(myEndItem,
                       myEndItem->idealSize().width(),
                       myEndItem->idealSize().height()/2);
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                  QWidget *)
{
//    if (myStartItem->collidesWithItem(myEndItem)) return;

    QPen myPen = pen();
    myPen.setColor(myColor);
    qreal arrowSize = 10;
    painter->setPen(myPen);
    painter->setBrush(myColor);

    setLine(QLineF(startPoint(), endPoint()));

    line1 = QLineF(startPoint(), midPoint());
    line2 = QLineF(midPoint(), endPoint());

    double angle = acos(line1.dx() / line1.length());
    if (line1.dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line1.p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                            cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line1.p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                            cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line1.p1() << arrowP1 << arrowP2;
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(line1);
    painter->drawLine(line2);
    painter->drawPolygon(arrowHead);
}
