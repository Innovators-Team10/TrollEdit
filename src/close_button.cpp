/** 
* @file fold_button.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class FoldButton and it's functions and identifiers
*/

#include "close_button.h"
#include "block.h"
#include "block_group.h"
#include "text_item.h"
#include "tree_element.h"
#include "close_button.h"

CloseButton::CloseButton(Block *parentBlock)
    : QGraphicsPixmapItem(parentBlock)
{
    Q_ASSERT(parentBlock != 0);

    setAcceptHoverEvents(true);
    close = QPixmap(":/closeButton");
    myBlock = parentBlock;

    setPixmap(close);
    setTransformOriginPoint(close.size().width()/2, close.size().height()/2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(3);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void CloseButton::updatePos() {

    qreal x = 0;
    qreal y = myBlock->blockGroup()->CHAR_HEIGHT/2 - close.size().height()/2;

    QFontMetrics fm(myBlock->textItem()->font());
    int width = fm.width(myBlock->textItem()->toPlainText());

    if (!myBlock->getElement()->isFloating())
    {
        Block *lineStart = myBlock->blockGroup()->getBlockIn(myBlock->getLine());

        if (myBlock->getAncestorWhereFirst() != lineStart)
        {
            x = myBlock->mapFromItem(lineStart, 0, 0).x() + width ;
        }
    }

    x = x - (close.size().width() + 5) + width;
    setPos(x, y);
}

void CloseButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->myBlock->removeBlock(true);

    event->accept();
}
