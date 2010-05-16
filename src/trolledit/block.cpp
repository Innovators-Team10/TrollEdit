#include "block.h"
#include "fold_button.h"
#include "block_group.h"
#include "doc_block.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"

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
        // destroy text item if needed
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
            if (!childEl->isFloating()) {   // create block from child element
                new Block(childEl, this);
            } else {                        // create docblock form child element
                QString text = childEl->getText();
                if (text.endsWith('\n')) {
                    text.chop(1);
                    childEl->setLineBreaking(true);
                }
                childEl->deleteAllChildren();
                DocBlock *bl = new DocBlock(childEl, this);
                bl->addText(text);
            }
        }        
    }

    // set highlighting
    assignHighlighting(element);

    // set flags
        setAcceptHoverEvents(true);
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

    // set size
    updateGeometry(true);
    updateFoldButton();
    updatePen();
}

Block::~Block()
{
    if (foldButton != 0) {
        delete foldButton;
        foldButton = 0;
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
        if (group->docScene->getHighlightning().contains(el->getType())) {
            highlightFormat = group->docScene->getHighlightning().value(el->getType());
            f = true;
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
        // resolve line breaks
        Block *ancestor = getAncestorWhereLast();
        if (nextSib == 0 && !element->isFloating()) {
            if(ancestor->element->isLineBreaking()) {
                element->setLineBreaking(true);
                if (prevSib != 0 && prevSib->element->isLineBreaking())
                    prevSib->element->setLineBreaking(false);
            } else {
                if (prevSib != 0 && prevSib->element->isLineBreaking()) {
                    ancestor->element->setLineBreaking(true);
                    prevSib->element->setLineBreaking(false);
                }
            }
        }
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
        if (!element->isFloating()) {
            // update links
            QList<Block*> siblings = newParent->childBlocks();
            if (siblings.size() > 0) {      // this block is not in siblings yet!
                prevSib = siblings.at(siblings.size() - 1);
                prevSib->nextSib = this;
                if (getAncestorWhereLast()->element->isLineBreaking()) {
                    getAncestorWhereLast()->element->setLineBreaking(false);
                    element->setLineBreaking(true);
                }
            } else {
                newParent->firstChild = this;
                prevSib = 0;
            }
            nextSib = 0;
            // remove textItem if needed
            if (newParent->myTextItem != 0) {
                delete newParent->myTextItem;
                newParent->myTextItem = 0;
            }
            // resolve line breaks
            Block *ancestor = getAncestorWhereLast();
            if (prevSib != 0 && ancestor->element->isLineBreaking()) {
                prevSib->element->setLineBreaking(true);
            }
            if(element->isLineBreaking()) {
                element->setLineBreaking(false);
                ancestor->element->setLineBreaking(true);
            }
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
            // resolve line breaks
            Block *ancestor = getAncestorWhereLast();
            if (nextSib == 0 && ancestor->element->isLineBreaking()) {
                element->setLineBreaking(true);
                if (prevSib->element->isLineBreaking())
                    prevSib->element->setLineBreaking(false);
            }
            // update links
            removeLinks();
            if (!element->isFloating()) {
                prevSib = nextSibling->prevSib;
                if (prevSib != 0) {
                    prevSib->nextSib = this;
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

Block *Block::removeBlock(bool deleteThis)
{
//    int remSpaces = 0;
    QList<Block*> toDelete;
    Block *toRemove = this;
    Block *next = getNext();
    bool changeSelected = false;
    bool breaking = false;
    Block *selected = group->selectedBlock();

    do {    // remove block and all ancestors (that would became leafs) from hierarchy
//        remSpaces += toRemove->element->getSpaces();    // collect spaces from deleted blocks
        Block *oldParent = toRemove->parent;
        toRemove->setParentBlock(0);                                // remove from hierarchy
        if (toRemove == selected) changeSelected = true;            // set flag if selected
        if (toRemove->element->isLineBreaking()) breaking = true;   // set flag if line breaking
        toDelete << toRemove;
        toRemove = oldParent;
    } while (toRemove != 0 && toRemove->element->isLeaf());

    if (!deleteThis) {
        toDelete.removeOne(this);                   // input block is not destroyed
//        element->setSpaces(remSpaces);   // add collected spaces
    }

    if (!toDelete.contains(next)) {
        if (changeSelected && toRemove != 0)       // reselect if needed
            group->selectBlock(toRemove);
        if (breaking && next->prevSib != 0)    // add linebreak if needed
            next->prevSib->element->setLineBreaking(true);
        next->edited = true;
    } else {
        next = 0;
        group->deselect();
    }
    foreach (Block* bl, toDelete) {  // destroy collected blocks
        group->removeFoldable(bl);
        bl->deleteLater();
    }

    group->mainBlock()->updateBlock();
    return next;
}

void Block::removeLinks()
{
    if (nextSib != 0) {
        nextSib->prevSib = prevSib;
        if (prevSib == 0)
            nextSib->element->setSpaces(0);
    }
    if (prevSib != 0) {
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
        return myTextItem->document()->lineCount();
    } else {
        Block *last = childBlocks().last();
        return (last->line + last->numberOfLines() - 1) - this->line + 1;
    }
}

bool Block::hasMoreLines() const
{
    if (isTextBlock()) {
        return myTextItem->document()->lineCount() > 1;
    } else {
        Block *last = childBlocks().last();
        return (last->line > this->line) || last->hasMoreLines();
    }
}

void Block::addBlockAt(Block *block, QPointF pos)
{
    QPair<Block*, bool> targetRight = findClosestBlock(pos);
    Block *target = targetRight.first;
    if (target == 0 || target->isTextBlock()) {
        if (!targetRight.second)
            target = target->nextSib;
        block->setParentBlock(this);
        block->stackBeforeBlock(target);
    } else {
        target->addBlockAt(block, target->mapFromParent(pos));
    }
}

Block *Block::addTextCursorAt(QPointF pos)
{
    if (isTextBlock()) {        // add cursor to this block
        pos = mapToItem(myTextItem, pos);   // map to my TextItem
        int cursorPos;
        // find cursor position:
        if (element->allowsParagraphs()) {  // use hitTest for multiline blocks
            cursorPos = myTextItem->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
        } else {                            // use calculation for other blocks
            cursorPos = pos.x() / group->CHAR_WIDTH;
        }
        // set cursor position
        if (cursorPos < 0 || !myTextItem->setTextCursorPos(cursorPos))
            myTextItem->setTextCursorPos(0);
        return this;    // return block with cursor
    }

    // add cursor to some child of this block
    QPair<Block*, bool> targetRight = findClosestBlock(pos);
    Block *target = targetRight.first;

    if (target == 0 || target->isTextBlock()) { // to target or text block
        if (target == 0) {  // no target = after last child - add to last child
            target = getLastLeaf();
            target->textItem()->setTextCursorPos(-1);
        } else {            // text block target
            pos = mapToItem(target->textItem(), pos);   // map to target's TextItem
            int cursorPos;
            // find cursor position:
            if (target->element->allowsParagraphs()) {  // use hitTest for multiline blocks
                cursorPos = target->textItem()->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
            } else {                                    // use calculation for other blocks
                cursorPos = pos.x() / group->CHAR_WIDTH;
            }
            // set cursor position
            if (cursorPos < 0 || !target->textItem()->setTextCursorPos(cursorPos)) {
                if (targetRight.second)
                    target->textItem()->setTextCursorPos(0);
                else
                    target->textItem()->setTextCursorPos(-1);
            }
        }
        return target;    // return block with cursor
    } else {                                    // non text target - continue recursively
        return target->addTextCursorAt(target->mapFromParent(pos));
    }
}

QPair<Block*, bool> Block::findClosestBlock(QPointF pos) {
    Block *sibling = 0;
    qreal minDistX = idealSize().width(), minDistY = idealSize().height();
    bool siblingIsRight = true;

    // test distance from block hotspots
    Block *child = firstChild;
    while (child != 0) {
        QRectF rect = child->idealGeometry;
        if (pos.y() >= rect.top() && pos.y() < rect.bottom()) {
            if (qAbs(pos.x() - rect.left()) < minDistX ||
                qAbs(pos.y() - rect.center().y()) < minDistY) {
                minDistX = qAbs(pos.x() - rect.left());
                minDistY = qAbs(pos.y() - rect.center().y());
                sibling = child;
                siblingIsRight = true;
            }
            if (qAbs(pos.x() - rect.right()) < minDistX ||
                qAbs(pos.y() - rect.center().y()) < minDistY) {
                minDistX = qAbs(pos.x() - rect.right());
                minDistY = qAbs(pos.y() - rect.center().y());
                sibling = child;
                siblingIsRight = false;
            }
        }
        child = child->nextSib;
    }
    // returns closest sibling and true if it's to right of pos, false if it's to left
    return QPair<Block*, bool>(sibling, siblingIsRight);
}

void Block::textFocusChanged(QFocusEvent* event)// TODO
{
    if (event->gotFocus()) {    // focus in
        if (element->isPaired()) {
            TreeElement *pair = element->getPair();
            if (isTextBlock()) {
                if (pair != 0) {
                    myTextItem->setDefaultTextColor(Qt::red);
                    pair->getBlock()->textItem()->setDefaultTextColor(Qt::red);
                }
            }
        }
    } else {                    // focus out
        if (element->isPaired()) {
            TreeElement *pair = element->getPair();
            if (isTextBlock()) {
                myTextItem->setDefaultTextColor(Qt::black);
                if (pair != 0) {
                    pair->getBlock()->textItem()->setDefaultTextColor(Qt::black);
                }
            }
        }
    }
}

void Block::textChanged()
{
//    if (element->isFloating()) return;

    QString text = myTextItem->toPlainText();
    bool toUpdate = false;

    myTextItem->document()->blockSignals(true);

    if (text.isEmpty()) {   // delete block
        if (!(element->isLineBreaking() && getPrev(true)->line != line)//) {
            && !myTextItem->hasFocus()) {
            // don't delete if block is single newline in this line OR has focus
            // focused blocks will de deleted when they lose focus
            removeBlock(true);
            return;
        } else if(isFolded()) {
            setFolded(false);
            getFirstLeaf()->textItem()->setTextCursorPos(0);
            return;
        }
    } else if (text.at(0).isSpace() && !isFolded()) {      // remove leading spaces and tabs
        Block *ancestor = getAncestorWhereFirst();
        int count = 1;
        if (text.at(0) == '\t') count = group->TAB_LENGTH;
        do {
            text.remove(0, 1);
            ancestor->element->addSpaces(count);
        } while(text.at(0).isSpace());
        textItem()->setPlainText(text);
        toUpdate = true;
    }

    if (element->getType() != text || toUpdate) {
        if(!isFolded()) {
            // remove highlighting if text changed
            if (element->getType() != text) {
                highlight(group->docScene->getHighlightning().value("text_style"));
                format = group->docScene->getBlockFormatting().value("block_style");
                edited = true;
            }
            element->setType(text);
        }
        updateGeometryAfter(false);
    }

    myTextItem->document()->blockSignals(false);
}

bool Block::isEdited() const
{
    if (edited)
        return true;
    Block *child = firstChild;
    while (child != 0) {
        if (child->isEdited())
            return true;
        child = child->nextSib;
    }
    return false;
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemVisibleHasChanged) {
        updateFoldButton();
        emit visibilityChanged(value.toBool());
    }
    return QGraphicsItem::itemChange(change, value);
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (element->isSelectable()) {
        Block *selected;
        if (isEdited()) {
            selected = group->reanalyze(this, event->scenePos());
            selected->startDrag(event->widget());
        } else {
            group->selectBlock(this);
            updateGeometryAfter();
            addTextCursorAt(event->pos());
            startDrag(event->widget());
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

void Block::startDrag(QWidget *widget)
{
    if (!element->isFloating()) {
        QDrag *drag = new QDrag(widget);
        QMimeData *mimeData = new QMimeData();
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << element->getType();
        mimeData->setData(BlockGroup::BLOCK_MIME, itemData);
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
    }
}

void Block::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME)) {
        Block *selected = group->selectedBlock();
        if (selected != 0) {
            if (selected != this && !selected->isAncestorOf(this)) {
                selected->group->deselect();
                if (event->dropAction() == Qt::CopyAction) {
                    selected = new Block(selected->element->clone(), 0, group);
                } else {
                    selected->removeBlock(false);
                    selected->setPos(selected->mapToScene(selected->idealPos()));
                    selected->updatePos(true);
                }
                addBlockAt(selected, event->pos());

                offsetChildren(false);
                group->reanalyze(selected, event->scenePos());
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
    if (event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME)) {
        event->acceptProposedAction();
        offsetChildren(true, event->pos());
//        updateGeometryAfter(); // better updater!! complete update not necessary at every move
        group->mainBlock()->updateBlock();
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
    if (event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME)) {
        event->acceptProposedAction();
        offsetChildren(true, event->pos());
//        updateGeometryAfter();
        group->mainBlock()->updateBlock();
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
//    updateGeometryAfter();
    group->mainBlock()->updateBlock();
}

void Block::offsetChildren(bool flag, QPointF pos)
{
    Block* child = firstChild;
    while (child != 0) {
        QRectF rect = child->idealGeometry;
        if (flag && pos.y() >= rect.top() && pos.y() < rect.bottom()) {
            child->moreSpace = true;
        } else {
            child->moreSpace = false;
        }
        if (!child->element->isSelectable())
            child->offsetChildren(flag, child->mapFromParent(pos));
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
        if (block != this) {
            block->pointed = false;
            block->updatePen();
            block->update();
        }
        updatePen();
        update();
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
        if (block != this && block->hovered) {
            block->pointed = true;
            block->updatePen();
            block->update();
        }
        updatePen();
        update();
    }
}

void Block::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!element->isSelectable()){
        event->ignore();
        return;
    }
    QMenu menu; // TODO
}

void Block::updateBlock(bool doAnimation)
        // used to update everything from root up
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

void Block::updateBlockAfter(bool doAnimation) {
    qWarning("updateBlockAfter: method not implemented yet!");
    // used to update everything after this block
    // updates line numbers, geometry and foldbutton
//    Block *child = firstChild;
//    // update children's positions
//    while (child != 0) {
//        child->updatePos(!doAnimation);
//        if (doAnimation) child->animate();
//        child = child->nextSib;
//    }
//    // update my size
//    updateSize(!doAnimation);
//    // let parent update siblings' and my position togethr with its size
//    if (parent != 0)
//        parent->updateBlockAfter(doAnimation);
}

void Block::updateGeometryAfter(bool doAnimation) {
    // used to update everything after this block
    // updates only geometry
    Block *child = firstChild;
    // update children's positions
    while (child != 0) {
        child->updatePos(!doAnimation);
        if (doAnimation) child->animate();
        child = child->nextSib;
    }
    // update my size
    updateSize(!doAnimation);
    // let parent update siblings' and my position together with its size
    if (parent != 0)
        parent->updateGeometryAfter(doAnimation);
    else
        if (doAnimation)
            animate();
}

void Block::animate()
{
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
    if (isTextBlock()) {
        myTextItem->setPos(getOffset(InnnerTopLeft));
    }
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
                Block *lastLeaf = prevSib->getLastLeaf();
                pos = lastLeaf->mapIdealToAncestor(prevSib->parent, lastLeaf->idealRect().topRight());
                if (prevSib->showing || lastLeaf->moreSpace) {
                    pos.rx() = prevSib->idealPos().x() + prevSib->idealSize().width();

                    offs = prevSib->getOffset(Outer);
                } else {
                    offs = lastLeaf->getOffset(Outer);
                }
            } else {
                offs = prevSib->getOffset(Outer);
                pos = prevSib->idealPos();
                pos.rx() += prevSib->idealSize().width();
            }
            pos.rx() += getOffset(Outer).x() + offs.x();
        } else {
            qreal maxY = 0;
            qreal offsY = 0;
            foreach (Block *child, parent->childBlocks()) {
                int y = child->idealPos().y() + child->idealSize().height();
                if (y > maxY) {
                    maxY = y;
                    offsY = child->getOffset(Outer).y();
                }
                if (child == prevSib) break;
            }
            pos.rx() = parent->getOffset(InnnerTopLeft).x();
            pos.ry() += maxY + getOffset(Outer).y() + offsY;
        }
    } else {
        if (parent != 0)
            pos = parent->getOffset(InnnerTopLeft);
        else
            pos = this->pos();
    }

    pos.rx() += element->getSpaces() * group->CHAR_WIDTH;
    idealGeometry.moveTo(pos);

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
                child->getAncestorWhereLast()->element->isLineBreaking()) {
                childRect.setBottomRight(childRect.bottomRight() +
                                         child->getOffset(Outer));
            }
            rect = rect.united(childRect);
            child = child->nextSib;
        }
        size = rect.size();
    }
    QPointF offs = getOffset(InnnerTopLeft) + getOffset(InnerBottomRight);

    size.rwidth() += offs.x();
    size.rheight() += offs.y();
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
    switch (type) {
    case InnnerTopLeft :
        if (showing)
            return group->OFFSET_IN_TL;
        else
            return group->NO_OFFSET;
    case InnerBottomRight :
        if (showing)
            return group->OFFSET_IN_BR;
        else
            return group->NO_OFFSET;
    case Outer :
        if (showing)
            return group->OFFSET_OUT;
        else if (moreSpace)
            return group->OFFSET_INSERT;
        else
            return group->NO_OFFSET;
//    case Drop :
//        return group->OFFSET_INSERT;
    default:
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
//    if (element->isFloating()) {
//        painter->fillRect(rect, Qt::white);
//    }

        if (showing) {
            painter->fillRect(rect, Qt::white);
            QColor color = Qt::blue;//format["hovered"];
            if (element->isUnknown()) color = Qt::red; //

            if (level > 0) {
                color.setAlpha(level*15);
                painter->fillRect(rect, color);
            }
        }
        if (showing || pointed) {
            QColor color = Qt::blue;//format["hovered_border"];
            if (element->isUnknown()) color = Qt::red; //
            color.setAlpha(8*15);
            painter->setPen(color);
            painter->drawRect(rect);
        }
//    QPen myPen = pen();
//    if (myPen.color() != Qt::white) {
//        painter->setPen(myPen);
//
//        QPainterPath line(rect.topRight());
//        line.lineTo(rect.bottomRight());
////        line.lineTo(rect.bottomLeft());
////        painter->drawPath(line);
//    }
}

void Block::updatePen()
{
    qreal width = 1;
    Qt::PenStyle style = Qt::SolidLine;
    QColor color = Qt::black;
    if (showing) {
        if (group->selectedBlock() == this) {
            width = 4;
            color = format["selected"];
        } else {
            width = 2;
//            style = Qt::DotLine;
            color = format["showing"];
        }
    } else if (pointed) {
        width = 2; color = format["hovered_border"];
//    } else if (folded) {
//        color = Qt::blue;
    } else {
//        color = Qt::lightGray;
        color = Qt::white;
//        width = 0;
    }
    if (element->isUnknown()) {
        color = Qt::red;
    }
    setPen(QPen(QBrush(color), width, style, Qt::FlatCap, Qt::MiterJoin));
}

void Block::setShowing(bool newState, Block *until) {
    if (!newState) {
        level = 0;
        if (this != until) {
            showing = false;
        } else {
            until = parent;
        }
        if (parent != 0)
            parent->setShowing(false, until);
    } else {
        Block *block = this;
        QList<Block *> ancestors;
        while (block != 0) {
            if (block->element->isSelectable())
                ancestors << block;
            block = block->parent;
        }
        int level = qMin(ancestors.size()-1, 7);
        foreach (Block *bl, ancestors) {
            bl->level = qMax(0, level--);
            bl->showing = true;
        }
    }
    updatePen();
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
    if (!ancestor->isAncestorOf(this)) {
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
    folded = fold;

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
            myTextItem->deleteLater();;
        myTextItem = 0;
        firstChild = childBlocks().first();
        child = firstChild;
    }

    while(child != 0) {
        child->setVisible(!fold);
        child = child->nextSib;
    }

    if (fold && group->selectedBlock() != 0 // selected block is descendant of this -> select this
        && (this->isAncestorOf(group->selectedBlock())
            || group->selectedBlock() == this)) {
        group->selectBlock(this);
        myTextItem->setTextCursorPos(0);
    }
    updatePen();
    group->mainBlock()->updateBlock();
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
            foldButton->updatePos();
        }
    } else {
        if (foldButton != 0) {
            delete foldButton;
            group->removeFoldable(this);
        }
        foldButton = 0;
    }
}

