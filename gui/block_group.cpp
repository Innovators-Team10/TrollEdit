#include "block_group.h"
#include "block.h"
#include "doc_block.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../analysis/analyzer.h"
#include "../widget/document_scene.h"

const QPointF BlockGroup::OFFSET_IN = QPointF(5, 5);
const QPointF BlockGroup::OFFSET_OUT = QPointF(8, 0);
const QPointF BlockGroup::NO_OFFSET = QPointF(0, 0);

BlockGroup::BlockGroup(QString text, Analyzer* analyzer, DocumentScene *scene)
    : QGraphicsRectItem(0, scene)
{
    this->analyzer = analyzer;
    this->docScene = scene;

    // set flag
    root = 0;
    lastLine = -1;
    selected = 0;
    lastXPos = -1;
    foldableBlocks.clear();
    computeTextSize();
//    setFlag(QGraphicsItem::ItemIsMovable);

    analyzeAll(text);
}

BlockGroup::~BlockGroup()
{
}

void BlockGroup::computeTextSize()
{
    TAB_LENGTH = analyzer->TAB.length();
    Block *temp = new Block(new TreeElement("temp"), 0, this);
    QFontMetricsF *fm = new QFontMetricsF(temp->textItem()->font());
    CHAR_WIDTH = fm->width(' ');
    CHAR_HEIGHT = temp->textItem()->boundingRect().height();
    delete temp;
}

Block *BlockGroup::getBlockIn(int line) const
{
    if (lineStarts.size() > line)
        return lineStarts[line];
    else
        return lineStarts.last();
}
void BlockGroup::setBlockIn(Block *block, int line)
{
    if (lineStarts.size() > line) {
        lineStarts[line] = block;
    } else {
        while (lineStarts.size() < line) {
            lineStarts << 0;
            qWarning("Line skipped!");
        }
        lineStarts << block;
    }
    lastLine = line;
}

bool BlockGroup::addFoldable(Block *block)
{
    bool able = true;
    foldableBlocks.remove(block);
    foreach (Block *bl, foldableBlocks) {
        if (bl->getLine() == block->getLine()) {
            if (bl->getElement()->getAncestors().contains(block->getElement()))
                foldableBlocks.remove(bl); // if block is ancestor, throw out bl
            else
                able = false;
            break;
        }
    }
    if (able)
        foldableBlocks << block;

    return able;
}

void BlockGroup::removeFoldable(Block *block)
{
    foldableBlocks.remove(block);
}

DocBlock *BlockGroup::addDocBlock(QPointF pos)
{
    DocBlock *block = new DocBlock(pos, this);
    docBlocks << block;
    return block;
}

Block *BlockGroup::blockAt(QPointF scenePos) const
{
    QGraphicsItem *item = docScene->itemAt(scenePos);
    if (item == 0)
        return 0;
    QGraphicsTextItem *textItem;    // leaf item would be covered by its QGraphicsTextItem child
    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();
    return qgraphicsitem_cast<Block*>(item);
}

void BlockGroup::selectBlock(Block *block)
{
    if (block == 0) {
        deselect();
        return;
    }
    if (!block->getElement()->isSelectable()) {
        selectBlock(block->parentBlock());
        return;
    }
    Block *commonAncestor = qgraphicsitem_cast<Block*>(block->commonAncestorItem(selected));

    if (selected != 0) {
        selected->setShowing(false, commonAncestor);
        Block * oldSelected = selected;
        selected = 0;
        oldSelected->updatePen();
    }

    selected = block;
    selected->setShowing(true);
    selected->updatePen();
    updateAll();
}

void BlockGroup::deselect()
{
    if (selected != 0) {
        selected->setShowing(false);
        Block * oldSelected = selected;
        selected = 0;
        oldSelected->updatePen();
        updateAll();
    }
}

/* **** slots called by signals form TextItem **** */
void BlockGroup::keyPressed(QKeyEvent* event)
{
    if (event->key() != Qt::Key_Up && event->key() != Qt::Key_Down)
        lastXPos = -1;
}

void BlockGroup::splitLine(Block *block, int cursorPos)
{
    if (block->parentBlock() == 0) return;
    Block *temp;

    // check what block should be splitted
    if (cursorPos == 0) {
        temp = block->getPrev();             // split previous block
        if (temp->parentBlock() != 0) {        // it is not very first block
            cursorPos = -1;
            block = temp;
        }
    } else if (cursorPos == -1 && block->getNextSibling() == 0) {   // -1 means end of text
        temp = block->getAncestorWhereLast();// split ancestor
        if (temp->parentBlock() != 0) {        // it is not very last block
            block = temp;
        }
    }

    // split this block
    QString text = "";
    bool alreadyBreaking = !block->getElement()->setLineBreaking(true);
    if (cursorPos >= 0) {   // leave some text in original block
        text = block->textItem()->toPlainText();
//        if (cursorPos == 0)
//            block->textItem()->document()->clear();
//        else
            block->textItem()->setPlainText(text.left(cursorPos));
        text.remove(0,cursorPos);
    }
    Block *next = block->getNext();
    if (next->parentBlock() == 0) next = 0;        // block is not very last block

    // create new block (either with text or with newline)
    if (!text.isEmpty() || alreadyBreaking || next == 0) {
        Block *newBlock = new Block(new TreeElement(text, 0, 0, alreadyBreaking),
                                    block->parentBlock());
        newBlock->stackBeforeBlock(next);
        block->getElement()->setLineBreaking(false);
        newBlock->updatePos(true);
        block->getElement()->setLineBreaking(true);
        newBlock->textItem()->setTextCursorPosition(0);
    } else {
        next->getElement()->setSpaces(0);
        next->getFirstLeaf()->textItem()->setTextCursorPosition(0);
    }
    updateAll();//updateAfter(true);
}

void BlockGroup::eraseChar(Block *block, int key)
{
    Block *target = 0;
    if (key == Qt::Key_Backspace) {             // move to previous block
        target = block->getAncestorWhereFirst();
        if (target->getElement()->getSpaces() > 0) {
            target->getElement()->addSpaces(-1);
            updateAll(false);//updateAfter(true);
        } else {
            target = block->getPrev(true);
            if (target->getLine() < block->getLine()) {          // jumped to previous line
                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0) {
                    target = target->parentBlock();
                }
                target->getElement()->setLineBreaking(false);
                //                target->updateAfter();
                updateAll();
            } else if (target->getLine() > block->getLine()) {   // jumped to the end of file
                return;
            } else {                            // on same line
                target->textItem()->removeCharAt(-1);
            }
        }
    } else if (key == Qt::Key_Delete) {     // move to next block
        target = block->getNext();
        if (target->getElement()->getSpaces() > 0) {
            target->getElement()->addSpaces(-1);
            updateAll(false);//updateAfter(true);
        } else {
            target = block->getNext(true);
            if (target->getLine() > block->getLine()) {          // jumped to next line
                target = block;
                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0) {
                    target = target->parentBlock();
                }
                target->getElement()->setLineBreaking(false);
                //                target->updateAfter();
                updateAll();
            } else if (target->getLine() < block->getLine()) {   // jumped to the beginning of file
                return;
            } else {                            // on same line
                target->textItem()->removeCharAt(0);
            }
        }
    }
}

void BlockGroup::moveFrom(Block *start, int key, int cursorPos)
{
    switch (key) {
    case Qt::Key_Up :
        moveCursorUpDown(start, true, cursorPos);
        break;
    case Qt::Key_Down :
        moveCursorUpDown(start, false, cursorPos);
        break;
    case Qt::Key_Right :
        moveCursorLeftRight(start, false);
        break;
    case Qt::Key_Left :
        moveCursorLeftRight(start, true);
        break;
    }
}

void BlockGroup::moveCursorLeftRight(Block *start, bool moveLeft)
{
    Block *target = 0;
    int position;
    if (moveLeft) {         // move to previous block
        target = start->getPrev(true);
        position = -2;
        if (target->getLine() != start->getLine() ||
            start->getAncestorWhereFirst()->getElement()->getSpaces() > 0)
            position = -1;
    } else {                // move to next block
        target = start->getNext(true);
        position = 1;
        if (target->getLine() != start->getLine() ||
            target->getAncestorWhereFirst()->getElement()->getSpaces() > 0)
            position = 0;
    }
    target->textItem()->setTextCursorPosition(position);
    lastXPos = -1;
    selectBlock(target);
}

void BlockGroup:: moveCursorUpDown(Block *start, bool moveUp, int from)
{
    int y;
    int line = start->getLine();
    if (moveUp) {            // move up
        if (line == 0)
            y = lastLine;
        else
            y = line - 1;
    } else {                 // move down
        if (line == lastLine)
            y = 0;
        else
            y = line + 1;
    }

    QPointF pos(0,0);
    Block *firstInY = getBlockIn(y)->getFirstLeaf();

    pos.setY(firstInY->textItem()->scenePos().y() + CHAR_HEIGHT/2);

    if (lastXPos < 0) {
        pos.setX(start->textItem()->scenePos().x() + from * CHAR_WIDTH);
        lastXPos = pos.x();
    } else {
        pos.setX(lastXPos);
    }

    Block *target = blockAt(pos);
    target->addTextCursorAt(target->mapFromScene(pos));
    selectBlock(target);
}

QRectF BlockGroup::boundingRect() const
{
    return childrenBoundingRect();
//    return QGraphicsRectItem::boundingRect();
}

void BlockGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    painter->drawRect(boundingRect());
}

void BlockGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);
}

void BlockGroup::reanalyze()
{
    if (root == 0) {
        return;
    }
    if (selected != 0 && !reanalyze(selected, QPoint(0, selected->getLine())))
        analyzeAll(root->getElement()->getText());
}

bool BlockGroup::reanalyze(Block *block, QPoint cursorPos)
{
    if (block == 0) return false;

    TreeElement *analysedEl = analyzer->getAnalysableAncestor(block->getElement());

    if (analysedEl == 0) {
        return false;
    }
    TreeElement *newEl = analyzer->analyzeElement(analysedEl);

    Block *analysedBl;
    do {
        analysedBl = analysedEl->getBlock();
        analysedEl = (*analysedEl)[0];
    } while (analysedBl == 0);

    bool lineBreaking = analysedBl->getElement()->isLineBreaking();
    Block *parentBl = analysedBl->parentBlock();
    Block *nextSib = analysedBl->getNextSibling();

    analysedBl->setParentBlock(0);
    analysedBl->deleteLater();

    Block *newBlock = new Block(newEl, parentBl);
    newBlock->getElement()->setLineBreaking(lineBreaking);
    if (nextSib != 0)
        newBlock->stackBeforeBlock(nextSib);

    foldableBlocks.clear();
    updateAll(false);

    Block *lineBl = getBlockIn(cursorPos.y())->getFirstLeaf();
    lineBl->textItem()->setTextCursorPosition(0);
    selectBlock(lineBl);
    return true;
}

bool BlockGroup::analyzeAll(QString text)
{
    if (text.isEmpty())
        return false;
    if (root != 0)
        delete root;
    foldableBlocks.clear();

    root = new Block(analyzer->analyzeFull(text), 0, this);
    root->setPos(30, 20);
    foldableBlocks.clear();
    updateAll(false);
    selectBlock(root);

    docScene->update();
    return true;
}

QString BlockGroup::toText() const
{
    return root->getElement()->getText();
}

// helpers
QList<Block*> BlockGroup::blocklist_cast(QList<QGraphicsItem*> list)
{
    QList<Block*> blocks;
    foreach (QGraphicsItem *item, list) {
        Block *block = qgraphicsitem_cast<Block*>(item);
        if (block != 0)
            blocks << block;
    }
    return blocks;
}

void BlockGroup::updateAll(bool animate) // remove this method when another updater is finished
{
    root->updateBlock(animate);
    foreach (DocBlock *block, docBlocks)
        block->updateBlock(animate);
//    docScene->update();
}

void BlockGroup::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_M :
            reanalyze();
            break;
        }
    }
    QGraphicsRectItem::keyPressEvent(event);
}
