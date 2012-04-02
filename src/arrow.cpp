#include <QtGui>
#include <math.h>

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
    setPen(QPen(myColor, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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
    QPainterPath path;
    path.moveTo(line1.p1());
    path.lineTo(line1.p2());
    path.lineTo(line2.p2());

    path.addPolygon(arrowHead);
    return path;
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

int S = 40;

QPointF Arrow::startPoint()
{
    QPointF startLT = mapFromItem(myStartItem, 0, 0);
    QPointF endLT = mapFromItem(myEndItem, 0, 0);
    if(startLT.x() + myStartItem->idealSize().width() + S
        < endLT.x() + myEndItem->idealSize().width())
        return startLT + QPointF(myStartItem->idealSize().width(), 0);
    else
        return startLT;
}

QPointF Arrow::midPoint()
{
    QPointF startLT = mapFromItem(myStartItem, 0, 0);
    QPointF endLT = mapFromItem(myEndItem, 0, 0);
    qreal x;

    if(startLT.x() + myStartItem->idealSize().width() + S
        < endLT.x() + myEndItem->idealSize().width())
        x = startLT.x() + myStartItem->idealSize().width() + S;
    else
        x = startLT.x() - S;

    return QPointF(x, endLT.y());
}

QPointF Arrow::endPoint() // TODO
{
    return mapFromItem(myEndItem, myEndItem->idealSize().width(),0);
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

    //arrow corner
    cornerLine1= QLineF(endPoint(),endPoint()+QPointF(0,10));
    cornerLine2= QLineF(endPoint(),endPoint()-QPointF(10,0));

    arrowHead.clear();
    arrowHead << line1.p1() << arrowP1 << arrowP2;
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawLine(line1);
    painter->drawLine(line2);
    painter->drawLine(cornerLine1);
     painter->drawLine(cornerLine2);
    painter->drawPolygon(arrowHead);
}
