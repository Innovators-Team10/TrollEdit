#include "document_scene.h"

DocumentScene::DocumentScene(Analyzer *analyzer, QObject *parent)
    : QGraphicsScene(parent)
{
    this->analyzer = analyzer;
    QString sample = "int main() {\nif (true) quack();\nreturn 0;\n}";

    root = analyzer->analyzeFull(sample);
    Block *block = new Block(root, 0, this);
    block->setPos(10,10);

    setFocus(Qt::MouseFocusReason);
    modified = false;
}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MidButton) { // create new block
        Block *parent = blockAt(event->scenePos());
        Block *block = new Block(0, parent, this);
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
    QGraphicsTextItem *textItem = block->textItem();
    if (textItem != 0) {
        QTextCursor cursor = textItem->textCursor();
        cursor.clearSelection();
        //    block->setTextCursor(cursor);
//        if (textItem->toPlainText().isEmpty()) {
//            removeItem(bltextItemock);
//            textItem->deleteLater();
//        }
    }
}

Block* DocumentScene::blockAt(QPointF pos)
{
    return qgraphicsitem_cast<Block*>(itemAt(pos));
}

