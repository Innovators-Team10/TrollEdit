#include "document_scene.h"
#include "analyzer.h"
#include "tree_element.h"
#include "block.h"


DocumentScene::DocumentScene(Analyzer *analyzer, QObject *parent)
    : QGraphicsScene(parent)
{
    this->analyzer = analyzer;
//    QString sample = "int   p  = 9;";
    QString sample = "int main() {//comment\nif (isDuck)\n\tquack();\nreturn 0;\n}";

    root = analyzer->analyzeFull(sample);
    mainBlock = new Block(root, 0, this);
    mainBlock->setPos(50,50);

    setFocus(Qt::MouseFocusReason);
    modified = false;

    insertLine = new QGraphicsLineItem(0, this);
    insertLine->setVisible(false);
    insertLine->setPen(QPen(QBrush(Qt::red), 2));
    insertLine->setZValue(1);
}

void DocumentScene::loadFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString content = in.readAll();

    // we have things from constructor we need to ged rid of
    delete(mainBlock);
    delete(root);

    // these three lines almost same in constructor
    root = analyzer->analyzeFull(content);
    mainBlock = new Block(root, 0, this);
    mainBlock->setPos(50,50);

    // not needed later, but now we are painting all blocks
    update();

    QApplication::restoreOverrideCursor();
}

void DocumentScene::saveFile(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream out(&file);

    if (root)
        out << root->getText();

    QApplication::restoreOverrideCursor();

    modified = false;
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
        Block *parent = blockAt(event->scenePos());
        if (parent != 0 && parent->isTextBlock()) { // is leaf
            parent = parent->getParentBlock();
        }
        Block *block = new Block(new TreeElement("TROLL!"), parent, this);
        if (parent != 0) {
            Block * next = parent->findNextChildAt(parent->mapFromScene(event->scenePos()));
            block->stackBefore(next);
            block->updatePos();
            block->setFocus();
        } else {
            block->setPos(event->scenePos());
        }
        update();
    }
    if (event->button() == Qt::LeftButton) {

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
}

void DocumentScene::reanalyze() {
    mainBlock->setChanged();
    update();
}

Block* DocumentScene::blockAt(QPointF pos) const
{
    QGraphicsItem *item = itemAt(pos);
    if (item == 0)
        return 0;
    QGraphicsTextItem *textItem;    // leaf item would be covered by its QGraphicsTextItem child
    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();
    return qgraphicsitem_cast<Block*>(item);
}

