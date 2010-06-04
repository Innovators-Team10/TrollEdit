#include "fold_button.h"
#include "block.h"
#include "block_group.h"
#include "text_item.h"
#include "tree_element.h"

FoldButton::FoldButton(Block *parentBlock)
    : QGraphicsPixmapItem(parentBlock)
{
    Q_ASSERT(parentBlock != 0);

    setAcceptHoverEvents(true);
    plus = QPixmap(":/plus.png");
    minus = QPixmap(":/minus.png");
    myBlock = parentBlock;
    foldText = "";
//    setToolTip(myBlock->toolTip());

    setPixmap(minus);
    setTransformOriginPoint(plus.size().width()/2, plus.size().height()/2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(5);
    setAcceptedMouseButtons(Qt::LeftButton);

//    updatePos();
}

void FoldButton::updatePos() {
//    Block *refBlock = myBlock; // refBlock is floating ancestor-or-self of myBlock
//    while (!refBlock->getElement()->isFloating() && refBlock->parentBlock() != 0)
//        refBlock = refBlock->parentBlock();

//    QPointF flBlockPos = myBlock->mapFromItem(refBlock, 0, 0);

    qreal x = /*flBlockPos.x()*/ - plus.size().width() - 10;
    qreal y = myBlock->blockGroup()->CHAR_HEIGHT/2 - plus.size().height()/2;
    setPos(x, y);


//    Block *refBlock = myBlock; // refBlock is floating ancestor-or-self of myBlock
//    while (!refBlock->getElement()->isFloating() && refBlock->parentBlock() != 0)
//        refBlock = refBlock->parentBlock();
//
//    QPointF myBlockPos = myBlock->blockGroup()->mapFromItem(myBlock, 0, 0);
//    QPointF flBlockPos = refBlock->blockGroup()->mapFromItem(refBlock, 0, 0);
//
//    qreal x = flBlockPos.x() - plus.size().width() - 10;
//    qreal y = myBlock->blockGroup()->CHAR_HEIGHT/2 - plus.size().height()/2;
//    if (myBlock->getElement()->isFloating())
//        y += flBlockPos.y();
//    else
//        y += myBlockPos.y();
//    setPos(x, y);
}

void FoldButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rect = boundingRect();//.adjusted(-1,-1,0,0);
    painter->fillRect(rect, Qt::white);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QBrush(Qt::black), 1));
    painter->drawRoundRect(rect);
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
//    QGraphicsPixmapItem::mousePressEvent(event);
}

void FoldButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setScale(1.5);
//    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void FoldButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setScale(1);
//    QGraphicsPixmapItem::hoverLeaveEvent(event);
}

