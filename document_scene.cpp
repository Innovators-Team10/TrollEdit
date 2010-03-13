#include "document_scene.h"

DocumentScene::DocumentScene(Analyzer *analyzer, QObject *parent)
    : QGraphicsScene(parent)
{
    this->analyzer = analyzer;
//    QString sample = "int   p  = 9;";
    QString sample = "int main() {//aha\n if (true) quack();\nreturn 0;\n}";

    root = analyzer->analyzeFull(sample);
    Block *block = new Block(root, 0, this);
    block->setPos(50,50);

    setFocus(Qt::MouseFocusReason);
    modified = false;


    insertLine = new QGraphicsLineItem(0, this);
    insertLine->setVisible(false);
    insertLine->setPen(QPen(QBrush(Qt::red), 2));
    insertLine->setZValue(1);
}

void DocumentScene::showInsertLine(QLineF line) {
    insertLine->setLine(line);
    insertLine->setVisible(true);
}

void DocumentScene::hideInsertLine() {
    insertLine->setVisible(false);
}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::MidButton) { // create new block
//        Block *parent = blockAt(event->scenePos());
//        if (parent != 0 && parent->textItem() != 0) { // is leaf, todo: better function
//            parent = parent->parentBlock();             // new blocks are only appended for now
//        }
//        Block *block = new Block(new TreeElement("TROLL!"), parent, this);
//        if (parent == 0) {
//            block->setPos(event->scenePos());
//        }
    }
    if (event->button() == Qt::LeftButton) {
        //
    }
    if (event->button() == Qt::RightButton) { // AST testing
        QGraphicsItem *text = addText(root->getText());
        text->setPos(event->scenePos());
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
    QGraphicsItem *item = itemAt(pos);
    if (item == 0)
        return 0;
    QGraphicsTextItem *textItem;    // leaf item would be covered by its QGraphicsTextItem child
    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();
    return qgraphicsitem_cast<Block*>(item);
}

