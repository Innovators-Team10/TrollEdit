#include "block.h"


static int counter = 0;

Block::Block(QGraphicsScene *parentScene, Block *parentBlock)
    : QGraphicsRectItem(parentBlock)
{
    if (parentBlock == 0) parentScene->addItem(this);
    setRect(0, 0, 100, 50);

    setFlag(QGraphicsItem::ItemIsMovable);
    //setFlag(QGraphicsItem::ItemIsSelectable);
    //setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent);

    // connect(this, SIGNAL(lostFocus(Block*)), scene(), SLOT(lostFocus(Block*)));
    // connect(this, SIGNAL(zChanged()), this, SLOT(writeZ()));
    // connect(this, SIGNAL(parentChanged()), this, prepareGeometryChange());

    folded = false;
    pressed = false;
    //backupText = new QTextDocument();

    hideButton = new HideBlockButton(this);
    hideButton->setPixmap(QPixmap(":/res/Untitled.png"));
    QRectF rect = QGraphicsRectItem::boundingRect();
    hideButton->setPos(rect.topLeft());

    // testing
    //setTextInteractionFlags(Qt::NoTextInteraction);
    id = counter++;
    if (parentBlock!=0) writeToParent();
    //setPlainText(QString("-blok-\nid: %1\n\nz: %2").arg(id).arg(zValue()));

    // setPlainText(QString("toto\nje\nblok s textom\n\n\n\n\n(slon slon slon slon)\nid: %1").arg(id));
    //    setToolTip(QString("-blok-\nid: %1\n\nz: %2").arg(id).arg(zValue()));
    QGraphicsSimpleTextItem *text = new QGraphicsSimpleTextItem(QString(" blok %1").arg(id), this);
    text->setPos(hideButton->boundingRect().topRight());
}

QVariant Block::itemChange(GraphicsItemChange change, const QVariant &value)
{
    //    if (change == QGraphicsItem::ItemChildAddedChange) {
    //        QGraphicsItem *item = value.value<QGraphicsItem*>();
    //        bool a = (type() == item->type());
    //        Block *child = qgraphicsitem_cast<Block*>(item);
    //
    //        if (child != 0) {
    //            child->setPos(10, 10);
    //        }
    //        a = false;
    //    }

    return QGraphicsRectItem::itemChange(change, value);
}

int Block::type() const
{
    return Type;
}

QRectF Block::boundingRect() const
{
    QRectF rect = QGraphicsRectItem::boundingRect();
    rect = rect.united(childrenBoundingRect());
    return rect;
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

    painter->fillRect(rect, Qt::white);
    painter->drawRect(rect);
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

Block *Block::parentBlock()
{
    QGraphicsItem *parent = parentItem();
    if (parent!=0 && parent->type() == type())
        return qgraphicsitem_cast<Block*>(parent);
    else return 0;
}

void Block::focusOutEvent(QFocusEvent *event)
{
    emit lostFocus(this);
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
    }
    QGraphicsRectItem::mouseReleaseEvent(event);
}

void Block::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    //    setRotation(20);
    QGraphicsRectItem::hoverEnterEvent(event);
}
void Block::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    //    setRotation(0);
    QGraphicsRectItem::hoverLeaveEvent(event);
}
void Block::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
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
    //document()->setPlainText(QString("-blok-\nid: %1\n\nz: %2").arg(id).arg(zValue()));
}
void Block::writeToParent()
{
    //document()->setPlainText(QString("-blok-\nid: %1\n\nz: %2\nin").arg(id).arg(zValue()));
    /*
    Block *p = parentBlock();
    QString str;
    if (p!=0) {
        str = p->document()->toPlainText();
        str.append(QString("\nch: %1").arg(id));
        p->document()->setPlainText(str);
    }*/
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

