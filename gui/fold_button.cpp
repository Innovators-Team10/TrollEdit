#include "fold_button.h"
#include "block.h"
#include "block_group.h"
#include "text_item.h"
#include "../analysis/tree_element.h"

FoldButton::FoldButton(Block *parentBlock, BlockGroup *parentGruoup)
    : QGraphicsPixmapItem(parentGruoup)
{
    Q_ASSERT(parentBlock != 0);

    setAcceptHoverEvents(true);
    plus = QPixmap(":/res/plus.png");
    minus = QPixmap(":/res/minus.png");
    myBlock = parentBlock;
    setToolTip(myBlock->getElement()->getType().replace("_", " "));

    setPixmap(minus);
    setTransformOriginPoint(plus.size().width()/2, plus.size().height()/2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(10);
    setAcceptedMouseButtons(Qt::LeftButton);

    updatePos();
}

void FoldButton::updatePos() {
    QPointF pos(7 ,0);
    pos.ry() = myBlock->scenePos().y();
    pos.ry() += 5;

    setPos(pos);
}

void FoldButton::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget)
{
    QRectF rect = boundingRect().adjusted(-1,-1,0,0);
    painter->fillRect(rect, Qt::white);
    painter->drawRect(rect);
    QGraphicsPixmapItem::paint(painter, option, widget);
}

void FoldButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (myBlock->isFolded()) {
        myBlock->setFolded(false);
        setPixmap(minus);
    } else {
        myBlock->setFolded(true);
        setPixmap(plus);
    }
    event->accept();
}

void FoldButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setScale(1.5);
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void FoldButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setScale(1);
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}
