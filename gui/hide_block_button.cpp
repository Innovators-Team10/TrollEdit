#include "hide_block_button.h"
#include "block.h"

HideBlockButton::HideBlockButton(Block *parent)
    : QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
    plus = QPixmap(":/res/plus.png");
    minus = QPixmap(":/res/minus.png");
    myBlock = parent;
    setPixmap(minus);
    setTransformOriginPoint(plus.size().width()/2, plus.size().height()/2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(10);
    setAcceptedMouseButtons(Qt::LeftButton);
    focus = false;
    updatePos();
}

void HideBlockButton::updatePos() {
    QPointF pos = myBlock->mapFromScene(QPointF(10,0));
    pos.ry() = 0;
    setPos(pos);
}

void HideBlockButton::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    if (focus)
        setScale(1.5);
    else
        setScale(1);
    painter->drawRect(boundingRect().adjusted(-1,-1,0,0));
    QGraphicsPixmapItem::paint(painter, option, widget);
}

void HideBlockButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (myBlock != 0) {
        if (myBlock->isFolded()) {
            myBlock->setFolded(false);
            setPixmap(minus);
        } else {
            myBlock->setFolded(true);
            setPixmap(plus);
        }
    }
}

void HideBlockButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    focus = true;
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void HideBlockButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    focus = false;
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
int HideBlockButton::type() const
{
    return Type;
}
