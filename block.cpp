#include "block.h"


static int counter = 0;

Block::Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0)
        parentScene->addItem(this);
    this->element = element;

    if (element->isLeaf()) {
        text = new QGraphicsTextItem(element->getType(), this);
        if (parentBlock != 0)
            parentBlock->setChanged();
    } else {
        text = 0;
        setToolTip(element->getType());
        if (!element->isImportant()) {
            setPen(QPen(Qt::lightGray));
        }
        foreach (TreeElement *childEl, element->getChildren()) {
            new Block(childEl, this);
        }
    }
    createControls();

    
    setFlag(QGraphicsItem::ItemIsMovable);
    //setFlag(QGraphicsItem::ItemIsSelectable);
    //setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent);

    // connect(this, SIGNAL(lostFocus(Block*)), scene(), SLOT(lostFocus(Block*)));
    // connect(this, SIGNAL(zChanged()), this, SLOT(writeZ()));
    // connect(this, SIGNAL(parentChanged()), this, prepareGeometryChange());

    folded = false;
    pressed = false;
    changed = true;

    id = counter++;
}

void Block::createControls() {
    if (true) { // potom zmenit
        hideButton = new HideBlockButton(this);
        hideButton->setPixmap(QPixmap(":/res/Untitled.png"));
        QRectF rect = QGraphicsRectItem::boundingRect();
        hideButton->setPos(rect.topLeft());
        hideButton->setVisible(false);
        setAcceptHoverEvents(true);
    } else {
        hideButton = 0;
    }
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
        if (child != 0)
            setChanged();
    }

    return QGraphicsRectItem::itemChange(change, value);
}

void Block::setChanged() {
    changed = true;
    Block *parent = parentBlock();
    if (parent != 0)
        parent->setChanged();
    else
        updateLayout();
}

void Block::updateLayout() {
    if (!changed)
        return;
    QList<Block*> blocks = blocklist_cast(childItems());
    QPointF nextPos = QPointF(OFFS, OFFS);
    qreal maxHeight = 0;
    foreach (Block *child, blocks) {
        child->updateLayout();
        child->setPos(nextPos);
        QRectF rect = child->boundingRect();
        rect = mapRectFromItem(child, rect);
        if (rect.bottom() > maxHeight) maxHeight = rect.bottom();

        if (child->element->getType() != "\n")   // toto nefunguje, preco??
            nextPos = rect.topRight() + QPointF(OFFS, 0);//right
        else
            nextPos = QPointF(OFFS, maxHeight + OFFS);//down
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
        return childrenBoundingRect().adjusted(0, 0, OFFS, OFFS);
//    QRectF rect = QGraphicsRectItem::boundingRect();
//    rect = rect.united(childrenBoundingRect());
//    rect.adjust(0, 0, OFFS, OFFS);
//    return rect;
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
    foreach (QGraphicsItem *child, childItems())     // hide/unhide children
        if (child->type() == type())        // blocks only (HideButton is child too)
            child->setVisible(!fold);
    folded = fold;                          // update folded flag
}

bool Block::isFolded()
{
    return folded;
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
            setPos(scenePos());
            setParentItem(0);
        } else {
            // we cannot remove from scene direcly (data is lost in this process)
            // z-value is used to get this item to front
            setZValue(100);
        }
    }
    QGraphicsRectItem::mousePressEvent(event);
}

void Block::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsRectItem::mouseMoveEvent(event);
}

void Block::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Block *target = 0;
        QList<Block*> blocks = blocklist_cast(scene()->items(scenePos()));
        // this should be always true: blocks.first() == this
        if (blocks.size() > 1)
            target = blocks.at(1);  // first block is this!, second is below it
        setPos(mapToItem(target, QPointF()));
        setParentItem(target);
        prepareGeometryChange();    // used to update graphics
        setZValue(0);               // restore z-value
        updateLayout();
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
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

QList<Block*> Block::blocklist_cast(QList<QGraphicsItem*> list) {
    QList<Block*> blocks;
    foreach (QGraphicsItem *item, list) {
        Block *block = qgraphicsitem_cast<Block*>(item);
        if (block != 0)
            blocks << block;
    }
    return blocks;
}

