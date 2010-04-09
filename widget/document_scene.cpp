#include "document_scene.h"
#include "../analysis/analyzer.h"
#include "../analysis/tree_element.h"
#include "../gui/block.h"


DocumentScene::DocumentScene(Analyzer *analyzer, QObject *parent)
    : QGraphicsScene(parent)
{
    this->analyzer = analyzer;
    mainBlock = 0;
    //    setFocus(Qt::MouseFocusReason);

    insertLine = new QGraphicsLineItem(0, this);
    insertLine->setVisible(false);
    insertLine->setPen(QPen(QBrush(Qt::red), 2));
    insertLine->setZValue(1);

    modified = false;
}

void DocumentScene::adjustSceneRect(QRectF rect)
{
    if (mainBlock) {
        QRectF blockRect = mainBlock->sceneBoundingRect();
        if (blockRect.right() > rect.right())
            rect.setRight(blockRect.right());
        if (blockRect.bottom() > rect.bottom())
            rect.setBottom(blockRect.bottom());
    }
    setSceneRect(rect);
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

    analyzeAll(content);
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

    if (mainBlock != 0)
        out << mainBlock->getElement()->getRoot()->getText();

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
        TreeElement *root = mainBlock->getElement()->getRoot();
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
            QMapIterator<int, Block*> i(Block::lineStarts);
            while (i.hasNext()) {
                i.next();
                str.append(QString("%1").arg(i.key())).append(" - "+i.value()->getElement()->getType()+"\n");
            }
            str.append(QString("Last line: %1").arg(Block::getLastLine()));
        } else if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
            QList<TreeElement*> list = root->getDescendants();
            foreach (TreeElement *el, list) {
                str.append(QString("%1").arg(el->getSpaces()));
                str.append("  "+el->getType());
                if (el->isLineBreaking()) str.append("*");
                str.append("\n");
            }
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
void DocumentScene::toggleOffset()
{
    if (Block::OFFSH != 0) {
        Block::OFFSH = 0;
        Block::OFFSV = 0;
    } else {
        Block::OFFSH = 10;
        Block::OFFSV = 3;
    }
    reanalyze();
}

void DocumentScene::reanalyze()
{
    if (mainBlock == 0) {
        update();
        return;
    }
    QGraphicsItem *item = focusItem();
    QGraphicsTextItem *textItem;
    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();
    Block *block = qgraphicsitem_cast<Block*>(item);

    if (block == 0) {
        analyzeAll(mainBlock->getElement()->getRoot()->getText());
        return;
    }
    TreeElement *analysedEl = analyzer->getAnalysableAncestor(block->getElement());
    block = 0;
    if (analysedEl == 0) {
        analyzeAll(mainBlock->getElement()->getRoot()->getText());
        return;
    }
    block = 0;
    TreeElement *newEl = analyzer->analyzeElement(analysedEl);

    Block *analysedBl;
    do {
        analysedBl = analysedEl->getBlock();
        analysedEl = (*analysedEl)[0];
    } while (analysedBl == 0);

    bool lineBreaking = analysedBl->getElement()->isLineBreaking();
    Block *parentBl = analysedBl->parentBlock();
    Block *nextSib = analysedBl->getNextSibling();

    delete(analysedBl);

    Block *newBlock = new Block(newEl, parentBl, this);
    newBlock->getElement()->setLineBreaking(lineBreaking);
    if (nextSib != 0)
        newBlock->stackBefore(nextSib);
    mainBlock->updateLayout();
    update();
}

void DocumentScene::analyzeAll(QString text)
{
    if (text.isEmpty())
        return;
    if (mainBlock != 0)
        delete(mainBlock);

    mainBlock = new Block(analyzer->analyzeFull(text), 0, this);
    mainBlock->setPos(30,20); //temp
    mainBlock->updateLayout();
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

void DocumentScene::update(const QRectF &rect)
{
    emit requestSize();

    QGraphicsScene::update(rect);
}

void DocumentScene::setHighlightning(const QHash<QString, QPair<QFont, QColor> > &highlightning)
{
    this->highlightning = highlightning;
}

QHash<QString, QPair<QFont, QColor> > DocumentScene::getHighlightning() const
{
    return highlightning;
}

