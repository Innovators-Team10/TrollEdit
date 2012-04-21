/** 
* @file fold_button.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class FoldButton and it's functions and identifiers
*/

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
    plus = QPixmap(":/plus");
    minus = QPixmap(":/minus");
    myBlock = parentBlock;
    foldText = "";

    setPixmap(minus);
    setTransformOriginPoint(plus.size().width()/2, plus.size().height()/2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    setZValue(3);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void FoldButton::updatePos() {

    qreal x = 0;
    qreal y = myBlock->blockGroup()->CHAR_HEIGHT/2 - plus.size().height()/2;

    if (!myBlock->getElement()->isFloating())
    {
        Block *lineStart = myBlock->blockGroup()->getBlockIn(myBlock->getLine());

        if (myBlock->getAncestorWhereFirst() != lineStart)
        {
            x = myBlock->mapFromItem(lineStart, 0, 0).x();
        }
    }

    x -= (plus.size().width() + 5);
    setPos(x, y);
}

void FoldButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (myBlock->isFolded())
    {
        myBlock->setFolded(false);
        setPixmap(minus);
    }
    else
    {
        myBlock->setFolded(true);
        setPixmap(plus);
    }

    event->accept();
}
