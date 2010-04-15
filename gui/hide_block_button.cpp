#include "hide_block_button.h"
#include "block.h"

HideBlockButton::HideBlockButton(QGraphicsItem *parent)
    : QGraphicsPixmapItem(QPixmap(":/res/Untitled.png"), parent)
{
//    setAcceptHoverEvents(true);
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
    if (event->button() == Qt::LeftButton) {
        Block *block = qgraphicsitem_cast<Block*>(parentItem());
        if (block != 0)
            block->setFolded(!block->isFolded());
    }
}

int HideBlockButton::type() const
{
    return Type;
}
