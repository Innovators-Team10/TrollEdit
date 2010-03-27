#include "document_scene.h"
#include "../analysis/analyzer.h"
#include "../analysis/tree_element.h"
#include "../gui/block.h"


DocumentScene::DocumentScene(Analyzer *analyzer, QObject *parent)
    : QGraphicsScene(parent)
{
    this->analyzer = analyzer;
    mainBlock = 0;
    root = 0;
    //    setFocus(Qt::MouseFocusReason);

    insertLine = new QGraphicsLineItem(0, this);
    insertLine->setVisible(false);
    insertLine->setPen(QPen(QBrush(Qt::red), 2));
    insertLine->setZValue(1);

    modified = false;
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
    if (mainBlock != 0)
        delete(mainBlock);
    if (root != 0)
        delete(root);

    // these three lines almost same in constructor
    root = analyzer->analyzeFull(content);
    mainBlock = new Block(root, 0, this);
    mainBlock->setPos(30,30);

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
    }
    if (event->button() == Qt::LeftButton) {
        Block *parent = blockAt(event->scenePos());
        if (parent == 0) {
//            Block *block = new Block(new TreeElement(" ", false, true), 0, this);
//            block->setFocus();
//            block->setPos(event->scenePos());
        }
        update();
    }
    if (event->button() == Qt::RightButton) { // AST testing
        QString str = "";
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
            QList<TreeElement*> list = root->getDescendants();
            foreach (TreeElement *el, list)
                str.append(el->getType()).append("\n");
            str.append(QString("%1").arg(Block::getLastLine()));
        } else {
            str = root->getText();
        }
        QGraphicsItem *text = addText(str);
        text->setPos(event->scenePos());
        text->setZValue(-1);
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
    QGraphicsItem *item = focusItem();
    QGraphicsTextItem *textItem;
    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();
//    Block *block = qgraphicsitem_cast<Block*>(item);

//    if (block == 0) {
        QString text = root->getText();
//        mainBlock->deleteLater();
        delete(mainBlock);
        delete(root);
        root = analyzer->analyzeFull(text);
        mainBlock = new Block(root, 0, this);
        mainBlock->setPos(30,30);
//    } else {
//        return;
//    }

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

