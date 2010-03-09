#include "hide_block_button.h"
#include "block.h"

HideBlockButton::HideBlockButton(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
}

void HideBlockButton::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    painter->drawRect(boundingRect());
    QGraphicsPixmapItem::paint(painter, option, widget);
}

void HideBlockButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Block *block = qgraphicsitem_cast<Block*>(parentItem());
    block->setFolded(!block->isFolded());
    //QGraphicsPixmapItem::mousePressEvent(event);  // ?
}

int HideBlockButton::type() const
{
    return Type;
}
