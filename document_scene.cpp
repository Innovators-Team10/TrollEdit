#include "document_scene.h"

DocumentScene::DocumentScene(QObject *parent)
    : QGraphicsScene(parent)
{
    Block *block = new Block(this);
    block->setPos(100,100);
    setFocus(Qt::MouseFocusReason);
    modified = false;
    root = new TreeElement("root");
}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MidButton) { // create new block
        Block *parent = blockAt(event->scenePos());
        Block *block = new Block(this, parent);
        if (parent == 0) {
            block->setPos(event->scenePos());
        } else {
            // parent sets position automatically
        }
    }
    // save clicked block
    if (event->button() == Qt::LeftButton) {
        clickedBlock = blockAt(event->scenePos());
        //if (clickedBlock != 0) {
            //QGraphicsItem *text = addText(QString("clicked on %1").arg(clickedBlock->id));
            //text->setPos(clickedBlock->scenePos() - QPointF(0,20));
        //}
    }
    QGraphicsScene::mousePressEvent(event);
}

void DocumentScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
}

void DocumentScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void DocumentScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
    //
}

void DocumentScene::lostFocus(Block *block)
{
//    QTextCursor cursor = block->textCursor();
//    cursor.clearSelection();
//    block->setTextCursor(cursor);
//
//    if (block->toPlainText().isEmpty()) {
//        removeItem(block);
//        block->deleteLater();
//    }
}

Block* DocumentScene::blockAt(QPointF pos)
{
    return qgraphicsitem_cast<Block*>(itemAt(pos));
}

