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

    // connect(this, SIGNAL(lostFocus(Block*)), scene(), SLOT(lostFocus(Block*)));

    folded = false;
    pressed = false;

    line = 0;

    id = counter++;
    if (parentBlock == 0)
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

void Block::childAdded(Block *newChild)
{

}

void Block::childRemoved(Block *oldChild)
{
}

void Block::setParentItem (QGraphicsItem *parent)
{
    if (parent == this) {
        parent = 0;//debug - toto nemoze nastavat
    }
    // remove from old parent element
    Block *oldParent = parentBlock();
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
    Block *newParent = qgraphicsitem_cast<Block*>(parent);
    if (newParent != 0) {
        TreeElement *newParentEl = newParent->element;
        newParentEl->appendChild(element);
    }
    // remove from old parent block & append to new parent block
    QGraphicsRectItem::setParentItem(parent);
}

void Block::stackBefore (const QGraphicsItem *sibling)
{
    const Block *nextSibling = qgraphicsitem_cast<const Block*>(sibling);
    if (nextSibling != 0) {
        Block *parent = parentBlock();
        if (parent != 0 && nextSibling->parentBlock() == parent) {
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

Block *Block::getNextSibling() const
{
    Block *next = 0;
    Block *parent = parentBlock();
    if (parent != 0) {
        QList<Block*> children = parent->childBlocks();
        int index = children.indexOf(const_cast<Block*>(this));
        if (index < children.size()-1)
            next = children.at(index+1);
    }
    return next;
}
Block *Block::getPreviousSibling() const
{
    Block *prev = 0;
    Block *parent = parentBlock();
    if (parent != 0) {
        QList<Block*> children = parent->childBlocks();
        int index = children.indexOf(const_cast<Block*>(this));
        if (index > 0)
            prev = children.at(index-1);
    }
    return prev;
}
int Block::numberOfLines() const
{
    if (isTextBlock()) {
        return 1;   // todo: vynimka pre dokumentacne bloky
    } else {
        return childBlocks().last()->line - this->line + 1;
    }
}
int Block::getLineAfter() const
{
    if (element->getType().contains("\n"))
        return line + 1;
    if (isTextBlock())
        return line;
    return childBlocks().last()->getLineAfter();
}
int Block::getLineYPos(int line) const
{
    foreach (Block *child, childBlocks()) {
        if (child->line == line)
            return child->pos().y();
    }
    return 0;
}

DocumentScene *Block::docScene() const
{
    return (DocumentScene*)QGraphicsItem::scene();
}

Block *Block::parentBlock() const
{
    QGraphicsItem *parent = parentItem();
    if (parent!=0 && parent->type() == type())
        return qgraphicsitem_cast<Block*>(parent);
    else
        return 0;
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
    if (change == QGraphicsItem::ItemChildAddedChange
        || change == QGraphicsItem::ItemChildRemovedChange) {
        QGraphicsItem *item = value.value<QGraphicsItem*>();
        Block *child = qgraphicsitem_cast<Block*>(item);
        if (child != 0) {
            //updateChildrenPosAfter(child, -1);
            setChanged();
        }
    }
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
    Block *parent = parentBlock();
    if (parent != 0) {
        parentBlock()->updateChildrenPosInLine(line);
    }

}
void Block::addNewLineAfterThis()
{
    Block *parent = parentBlock();
    if (parent != 0) {
        Block *newBlock = new Block(new TreeElement(" "), 0, scene());
        newBlock->setParentItem(parent);
        newBlock->stackBefore(getNextSibling());

        Block *newLine = new Block(new TreeElement("\n"), 0, scene());
        newLine->setParentItem(parentBlock());
        newLine->stackBefore(newBlock);

//        parentBlock()->updateChildrenPosAfter(this, -1);//
        setChanged();

        newBlock->textItem()->setFocus();
        scene()->update();
    }

}
void Block::moveCursorLR(int key)
{
    Block *target = 0;
    Block *parent = parentBlock();

    if (parent != 0) {
        if (key == Qt::Key_Left) {
            target = getPreviousSibling();
        } else if (key == Qt::Key_Right) {
            target = getNextSibling();
        }
        if (target == 0) {
            parentBlock()->moveCursorLR(key);
            return;
        }
    } else {
        target = this;
    }
    if (key == Qt::Key_Left) {
        while (!target->isTextBlock()) {
            target = target->childBlocks().last();
        }
        target->textItem()->setTextCursorPosition(-1);
    } else if (key == Qt::Key_Right) {
        while (!target->isTextBlock()) {
            target = target->childBlocks().first();
        }
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
        Block *oldParent = parentBlock();
        // remove from parent and add directly to scene
        // item is now on top of everything (that have z-value==0)
        // new parent will be resolved after mouse is released
        if (oldParent != 0) {
            setPos(scenePos());
            setParentItem(0);
        }
        pressed = true;
        setZValue(100);
        QPointF curPos = pos();
        mouseMoveEvent(event);  // updates futureParent/Sibling and draw separator line immediatelly
        setPos(curPos);
    }
    QGraphicsRectItem::mousePressEvent(event);
    scene()->update();
}

void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!pressed) return;

    futureParent = 0;
    // target is under left top corner of moved block OR target is under cursor
    QList<Block*> blocks = blocklist_cast(scene()->items(/*event->*/scenePos()));
    foreach (Block *block, blocks) { // find next different block
        if (block != this) {
            futureParent = block;
            break;
        }
    }
    if (futureParent != 0 && futureParent->element->isLeaf()) {// leaf test - cannot add child to leaf
        futureParent = futureParent->parentBlock();
        if (futureParent == this) {
            futureParent = 0;//debug - toto nemoze nastavat
        }
    }
    if (futureParent != 0) {
        futureSibling = futureParent->findNextChildAt(futureParent->mapFromScene(/*event->*/scenePos()));
        docScene()->showInsertLine(futureParent->getInsertLineAt(futureSibling));
    } else {
        futureSibling = 0;
        docScene()->hideInsertLine();
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
            setChanged();
        }
        futureParent = 0;
        futureSibling = 0;
        docScene()->hideInsertLine();
    }
    pressed = false;
    QGraphicsRectItem::mouseReleaseEvent(event);
    scene()->update();
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
    Block *parent = parentBlock();
    if (parent != 0)
        parent->setChanged();
    else
        changed = true;
}

//static int H = 50;

int Block::updateLayout(int lineNo) // parent to child updater
        // used to update everything from root up, updates line numbers
{
    line = lineNo;
    QList<Block*> blocks = childBlocks();
    QPointF nextPos = QPointF(OFFS, OFFS);
    //    int line = 0;
    qreal maxHeight = 0;

    foreach (Block *child, blocks) {
        child->line = lineNo;
        lineNo = child->updateLayout(lineNo);
        child->setPos(nextPos);

        QSizeF size = child->boundingRect().size();
        if (size.height() > maxHeight) maxHeight = size.height();

        if (child->element->getType().contains("\n")) {// temporary
            child->textItem()->setTextInteractionFlags(Qt::NoTextInteraction);
            nextPos.rx() = OFFS;
            nextPos.ry() += maxHeight + OFFS;   // down
            lineNo++;
            maxHeight = 0;
        } else {
            nextPos.rx() += size.width() + OFFS;// right
        }
//        nextPos.setY((lineNo-line) * (H+OFFS));
    }
    return lineNo;
}
// zmrseny algoritmus... DOKONCIT
void Block::updateChildrenPosAfter(Block *child, int lineNo) // child to parent updater
        // used to update everything changed child down, updates line numbers
{
    QList<Block*> blocks = childBlocks();
    int index = blocks.indexOf(child);
    if (lineNo < 0) {
        if (index > 0)
            lineNo = child->getPreviousSibling()->getLineAfter();
        else
            lineNo = this->line;
        child->line = lineNo;
    }
//    if (index < 0) index = -1;

    Block *firstBlock = blocks.at(index+1); // first block to update
    if (firstBlock != 0) {
        QPointF nextPos = mapFromItem(child, child->boundingRect().topRight()) + QPointF(OFFS, 0);
        qreal maxHeight = 0;

        for (int i = 0; i < index+1; i++) { // check height of previous blocks
            Block *child = blocks.at(i);
            qreal height = child->boundingRect().height();
            if (child->line == lineNo)      // only current line is relevant
                if (height > maxHeight)
                    maxHeight = height;
        }
        for (int i = index+1; i < blocks.size(); i++) { // start updating
            Block *child = blocks.at(i);
            child->line = lineNo;
            child->setPos(nextPos);

            QSizeF size = child->boundingRect().size();
            if (size.height() > maxHeight) maxHeight = size.height();

            if (child->element->getType().contains("\n")) {// temporary
                nextPos.rx() = OFFS;
                nextPos.ry() += maxHeight + OFFS;   // down
                lineNo++;
                maxHeight = 0;
            } else {
                nextPos.rx() += size.width() + OFFS;// right
            }
        }
    } else {
        if (child->element->getType().contains("\n")) // temporary
            lineNo++;
    }

    Block *parent = parentBlock();
    if (parent != 0) // this block is updated, update parent accordingly
        parent->updateChildrenPosAfter(this, lineNo);
}

void Block::updateChildrenPosInLine(int lineNo) { // child to parent updater
    // used to update everything from changed child down, updates only 1 line

    QList<Block*> blocks = childBlocks();
    qreal xPos = OFFS;

    foreach (Block *child, blocks) {
        if (child->line < lineNo)
            continue;
        if (child->line > lineNo)
            break;
        child->setX(xPos);
        xPos = xPos + child->boundingRect().width() + OFFS;
    }
    Block *parent = parentBlock();
    if (parent != 0 && this->line == lineNo) // this block is updated, update parent accordingly
        parent->updateChildrenPosInLine(this->line);
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

