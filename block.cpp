#include "block.h"
#include "tree_element.h"
#include "hide_block_button.h"
#include "text_item.h"
#include "document_scene.h"

static int counter = 0;// testing only

Block::Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0) { // adding directly to scene, no parent blocks
        parentScene->addItem(this);
        docScene = (DocumentScene*)parentScene;
        parent = 0;
    } else {
        parent = parentBlock;
        docScene = parent->docScene;
    }

    while (!element->isImportant()) // skip "unimportant" elements
        element = (*element)[0];

    this->element = element;

    if (element->isLeaf()) {
        myTextItem = new TextItem(element->getType(), this);     
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

    line = 0;

    id = counter++;
    if (parent == 0)
        setChanged();  // this block (root) will update layout when first painted
}

Block::~Block()
{
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
{
    if (parentItem == this) {
        parentItem = 0;//debug - toto nemoze nastavat
    }
    // remove from old parent element
    Block *oldParent = this->parent;
    if (oldParent != 0) {
        TreeElement *oldParentEl = oldParent->element;
        // all links between this block's element and his parent's element are deleted
        oldParentEl->deleteBranchTo(element);
        // temporary: if non-leaf element becomes leaf, fill it with text
        if (oldParentEl->isLeaf()) {
            oldParent->myTextItem = new TextItem(oldParentEl->getType(), oldParent);
        }
    }
    // append to new parent element
    Block *newParent = qgraphicsitem_cast<Block*>(parentItem);
    if (newParent != 0) {
        TreeElement *newParentEl = newParent->element;
        newParentEl->appendChild(element);
    }
    // remove from old parent block & append to new parent block
    QGraphicsRectItem::setParentItem(parentItem);
    this->parent = newParent;
}

void Block::stackBefore (const QGraphicsItem *sibling)
{
    const Block *nextSibling = qgraphicsitem_cast<const Block*>(sibling);
    if (nextSibling != 0) {
        if (parent != 0 && nextSibling->parent == parent) {
            TreeElement *parentEl = parent->element;
            // remove from original position (all links deleted)
            parentEl->deleteBranchTo(element);
            // compute new index
            int index = parentEl->indexOfDescendant(nextSibling->element);
            // insert at new position
            parentEl->insertChild(index, element);
        }
    }
    QGraphicsRectItem::stackBefore(sibling);
}

Block *Block::getParentBlock() const
{
    return parent;
}

Block *Block::getNextSibling() const
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
}
Block *Block::getNextBlock(bool textOnly) const
{
    Block *next = 0;
    if (parent != 0) {
        next = getNextSibling();
        if (next == 0)
            return parent->getNextBlock(textOnly);
    } else {
        next = const_cast<Block*>(this);
    }
    if (textOnly) {
        while (!next->isTextBlock()) {
            next = next->childBlocks().first();
        }
        if (next->element->isNewline())
            return next->getNextBlock(textOnly);
    }
    return next;
}
Block *Block::getPrevBlock(bool textOnly) const
{
    Block *prev = 0;
    if (parent != 0) {
        prev = getPrevSibling();
        if (prev == 0)
            return parent->getPrevBlock(textOnly);
    } else {
        prev = const_cast<Block*>(this);
    }
    if (textOnly) {
        while (!prev->isTextBlock()) {
            prev = prev->childBlocks().last();
        }
        if (prev->element->isNewline())
            return prev->getPrevBlock(textOnly);
    }
    return prev;
}

/*int Block::numberOfLines() const
{
    if (isTextBlock()) {
        return 1;   // todo: vynimka pre dokumentacne bloky
    } else {
        return childBlocks().last()->line - this->line + 1;
    }
}*/
QPointF Block::computeNextSiblingPos() const
{
    QPointF position = pos();
    if (!element->isNewline() || parent == 0) {
        position.rx() += OFFS + boundingRect().width();
    } else {
        position.rx() = OFFS;
        // docasne kym neni pevna vyska riadku:
        qreal maxHeight = 0;
        foreach (Block *child, parent->childBlocks()) {
            maxHeight = qMax(maxHeight, child->boundingRect().height());
            if (child == this) break;
        }
        position.ry() += OFFS + maxHeight;
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

void Block::focusOutEvent(QFocusEvent *event)
{
    if (isTextBlock()) {
        element->setType(myTextItem->toPlainText());
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
}
void Block::addNewLineAfterThis(QString text)
{
    if (parent != 0) {
        Block *newBlock = new Block(new TreeElement(text), 0, docScene);
        newBlock->setParentItem(parent);
        newBlock->stackBefore(getNextSibling());

        Block *newLine = new Block(new TreeElement("\n"), 0, docScene);
        newLine->setParentItem(parent);
        newLine->stackBefore(newBlock);
        newLine->updatePos();

        newBlock->textItem()->setFocus();
        docScene->update();
    }

}
void Block::moveCursorLR(int key)
{
    Block *target = 0;
    if (key == Qt::Key_Left) {
        target = getPrevBlock(true);
        target->textItem()->setTextCursorPosition(-1);
    } else if (key == Qt::Key_Right) {
        target = getNextBlock(true);
        target->textItem()->setTextCursorPosition(0);
    }
}
void Block::moveCursorUD(int key)
{
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!QRectF(0,0,OFFS,boundingRect().height()).contains(event->pos()))
        return;

    if (event->button() == Qt::LeftButton) {
        Block *oldParent = parent;
        // remove from parent and add directly to scene
        // item is now on top of everything (that have z-value==0)
        // new parent will be resolved after mouse is released
        if (oldParent != 0) {
            Block *next = getNextBlock();
            setPos(scenePos());
            setParentItem(0);
            next->updatePos();  // update blocks after removal
        }
        pressed = true;
        setZValue(100);
        QPointF curPos = pos();
        mouseMoveEvent(event);  // updates futureParent/Sibling and draw separator line immediatelly
        setPos(curPos);
    }
    QGraphicsRectItem::mousePressEvent(event);
    docScene->update();
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

void Block::updateLayout(int lineNo) // parent to child updater
        // used to update everything from root up, updates line numbers
{
    line = lineNo;
    QPointF nextPos = QPointF(OFFS, OFFS);
    foreach (Block *child, childBlocks()) {
        child->line = lineNo;
        child->updateLayout(lineNo);
        child->setPos(nextPos);

        if (child->element->isNewline()) lineNo++;
        nextPos = child->computeNextSiblingPos();
    }
}

void Block::updatePos() // child to parent updater
        // used to update everything after this block, after int parent etc.,
        // updates also this block's position
        // updates line numbers
{
    if (parent == 0) return;
    int lineNo;
    QPointF nextPos;
    Block *prevSibling = getPrevSibling();

    if (prevSibling != 0) {
        lineNo = prevSibling->line;
        if (prevSibling->element->isNewline()) lineNo++;
        nextPos = prevSibling->computeNextSiblingPos();
    } else {
        lineNo = parent->line;
        nextPos = QPointF(OFFS, OFFS);
    }

    QList<Block*> children = parent->childBlocks();
    int index = children.indexOf(this);
    for (int i = index; i < children.size(); i++) { // start with this block
        Block *child = children.at(i);
        child->setPos(nextPos);
        child->setLine(lineNo);

        nextPos = child->computeNextSiblingPos();
        if (child->element->isNewline()) lineNo++;
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
    if (isTextBlock())
        return childrenBoundingRect().adjusted(-OFFS,0,0,0);
    QRectF rect = childrenBoundingRect();
    if (hideButton != 0)
        return rect.adjusted(0, 0, OFFS, OFFS);
    else
        return rect.adjusted(-OFFS, -OFFS, OFFS, OFFS);
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
        updateLayout(line);
        changed = false;
    }

    QRectF rect = boundingRect();
    painter->setPen(pen());
    painter->fillRect(rect, Qt::white);

    painter->fillRect(QRectF(0,0,OFFS,rect.height()), Qt::lightGray);
    painter->drawText(3, 15, QString("%1").arg(line));

    if (isTextBlock()) {
        // myTextItem->paint(painter, option, widget);
    }
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

