#include "block.h"
#include "hide_block_button.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"

static int counter = 0;// testing only
int Block::lastLine = 0;
int Block::OFFS = 10;
static int lastX = 0;

Block::Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0) { // adding directly to scene, no parent blocks
        parentScene->addItem(this);
        docScene = (DocumentScene*)parentScene;
        parent = 0;
        prevSib = 0;
    } else {
        parent = parentBlock;
        docScene = parent->docScene;

        QList<Block*> siblings = parent->childBlocks();
        if (siblings.size() > 1) {
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
    line = 0;

    while (!element->isImportant()) // skip "unimportant" elements
        element = (*element)[0];

    this->element = element;

    if (element->isLeaf()) {
        myTextItem = new TextItem(element->getType(), this, element->allowsParagraphs());
        myTextItem->setPos(OFFS, 0);
    } else {
        myTextItem = 0;
        setToolTip(element->getType());
        foreach (TreeElement *childEl, element->getChildren()) {
            new Block(childEl, this);
        }
    }
    createControls();
    
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsFocusable);

    folded = false;
    pressed = false;
    edited = true;

    id = counter++;
    if (parent == 0)
        setChanged();  // this block (root) will update layout when first painted
}

Block::~Block()
{
    removeLinks();
}
void Block::removeLinks()
{
    if (prevSib != 0) prevSib->nextSib = nextSib;
    if (nextSib != 0) nextSib->prevSib = prevSib;
    if (parent->firstChild == this) parent->firstChild = nextSib;
}

void Block::createControls()
{
    //    if (element->isMultiLine()) {
    //        hideButton = new HideBlockButton(this);
    //        hideButton->setPos(0,0);
    //        hideButton->setVisible(false);
    //        setAcceptHoverEvents(true);
    //    } else
    hideButton = 0;
}

void Block::setParentItem (QGraphicsItem *parentItem)
        // moves this element and all unimportant elements on way to parentBlock's element ("branch")
        // to new parent
{
    if (parentItem == this) {
        parentItem = 0;//debug - toto nemoze nastavat
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
        // temporary: if non-leaf element becomes leaf, fill it with text
        if (oldParentEl->isLeaf()) {
            oldParent->myTextItem = new TextItem(oldParentEl->getType(), oldParent);
            oldParent->myTextItem->setPos(OFFS, 0);
        }
        // update links
        removeLinks();
    } else {
        branch = element->getRoot(); // get root of unvisualized(?) branch
    }
    // append to new parent element
    Block *newParent = qgraphicsitem_cast<Block*>(parentItem);
    if (newParent != 0) {
        TreeElement *newParentEl = newParent->element;
        newParentEl->appendChild(branch);
        // update links
        QList<Block*> siblings = newParent->childBlocks();
        if (siblings.size() > 1) {
            prevSib = siblings.at(siblings.size() - 2);
            prevSib->nextSib = this;
        } else {
            parent->firstChild = this;
            prevSib = 0;
        }
        nextSib = 0;
    }
    // remove from old parent block & append to new parent block
    QGraphicsRectItem::setParentItem(parentItem);
    this->parent = newParent;
}

void Block::stackBefore (const QGraphicsItem *sibling)
        // moves this element and all unimportant elements on way to parentBlock's element ("branch")
        // within current parent of this branch
{
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
            prevSib = nextSibling->prevSib;
            prevSib->nextSib = this;
            nextSib = nextSibling;
            nextSibling->prevSib = this;
        }
    }
    QGraphicsRectItem::stackBefore(sibling);
}

Block *Block::getParentBlock() const
{
    return parent;
}

/*Block *Block::getNextSibling() const
{
    Block *next = 0;
    if (parent != 0) {
        QList<Block*> children = parent->childBlocks();
        int index = children.indexOf(const_cast<Block*>(this));
        if (index < children.size()-1)
            next = children.at(index+1);
    }
    return next;
}
Block *Block::getPrevSibling() const
{
    Block *prev = 0;
    if (parent != 0) {
        QList<Block*> children = parent->childBlocks();
        int index = children.indexOf(const_cast<Block*>(this));
        if (index > 0)
            prev = children.at(index-1);
    }
    return prev;
}*/
Block *Block::getFirstLeaf() const
{
    if (firstChild == 0) return const_cast<Block*>(this);
    Block *block = firstChild;
    while (!block->isTextBlock())
        block = block->firstChild;
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
        while (!prev->isTextBlock()) {
            prev = prev->childBlocks().last();
        }
    }
    return prev;
}

int Block::numberOfLines() const
{
    if (isTextBlock()) {
        return textItem()->document()->lineCount();
    } else {
        Block *last = childBlocks().last();
        return (last->line + last->numberOfLines() - 1)  - this->line + 1;
    }
}
int Block::computeNextSiblingLine() const
{
    int lineNo = line + numberOfLines() - 1;
    if (element->isLineBreaking()) lineNo++;
    return lineNo;
}
QPointF Block::computeNextSiblingPos() const
{
    QPointF position = pos();
    if (!element->isLineBreaking() || parent == 0) {
        position.rx() += OFFS + boundingRect().width();
    } else {
        position.rx() = OFFS;
        // docasne kym neni pevna vyska riadku:
        qreal maxHeight = 0;
        foreach (Block *child, parent->childBlocks()) {
            maxHeight = qMax(maxHeight, child->boundingRect().height());
            if (child->element->isLineBreaking() && child != this)
                maxHeight = 0;
            if (child == this) break;
        }
        position.ry() += OFFS/2 + maxHeight;//boundingRect().height();
    }
    return position;
}

QList<Block*> Block::childBlocks() const
{
    return blocklist_cast(childItems());
}

TextItem *Block::textItem() const
{
    return myTextItem;
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    QVariant ret = QGraphicsRectItem::itemChange(change, value);
    //    if (change == QGraphicsItem::ItemChildAddedChange
    //        || change == QGraphicsItem::ItemChildRemovedChange) {
    //        QGraphicsItem *item = value.value<QGraphicsItem*>();
    //        Block *child = qgraphicsitem_cast<Block*>(item);
    //        if (child != 0) {
    //            setChanged();
    //        }
    //    }
    return ret;
}

void Block::focusInEvent(QFocusEvent *event)
{
    //    if (isTextBlock())
    //        textItem()->setFocus();
}
void Block::focusOutEvent(QFocusEvent *event)
{
    if (isTextBlock()) {
        QString text = myTextItem->toPlainText();
        if (text.isEmpty() && !element->isLineBreaking()) {
            Block *next = getNext();
            if (next == this) next = 0;
            setParentItem(0);
            delete(this);
            delete(element);
            if (next != 0) next->updatePos();
        } else {
            element->setType(text);
        }
    }
}
void Block::textFocusChanged(QFocusEvent* event)
{
    if (event->lostFocus())
        focusOutEvent(event);
}
void Block::textChanged()
{
    element->setType(myTextItem->toPlainText());
    updateXPosInLine();
    edited = true;
    docScene->update();

    lastX = 0;
}
void Block::keyPressed(QKeyEvent* event)
{
    if (event->key() != Qt::Key_Up && event->key() != Qt::Key_Down)
        lastX = 0;
}

void Block::splitLine(int cursorPos)    // algorithm still in construction
{
    if (parent == 0) return;
    if (element->allowsParagraphs()) {
        updatePos();
        return;
    }
    if (parent->element->allowsLineBreaks()) {
        // update this
        QString text = "";
        if (cursorPos >= 0) {
            text = textItem()->toPlainText();
            textItem()->setPlainText(text.left(cursorPos));
            text.remove(0,cursorPos);
        }
        bool alreadyBreaking = !this->element->setLineBreaking(true);

        // create new block
        if (!text.isEmpty() || alreadyBreaking) {
            Block *next = getNext();
            Block *newBlock = new Block(new TreeElement(text), parent);
            newBlock->element->setLineBreaking(alreadyBreaking);
            newBlock->stackBefore(next);
            next->setFocus();
        } else {
            getNext(true)->setFocus();
        }
        updatePos();
        docScene->update();
    } else if (cursorPos >= 0) {
        if (cursorPos == 0) {
            Block *block = getPrev();
            if (block->parent != 0 && block->parent->element->isLineBreaking())
                block->splitLine();
        } else if (cursorPos == length()) {
            Block *block = getNext();
            if (block->parent != 0 && block->parent->element->isLineBreaking())
                block->getPrev()->splitLine();
        }
    }
}

void Block::moveCursorLR(int key)
{
    Block *target = 0;
    int skipped;
    if (key == Qt::Key_Left) {
        target = getPrev(true);
        skipped = -1;
        while(target->element->isWhite()) {
            skipped = 0;
            target = target->getPrev(true);
        }
        target->textItem()->setTextCursorPosition(-1 + skipped);
    } else if (key == Qt::Key_Right) {
        target = getNext(true);
        skipped = 1;
        while(target->element->isWhite()) {
            skipped = 0;
            target = target->getNext(true);
        }
        target->textItem()->setTextCursorPosition(0 + skipped);
    }
}
void Block::moveCursorUD(int key, int from)
{
    Block *target = getPrev(true);
    int x = from;
    int y = line;
    while (target->line == line) {
        x += target->length();
        target = target->getPrev(true);
    }
    target = this;

    if (from == length() && getNext(true)->line > line && lastX > x)
        x = lastX;
    lastX = x;
    if (key == Qt::Key_Up) {
        if (line == 0)
            y = lastLine;
        else
            y = line - 1;
        do {
            target = target->getPrev(true);
        } while (target->line != y-1 && !(y == 0 && target->line == lastLine));
        target = target->getNext(true);
        do {
            int le = target->length();
            if (le >= x) {
                target->textItem()->setTextCursorPosition(x);
                return;
            } else {
                x -= le;
            }
            target = target->getNext(true);
        } while (target->line == y);
        target->getPrev(true)->textItem()->setTextCursorPosition(-1);

    } else if (key == Qt::Key_Down) {
        if (line == lastLine)
            y = 0;
        else
            y = line + 1; 
        do {
            target = target->getNext(true);
        } while (target->line != y);
        do {
            int le = target->length();
            if (le >= x) {
                target->textItem()->setTextCursorPosition(x);
                return;
            } else {
                x -= le;
            }
            target = target->getNext(true);
        } while (target->line == y);
        target->getPrev(true)->textItem()->setTextCursorPosition(-1);
    }
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!QRectF(0,0,OFFS,boundingRect().height()).contains(event->pos())) {
        // create new block
        Block *block = new Block(new TreeElement(" ",false, true), this);
        Block *next = findNextChildAt(event->pos());
        block->stackBefore(next);
        block->updatePos();
        block->setFocus();
        QGraphicsRectItem::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        Block *oldParent = parent;
        // remove from parent and add directly to scene
        // item is now on top of everything (that have z-value==0)
        // new parent will be resolved after mouse is released
        if (oldParent != 0) {
            Block *next = getNext();
            setPos(scenePos());
            setParentItem(0);
            next->updatePos();  // update blocks after removal
            oldParent->edited = true;
        }
        pressed = true;
        edited = true;
        setZValue(100);
        QPointF curPos = pos();
        mouseMoveEvent(event);  // updates futureParent/Sibling and draw separator line immediatelly
        setPos(curPos);
        docScene->update();
    }
    QGraphicsRectItem::mousePressEvent(event);
}
void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!pressed) return;

    futureParent = 0;
    // target is under left top corner of moved block OR target is under cursor
    QList<Block*> blocks = blocklist_cast(docScene->items(/*event->*/scenePos()));
    foreach (Block *block, blocks) { // find next different block
        if (block != this) {
            futureParent = block;
            break;
        }
    }
    if (futureParent != 0 && futureParent->element->isLeaf()) {// leaf test - cannot add child to leaf
        futureParent = futureParent->getParentBlock();
        if (futureParent == this) {
            futureParent = 0;//debug - toto nemoze nastavat
        }
    }
    if (futureParent != 0) {
        futureSibling = futureParent->findNextChildAt(futureParent->mapFromScene(/*event->*/scenePos()));
        docScene->showInsertLine(futureParent->getInsertLineAt(futureSibling));
    } else {
        futureSibling = 0;
        docScene->hideInsertLine();
    }
    QGraphicsRectItem::mouseMoveEvent(event);
}
void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!pressed) return;

    if (event->button() == Qt::LeftButton) {
        setZValue(0);               // restore z-value

        if (futureParent != 0) {
            setParentItem(futureParent);
            if (futureSibling != 0) {
                stackBefore(futureSibling);
            }
            futureParent->prepareGeometryChange();    // used to update graphics
            futureParent->edited = true;
            updatePos();
        }
        futureParent = 0;
        futureSibling = 0;
        docScene->hideInsertLine();
    }
    pressed = false;
    QGraphicsRectItem::mouseReleaseEvent(event);
    docScene->update();
}

Block* Block::findNextChildAt(QPointF pos) const
{// note: distance is computed from pot left corner of block's bounding rect
    QList<Block*> blocks = childBlocks();
    if (blocks.isEmpty())
        return 0;

    Block *nextBlock = 0;
    QLineF dist = QLineF(QPointF(), pos);
    qreal minDist = dist.length();
    // test distance from block starts
    foreach (Block *block, blocks) {
        dist.setP1(mapFromItem(block, block->boundingRect().topLeft()));
        if (dist.length() < minDist) {
            minDist = dist.length();
            nextBlock = block;
        }
    }
    // test distance from last block end
    Block *lastBlock = blocks.last();
    dist.setP1(mapFromItem(lastBlock, lastBlock->boundingRect().topRight()));
    if (dist.length() < minDist) {
        nextBlock = 0;
    }

    return nextBlock;
}

QLineF Block::getInsertLineAt(const Block* nextBlock) const
{
    QLineF line;
    if (nextBlock != 0) {   // before child if provided
        QRectF rect = nextBlock->mapRectToScene(nextBlock->boundingRect());
        line = QLineF(rect.topLeft(), rect.bottomLeft());
        line.translate(-OFFS/2, 0);
    } else {                // after child if not provided
        Block *lastChild = childBlocks().last();// must have at least 1 child
        QRectF rect = lastChild->mapRectToScene(lastChild->boundingRect());
        line = QLineF(rect.topRight(), rect.bottomRight());
        line.translate(OFFS/2, 0);
    }
    return line;
}

void Block::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    hideButton->setVisible(true);
    //QGraphicsRectItem::hoverEnterEvent(event);
}
void Block::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideButton->setVisible(false);
    //QGraphicsRectItem::hoverLeaveEvent(event);
}
void Block::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    //QGraphicsRectItem::hoverMoveEvent(event);
}

void Block::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
}
void Block::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
}
void Block::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
}
void Block::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
}

void Block::setChanged()
{
    if (parent != 0)
        parent->setChanged();
    else
        changed = true;
}

//static int H = 50;

void Block::updateLayout() // parent to child updater
        // used to update everything from root up, updates line numbers
{
    int lineNo = line;
    QPointF nextPos = QPointF(OFFS, OFFS/2);
    foreach (Block *child, childBlocks()) {
        child->line = lineNo;
        child->setPos(nextPos);
        child->updateLayout();

        lastLine = lineNo;
        lineNo = child->computeNextSiblingLine();
        nextPos = child->computeNextSiblingPos();
    }
    edited = false;
}

void Block::updatePos() // child to parent updater
        // used to update everything after this block, after int parent etc.,
        // updates also this block's position
        // updates line numbers
{
    if (parent == 0) return;
    int lineNo;
    QPointF nextPos;

    if (prevSib != 0) {
        lineNo = prevSib->computeNextSiblingLine();
        nextPos = prevSib->computeNextSiblingPos();
    } else {
        lineNo = parent->line;
        nextPos = QPointF(OFFS, OFFS/2);
    }

    QList<Block*> children = parent->childBlocks();
    int index = children.indexOf(this);
    for (int i = index; i < children.size(); i++) { // start with this block
        Block *child = children.at(i);
        child->setPos(nextPos);
        child->setLine(lineNo);

        lastLine = lineNo;
        nextPos = child->computeNextSiblingPos();
        lineNo = child->computeNextSiblingLine();
    }
    // this block and its siblings are updated, repeat with parent
    parent->updatePos();
}

void Block::updateXPosInLine() // child to parent updater
        // used to update everything from changed child down, updates only 1 line
        // doesn't update this block's position!
{    
    if (parent == 0) return;
    qreal nextX = computeNextSiblingPos().x();

    QList<Block*> children = parent->childBlocks();
    int index = children.indexOf(this);
    for (int i = index+1; i < children.size(); i++) { // start with next block
        Block *child = children.at(i);
        child->setX(nextX);
        nextX = child->computeNextSiblingPos().x();
    }
    // this block and its siblings are updated, repeat with parent
    if (parent->line == line)
        parent->updateXPosInLine();
}

int Block::type() const
{
    return Type;
}

QRectF Block::boundingRect() const
{
    QRectF rect = childrenBoundingRect();
    if (isTextBlock()) {
        return rect.adjusted(-OFFS,0,0,0);
    }
    return rect.adjusted(-OFFS, -OFFS/2, OFFS, OFFS/2);
}

QPainterPath Block::shape() const   // default implementation
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Block::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    if (changed) {
        updateLayout();
        changed = false;
    }

    QRectF rect = boundingRect();
    painter->setPen(pen());
    painter->fillRect(rect, Qt::white);
    
    QColor color = Qt::lightGray;
    if (edited) color = Qt::yellow;
    if (element->isUnknown()) color = Qt::red;
    painter->fillRect(QRectF(0,0,OFFS,rect.height()), color);

    //*****
    if (element->isLineBreaking())
        painter->fillRect(QRectF(0,0,OFFS,OFFS), Qt::blue);
    if (element->allowsLineBreaks())
        painter->fillRect(QRectF(0,OFFS,OFFS,OFFS), Qt::green);
    if (element->allowsParagraphs())
        painter->fillRect(QRectF(0,OFFS,OFFS,OFFS), Qt::red);
    if (OFFS>0)
        painter->drawText(3, OFFS, QString("%1").arg(line));
    if (element->isWhite())
        painter->setPen(Qt::gray);
    //*****

    painter->drawRect(rect);
}

void Block::setFolded(bool fold)
{
    if (fold == folded) return; // do nothing
    if (fold) {
        ;// todo fold
    } else {
        ;//todo unfold
    }
    foreach (Block *child, childBlocks()) {    // hide/unhide children
        if (child->line > 0) {
            child->setVisible(!fold);
        }
    }
    folded = fold;                          // update folded flag
    setChanged();
}
bool Block::isFolded() const
{
    return folded;
}

void Block::setLine(int newLine) {
    if (line == newLine) return;
    int diff = newLine - line;
    line = newLine;
    foreach (Block *child, childBlocks()) {
        child->setLine(child->line + diff);
    }
}

bool Block::isTextBlock() const
{
    return myTextItem != 0;
}
int Block::length() const
{
    if (!isTextBlock()) return 0;
    return element->getType().length();
}

QList<Block*> Block::blocklist_cast(QList<QGraphicsItem*> list) const
{
    QList<Block*> blocks;
    foreach (QGraphicsItem *item, list) {
        Block *block = qgraphicsitem_cast<Block*>(item);
        if (block != 0)
            blocks << block;
    }
    return blocks;
}

