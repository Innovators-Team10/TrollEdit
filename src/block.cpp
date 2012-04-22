/** 
* @file block.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class Block and it's functions and identifiers
*/

#include "block.h"
#include "fold_button.h"
#include "block_group.h"
#include "doc_block.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"

#include <QDebug>
/**
 * Block class contructor, that creates a block from a specified element
 * as a child of a specified parent to the specified block group
 * @param el specified TreeElement
 * @param parentBlock parent block of a created block
 * @param blockGroup group of a created block
 */
Block::Block(TreeElement *el, Block *parentBlock, BlockGroup *blockGroup)
    : QGraphicsRectItem(parentBlock)
{
    repaintNeeded = false;

    if (parentBlock == 0) //! adding directly to group, no parent block
    {
        Q_ASSERT(blockGroup != 0);
        setParentItem(blockGroup);
        group = blockGroup;
        parent = 0;
        prevSib = 0;
    }
    else
    {
        parent = parentBlock;

        // destroy text item if needed
        if (parent->isTextBlock())
        {
            delete parent->myTextItem;
            parent->myTextItem = 0;
        }

        group = parent->group;

        // set links
        QList<Block*> siblings = parent->childBlocks();

        if (siblings.size() > 1) //! this block is in siblings already!
        {
            prevSib = siblings.at(siblings.size() - 2);
            prevSib->nextSib = this;
        }
        else
        {
            parent->firstChild = this;
            prevSib = 0;
        }

        if (el->getParent() == 0)
        {
            parentBlock->element->appendChild(el);
        }
    }

    firstChild = 0;
    nextSib = 0;

    // set line
    updateLine();

    // set element
    element = el;

    while (!element->isImportant())     //! skip unimportant
        element = (*element)[0];

    element->setBlock(this);

    // process rest of the AST
    if (element->isLeaf()) //! leaf - create text area
    {
        //qDebug() << "text " << element->getType() << " par: " << element->allowsParagraphs()  ;
        myTextItem = new TextItem(element->getType(), this, element->allowsParagraphs(), element->isPaired());
        //qDebug() << "this " << myTextItem;
    }
    else //! non-leaf - create rest of the tree
    {
        myTextItem = 0;
        setToolTip(element->getType().replace("_", " "));

       // int child_count = element->childCount();
        QList<TreeElement*> children = element->getChildren();
       // for (int i = 0; i < child_count; i++)children.size()
       for (int i = 0; i < children.size(); i++)
        {
          //  qDebug() << "child_count " << child_count <<"  size() "<< children.size() << " element:"<< element->getType();
            TreeElement *childEl = children[i];
            if (!childEl->isFloating()) //! create block from child element
            {
                new Block(childEl, this);
            }
            else //! create docblock form child element
            {
               if(TreeElement::DYNAMIC){
                    //! zisti ako to reprezentovat z AST-cka
                    //! preco sa vytvori DocBlock, ktory ma prazdny TextItem?
               }else{
                    QString text = childEl->getText();
                    childEl->deleteAllChildren();
                    new DocBlock(text, childEl, this, group);
               }
            }
        }
    }

    // set highlighting
    assignHighlighting(element);

    // set flags
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptDrops(true);
    folded = false;
    edited = false;
    showing = false;
    moreSpace = false;
    moveStarted = false;
    pointed = false;
    isSearchResult = false;
    foldButton = 0;
    level = 0;

    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);

    if (element->isSelectable())
    {
        setPen(QPen(QBrush(Qt::black), 2));
        setAcceptHoverEvents(true);
        timer = new QTimer(this);
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(acceptHover()));
    }
    else
    {
        timer = 0;
    }

    // set size
    updateGeometry(true);

//    updateFoldButton();

    if (parent == 0)
        setVisible(false);
}

Block::~Block()
{
    delete element;
}

void Block::assignHighlighting(TreeElement *el)
        // todo - remove hardcoded vales such as "declarator"
{
    if (el->isLeaf())
    {
        highlightFormat = group->docScene->getDefaultFormat();

        if (el->getParent())
        {
            QString parentType = el->getParent()->getType();

            if (group->docScene->hasFormatFor(parentType) && !el->getParent()->getType().startsWith("funct_"))
            {
                highlightFormat = group->docScene->getFormatFor(parentType);
            }
        }

        highlight(highlightFormat);
    }
    else
    {
        if (group->docScene->hasFormatFor(el->getType()))
        {
            QPair<QFont, QColor> highlightFormat = group->docScene->getFormatFor(el->getType());

            if (!el->getType().compare("funct_call"))
            {
                getFirstLeaf()->highlight(highlightFormat);
            }
            else if (!el->getType().compare("funct_definition"))
            {
                QList<Block*> blocks = childBlocks();
                foreach(Block* block, blocks)
                {
                    if (!block->element->getType().compare("declarator"))
                    {
                        block->getFirstLeaf()->highlight(highlightFormat);
                        break;
                    }
                }
            }
        }
    }
}

void Block::setParentBlock(Block *newParent, Block *nextSibling)
        // moves this element and all unimportant elements on way to parentBlock's element ("branch")
        // to newParent and stacks it before nextSibling (appends if nextSibling == 0)
        // NOTE: to remove block from its parent use removeBlock() which removes all empty ancestors too
        // and checks is root was removed
{
    if (newParent == this)
    {
        newParent = 0;//debug - toto nemoze nastavat
        qWarning("newParent == this in setParentBlock()");
    }

//    if (newParent == parent && nextSibling == nextSib) return;

    TreeElement *branch;
    // remove from old parent element
    Block *oldParent = this->parent;

    if (oldParent != 0)
    {
        TreeElement *oldParentEl = oldParent->element;
        // find ancestor that is child of oldParentEl (i.e. root of the branch)
        int index = oldParentEl->indexOfBranch(this->element);
        branch = (*oldParentEl)[index];
        // remove branch from original parent
        oldParentEl->removeChild(branch);
        // remove spaces
        element->setSpaces(0);
        // adjust line breaks
        Block *ancestor = getAncestorWhereLast();

        if (nextSib == 0 && !element->isFloating())
        {
            if(ancestor->element->isLineBreaking())
            {
                if (prevSib != 0 && prevSib->element->isLineBreaking())
                {
                    element->setLineBreaking(true);
                    prevSib->element->setLineBreaking(false);
                }
            }
            else
            {
                if (prevSib != 0 && prevSib->element->isLineBreaking())
                {
                    ancestor->element->setLineBreaking(true);
                    prevSib->element->setLineBreaking(false);
                }
            }
        }
        // remove links
        removeLinks();
    }
    else
    {
        branch = element->getRoot(); //! get root of unvisualized(?) branch
    }

    this->parent = 0;
    QGraphicsRectItem::setParentItem(0);

    // add to new parent element before nextSibling
    if (newParent != 0)
    {
        Q_ASSERT(newParent->group == group);

        if (nextSibling != 0 && nextSibling->parent != newParent)
        {
            qWarning("nextSibling is not child od newParent");
            nextSibling = 0;
        }

        TreeElement *newParentEl = newParent->element;

        if (nextSibling != 0) //! add to newParent before nextSibling
        {
            int index = newParentEl->indexOfBranch(nextSibling->element);
            newParentEl->insertChild(index, branch);
        }
        else //! append to newParent
        {
            newParentEl->appendChild(branch);
        }
        branch = 0;

        if (!element->isFloating())
        {
            // update links
            if (nextSibling != 0) //! use nextSibling to update links
            {
                prevSib = nextSibling->prevSib;

                if (prevSib != 0)
                {
                    prevSib->nextSib = this;
                }
                else
                {
                    newParent->firstChild = this;
                }

                nextSib = nextSibling;
                nextSibling->prevSib = this;
            }
            else //! use newParent to update links
            {
                QList<Block*> siblings = newParent->childBlocks();

                if (siblings.size() > 0) //! NOTE: this block is not in siblings yet!
                {
                    prevSib = siblings.at(siblings.size() - 1);
                    prevSib->nextSib = this;
                }
                else
                {
                    newParent->firstChild = this;
                    prevSib = 0;
                }

                nextSib = 0;
                // adjust line breaks when appending a linebreaking element

                if(element->isLineBreaking())
                {
                    element->setLineBreaking(false);
                    newParent->element->setLineBreaking(true);
                }
            }
            // remove textItem if needed
            if (newParent->myTextItem != 0)
            {
                delete newParent->myTextItem;
                newParent->myTextItem = 0;
            }
        }
        else
        {
            removeLinks();
        }
    }

    // remove from old parent block & append to new parent block
    QGraphicsRectItem::setParentItem(newParent);
    this->parent = newParent;

    if (nextSibling != 0)
        QGraphicsRectItem::stackBefore(nextSibling);
}

Block *Block::removeBlock(bool deleteThis)
{
    group->setModified(true);
    QList<Block*> toDelete;
    Block *toRemove = this;
    Block *next = getNext();
    bool changeSelected = false;
    bool breaking = false;
    Block *selected = group->selectedBlock();

    do //! remove block and all ancestors (that would became leafs) from hierarchy
    {
        Block *oldParent = toRemove->parent;

        if (oldParent == 0) {                   //! root test
            foreach (Block* bl, toDelete)       //! destroy collected blocks
                bl->deleteLater();
            group->analyzeAll("");              //! reset hierarchy
            return group->mainBlock();
        }
        toRemove->setParentBlock(0);                                //! remove from hierarchy

        if (toRemove == selected) changeSelected = true;            //! set flag if selected

        if (toRemove->element->isLineBreaking()) breaking = true;   //! set flag if line breaking

        toDelete << toRemove;
        toRemove = oldParent;
    }
    while (toRemove->firstChild == 0);

    if (!deleteThis)
    {
        toDelete.removeOne(this);                  //! top block (this) is not destroyed
    }

    if (!toDelete.contains(next))
    {
        if (changeSelected && toRemove != 0)       //! reselect if needed
            group->selectBlock(toRemove);

        if (breaking && next->prevSib != 0)        //! add linebreak if needed
            next->prevSib->element->setLineBreaking(true); //! !!!!! not sure if this is correct

        next->edited = true;
    }
    else
    {
        next = 0;
        group->deselect();
    }    

    foreach (Block* bl, toDelete) //! destroy collected blocks
    {
        group->removeFoldable(bl);
        bl->deleteLater();
    }

    return next;
}

void Block::removeLinks()
{
    if (nextSib != 0)
    {
        nextSib->prevSib = prevSib;

        if (prevSib == 0)
            nextSib->element->setSpaces(0);
    }

    if (prevSib != 0)
    {
        prevSib->nextSib = nextSib;
    }

    if (parent != 0 && parent->firstChild == this)
        parent->firstChild = nextSib;

    prevSib = nextSib = 0;
}


Block *Block::getFirstLeaf() const
{
    if (isTextBlock()) return const_cast<Block*>(this);

    Block *block = firstChild;

    while (!block->isTextBlock())
        block = block->firstChild;

    return block;
}

Block *Block::getLastLeaf() const
{
    if (isTextBlock()) return const_cast<Block*>(this);

    Block *block = childBlocks().last();

    while (!block->isTextBlock())
        block = block->childBlocks().last();

    return block;
}

Block *Block::getAncestorWhereFirst() const
{
    Block *block = const_cast<Block*>(this);

    if (element->isFloating()) return block;

    while (block->prevSib == 0 && block->parent != 0)
        block = block->parent;

    return block;
}

Block *Block::getAncestorWhereLast() const
{
    Block *block = const_cast<Block*>(this);

    if (element->isFloating()) return block;

    while (block->nextSib == 0 && block->parent != 0)
        block = block->parent;

    return block;
}

Block *Block::getNext(bool textOnly) const
{
    Block *next = const_cast<Block*>(this);

    if (parent != 0)
    {
        if (nextSib == 0)
            return parent->getNext(textOnly);

        next = nextSib;
    }

    if (textOnly)
        return next->getFirstLeaf();

    return next;
}

Block *Block::getPrev(bool textOnly) const
{
    Block *prev = const_cast<Block*>(this);

    if (parent != 0)
    {
        if (prevSib == 0)
            return parent->getPrev(textOnly);

        prev = prevSib;
    }

    if (textOnly)
    {
        return prev->getLastLeaf();
    }

    return prev;
}

Block *Block::getFirstSelectableAncestor() const
{
    Block *block = const_cast<Block*>(this);

    if (element->isFloating())
        return block;

    if (parent != 0)
        block = block->parent;

    while (!block->element->isSelectable() && block->parent != 0)
        block = block->parent;

    return block;
}

int Block::numberOfLines() const
{
    if (isTextBlock())
    {
        return myTextItem->document()->lineCount();
    }
    else
    {
        Block *last = childBlocks().last();
        return (last->line + last->numberOfLines() - 1) - this->line + 1;
    }
}

bool Block::hasMoreLines() const
{
    if (isTextBlock())
    {
        return myTextItem->document()->lineCount() > 1;
    }
    else
    {
        Block *last = childBlocks().last();
        return (last->line > this->line) || last->hasMoreLines();
    }
}

int Block::getLineAfter(QPointF pos) const
{
    if (!hasMoreLines()) return line;

    QPair<Block*, bool> targetRight = findClosestChild(pos);    //! - QPointF(0, group->CHAR_HEIGHT/2.0));
    Block *target = targetRight.first;

    if (target == 0)
        return getLastLeaf()->line;

    if (!target->hasMoreLines())
        return target->line;
    else
        return target->getLineAfter(target->mapFromParent(pos));

}

//void Block::addBlockInLine(Block *block, QPointF pos)
//{
//    QPair<Block*, bool> targetRight = findClosestLeaf(pos);
//    Block *target = targetRight.first;
//    if (!targetRight.second)
//        target = target->nextSib;
//    block->setParentBlock(this);
//    block->stackBeforeBlock(target);
//    block->element->setLineBreaking(false);
//}
//
//void Block::addBlockBeforeLine(Block *block, int lineNo)
//{
//    Block *target = group->getBlockIn(lineNo)->getFirstLeaf();
//    block->setParentBlock(target->parent);
//    if (lineNo <= group->getLastLine())
//        block->stackBeforeBlock(target);
//    block->element->setLineBreaking(true);
//}

Block *Block::addTextCursorAt(QPointF pos)
{
    if (isTextBlock()) //! add cursor to this block
    {
        pos = mapToItem(myTextItem, pos);   //! map to my TextItem
        int cursorPos;

        // find cursor position:
        if (element->allowsParagraphs()) //! use slow hitTest for multiline blocks only
        {
            cursorPos = myTextItem->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
        }
        else //! use calculation for other blocks
        {
            cursorPos = pos.x() / group->CHAR_WIDTH;
        }
        // set cursor position
        if (cursorPos < 0 || !myTextItem->setTextCursorPos(cursorPos))
            myTextItem->setTextCursorPos(0);

        return this;    //! return block with cursor
    }

    // add cursor to some child of this block
    QPair<Block*, bool> targetRight = findClosestLeaf(pos);
    Block *target = targetRight.first;

    pos = mapToItem(target->textItem(), pos);   //! map to target's TextItem
    int cursorPos;

    // find cursor position:
    if (target->element->allowsParagraphs()) //! use hitTest for multiline blocks
    {
        cursorPos = target->textItem()->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
    }
    else //! use calculation for other blocks
    {
        cursorPos = pos.x() / group->CHAR_WIDTH;
    }
    // set cursor position
    if (cursorPos < 0 || !target->textItem()->setTextCursorPos(cursorPos))
    {
        if (targetRight.second)
            target->textItem()->setTextCursorPos(0);
        else
            target->textItem()->setTextCursorPos(-1);
    }

    return target;    //! return block with cursor
}

QPair<Block*, bool> Block::findClosestLeaf(QPointF pos) const
{
    QPair<Block*, bool> targetRight = findClosestChild(pos);
    Block *target = targetRight.first;

    if (target == 0) //! target is after last child
    {
        targetRight.first = getLastLeaf();
        targetRight.second = false;

        return targetRight;
    }

    if (target->isTextBlock()) //! target is leaf
    {
        return targetRight;
    }

    // non-text target - continue recursively
    return target->findClosestLeaf(target->mapFromParent(pos));
    // returns closest leaf descendant and true if it's to right of pos, false if it's to left
    // never returns 0
}

QPair<Block*, bool> Block::findClosestChild(QPointF pos) const
{
    Block *sibling = 0;
    qreal minDistX = idealSize().width(), minDistY = idealSize().height();
    bool siblingIsRight = true;

    // test distance from child blocks' hotspots
    Block *child = firstChild;
    while (child != 0)
    {
        QRectF rect = child->idealGeometry;

        if (pos.y() >= rect.top() && pos.y() < rect.bottom())
        {
            // test if pos is inside block
            if (rect.left() <= pos.x() && pos.x() <= rect.right())
            {
                sibling = child;
                siblingIsRight = true;

                break;
            }
            // test distance from left side of block
            if (qAbs(pos.x() - rect.left()) < minDistX ||
                qAbs(pos.y() - rect.center().y()) < minDistY)
            {
                minDistX = qAbs(pos.x() - rect.left());
                minDistY = qAbs(pos.y() - rect.center().y());
                sibling = child;
                siblingIsRight = true;
            }
            // test distance from right side of block
            if (qAbs(pos.x() - rect.right()) < minDistX ||
                qAbs(pos.y() - rect.center().y()) < minDistY)
            {
                minDistX = qAbs(pos.x() - rect.right());
                minDistY = qAbs(pos.y() - rect.center().y());
                sibling = child;
                siblingIsRight = false;
            }
        }
        child = child->nextSib;
    }
    // returns closest child and true if it's to right of pos, false if it's to left
    // if sibling == 0 then siblingIsRight == true
    return QPair<Block*, bool>(sibling, siblingIsRight);
}

void Block::textFocusChanged(QFocusEvent* event)// TODO
{
    if (event->gotFocus()) //! focus in
    {
        if (element->isPaired())
        {
            QFont font(highlightFormat.first);
            font.setBold(true);
            QPair<QFont, QColor> pairHighlightFormat(font, QColor(Qt::red));
            TreeElement *pair = element->getPair();

            if (isTextBlock())
            {
                if (pair != 0)
                {
                    highlight(pairHighlightFormat);
                    pair->getBlock()->highlight(pairHighlightFormat);
                }
            }
        }
    }
    else //! focus out
    {
        if (element->isPaired())
        {
            TreeElement *pair = element->getPair();

            if (isTextBlock())
            {
                if (pair != 0)
                {
                    highlight(highlightFormat);
                    pair->getBlock()->highlight(pair->getBlock()->highlightFormat);
                }
            }
        }
    }
}

void Block::textChanged()
{
    QString text = myTextItem->toPlainText();
    bool toUpdate = false;
    myTextItem->document()->blockSignals(true);

    if (text.isEmpty()) //! delete block
    {
        if (!(element->isLineBreaking() && getPrev(true)->line != line))
        {
            if (myTextItem->hasFocus())
            {
                showing = false; level = 0;
            }
            else
            {
                // only delete if block isn't a single newline in this line
                // AND isn't focused
                // focused blocks will de deleted when they lose focus
                setVisible(false);
                removeBlock(true);
                group->mainBlock()->updateBlock();
                return;
            }
        }
        else if(isFolded()) //! if folded block is empty unfold it
        {
            setFolded(false);
            getFirstLeaf()->textItem()->setTextCursorPos(0);

            return;
        }
    }
    else if (text.at(0).isSpace() && !isFolded()) //! remove leading spaces and tabs
    {
        Block *ancestor = getAncestorWhereFirst();
        int count = 1;

        if (text.at(0) == '\t') count = group->TAB_LENGTH;
        do
        {
            text.remove(0, 1);
            ancestor->element->addSpaces(count);
        }
        while(!text.isEmpty() && text.at(0).isSpace());

        textItem()->setPlainText(text);
        toUpdate = true;
    }

    if (element->getType() != text || toUpdate)
    {
        group->clearSearchResults();

        if(!isFolded()) {
            // remove highlighting if text changed
            if (element->getType() != text && ! edited)
            {
                highlight(group->docScene->getDefaultFormat());
                edited = true;
            }

            element->setType(text);
            group->setModified(true);
        }
        else
        {
            foldButton->foldText = text;
        }
        updateGeometryAfter(group->smoothTextAnimation);
    }

    myTextItem->document()->blockSignals(false);
}

bool Block::isEdited() const // returns true if this or its ancestors are edited
{
    if (edited || element->isSelectable())
        return edited;

    if (parent == 0)
        return false;

    return parent->isEdited();
}

bool Block::isOverlapPossible() const
        // used to determine if animating might be needed
        // when selecting this block
{
    if (!hasMoreLines()) return false;

    if (element->isLineBreaking()) return false;

    if (nextSib == 0) return false;

    return true;
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemVisibleHasChanged)
    {
        emit visibilityChanged(value.toBool());     //! used to notify docblock
    }

    return QGraphicsItem::itemChange(change, value);
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (element->isSelectable())
    {
        Block *selected;

        if (isEdited())
        {
            selected = group->reanalyze(this, event->scenePos());
            selected->startDragPos = event->pos();
            selected->group->docScene->selectGroup(selected->group);
        }
        else
        {
            group->selectBlock(this, true);
            addTextCursorAt(event->pos());
            startDragPos = event->pos();
            group->docScene->selectGroup(group);
        }
    }
    else
    {
        event->ignore();
        return;
    }
}

void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // start drag only if minimal distance is exceeded
    if (element->isSelectable())
    {
        if ((startDragPos - event->pos()).manhattanLength() >= QApplication::startDragDistance())
            startDrag(event->widget());
    }
}

void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void Block::startDrag(QWidget *widget)
{
    if (!element->isFloating())
    {
        QStatusBar *sb = group->getStatusBar();
        sb->showMessage(tr("Hold Shift to insert whithin line; Hold Ctrl to copy"));
        QDrag *drag = new QDrag(widget);
        QMimeData *mimeData = new QMimeData();
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << element->getType();
        mimeData->setData(BlockGroup::BLOCK_MIME, itemData);
        drag->setMimeData(mimeData);
        int result = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
        QString msg;

        if (result == Qt::MoveAction)
            msg = "Block moved";
        else if (result == Qt::CopyAction)
            msg = "Block copied";
        else
            msg = "Drag canceled";

        sb->showMessage(tr(qPrintable(msg)), 2000);
    }
}

void Block::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

void Block::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

void Block::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

void Block::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->ignore();
}

int HOVER_TIMER = 200;

void Block::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!element->isSelectable())
    {
        event->ignore();
        return;

    }
    else
    {
        timer->start(HOVER_TIMER);
        Block *block = getFirstSelectableAncestor();

        if (block != this)
        {
            block->timer->stop();
        }
    }
}

void Block::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!element->isSelectable())
    {
        event->ignore();
        return;
    }
    else
    {
        timer->stop();
        pointed = false;
        repaintNeeded = true;
        update();
        Block *block = getFirstSelectableAncestor();

        if (block != this)
        {
            block->timer->start(HOVER_TIMER/5);
        }
    }
}

void Block::acceptHover()
{
    pointed = true;
    repaintNeeded = true;

    Block *block = parent;

    while (block != 0)
    {
        if (block->pointed)
        {
            block->pointed = false;
            block->repaintNeeded = true;
            block->update();

            return;
        }
        block = block->parent;
    }
    update();
}

void Block::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    //
}

void Block::updateBlock(bool doAnimation)
        // used to update everything from root down
        // updates line numbers, geometry and foldbutton
{
    // update line
    updateLine();

    if (parent == 0 || line > group->getLastLine())
        group->setBlockIn(this, line);
    // update pos
    updatePos(!doAnimation);
    // update children
    Block *child = firstChild;

    while(child != 0)
    {
        child->updateBlock(doAnimation);
        child = child->nextSib;
    }
    // update size
    updateSize(!doAnimation);
    // update fold button
    updateFoldButton();
    // animate
    if (doAnimation)
        animate();
    // root updates group size
    if (parent == 0)
    {
        group->clearSearchResults();
        group->updateSize();
        qDebug("   Blocks updated");
    }
}

void Block::updateGeometryAfter(bool doAnimation)
{
    // used to update everything after this block
    // updates only geometry
    Block *child = firstChild;
    // update children's positions

    while (child != 0)
    {
        child->updatePos(!doAnimation);
        child->updateFoldButton();

        if (doAnimation) child->animate();  // block animates its children

        child = child->nextSib;
    }
    // update my size
    updateSize(!doAnimation);
    // let parent update siblings' and my position together with its size
    if (parent != 0)
    {
        parent->updateGeometryAfter(doAnimation);
    }
    else
    {
        if (doAnimation)     // root must animate itself
            animate();

        group->updateSize(); // root updates group size
        qDebug("   Geometry updated");
    }
}

void Block::animate()
{
    if (geometry() == idealGeometry) return;

    animation->setStartValue(geometry());
    animation->setEndValue(idealGeometry);
    animation->start();
}

void Block::updateGeometry(bool updateReal)
{
    updatePos(updateReal);
    updateSize(updateReal);
}


void Block::updatePos(bool updateReal)
{
    //    if (isTextBlock()) {
    //        myTextItem->setPos(getOffset(InnnerTopLeft));
    //    }
    if (element->isFloating())
    {
        idealGeometry.moveTo(pos());
        return;
    }
    QPointF pos = QPointF();

    if (prevSib != 0)
    {
        if (!prevSib->element->isLineBreaking() || parent == 0)
        {
            //            QPointF offs;
            if (prevSib->hasMoreLines())
            {
                Block *lastLeaf = prevSib->getLastLeaf();
                pos = lastLeaf->mapIdealToAncestor(prevSib->parent, lastLeaf->idealRect().topRight());

                if (prevSib->showing || lastLeaf->moreSpace)
                {
                    pos.rx() = prevSib->idealPos().x() + prevSib->idealSize().width();
                    //                    offs = prevSib->getOffset(Outer);
                } //else {
                //                    offs = lastLeaf->getOffset(Outer);
                //                }
            }
            else
            {
                //                offs = prevSib->getOffset(Outer);
                pos = prevSib->idealPos();
                pos.rx() += prevSib->idealSize().width();
            }
            //            pos.rx() += getOffset(Outer).x() + offs.x();
        }
        else
        {
            qreal maxY = 0;
            //            qreal offsY = 0;
            foreach (Block *child, parent->childBlocks())
            {
                int y = child->idealPos().y() + child->idealSize().height();

                if (y > maxY)
                {
                    maxY = y;
                    //                    offsY = child->getOffset(Outer).y();
                }
                if (child == prevSib) break;
            }
            //            pos.rx() = parent->getOffset(InnnerTopLeft).x();
            pos.ry() += maxY ;//+ getOffset(Outer).y() + offsY;
        }
    }
    else
    {
        if (parent != 0)
            ;//pos = parent->getOffset(InnnerTopLeft);
        else
            pos = this->pos();
    }

    pos.rx() += element->getSpaces() * group->CHAR_WIDTH;

    if (pos != idealPos()) repaintNeeded = true;

    idealGeometry.moveTo(pos);

    if (updateReal) setPos(pos);
}

void Block::updateSize(bool updateReal)
{
    QSizeF size;

    if (isTextBlock())
    {
        size = myTextItem->boundingRect().size();
        size.rwidth() -= 1;
    }
    else
    {
        QRectF rect = QRectF(0, 0, 0, 0);
        Block *child = firstChild;

        while(child != 0)
        {
            QRectF childRect = child->idealGeometry;

            // NOTE: not very elegant trick, think about it
            // used to add some extra space at the end of the line for block inserting
            // (moreSpace flag will be used in getOffset() and OFFSET_INSERT will be added)
            //            if (child->isTextBlock() &&
            //                child->getAncestorWhereLast()->element->isLineBreaking()) {
            //                childRect.setBottomRight(childRect.bottomRight() +
            //                                         child->getOffset(Outer)*2);
            //            }
            rect = rect.united(childRect);
            child = child->nextSib;
        }
        size = rect.size();
    }
    //    QPointF offs = getOffset(InnnerTopLeft) + getOffset(InnerBottomRight);
    //
    //    size.rwidth() += offs.x();
    //    size.rheight() += offs.y();
    if (size != idealSize()) repaintNeeded = true;

    idealGeometry.setSize(size);

    if (updateReal)
        setRect(QRectF(QPointF(), size));
}

void Block::updateLine()
{
    if (prevSib == 0) {

        if (parent == 0) line = 0;
        else line = parent->line;
    }
    else
    {
        line = prevSib->line + prevSib->numberOfLines() - 1;
        if (prevSib->element->isLineBreaking()) line++;
    }
}

//QPointF Block::getOffset(OffsetType type) const
//{
//    switch (type) {
//    case InnnerTopLeft :
//        if (showing)
//            return group->OFFSET_IN_TL;
//        else
//            return group->NO_OFFSET;
//    case InnerBottomRight :
//        if (showing)
//            return group->OFFSET_IN_BR;
//        else
//            return group->NO_OFFSET;
//    case Outer :
//        if (showing)
//            return group->OFFSET_OUT;
//        else if (moreSpace)
//            return group->OFFSET_INSERT;
//        else
//            return group->NO_OFFSET;
//    default:
//        return group->NO_OFFSET;
//    }
//}

void Block::highlight(QPair<QFont, QColor> format)
{
    if (isTextBlock())
    {
        myTextItem->setFont(format.first);
        myTextItem->setDefaultTextColor(format.second);
        // NOTE: according to Qt doc this "Sets the color for unformatted text"
        // is this the reason why cursor is colored as well? how to set color for formated text?
    }
    else
    {
        Block *child = firstChild;

        while (child != 0)
        {
            child->highlight(format);
            child = child->nextSib;
        }
    }
}

QRectF Block::boundingRect() const
{
    return rect();
}

QPainterPath Block::shape() const
{
    QPainterPath path;

    int width = pen().width();
    int mod = width % 2;
    int half = width / 2;
    path.addRect(boundingRect().adjusted(-half, -half, half+mod, half+mod));

    return path;
}

int MAX_LEVEL = 8;
int GRAD_STEP = 15;

void Block::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    repaintNeeded  = false;

    QRectF rect = boundingRect();
    QPainterPath path;
    //    path.addRect(rect);
    path.addRoundedRect(rect, 6, 6, Qt::AbsoluteSize);

    if (element->isFloating())
        painter->fillPath(path, Qt::white);

    if (showing)  //! background
    {
        painter->fillPath(path, group->docScene->getFormatFor("background_style").second);//Qt::white

        if (level > 0)
        {
            QColor color = getHoverColor();
            color.setAlpha(level * GRAD_STEP);
            painter->fillPath(path, color);
        }
    }

    if (isSearchResult) //! search background
    {
        QColor color = group->docScene->getFormatFor("search_result_style").second;
        color.setAlpha(150);
        painter->fillRect(0, 4, rect.width(), rect.height() - 2*4, color);
    }

    if (pointed) //! hover background
    {
        QColor color = group->docScene->getFormatFor("background_hover_style").second;//Qt::red;
        color.setAlpha(80);
        painter->fillPath(path, color);
    }

    if (showing) //! frame
    {
        QColor color = getHoverColor();
        color.setAlpha((MAX_LEVEL+1) * GRAD_STEP);
        painter->setPen(color);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawPath(path);
    }
}

QColor Block::getHoverColor() const
{
    if (element->isUnknown())
        return group->docScene->getFormatFor("hover_unknown_style").second;//Qt::green;
    else
        return group->docScene->getFormatFor("hover_style").second;//Qt::blue;
}

void Block::setShowing(bool newState, Block *until)
{
    if (!newState) //! switch off (recursive)
    {
        level = 0;           //! reset level

        if (this != until)
        {
            showing = false;    //! reset flag if this block won't be reselected again
            if (isOverlapPossible()) updateGeometryAfter();
        }
        else
        {
            until = parent;
        }

        repaintNeeded = true;

        if (parent != 0 && (!parent->element->isSelectable() || parent->showing)) {
            parent->setShowing(false, until);
        }
    }
    else //! switch on (iterative)
    {
        Block *block = this;
        QList<Block *> ancestors;

        while (block != 0) //! collect all selectable ancestors
        {
            if (block->element->isSelectable())
                ancestors << block;

            block = block->parent;
        }

        int level = qMin(ancestors.size()-1, MAX_LEVEL);

        foreach (Block *bl, ancestors)
        {
            bl->level = qMax(0, level--);   //! set level if whithin range
            if (bl->showing) continue;
            bl->showing = true;             //! set showing

            if (bl->isOverlapPossible())
            {
                bl->updateGeometryAfter();
            }

            bl->repaintNeeded = true;
        }
    }
}

QRectF Block::geometry() const
{
    QRectF geometry = rect();
    geometry.translate(pos());
    return geometry;
}

void Block::setGeometry(QRectF geometry)
{
    QPointF pos = geometry.topLeft();
    geometry.translate(-pos);
    setPos(pos);
    setRect(geometry);
}

QList<Block*> Block::childBlocks() const
{
    return BlockGroup::blocklist_cast(childItems());
}

QPointF Block::mapIdealToAncestor(Block* ancestor, QPointF pos) const
{
    if (ancestor == this) return pos;

    if (!ancestor->isAncestorOf(this))
    {
        qWarning("Not ancestor");
        return QPointF();
    }

    const Block *block = this;

    while (block != ancestor)
    {
        pos += block->idealPos();
        block = block->parent;
    }

    return pos;
}

void Block::setFolded(bool fold)
{
    if (fold == folded) return; //! do nothing
    folded = fold;

    Block *child;

    if (fold)
    {
        QString text;

        if (foldButton->foldText.isEmpty())
        {
            Block *block = firstChild->getFirstLeaf();
            text.append(block->element->getType());
            block = block->getNext(true);

            while (block->line == line)
            {
                QString spacesStr = QString().fill(' ',
                                                   block->getAncestorWhereFirst()->element->getSpaces());
                text.append(spacesStr);
                text.append(block->element->getType());
                block = block->getNext(true);
            }

            text.append(" ...");
            foldButton->foldText = text;
        }
        else
        {
            text = foldButton->foldText;
        }

        foldButton->foldText.clear();
        myTextItem = new TextItem(text, this, true);
        highlight(group->docScene->getDefaultFormat());
        child = firstChild;
        firstChild = 0;
    }
    else
    {
        if (myTextItem != 0)
        {
            myTextItem->deleteLater();
        }

        myTextItem = 0;
        firstChild = childBlocks().first();
        child = firstChild;
    }

    while(child != 0)
    {
        child->setVisible(!fold);
        child = child->nextSib;
    }

    if (fold && group->selectedBlock() != 0 //! selected block is descendant of this -> select this
        && (this->isAncestorOf(group->selectedBlock())
            || group->selectedBlock() == this))
    {
        group->selectBlock(this);
        myTextItem->setTextCursorPos(0);
    }
    group->mainBlock()->updateBlock(false);
}

bool Block::isFoldable() const
{
    return folded ||
            (element->isSelectable() && hasMoreLines() && element->getType() != "block");
}

void Block::updateFoldButton()
{
    if (isFoldable())
    {
        if (foldButton == 0)
        {
            foldButton = new FoldButton(this);
        }
        if (!isVisible())
        {
            group->removeFoldable(this);
        }
        else
        {
            bool able = group->addFoldable(this);
            foldButton->setVisible(able);
            foldButton->updatePos();
        }
    }
    else
    {
        if (foldButton != 0)
        {
            delete foldButton;
            group->removeFoldable(this);
        }

        foldButton = 0;
    }
}
