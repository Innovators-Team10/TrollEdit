#include "block.h"
#include "tree_element.h"
#include "hide_block_button.h"
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
        text = new QGraphicsTextItem(element->getType(), this);
        if (parentBlock != 0)
            parentBlock->setChanged();
    } else {
        text = 0;
        setToolTip(element->getType());
        foreach (TreeElement *childEl, element->getChildren()) {
            new Block(childEl, this);
        }
    }
    createControls();
    
    setFlag(QGraphicsItem::ItemIsMovable);
    //setFlag(QGraphicsItem::ItemIsSelectable);

    // connect(this, SIGNAL(lostFocus(Block*)), scene(), SLOT(lostFocus(Block*)));

    folded = false;
    pressed = false;
    changed = true;

    id = counter++;
}

Block::~Block()
{
}

void Block::createControls()
{
//    if (element->isMultiLine()) {
//        hideButton = new HideBlockButton(this);
//        hideButton->setPixmap(QPixmap(":/res/Untitled.png"));
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
            oldParent->text = new QGraphicsTextItem(oldParentEl->getType(), oldParent);
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

DocumentScene *Block::scene() const
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

QGraphicsTextItem *Block::textItem() const
{
    return text;
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemChildAddedChange
        || change == QGraphicsItem::ItemChildRemovedChange) {
        QGraphicsItem *item = value.value<QGraphicsItem*>();
        Block *child = qgraphicsitem_cast<Block*>(item);
        if (child != 0) {
            setChanged();
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

void Block::focusOutEvent(QFocusEvent *event)
{
    QGraphicsRectItem::focusOutEvent(event);
}

void Block::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
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
    scene()->update(scene()->sceneRect());
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
        scene()->showInsertLine(futureParent->getInsertLineAt(futureSibling));
    } else {
        futureSibling = 0;
        scene()->hideInsertLine();
    }
    QGraphicsRectItem::mouseMoveEvent(event);
}

void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
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
        scene()->hideInsertLine();
        pressed = false;
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
    scene()->update(scene()->sceneRect());
}

Block* Block::findNextChildAt(QPointF pos) const
{// note: distance is computed from pot left corner of block's bounding rect
    QList<Block*> blocks = blocklist_cast(childItems());
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
        Block *lastChild = blocklist_cast(childItems()).last();// must have at least 1 child
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
    changed = true;
    Block *parent = parentBlock();
    if (parent != 0)
        parent->setChanged();
    else
        updateLayout();
}

void Block::updateLayout()
{
    if (!changed)
        return;

//    if (element->isMultiLine())
//        setAcceptHoverEvents(true);

    QList<Block*> blocks = blocklist_cast(childItems());
    QPointF nextPos = QPointF(OFFS, OFFS);
    qreal maxHeight = 0;
    foreach (Block *child, blocks) {
        child->updateLayout();
        child->setPos(nextPos);
        QRectF rect = child->boundingRect();
        rect = mapRectFromItem(child, rect);
        if (rect.bottom() > maxHeight) maxHeight = rect.bottom();

        if (child->element->getType().contains("\n"))// temporary
            nextPos = QPointF(OFFS, maxHeight + OFFS);//down
        else
            nextPos = rect.topRight() + QPointF(OFFS, 0);//right
    }
    changed = false;
}

int Block::type() const
{
    return Type;
}

QRectF Block::boundingRect() const
{
    if (text != 0)
        return text->boundingRect(); 
    QRectF rect = childrenBoundingRect();
    if (hideButton != 0)
        return rect.adjusted(0, 0, OFFS, OFFS);
    else
        return rect.adjusted(-OFFS, -OFFS, OFFS, OFFS);
}

QPainterPath Block::shape() const   // shape is used for collision detection
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

void Block::paint(QPainter *painter,
                  const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    QRectF rect = boundingRect();
    painter->setPen(pen());
    painter->fillRect(rect, Qt::white);
    if (text != 0) {
        text->paint(painter, option, widget);
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

    bool firstLine = true;
    foreach (Block *child, blocklist_cast(childItems())) {    // hide/unhide children
        if (firstLine) {
            if (child->element->getType().contains("\n")) // todo: change to constant
                firstLine = false;
        } else {
            child->setVisible(!fold);
        }
    }
    folded = fold;                          // update folded flag
//    setChanged();
}

bool Block::isFolded() const
{
    return folded;
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

