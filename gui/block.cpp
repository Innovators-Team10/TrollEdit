#include "block.h"
#include "fold_button.h"
#include "block_group.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"

Block::Block(TreeElement *element, Block *parentBlock, BlockGroup *blockGroup)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0) { // adding directly to group, no parent block
        Q_ASSERT(blockGroup != 0);
        setParentItem(blockGroup);
        group = blockGroup;
        parent = 0;
        prevSib = 0;
    } else {
        parent = parentBlock;
        if (parent->isTextBlock()) {
            delete parent->myTextItem;
            parent->myTextItem = 0;
        }
        group = parent->group;

        // set links
        QList<Block*> siblings = parent->childBlocks();
        if (siblings.size() > 1) {      // this block is in siblings already!
            prevSib = siblings.at(siblings.size() - 2);
            prevSib->nextSib = this;
        } else {
            parent->firstChild = this;
            prevSib = 0;
        }
        if (element->getParent() == 0) {
            parentBlock->element->appendChild(element);
        }
    }
    firstChild = 0;
    nextSib = 0;
    // set line
    updateLine();

    // set element
    while (!element->isImportant())     // skip unimportant
        element = (*element)[0];
    this->element = element;
    element->setBlock(this);

    // process rest of the AST
    if (element->isLeaf()){ // leaf - create text area
        myTextItem = new TextItem(element->getType(), this, element->allowsParagraphs());
    } else {                // non-leaf - create rest of the tree
        myTextItem = 0;
        setToolTip(element->getType().replace("_", " "));
        foreach (TreeElement *childEl, element->getChildren()) {
            new Block(childEl, this);
        }        
    }

    // set highlighting
    assignHighlighting(element);

    // set flags
    //    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemClipsToShape);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    setAcceptDrops(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    folded = false;
    edited = false;
    showing = false;
    moreSpace = false;
    moveStarted = false;
    hovered = false;
    pointed = false;

    createControls();

    // set geometry
    updateGeometry(true);
    updatePen();
}

Block::~Block()
{
    if (foldButton != 0) {
        delete foldButton;
    }
    delete element;
}

void Block::assignHighlighting(TreeElement *el)
        // todo - remove hardcoded vales such as "declarator"
{
    if (el->isLeaf()) {
        QPair<QFont, QColor> highlightFormat;
        bool f = false;
        if(el->getParent()) {
            QString parentType = el->getParent()->getType();
            if (group->docScene->getHighlightning().contains(parentType) && !el->getParent()->getType().startsWith("funct_")) {
                highlightFormat = group->docScene->getHighlightning().value(parentType);
                f = true;
            }
        }
        if (!f)
            highlightFormat = group->docScene->getHighlightning().value("text_style");
        highlight(highlightFormat);
    } else {
        if (group->docScene->getHighlightning().contains(el->getType())) {
            QPair<QFont, QColor> highlightFormat = group->docScene->getHighlightning().value(el->getType());
            if (!el->getType().compare("funct_call")) {
                getFirstLeaf()->highlight(highlightFormat);
            } else if (!el->getType().compare("funct_definition")) {
                QList<Block*> blocks = childBlocks();
                foreach(Block* block, blocks) {
                    if (!block->element->getType().compare("declarator")) {
                        block->getFirstLeaf()->highlight(highlightFormat);
                        break;
                    }
                }
            }
        }
    }
    if (group->docScene->getBlockFormatting().contains(element->getType()))
        format = group->docScene->getBlockFormatting().value(element->getType());
    else
        format = group->docScene->getBlockFormatting().value("block_style");
}

void Block::createControls()
{
    animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(200);

    foldButton = 0;
    updateFoldButton();
}

void Block::setParentBlock(QGraphicsItem *parentItem)
        // moves this element and all unimportant elements on way to parentBlock's element ("branch")
        // to new parent
        // NOTE: to remove block from its parent use removeBlock() which removes all empty ancestors too
{
    if (parentItem == this) {
        parentItem = 0;//debug - toto nemoze nastavat
        qWarning("parentItem == this in setParentBlock()");
    }
    TreeElement *branch;
    // remove from old parent element
    Block *oldParent = this->parent;
    if (oldParent != 0) {
        TreeElement *oldParentEl = oldParent->element;
        // find ancestor that is child of oldParentEl (i.e. root of the branch)
        int index = oldParentEl->indexOfBranch(this->element);
        branch = (*oldParentEl)[index];
        // remove branch from original parent
        oldParentEl->removeChild(branch);
        // remove spaces
        element->setSpaces(0);
        // remove links
        removeLinks();
    } else {
        branch = element->getRoot(); // get root of unvisualized(?) branch
    }
    // append to new parent element
    Block *newParent = qgraphicsitem_cast<Block*>(parentItem);
    if (newParent != 0) {
        TreeElement *newParentEl = newParent->element;
        newParentEl->appendChild(branch);
        branch = 0;
        // update links if not floating
        if (!element->isFloating()) {
            QList<Block*> siblings = newParent->childBlocks();
            if (siblings.size() > 0) {      // this block is not in siblings yet!
                prevSib = siblings.at(siblings.size() - 1);
                prevSib->nextSib = this;
                if (prevSib->element->isLineBreaking()) {
                    prevSib->element->setLineBreaking(false);
                    element->setLineBreaking(true);
                }
            } else {
                newParent->firstChild = this;
                prevSib = 0;
            }
            nextSib = 0;
        }
        if (newParent->isTextBlock()) {
            delete newParent->myTextItem;
            newParent->myTextItem = 0;
        }
    }

    // remove from old parent block & append to new parent block
    QGraphicsRectItem::setParentItem(parentItem);
    this->parent = newParent;
}

void Block::stackBeforeBlock(const QGraphicsItem *sibling)
        // moves this element and all unimportant elements on way to parentBlock's element ("branch")
        // within current parent of this branch
{
    if (sibling == 0) return;
    Block *nextSibling = qgraphicsitem_cast<Block*>(const_cast<QGraphicsItem*>(sibling));
    if (nextSibling != 0 && nextSibling != nextSib) {
        if (parent != 0 && nextSibling->parent == parent) {
            TreeElement *parentEl = parent->element;
            // find ancestor that is child of parentEl (i.e root of the branch)
            int index = parentEl->indexOfBranch(this->element);
            TreeElement *branch = (*parentEl)[index];
            // remove branch from original position
            parentEl->removeChild(branch);
            // compute new index
            index = parentEl->indexOfBranch(nextSibling->element);
            // insert branch at new position
            parentEl->insertChild(index, branch);
            // update links
            removeLinks();
            if (!element->isFloating()) {
                prevSib = nextSibling->prevSib;
                if (prevSib != 0) {
                    prevSib->nextSib = this;
                    if (prevSib->element->isLineBreaking()) {
                        prevSib->element->setLineBreaking(false);
                        element->setLineBreaking(true);
                    }
                } else {
                    parent->firstChild = this;
                }
                nextSib = nextSibling;
                nextSibling->prevSib = this;
            }
        }
    }
    QGraphicsRectItem::stackBefore(sibling);
}

void Block::removeLinks()
{
    if (nextSib != 0) {
        nextSib->prevSib = prevSib;
        if (prevSib == 0 && nextSib->element->getSpaces() > 0)
            nextSib->element->setSpaces(0);
    }
    if (prevSib != 0) {
        prevSib->nextSib = nextSib;
        if (nextSib == 0 && prevSib->element->isLineBreaking()) {
            prevSib->element->setLineBreaking(false);
            if (parent != 0)
                parent->element->setLineBreaking(true);
        }
    }
    if (parent != 0 && parent->firstChild == this) parent->firstChild = nextSib;
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
    while (block->prevSib == 0 && block->parent != 0)
        block = block->parent;
    return block;
}

Block *Block::getAncestorWhereLast() const
{
    Block *block = const_cast<Block*>(this);
    while (block->nextSib == 0 && block->parent != 0)
        block = block->parent;
    return block;
}

Block *Block::getNext(bool textOnly) const
{
    Block *next = const_cast<Block*>(this);
    if (parent != 0) {
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
    if (parent != 0) {
        if (prevSib == 0)
            return parent->getPrev(textOnly);
        prev = prevSib;
    }
    if (textOnly) {
        return prev->getLastLeaf();
    }
    return prev;
}

Block *Block::getFirstSelectableAncestor() const
{
    Block *block = const_cast<Block*>(this);
    if (parent != 0)
        block = block->parent;

    while (!block->element->isSelectable() && block->parent != 0)
        block = block->parent;
    return block;
}

int Block::numberOfLines() const
{
    if (isTextBlock()) {
        return 1;
    } else {
        Block *last = childBlocks().last();
        return (last->line + last->numberOfLines() - 1) - this->line + 1;
    }
}

bool Block::hasMoreLines() const
{
    if (isTextBlock()) {
        return false;
    } else {
        Block *last = childBlocks().last();
        return (last->line > this->line) || last->hasMoreLines();
    }
}

void Block::addBlockAt(Block *block, QPointF pos)
{
    Block *sibling = 0;
    QLineF distance = QLineF(QPointF(), pos);
    qreal maxYdist = group->CHAR_HEIGHT * 0.6;
    qreal minDist = 1000;
    bool siblingIsRight = true;

    // test distance from block hotspots
    Block *child = firstChild;
    while (child != 0) {
        distance.setP1(child->idealPos() + QPointF(0, child->idealSize().height()/2));
        // first hotspot is in the middle of left side of the bounding rectangle
        if (qAbs(distance.dy()) < maxYdist && qAbs(distance.dx()) < minDist) {
            minDist = qAbs(distance.dx());
            sibling = child;
            siblingIsRight = true;
        }
        distance.setP1(child->idealPos() +
                   QPointF(child->idealSize().width(), child->idealSize().height()/2));
        // second hotspot is in the middle of right side of the bounding rectangle
        if (qAbs(distance.dy()) < maxYdist && qAbs(distance.dx()) < minDist) {
            minDist = qAbs(distance.dx());
            sibling = child;
            siblingIsRight = false;
        }
        child = child->nextSib;
    }
    if (sibling == 0 || sibling->isTextBlock()) {
        if (!siblingIsRight)
            sibling = sibling->nextSib;
        block->setParentBlock(this);
        block->stackBeforeBlock(sibling);
    } else {
        sibling->addBlockAt(block, sibling->mapFromParent(pos));
    }
}

void Block::textFocusChanged(QFocusEvent* event)
{
    if (event->gotFocus()) {    // focus in
        //        if (element->isPaired()) {
        //            TreeElement *pair = element->getPair();
        //            if (pair != 0 && pair->getBlock()->isTextBlock() && isTextBlock()) {//temp
        //                pair->getBlock()->textItem()->setDefaultTextColor(Qt::blue);
        //                myTextItem->setDefaultTextColor(Qt::blue);
        //            } else if (isTextBlock()) {
        //                myTextItem->setDefaultTextColor(Qt::red);
        //            }
        //       }
    } else {                    // focus out
        //        if (element->isPaired()) {
        //            TreeElement *pair = element->getPair();
        //            if (pair != 0 && pair->getBlock()->isTextBlock() && isTextBlock()) {// temp
        //                pair->getBlock()->textItem()->setDefaultTextColor(Qt::black);
        //                myTextItem->setDefaultTextColor(Qt::black);
        //            } else if (isTextBlock()) {
        //                myTextItem->setDefaultTextColor(Qt::black);
        //            }
        //
    //        }
    }
}

void Block::textChanged()
{
    QString text = myTextItem->toPlainText();
    myTextItem->document()->blockSignals(true);
    if (text.isEmpty()) {   // delete block
        if (!(element->isLineBreaking() && getPrev(true)->line != line) || false) { // todo
            // don't delete if block is single newline in this line OR floating

            Block *next = group->removeBlock(this, true);

            if (next != 0) {
                if (next->line > line) { // jumped to next line
                    Block *prev = next->getPrev(true);
                    if (prev->line > line) {  // jumped to the end of file
                        next->getFirstLeaf()->textItem()->setTextCursorPosition(0);
                    } else {                  // same line
                        prev->textItem()->setTextCursorPosition(-1);
                    }
                } else if (next->line < line) { // jumped to the beginning of file
                    Block *prev = next->getPrev(true);
                    prev->textItem()->setTextCursorPosition(-1);
                } else {                         // on same line
                    next->element->addSpaces(element->getSpaces());
                    next->getFirstLeaf()->textItem()->setTextCursorPosition(0);
                }
            }
            return;
        }
    }
    if (text.at(0).isSpace()) {         // remove leading spaces and tabs
        Block *ancestor = getAncestorWhereFirst();
        int count = 1;
        if (text.at(0) == '\t') count = group->TAB_LENGTH;
        do {
            text.remove(0, 1);
            ancestor->element->addSpaces(count);
        } while(text.at(0).isSpace());
        element->setType(text);
        myTextItem->setPlainText(text);
        group->updateAll(false);//ancestor->updateAfter(true);
    } else {
        if (element->getType() != text) {
            edited = true;
            element->setType(text);
            group->updateAll(false);//updateXPosInLine(line);
        }
    }   
    myTextItem->document()->blockSignals(false);
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemVisibleHasChanged) {
        updateFoldButton();
    }
    return QGraphicsItem::itemChange(change, value);
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (isTextBlock()) {
        QPointF clickPos = event->scenePos();
        clickPos = myTextItem->mapFromScene(clickPos);
        int pos = myTextItem->document()->documentLayout()->hitTest(clickPos, Qt::FuzzyHit);
        myTextItem->setTextCursorPosition(qMax(0, pos));
        group->selectBlock(this);
    }
    if (element->isSelectable()) {
        event->accept();
        if (group->selectedBlock() != this) {
            getFirstLeaf()->myTextItem->setTextCursorPosition(0);
            group->selectBlock(this);
        }
        if (!element->isFloating()) {
            QDrag *drag = new QDrag(event->widget());
            QMimeData *mimeData = new QMimeData();
            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << element->getType();
            mimeData->setData("block_data", itemData);
            drag->setMimeData(mimeData);
            drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
        }
    } else {
        event->ignore();
        return;
    }
}

void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseMoveEvent(event);
}

void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void Block::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("block_data")) {
        Block *selected = group->selectedBlock();
        if (selected != 0) {
            if (selected != this && !this->element->getAncestors().contains(selected->element)) {
                group->deselect();
                group->removeBlock(selected, false);
                selected->setPos(event->pos());
                selected->updatePos();

                addBlockAt(selected, event->pos());
                offsetChildren(false);
                group->selectBlock(this);
            }
        }
        event->accept();
    } else {
        event->ignore();
    }
}

void Block::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (group->selectedBlock() == this) return;
    if (isTextBlock() || firstChild->nextSib == 0 || !element->isSelectable()) {
        event->ignore();
        return;
    }
    if (event->mimeData()->hasFormat("block_data")) {
        event->acceptProposedAction();
        offsetChildren(true, event->pos());
        group->updateAll(); // better updater!! complete update not necessary at every move
    } else {
        event->ignore();
    }
}

void Block::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (group->selectedBlock() == this) return;
    if (isTextBlock() || firstChild->nextSib == 0 || !element->isSelectable()) {
        event->ignore();
        return;
    }
    if (event->mimeData()->hasFormat("block_data")) {
        event->acceptProposedAction();
        offsetChildren(true, event->pos());
        group->updateAll();
    } else {
        event->ignore();
    }
}

void Block::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (group->selectedBlock() == this) return;
    if (isTextBlock() || firstChild->nextSib == 0 || !element->isSelectable()) {
        event->ignore();
        return;
    }
    offsetChildren(false);
    group->updateAll();
}

void Block::offsetChildren(bool flag, QPointF pos)
{
    Block* child = firstChild;
    while (child != 0) {
        if (flag && qAbs(child->idealPos().y() - pos.y()) < group->CHAR_HEIGHT)
            child->moreSpace = true;
        else
            child->moreSpace = false;
        if (!child->element->isSelectable())
            child->offsetChildren(flag, pos);
        child = child->nextSib;
    }
}

void Block::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!element->isSelectable()){
        event->ignore();
        return;
    } else {
        hovered = true;
        pointed = true;
        Block *block = getFirstSelectableAncestor();
        if (block != this)
            block->pointed = false;
//        group->update();
        updatePen();
    }
}

void Block::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!element->isSelectable()){
        event->ignore();
        return;
    } else {
        hovered = false;
        pointed = false;
        Block *block = getFirstSelectableAncestor();
        if (block != this && block->hovered)
            block->pointed = true;
//        group->update();
        updatePen();
    }
}

void Block::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!element->isSelectable()){
        event->ignore();
        return;
    }
    QMenu menu; // todo
}

void Block::updateBlock(bool doAnimation) // parent to child updater
        // used to update everything from root up
        // updates line numbers
        // used when new root is created
{
    // update line
    updateLine();
    if (parent == 0 || line > group->getLastLine())
        group->setBlockIn(this, line);
    // update pos
    updatePos(!doAnimation);
    // update children
    Block *child = firstChild;
    while(child != 0) {
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
}

void Block::animate()
{
    animation->setStartValue(geometry());
    animation->setEndValue(idealGeometry);
    animation->start();
}

/*void Block::updateAfter(bool updateThis) // child to parent updater
        // used to update everything after this block, after its parent etc.,
        // updates line numbers
        // used after blocks moving or typing newlines
{
    if (parent == 0) {
        setRect(computeRect());
        return;
    }
    int lineNo;
    QPointF nextPos;
    Block *sibling;
    bool newLineComming = false;

    if (updateThis) {   // start with this block
        sibling = prevSib;
    } else {            // start with next block
        if (nextSib == 0) {// no more siblings, update after parent
            parent->updateAfter();
            return;
        }
        sibling = this;
    }
    // initialize lineNo and nextPos
    if (sibling != 0) {
        lineNo = sibling->computeNextSiblingLine();
        nextPos = sibling->computeNextSiblingPos();
        if (sibling->element->isLineBreaking())
            newLineComming = true;
        sibling = sibling->nextSib;
    } else {
        lineNo = parent->line;
        nextPos = QPointF();//parent->getOffset();
        if (getPrev(true)->line != lineNo)
            newLineComming = true;
        sibling = this;
    }
    // update siblings
    while (sibling != 0) {
        nextPos.rx() +=  sibling->getSpaces() * SPACE_WIDTH;
        nextPos += sibling->getOffset();

        sibling->setLine(lineNo);
        sibling->setPos(nextPos);
        if (newLineComming) {
            lineStarts[lineNo] = sibling;
            newLineComming = false;
        }
        lastLine = lineNo;
        if (sibling->hasMoreLines())
            sibling->updateLineStarts();

        lineNo = sibling->computeNextSiblingLine();
        nextPos = sibling->computeNextSiblingPos();
        if (sibling->element->isLineBreaking())
            newLineComming = true;
        sibling->setRect(sibling->computeRect());
        sibling = sibling->nextSib;
    }
    // this block and its siblings are updated, repeat with parent
    updateThis = parent->firstChild == this && updateThis;

    setRect(computeRect());
    parent->updateAfter(updateThis);
}

void Block::updatePosAfter() // child to parent updater
        // used to update everything after this block, after ins parent etc.,
        // updates positions only
        // used
{
    if (parent == 0) {
        setRect(computeRect());
        return;
    }

    QPointF nextPos;
    Block *sibling = this;

    // initialize nextPos
    if (prevSib != 0) {
        nextPos = prevSib->computeNextSiblingPos();
    } else {
        nextPos = QPointF();
    }

    QParallelAnimationGroup *group = new QParallelAnimationGroup();
    // update siblings, start with this
    while (sibling != 0) {
         QPropertyAnimation *animation = new QPropertyAnimation(sibling, "pos");

        nextPos.rx() +=  sibling->getSpaces() * SPACE_WIDTH;
        nextPos += sibling->getOffset();

        QPointF abcd = sibling->pos();
        animation->setStartValue(abcd);
        animation->setEndValue(nextPos);


        sibling->setPos(nextPos);
        nextPos = sibling->computeNextSiblingPos();

        sibling->setRect(sibling->computeRect());
//        sibling->setPos(abcd);
        group->addAnimation(animation);

        sibling = sibling->nextSib;
    }
    // this block and its siblings are updated, repeat with parent
    parent->updatePosAfter();
    group->start();
}

void Block::updateLineStarts()
{
    Block *child = firstChild;
    bool newLineComming  = false;
    while (child != 0) {
        if (newLineComming) {
            lineStarts[child->line] = child;
            lastLine = child->line;
            newLineComming = false;
        }
        if (child->hasMoreLines())
            child->updateLineStarts();
        if (child->element->isLineBreaking())
            newLineComming = true;
        child = child->nextSib;
    }
}

void Block::updateXPosInLine(int lineNo) // child to parent updater
        // used to update everything from changed child down, updates only this line
        // doesn't update this block's position!
        // used after user's typing (without newlines)
{    
   if (parent == 0) return;
    setRect(computeRect());

    qreal nextX = computeNextSiblingPos().x();
    Block *sibling = nextSib;
    while (sibling != 0) { // start with next block
        nextX += sibling->getSpaces() * SPACE_WIDTH;
        nextX += sibling->getOffset().x();

        sibling->setX(nextX);
        nextX = sibling->computeNextSiblingPos().x();

        sibling->setRect(sibling->computeRect());
        sibling = sibling->nextSib;
    }
    // this block and its siblings are updated, repeat with parent
    //    if (parent->line == line)
    parent->updateXPosInLine(lineNo);
}*/

void Block::updateGeometry(bool updateReal)
{
    updatePos(updateReal);
    updateSize(updateReal);
}

void Block::updatePos(bool updateReal)
{
    if (element->isFloating()) {
        idealGeometry.moveTo(pos());
        if (foldButton != 0) foldButton->updatePos();
        return;
    }

    QPointF pos = QPointF();
    if (prevSib != 0) {
        if (!prevSib->element->isLineBreaking() || parent == 0) {
            QPointF offs;
            if (prevSib->hasMoreLines()) {
                Block *block = prevSib->getLastLeaf();
                pos = block->mapIdealToAncestor(prevSib->parent, block->idealRect().topRight());
                if (prevSib->showing) {
                    pos.rx() = prevSib->idealPos().x() + prevSib->idealSize().width();
                    offs = prevSib->getOffset(OffsetOut);
                } else {
                    offs = block->getOffset(OffsetOut);
                }
            } else {
                offs = prevSib->getOffset(OffsetOut);
                pos = prevSib->idealPos();
                pos.rx() += prevSib->idealSize().width();
            }
            pos.rx() += getOffset(OffsetOut).x() + offs.x();
        } else {
            qreal maxY = 0;
            qreal offsY = 0;
            foreach (Block *child, parent->childBlocks()) {
                int y = child->idealPos().y() + child->idealSize().height();
                if (y > maxY) {
                    maxY = y;
                    offsY = child->getOffset(OffsetOut).y();
                }
                if (child == prevSib) break;
            }
            pos.rx() = parent->getOffset(OffsetIn).x();
            pos.ry() += maxY + getOffset(OffsetOut).y() + offsY;
        }
    } else {
        if (parent != 0)
            pos = parent->getOffset(OffsetIn);
        else
            pos = this->pos();
    }

    pos.rx() += element->getSpaces() * group->CHAR_WIDTH;
    idealGeometry.moveTo(pos);

    if (isTextBlock()) {
        myTextItem->setPos(getOffset(OffsetIn));
    }

    if (updateReal) {
        setPos(pos);
        if (foldButton != 0) foldButton->updatePos();
    }
}

void Block::updateSize(bool updateReal)
{
    QSizeF size;
    if (isTextBlock()) {
        size = myTextItem->boundingRect().size();
        size.rwidth() -= 1;
    } else {
        QRectF rect = QRectF(0, 0, 0, 0);
        Block *child = firstChild;
        while(child != 0) {
            QRectF childRect = child->idealGeometry;

            if (child->isTextBlock() && // not very elegant trick, think about it
                child->getAncestorWhereLast()->element->isLineBreaking())
                childRect.adjust(0, 0, child->getOffset(OffsetOut).x()*2, 0);

            rect = rect.united(childRect);
            child = child->nextSib;
        }
        size = rect.size();
    }
    QPointF offs = getOffset(OffsetIn);
    size.rwidth() += offs.x() * 2;
    size.rheight() += offs.y() * 2;

    idealGeometry.setSize(size);
    if (updateReal)
        setRect(QRectF(QPointF(), size));
}

void Block::updateLine()
{
    if (prevSib == 0) {
        if (parent == 0) line = 0;
        else line = parent->line;
    } else {
        line = prevSib->line + prevSib->numberOfLines() - 1;
        if (prevSib->element->isLineBreaking()) line++;
    }
}

QPointF Block::getOffset(OffsetType type) const
{
    if (type == OffsetIn) {
        if (showing)
            return group->OFFSET_IN;
        else if (moreSpace)
            return group->NO_OFFSET;
        else
            return group->NO_OFFSET;
    } else {
        if (showing)
            return group->OFFSET_OUT;
        else if (moreSpace)
            return QPoint(group->OFFSET_OUT.x(), 0);
        else
            return group->NO_OFFSET;
    }
}

void Block::highlight(QPair<QFont, QColor> format) // ??
{
    if (!isTextBlock()) return;
    myTextItem->setFont(format.first);
    myTextItem->setDefaultTextColor(format.second);
}

QPainterPath Block::shape() const
{
    QPainterPath path;
    QRectF rect = boundingRect();

    int width = pen().width();
    int mod = width % 2;
    int half = width / 2;

    rect.adjust(-half, -half, half+mod, half+mod);

    path.addRect(rect);
    return path;
}

void Block::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();
    //    painter->fillRect(rect, Qt::white);

    if (pointed) {
        painter->fillRect(rect, format["hovered"]);
    }
    QPen myPen = pen();
    if (myPen.color() != Qt::white) {
        painter->setPen(myPen);
//        if (folded && !showing) {
//            QVector<QPointF> lines;
//            lines << QPointF(0,0) << QPointF(0,rect.height())
//                  << QPointF(rect.width(),0) << QPointF(rect.width(),rect.height());
//            painter->drawLines(lines);
//        } else {
        painter->drawRect(rect);
//        }
    }
}

void Block::updatePen()
{
    qreal width = 1;
    Qt::PenStyle style = Qt::SolidLine;
    QColor color = Qt::black;
    if (showing) {
        if (group->selectedBlock() == this) {
            width = 4; color = format["selected"];
        } else {
            width = 2; style = Qt::DotLine; color = format["showing"];
        }
    } else if (pointed) {
        width = 2; color = format["hovered_border"];
//    } else if (folded) {
//        color = Qt::blue;
    } else {
        color = Qt::lightGray;
//        color = Qt::white;
//        width = 0;
    }
    if (element->isUnknown()) {
        color = Qt::red;
    }
    setPen(QPen(QBrush(color), width, style, Qt::SquareCap, Qt::MiterJoin));
}

void Block::setShowing(bool newState, Block *stopAt) {
    if (this == stopAt) return;
    if (element->isSelectable()) {
        if (newState == showing) {
            return;
        }
        showing = newState;
        updatePen();
    }
    if (parent != 0)
        parent->setShowing(newState, stopAt);
    return;
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
    if (foldButton != 0) foldButton->updatePos();
}

QRectF Block::boundingRect() const
{
    return rect();
}

QList<Block*> Block::childBlocks() const {
    return BlockGroup::blocklist_cast(childItems());
}

QPointF Block::mapIdealToAncestor(Block* ancestor, QPointF pos) const
{
    if (ancestor == this) return pos;
    if (!element->getAncestors().contains(ancestor->element)) {
        qWarning("Not ancestor");
        return QPointF();
    }
    const Block *block = this;
    while (block != ancestor) {
        pos += block->idealPos();
        block = block->parent;
    }
    return pos;
}

void Block::setFolded(bool fold)
{
    if (fold == folded) return; // do nothing

    Block *child;

    if (fold) {
        QString text;
        Block *block = firstChild->getFirstLeaf();
        text.append(block->element->getType());
        block = block->getNext(true);
        while (block->line == line) {
            QString spacesStr = QString().fill(' ',
                 block->getAncestorWhereFirst()->element->getSpaces());
            text.append(spacesStr);
            text.append(block->element->getType());
            block = block->getNext(true);
        }
        text.append(" ...");
        myTextItem = new TextItem(text, this, true);
        highlight(group->docScene->getHighlightning().value("text_style"));

        child = firstChild;
        firstChild = 0;
    } else {
        if (myTextItem != 0)
            delete myTextItem;
        myTextItem = 0;
        firstChild = childBlocks().first();
        child = firstChild;
    }

    while(child != 0) {
        child->setVisible(!fold);
        child = child->nextSib;
    }

    folded = fold;
    if (fold && group->selectedBlock() != 0 // selected block is descendant of this -> select this
        && (group->selectedBlock()->element->getAncestors().contains(element)
            || group->selectedBlock() == this)) {
        group->selectBlock(this);
        myTextItem->setTextCursorPosition(0);
    } else {
        group->updateAll();
    }
    updatePen();
}

bool Block::isFoldable() const
{
    return folded ||
            (element->isSelectable() && hasMoreLines() && element->getType() != "block");
}

void Block::updateFoldButton()
{
    if (isFoldable()) {
        if (foldButton == 0) {
            foldButton = new FoldButton(this, group);
        }
        if (!isVisible()) {
            group->removeFoldable(this);
            foldButton->setVisible(false);
        } else {
            bool able = group->addFoldable(this);
            foldButton->setVisible(able);
        }
    } else {
        if (foldButton != 0) {
            delete foldButton;
            group->removeFoldable(this);
        }
        foldButton = 0;
    }
}

