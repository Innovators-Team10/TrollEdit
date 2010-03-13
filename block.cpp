#include "block.h"


static int counter = 0;

Block::Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0) { // adding directly to scene, no parent blocks
        parentScene->addItem(this);
    }/* else {
        if (element->getParent() == 0) // no parent element, connect to parent block's element
            parentBlock->element->appendChild(element);
    }*/
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
    // connect(this, SIGNAL(zChanged()), this, SLOT(writeZ()));
    // connect(this, SIGNAL(parentChanged()), this, prepareGeometryChange());

    folded = false;
    pressed = false;
    changed = true;

    id = counter++;
}

void Block::createControls()
{
    //    hideButton = new HideBlockButton(this);
    //    hideButton->setPixmap(QPixmap(":/res/Untitled.png"));
    //    QRectF rect = QGraphicsRectItem::boundingRect();
    //    hideButton->setPos(rect.topLeft());
    //    hideButton->setVisible(false);
    //    setAcceptHoverEvents(true);
    hideButton = 0;

    separatorLine = new QGraphicsLineItem(this);
    separatorLine->setVisible(false);
    separatorLine->setPen(QPen(QBrush(), 2));
}

void Block::childAdded(Block *newChild)
{

}

void Block::childRemoved(Block *oldChild)
{

}

Block *Block::parentBlock()
{
    QGraphicsItem *parent = parentItem();
    if (parent!=0 && parent->type() == type())
        return qgraphicsitem_cast<Block*>(parent);
    else return 0;
}

QGraphicsTextItem *Block::textItem()
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
        // remove from parent and add directly to scene
        // item is now on top of everything (that have z-value==0)
        // new parent will be resolved after mouse is released
        if (parentItem() != 0) {
            futureParent = parentBlock();
            setPos(scenePos());
            setParentItem(0);
            futureParent->element->deleteBranchTo(element);
        } else {
            futureParent = 0;
            // we cannot remove from scene direcly (data is lost in this process)
            // z-value is used to get this item to front
            setZValue(100);
        }        
        futureSibling = 0;
        //mouseMoveEvent(event);  // to update futureSibling and draw separator line immediatelly
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // SEM

    }
    QGraphicsRectItem::mouseMoveEvent(event);
}

void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
//ODTIALTO
        futureParent = 0;
        QList<Block*> blocks = blocklist_cast(scene()->items(scenePos()));

        // list of blocks under this block's top left edge (including this one!)
        foreach (Block *block, blocks) { // find next different block
            if (block != this) {
                futureParent = block;
                break;
            }
        }

        if (futureParent != 0 && futureParent->element->isLeaf()) {// leaf test - cannot add child to leaf
            futureParent = futureParent->parentBlock();
        }

        if (futureParent != 0) {
            futureSibling = futureParent->findNextChildAt(mapToItem(futureParent, event->pos()));
        } else {
            futureSibling = 0;
        }
// POTIALTO - presunut hore


        setZValue(0);               // restore z-value

        if (futureParent != 0) {
            setParentItem(futureParent);
            int index;
            if (futureSibling != 0) {
                stackBefore(futureSibling);
                setChanged();
                index = futureParent->element->indexOfDescendant(futureSibling->element);
            } else {
                index = futureParent->element->childCount();
            }
            futureParent->element->insertChild(index, element);
            futureParent->prepareGeometryChange();    // used to update graphics
        }
        futureParent = 0;
        futureSibling = 0;
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}

Block* Block::findNextChildAt(QPointF pos)
{
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

void Block::drawInsertLine(Block* nextBlock)
{
    if (nextBlock != 0) {
        QRectF rect = mapRectFromItem(nextBlock, nextBlock->boundingRect());
        rect.translate(-OFFS/2, 0);
        separatorLine->setLine(QLineF(rect.topLeft(), rect.bottomLeft()));
    } else {
        //todo
    }

    separatorLine->setVisible(true);
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

void Block::writeZ()
{

}
void Block::writeToParent()
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
    else
        //        if (element->isImportant())
        return childrenBoundingRect().adjusted(-OFFS, -OFFS, OFFS, OFFS);
    //        else
    //            return childrenBoundingRect();
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
    painter->drawRect(rect);
    if (text != 0)
        text->paint(painter, option, widget);
    //QGraphicsRectItem::paint(painter, option, widget);    // kresli original rect
    scene()->update(scene()->sceneRect());
}

void Block::setFolded(bool fold)
{
    if (fold == folded) return; // do nothing
    if (fold) {
        ;// todo fold
    } else {
        ;//todo unfold
    }

    foreach (Block *child, blocklist_cast(childItems()))     // hide/unhide children
        child->setVisible(!fold);

    folded = fold;                          // update folded flag
}

bool Block::isFolded()
{
    return folded;
}

QList<Block*> Block::blocklist_cast(QList<QGraphicsItem*> list) {
    QList<Block*> blocks;
    foreach (QGraphicsItem *item, list) {
        Block *block = qgraphicsitem_cast<Block*>(item);
        if (block != 0)
            blocks << block;
    }
    return blocks;
}

