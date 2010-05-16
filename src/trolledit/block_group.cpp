#include "block_group.h"
#include "block.h"
#include "doc_block.h"
#include "text_item.h"
#include "tree_element.h"
#include "analyzer.h"
#include "document_scene.h"

const QString BlockGroup::BLOCK_MIME = "block_data";
const QPointF BlockGroup::OFFSET_IN_TL = QPointF(6, 0); // inner offset, left and top
const QPointF BlockGroup::OFFSET_IN_BR = QPointF(6, 0); // inner offset, right and bottom
const QPointF BlockGroup::OFFSET_OUT = QPointF(0, 0);   // outer offset
const QPointF BlockGroup::OFFSET_INSERT = QPointF(10, 0);   // offset while draging
const QPointF BlockGroup::NO_OFFSET = QPointF(0, 0);    // default offset

BlockGroup::BlockGroup(QString text, Analyzer* analyzer, DocumentScene *scene)
    : QGraphicsRectItem(0, scene)
{
    this->analyzer = analyzer;
    this->docScene = scene;

    // set flags
    root = 0;
    lastLine = -1;
    selected = 0;
    lastXPos = -1;
    foldableBlocks.clear();
    computeTextSize();
    //setFlag(QGraphicsItem::ItemClipsToShape);
//    setFlag(QGraphicsItem::ItemIsMovable);

    analyzeAll(text);
}

BlockGroup::~BlockGroup()
{
    docBlocks.clear();
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
    if (block->getLine() < 0) { // for docblocks
        foldableBlocks << block;
        return true;
    }

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
//    docBlocks << block;
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
    if (!block->getElement()->isSelectable()) {
        if (block->parentBlock() != 0)
            selectBlock(block->parentBlock());
        return;
    }
    // NOTE: only block that won't be selected later are deselected
    // otherwise the animations are twitching
    deselect((Block*)block->commonAncestorItem(selected));
    selected = block;
    selected->setShowing(true);
    selected->updatePen();
}

void BlockGroup::deselect(Block *until)
{
    if (selected != 0) {
        selected->setShowing(false, until);
        Block * oldSelected = selected;
        selected = 0;
        oldSelected->updatePen();
        oldSelected->updateGeometryAfter();
    }
}

Block *BlockGroup::addTextCursorAt(QPointF scenePos)
{
    return root->addTextCursorAt(root->mapFromScene(scenePos));
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
        newBlock->textItem()->setTextCursorPos(0);
        newBlock->edited = true;
    } else {
        next->getElement()->setSpaces(0);
        next->getFirstLeaf()->textItem()->setTextCursorPos(0);
    }
    root->updateBlock();//updateAfter(true);
}

void BlockGroup::eraseChar(Block *block, int key)
{
    Block *target = 0;
    if (key == Qt::Key_Backspace) {             // move to previous block
        target = block->getAncestorWhereFirst();
        if (target->getElement()->getSpaces() > 0) {
            target->getElement()->addSpaces(-1);
            target->updateGeometryAfter(false);//updateAfter(true);
            target->edited = true;
        } else {
            target = block->getPrev(true);
            if (target->getLine() < block->getLine()) {          // jumped to previous line
                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0) {
                    target = target->parentBlock();
                }
                target->getElement()->setLineBreaking(false);
                //                target->updateAfter();
                root->updateBlock();
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
            target->updateGeometryAfter(false);//updateAfter(true);
            target->edited = true;
        } else {
            target = block->getNext(true);
            if (target->getLine() > block->getLine()) {          // jumped to next line
                target = block;
                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0) {
                    target = target->parentBlock();
                }
                target->getElement()->setLineBreaking(false);
                //                target->updateAfter();
                root->updateBlock();
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
    case Qt::Key_Home :
        getBlockIn(start->getLine())->getFirstLeaf()->textItem()->setTextCursorPos(0);
        break;
    case Qt::Key_End :
        Block *next = start->getNext(true);
        while (next->getLine() == start->getLine()) {
            start = next;
            next = start->getNext(true);
        }
        start->textItem()->setTextCursorPos(-1);
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
    target->textItem()->setTextCursorPos(position);
    lastXPos = -1;
    selectBlock(target);
    target->updateGeometryAfter();
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

    QPointF scenePos(0,0);
    Block *firstInY = getBlockIn(y)->getFirstLeaf();

    scenePos.setY(firstInY->textItem()->scenePos().y() + CHAR_HEIGHT/2);

    if (lastXPos < 0) {
        scenePos.setX(start->textItem()->scenePos().x()
                      + start->textItem()->MARGIN
                      + from * CHAR_WIDTH);
        lastXPos = scenePos.x();
    } else {
        scenePos.setX(lastXPos);
    }

    if (selected != 0 && selected->isEdited()) {
        reanalyze(selected, scenePos);
        return;
    }

    Block *target = blockAt(scenePos);
    if (target == 0) target = root;
    target = target->addTextCursorAt(target->mapFromScene(scenePos));
    selectBlock(target);
    target->updateGeometryAfter();
}

QRectF BlockGroup::boundingRect() const
{
    QRectF rect = childrenBoundingRect();
    rect.setTopLeft(QPointF());
    return rect;
//    return QGraphicsRectItem::boundingRect();
}

void BlockGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    Q_UNUSED(option); Q_UNUSED(widget);
//    setRect(boundingRect());
//    painter->fillRect(boundingRect(), Qt::gray);
}

void BlockGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->ignore();
//    QGraphicsRectItem::mousePressEvent(event);
}

Block *BlockGroup::reanalyze(Block *block, QPointF cursorPos)
{
    if (root == 0) {
        return 0;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (block == 0 || !reanalyzeBlock(block)) {
        analyzeAll(root->getElement()->getText());
    }
    QApplication::restoreOverrideCursor();

    Block *target = addTextCursorAt(cursorPos);
    selectBlock(target);
    selected->updateGeometryAfter();
    // return new block at requested position
    return selected;
}

bool BlockGroup::reanalyzeBlock(Block *block)
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

    bool isPrevLB = false;
    if (analysedBl->prevSib != 0)
        isPrevLB = analysedBl->prevSib->getElement()->isLineBreaking();
    bool lineBreaking = analysedBl->getElement()->isLineBreaking();
    Block *parentBl = analysedBl->parentBlock();
    Block *nextSib = analysedBl->getNextSibling();

    analysedBl->setParentBlock(0);
    analysedBl->setVisible(false);
    analysedBl->deleteLater();

    Block *newBlock = new Block(newEl, parentBl);
    newBlock->getElement()->setLineBreaking(lineBreaking);
    if (nextSib != 0)
        newBlock->stackBeforeBlock(nextSib);

    if (newBlock->prevSib != 0)
        newBlock->prevSib->getElement()->setLineBreaking(isPrevLB);

    foldableBlocks.clear();
    root->updateBlock(false);

    foreach (DocBlock *block, docBlocks)
        block->setDefaultPos();
//    docBlocks.clear();

//    docScene->update();
    return true;
}

bool BlockGroup::analyzeAll(QString text)
{
    if (text.isEmpty())
        return false;
    if (root != 0)
        delete root;
    foldableBlocks.clear();
    selected = false;

    root = new Block(analyzer->analyzeFull(text), 0, this);
    root->setPos(30, 10);
    selectBlock(root);
    root->getFirstLeaf()->textItem()->setTextCursorPos(0);
    root->updateBlock(false);

    foreach (DocBlock *block, docBlocks)
        block->setDefaultPos();
//    docBlocks.clear();
//    docScene->update();
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

void BlockGroup::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_M :
//            reanalyze();
            break;
        case Qt::Key_Delete :
            if (selected != 0) {
                Block *next = selected->removeBlock(true);
                selectBlock(next);
                next->getFirstLeaf()->textItem()->setTextCursorPos(0);
                next->updateGeometryAfter();
            }
            break;
        }
    }
    QGraphicsRectItem::keyPressEvent(event);
}
