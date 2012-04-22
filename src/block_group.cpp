/** 
* @file block_group.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class BlockGroup and it's functions and identifiers.
*/

#include "block_group.h"
#include "text_group.h"
#include "block.h"
#include "doc_block.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"
#include "main_window.h"
#include "language_manager.h"

#include <QMessageBox>

const QString BlockGroup::BLOCK_MIME = "block_data";
//const QPointF BlockGroup::OFFSET_IN_TL = QPointF(0, 0);  // inner offset, left and top
//const QPointF BlockGroup::OFFSET_IN_BR = QPointF(0, 0);  // inner offset, right and bottom
//const QPointF BlockGroup::OFFSET_OUT = QPointF(0, 0);    // outer offset
const QPointF BlockGroup::OFFSET_INSERT = QPointF(8, 0); // offset while draging
//const QPointF BlockGroup::NO_OFFSET = QPointF(0, 0);     // default offset
const QString GRAMMAR_DIR = "/../share/trolledit/grammars";


BlockGroup::BlockGroup(QString text, QString file, DocumentScene *scene)
    : QGraphicsRectItem(0, scene)
{
    //this->analyzer = analyzer;
//    qDebug() << scene->main->getScriptBox()->currentText();
//    qDebug() << "filename split" << file.split(".")[1];
    qDebug() << "grammar = " << scene->main->getLangManager()->languages.value("C");
 //   qDebug() << "file" << file.split(".")[1];
    Analyzer *a;
    if(text.isEmpty()){
        a=new Analyzer(scene->main->getLangManager()->getLanguage(file.toLower()));
        a->readSnippet(scene->main->getLangManager()->snippetFile);
    }else{
        a=new Analyzer(scene->main->getLangManager()->getLanguage(file.split(".")[1]));
    }
    this->analyzer = a;
    this->docScene = scene;

    txt = new TextGroup(this, docScene);
    docScene->addItem(txt);
    txt->setVisible(false);

    highlight = true;

    // create insert cues
    horizontalLine = new QGraphicsLineItem(this);
    horizontalLine->setPen(QPen(Qt::darkRed, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    horizontalLine->setZValue(10);
    horizontalLine->setVisible(false);
    verticalLine = new QGraphicsLineItem(this);
    verticalLine->setPen(QPen(Qt::darkRed, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    verticalLine->setZValue(10);
    verticalLine->setVisible(false);

    // set flags
    root = 0;
    lineStarts.clear();
    lastLine = -1;
    selected = 0;
    lastXPos = -1;
    modified = true;
    searched = false;
    smoothTextAnimation = false;
    foldableBlocks.clear();

    computeTextSize();
    setAcceptDrops(true);
    setFlag(QGraphicsItem::ItemIsMovable);
    setPen(QPen(QBrush(Qt::black),1, Qt::DashLine)); //also color for filename
    
    
    runParalelized = false;
    groupRootEl = 0;

    time.start();

    analyzeAll(text);
    docScene->update();
}

BlockGroup::~BlockGroup()
{
    delete txt->rc;
    docScene = 0;
    root = 0;
    txt = 0;
}

void BlockGroup::setContent(QString content)
{
    analyzeAll(content);
    docScene->update();
}

/**
 * Set given root to be new block root.
 * Function sets new root and calls updateBlock to update each root in new hierarchy.
 * @see updateBlock()
 */
void BlockGroup::setRoot(Block *newRoot)
{
    if (newRoot == 0)
    {
        qWarning("Cannot set 0 as root");
        return;
    }

    if (root != 0 && root != newRoot)
    {
        root->deleteLater();
        root = 0;
    }
    // cleanup
    lineStarts.clear();
    lastLine = -1;
    selected = 0;
    lastXPos = -1;
    modified = true;
    foldableBlocks.clear();
    // set new root
    root = newRoot;
    root->setPos(20, 0);
    // select add cursor and update

    if (docScene->selectedGroup() == this)
    {
        selectBlock(root);
        root->getFirstLeaf()->textItem()->setTextCursorPos(0);
    }

    root->setVisible(true);
    clearSearchResults();
    root->updateBlock(false);

    QList<DocBlock*> docBlocksList = docBlocks();
    
    foreach (DocBlock *dbl, docBlocksList)
    {
        dbl->updateBlock(false);
    }

    updateSize();
}

void BlockGroup::setModified(bool flag)
{
    if (flag != modified)
    {
        modified = flag;
        docScene->groupWasModified(this);
    }
}

void BlockGroup::computeTextSize()
{
//    TAB_LENGTH = analyzer->TAB.length();
//    Block *temp = new Block(new TreeElement("temp"), 0, this);
//    QFontMetricsF *fm = new QFontMetricsF(temp->textItem()->font());
//    CHAR_WIDTH = fm->width(' ');
//    CHAR_HEIGHT = temp->textItem()->boundingRect().height();
//    delete temp;
    CHAR_WIDTH = 10;
    CHAR_HEIGHT = 26;
    TAB_LENGTH = 4;
//    qDebug()<<"CHAR_WIDTH: " << CHAR_WIDTH;     //10
//    qDebug()<<"CHAR_HEIGHT: " << CHAR_HEIGHT;   //26
//    qDebug()<<"TAB_LENGTH: " << TAB_LENGTH;     //4
}

Block *BlockGroup::getBlockIn(int line) const
{
    if (lastLine >= line)
        return lineStarts[line];
    else
        return lineStarts.last();
}
void BlockGroup::setBlockIn(Block *block, int line)
{
    if (lineStarts.size() > line)
    {
        lineStarts[line] = block;
    }
    else
    {
        while (lineStarts.size() < line)
        {
            lineStarts << 0;
            qWarning("Line %d skipped!", lineStarts.size() - 1);
        }

        lineStarts << block;
    }

    lastLine = line;
}

TextGroup* BlockGroup::getTextGroup()
{
    return this->txt;
}

bool BlockGroup::addFoldable(Block *block)
{
    if (block->getLine() < 0) //! for block out of hierarchy (always able to fold)
    {
        foldableBlocks << block;
        return true;
    }

    bool able = true;
    Block *toRemove = 0;
    foldableBlocks.remove(block);

    foreach (Block *bl, foldableBlocks)
    {
        if (bl->getLine() == block->getLine())
        {
            if (block->isAncestorOf(bl)) {
                toRemove = bl;                   //! if block is ancestor, throw out bl
            }
            else
            {
                able = false;
            }
            break;
        }
    }
    if (able)
        foldableBlocks << block;
    if (toRemove)
    {
        foldableBlocks.remove(toRemove);
        toRemove->updateFoldButton();
    }

    return able;
}

void BlockGroup::removeFoldable(Block *block)
{
    foldableBlocks.remove(block);
}

DocBlock *BlockGroup::addDocBlock(QPointF scenePos)
{
    DocBlock *block = new DocBlock(mapFromScene(scenePos), this);
    return block;
}

QList<DocBlock*> BlockGroup::docBlocks() const
{
    QList<DocBlock*> blocks;

    foreach (QGraphicsItem *item, childItems())
    {
        DocBlock *block = qgraphicsitem_cast<DocBlock*>(item);

        if (block != 0)
            blocks << block;
    }

    return blocks;
}

Block *BlockGroup::blockAt(QPointF scenePos) const
{
    qWarning("BlockGroup::blockAt() is probably not working");
    QGraphicsItem *item = docScene->itemAt(scenePos);

    if (item == 0)
        return 0;
    QGraphicsTextItem *textItem;    //! leaf item would be covered by its QGraphicsTextItem child

    if ((textItem = qgraphicsitem_cast<QGraphicsTextItem*>(item)) != 0)
        item = textItem->parentItem();

    return qgraphicsitem_cast<Block*>(item);
}

int BlockGroup::lineAt(QPointF scenePos) const
{
    scenePos = root->mapFromScene(scenePos);
    int line;

    if (scenePos.y() <= 0)
        return 0;

    if (scenePos.y() >= root->idealSize().height())
        return lastLine + 1;

    line = root->getLineAfter(scenePos);

    return qMin(line, lastLine + 1);
}

void BlockGroup::selectBlock(Block *block, bool updateNeeded)
{
    if (!block->getElement()->isSelectable())
    {
        if (block->parentBlock() != 0)
            selectBlock(block->parentBlock(), updateNeeded);

        return;
    }

    if (selected == block) return;
    // NOTE: only blocks that won't be selected later are deselected
    Block *commonAncestor = qgraphicsitem_cast<Block*>(block->commonAncestorItem(selected));
    deselect(commonAncestor, false);
    selected = block;
    selected->setShowing(true);

    if (!updateNeeded) return;

    if (commonAncestor != 0)
        commonAncestor->update();
    else
        update();
}

void BlockGroup::deselect(Block *until, bool updateNeeded)
{
    if (selected != 0)
    {
        selected->setShowing(false, until);
        selected = 0;
    }

    if (!updateNeeded) return;

    if (until != 0)
        until->update();
    else
        update();
}

Block *BlockGroup::addTextCursorAt(QPointF scenePos)
{
    Block *target = root;
    return target->addTextCursorAt(target->mapFromScene(scenePos));
}

/* **** slots called by signals form TextItem **** */
void BlockGroup::keyTyped(QKeyEvent* event)
{
    if (event->key() != Qt::Key_Up && event->key() != Qt::Key_Down)
        lastXPos = -1;
}

void BlockGroup::splitLine(Block *block, int cursorPos)
{
    if (block->parentBlock() == 0) return;

    Block *temp;

    // check what block should be splitted
    if (cursorPos == 0)
    {
        temp = block->getPrev();             //! split previous block
        if (temp->parentBlock() != 0) //! it is not very first block
        {
            cursorPos = -1;
            block = temp;
        }
    }
    else if (cursorPos == -1 && block->getNextSibling() == 0)   //! -1 means end of text
    {
        temp = block->getAncestorWhereLast();   //! split ancestor

        if (temp->parentBlock() != 0) //! it is not very last block
        {
            block = temp;
        }
    }

    smoothTextAnimation = true;

    // split this block
    QString text = "";
    if (cursorPos >= 0) //! leave some text in original block
    {
        text = block->textItem()->toPlainText();
        block->textItem()->setPlainText(text.left(cursorPos));
        text.remove(0,cursorPos);
    }

    Block *next = block->getNext();

    if (next->parentBlock() == 0) next = 0;        //! block is not very last block

    bool alreadyBreaking = !block->getElement()->setLineBreaking(true);

    // create new block (either with text or with newline)
    if (!text.isEmpty() || alreadyBreaking || next == 0)
    {
        next = block->getNextSibling();
        Block *newBlock = new Block(new TreeElement(text, 0, 0, alreadyBreaking),
                                    block->parentBlock());
        newBlock->setParentBlock(newBlock->parent, next);
        block->getElement()->setLineBreaking(false);
        newBlock->updatePos(true);
        block->getElement()->setLineBreaking(true);
        newBlock->textItem()->setTextCursorPos(0);

        if (!text.isEmpty())
            newBlock->edited = true;
    }
    else
    {
        next->getElement()->setSpaces(0);
        next->getFirstLeaf()->textItem()->setTextCursorPos(0);
        next->edited = true;
    }

    clearSearchResults();
    root->updateBlock();//updateAfter(true);
    smoothTextAnimation = false;
}

void BlockGroup::eraseChar(Block *block, int key)
{
    clearSearchResults();
    Block *target = 0;

    if (key == Qt::Key_Backspace) //! move to previous block
    {
        target = block->getAncestorWhereFirst();

        if (target->getElement()->getSpaces() > 0)
        {
            target->getElement()->addSpaces(-1);
            target->updateGeometryAfter(false);
            target->edited = true;
        }
        else
        {
            target = block->getPrev(true);

            if (target->getLine() < block->getLine()) //! jumped to previous line
            {
                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0)
                {
                    target = target->parentBlock();
                }

                target->getElement()->setLineBreaking(false);
                root->updateBlock(); //! todo more effective updater
            }
            else if (target->getLine() > block->getLine()) //! jumped to the end of file
            {
                return;
            }
            else //! on same line
            {
                target->textItem()->removeCharAt(-1);
            }
        }
    } else if (key == Qt::Key_Delete) //! move to next block
    {
        target = block->getNext();

        if (target->getElement()->getSpaces() > 0)
        {
            target->getElement()->addSpaces(-1);
            target->updateGeometryAfter(false);
            target->edited = true;
        }
        else
        {
            target = block->getNext(true);

            if (target->getLine() > block->getLine()) //! jumped to next line
            {
                target = block;

                while (!target->getElement()->isLineBreaking() && target->parentBlock() != 0)
                {
                    target = target->parentBlock();
                }

                target->getElement()->setLineBreaking(false);
                root->updateBlock(); //! todo more effective updater
            }
            else if (target->getLine() < block->getLine()) //! jumped to the beginning of file
            {
                return;
            }
            else //! on same line
            {
                target->textItem()->removeCharAt(0);
            }
        }
    }
}

void BlockGroup::moveFrom(Block *start, int key, int cursorPos)
{
    Block *bl;
    time.restart();

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
        bl = getBlockIn(start->getLine())->getFirstLeaf();
        bl->textItem()->setTextCursorPos(0);
        selectBlock(bl, true);
        break;
    case Qt::Key_End :
        bl = start->getNext(true);
        while (bl->getLine() == start->getLine()) {
            start = bl;
            bl = start->getNext(true);
        }
        start->textItem()->setTextCursorPos(-1);
        selectBlock(start, true);
        break;
    }
}

void BlockGroup::moveCursorLeftRight(Block *start, bool moveLeft)
{
    Block *target = 0;
    int position;

    if (moveLeft) //! move to previous block
    {
        target = start->getPrev(true);
        position = -2;

        if (target->getLine() != start->getLine() ||
            start->getAncestorWhereFirst()->getElement()->getSpaces() > 0)
            position = -1;
    }
    else    //! move to next block
    {
        target = start->getNext(true);
        position = 1;

        if (target->getLine() != start->getLine() ||
            target->getAncestorWhereFirst()->getElement()->getSpaces() > 0)
            position = 0;
    }

    target->textItem()->setTextCursorPos(position);
    lastXPos = -1;
    selectBlock(target, true);
}

void BlockGroup::moveCursorUpDown(Block *start, bool moveUp, int from)
{
    int y;
    int line = start->getLine();

    if (moveUp) //! move up
    {
        if (line == 0)
            y = lastLine;
        else
            y = line - 1;
    }
    else //! move down
    {
        if (line == lastLine)
            y = 0;
        else
            y = line + 1;
    }

    QPointF scenePos(0,0);
    Block *firstInY = getBlockIn(y)->getFirstLeaf();

    scenePos.setY(firstInY->textItem()->scenePos().y() + CHAR_HEIGHT/2);

    if (lastXPos < 0)
    {
        scenePos.setX(start->textItem()->scenePos().x()
                      + start->textItem()->MARGIN
                      + from * CHAR_WIDTH);
        lastXPos = scenePos.x();
    }
    else
    {
        scenePos.setX(lastXPos);
    }

    if (start->isEdited())
    {
        reanalyze(start, scenePos);

        return;
    }

    docScene->time.restart();
    Block *target = addTextCursorAt(scenePos);
    selectBlock(target, true);
}

// changes the mode and disables/enables the editing actions in the menu
void BlockGroup::changeMode(QList<QAction *> actionList)
{
    if(isVisible())
    {
        txt->setPlainText(this->toText());
        txt->rc->setPos(this->pos());
        txt->rc->setScale(this->scale());
        txt->rc->setRect(txt->boundingRect().adjusted(-10,-10,+10,+10));
        txt->setFocus();
        txt->rc->setVisible(true);
        txt->setVisible(true);
        this->setVisible(false);
        docScene->selectGroup(this);
        docScene->update();

        for (int i=0; i<actionList.size(); i++)
            actionList.at(i)->setEnabled(true);
    }
    else
    {
        txt->rc->setVisible(false);
        txt->setVisible(false);
        this->setContent(txt->toPlainText());
        this->setPos(txt->rc->pos());
        this->updateSize();
        this->setVisible(true);
        this->updateSize();
        docScene->update();

        for (int i=0; i<actionList.size(); i++)
            actionList.at(i)->setEnabled(false);
    }
}

void BlockGroup::changeMode(){
    if(isVisible()){
        txt->setPlainText(this->toText());
        txt->setPos(this->pos().x(),this->pos().y());
        txt->setScale(this->scale());
        txt->setFocus();
        txt->setVisible(true);
        this->setVisible(false);
        docScene->selectGroup(this);
        docScene->update();
    }else{
        txt->setVisible(false);
        this->setContent(txt->toPlainText());
        this->setPos(txt->pos().x(),txt->pos().y());
        this->updateSize();
        this->setVisible(true);
        this->updateSize();
        docScene->update();
    }
}

void BlockGroup::updateSize()
{
    // need to be called manually whenever size or position of blocks in this group changes
    QRectF rect = QRect();

    foreach (QGraphicsItem *item, childItems())
    {
        if (!item->isVisible()) continue;

        Block *block;

        if ((block = qgraphicsitem_cast<Block*>(item)) != 0)
        {
            if (block == root)
                rect = rect.united(QRectF(block->idealPos(), block->idealSize()));
        }
        else
        {
            rect = rect.united(QRectF(item->pos(), item->boundingRect().size()));
        }
    }

    rect.setTopLeft(QPointF());
    rect.adjust(-20, -20, 20, 20);
    setRect(rect);
    docScene->update();
}

void BlockGroup::showInsertLine(InsertLine type, QPointF scenePos)
{
    if (type == None)
    {
        horizontalLine->setVisible(false);
        verticalLine->setVisible(false);

        return;
    }
    qreal x, y;

    if (type == Horizontal) //! horizontal insert cue
    {
        verticalLine->setVisible(false);
        // determine line after scenePos
        int line = lineAt(scenePos + QPointF(0, CHAR_HEIGHT/2.0));

        if (line <= lastLine)
        {
            Block *block = getBlockIn(line);
            y = mapFromItem(block, 0, 0).y();
        }
        else
        {
            y = root->idealPos().y() + root->idealSize().height();
        }

        x = root->idealPos().x();
        horizontalLine->setLine(x, y, x + root->idealSize().width(), y);
        horizontalLine->setVisible(true);
    }
    else if (type == Vertical) //! vertical insert cue
    {
        horizontalLine->setVisible(false);
        QPair<Block*, bool> targetRight = root->findClosestLeaf(root->mapFromScene(scenePos));
        Block *target = targetRight.first;
        QRectF rect;
        rect = mapRectFromItem(target, target->idealRect());

        if (targetRight.second)
            x = rect.left();
        else
            x = rect.right();

        y = rect.bottom();
        verticalLine->setLine(x, y + 3, x, y - CHAR_HEIGHT - 3);
        verticalLine->setVisible(true);
    }
}

QRectF BlockGroup::boundingRect() const
{
    return rect();
}

QPainterPath BlockGroup::shape() const
{
    QPainterPath path;

    int width = pen().width();
    int mod = width % 2;
    int half = width / 2;

    path.addRect(boundingRect().adjusted(-half, -half, half+mod, half+mod));

    return path;
}


void BlockGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (docScene->selectedGroup() == this)
    {
        painter->setPen(pen());
        painter->drawRect(rect().adjusted(2,2,-2,-2));

        QFont font = painter->font() ;
        font.setPointSize ( 12 );
        font.setWeight(QFont::DemiBold);
        painter->setFont(font);
        QStringList list = this->getFilePath().split(QString(QDir::separator()));
        QPoint new_point = QPoint(widget->pos().x() - 10 ,widget->pos().y() - 5);
        painter->drawText(new_point, list.at(list.size()-1) );
        scene()->update();
    }
}

Block *BlockGroup::reanalyze(Block *block, QPointF cursorPos)
{    
    if (root == 0) return 0;

    selected = 0;

    getStatusBar()->showMessage("Analysing...");
    QApplication::setOverrideCursor(Qt::WaitCursor);

//    QApplication::setOverrideCursor(Qt::CrossCursor);
    if (!reanalyzeBlock(block))
    {
        analyzeAll(root->getElement()->getText());
    }

    QApplication::restoreOverrideCursor();
    getStatusBar()->clearMessage();
    qDebug("\nBlockGroup::reanalyze()");
    time.restart();

    Block *target = addTextCursorAt(cursorPos);

    if (target == 0) target = root;

    qDebug("add cursor to root: %d", time.restart());

    selectBlock(target);
    qDebug("block selection: %d", time.restart());
    // return new block at requested position
    docScene->update();

    return selected;
}

bool BlockGroup::reanalyzeBlock(Block *block)
{
    qDebug("\nBlockGroup::analyzeBlock()");
    time.restart();

    if (block == 0) return false;

    // get closest analyzable ancestor
    TreeElement *analysedEl = analyzer->getAnalysableAncestor(block->getElement());

    if (analysedEl == 0) return false;

    // create reanalyzed element
    TreeElement *newEl = analyzer->analyzeElement(analysedEl);
    qDebug("text analysis: %d", time.restart());

    // find block of original analyzed element
    Block *analysedBl;
    if(!TreeElement::DYNAMIC){
    do
    {
        analysedBl = analysedEl->getBlock();    //BUG!!! Dynamicka verzia da null
        analysedEl = (*analysedEl)[0];          //BUG!!! Ide mimo pola pri reanalyzovani
    }
    while (analysedBl == 0);
    }else{analysedBl = block;}
    // collect data from original block
    bool isPrevLB = false;

    if (analysedBl->prevSib != 0)
        isPrevLB = analysedBl->prevSib->getElement()->isLineBreaking();

    bool isAnalyzedLB = analysedBl->getElement()->isLineBreaking();
    Block *parentBl = analysedBl->parentBlock();
    Block *nextSib = analysedBl->getNextSibling();
    int spaces = analysedBl->getElement()->getSpaces();

    // destroy original block
    if(!TreeElement::DYNAMIC){
    analysedBl->setParentBlock(0); // NOTE: don't use removeBlock(), we don't want any aditional ancestors to be removed
    analysedBl->setVisible(false);
    analysedBl->deleteLater();}
    qDebug("old block deleted: %d", time.restart());

    // create new block
    Block *newBlock = 0;
    if(!TreeElement::DYNAMIC)
    newBlock = new Block(newEl, parentBl);
    else
    newBlock = new Block(newEl, 0, this);

    if (nextSib != 0)
        if(!TreeElement::DYNAMIC)newBlock->setParentBlock(newBlock->parent, nextSib);
    // set data
    newBlock->getElement()->setLineBreaking(isAnalyzedLB);

    if (newBlock->prevSib != 0)
        newBlock->prevSib->getElement()->setLineBreaking(isPrevLB);

    newBlock->element->addSpaces(spaces);
    qDebug("block creation: %d", time.restart());

    // reset root (root is the same)
    setRoot(root);
    qDebug("root update: %d", time.restart());

    return true;
}

void BlockGroup::analyzeAll(QString text)
{
    qDebug() << "text size = " << text.size();
    qDebug() << "runParalelized = " << runParalelized;
    qDebug() << "maxThreadCount = " << QThreadPool::globalInstance()->maxThreadCount();
    qDebug() << "currentThreadId(): " << QThread::currentThreadId(); 
    
    if (text.isEmpty()) //! use snippet if text is empty
    {
        text = analyzer->getSnippet();
        qDebug() << "Default snippet used";
        getStatusBar()->showMessage("File reset - default text used", 2000);

        if (text.isEmpty()) text = "    ";
    }
    time.restart();
    
    try 
    {
        if (runParalelized == true) {
            bool connected = QObject::connect(&watcher, SIGNAL(finished()), this, SLOT(updateAllInThreads()));
            future = QtConcurrent::run(this, &BlockGroup::analazyAllInThread, text);    
            watcher.setFuture(future); //! this is the line, on which function is run in thread
            qDebug() << "Connected:" << connected;
        }
        else {
            TreeElement* rootEl = analazyAllInMaster(text);
            updateAllInMaster(rootEl);
//            runParalelized = true;
        }        
    }
    catch (...) 
    {
        QMessageBox::information(0,"Error","Error in AnalyzeAll!");
    }
}

/** Function to run analyzis of text in worker thread on background. 
 * This function is run in thread, when finished, slot for updateAllInThread is invoked.
 * Called most of the time.
 * @see updateAllInThread()
 * @return returns Root Element of analyzed text - not used
 */
TreeElement* BlockGroup::analazyAllInThread (QString text) 
{
    qDebug("analazyAllInThread");
    mutex.lock();    
    TreeElement* rootEl = analyzer->analyzeFull(text);
    groupRootEl = rootEl;
    mutex.unlock();
   
    return rootEl;
}

/** Function to run analyzis of text in master thread. 
 * This function is run directly in master, while he is waiting. Blocking, application has to wait for result.
 * Run when creating / opening new file.
 * @return returns Root Element of analyzed text
 */
TreeElement* BlockGroup::analazyAllInMaster (QString text)  
{
    qDebug("analazyAllInMaster");    
    return analyzer->analyzeFull(text);
}

/** Function to update blocks based on analyzed text in worker thread.
 * This function is invoked after analysis in thread is done.
 * New Root element is set and all blocks are updated.
 */
void BlockGroup::updateAllInThreads () 
{
    if (groupRootEl != 0) {
        mutex.lock();
        Block *newRoot = new Block(groupRootEl, 0, this);
        mutex.unlock();
        setRoot(newRoot);
        
        qDebug("root update: %d", time.restart());
    }
    else {
        qDebug("groupRootEl is null");
    }
    qDebug("updateAllInThread");
}

/** Function to update blocks based on analyzed text in master thread.
 * This function is invoked after analysis is done. 
 * New Root element is set and all blocks are updated.
 */
void BlockGroup::updateAllInMaster (TreeElement* rootEl) 
{
    // create new root
    Block *newRoot = new Block(rootEl, 0, this);
    qDebug("root creation: %d", time.restart());
    
    // set new root
    setRoot(newRoot);
    qDebug("root update: %d", time.restart());
    
    qDebug("updateAllInMaster");
    return;
}


QString BlockGroup::toText(bool noDocs) const
{
    return root->getElement()->getText(noDocs);
}

QList<Block*> BlockGroup::blocklist_cast(QList<QGraphicsItem*> list)
{
    QList<Block*> blocks;

    foreach (QGraphicsItem *item, list)
    {
        Block *block = qgraphicsitem_cast<Block*>(item);

        if (block != 0) blocks << block;

    }

    return blocks;
}

QStatusBar *BlockGroup::getStatusBar()
{
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        QMainWindow *mainWin = qobject_cast<QMainWindow *>(widget);

        if (mainWin) return mainWin->statusBar();

    }

    Q_ASSERT(false);

    return 0;
}

void BlockGroup::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        switch (event->key()) {
        case Qt::Key_I :
            for (int i = 0; i <= lastLine; i++)
            {
                Block *block = getBlockIn(i);

                if (block == 0) continue;

                Block *prevBl = block->getPrevSibling();

                if (!block->element->isSelectable() || prevBl == 0)
                {
                    block->getElement()->setSpaces(0);
                }
                else
                {
                    if (prevBl->getElement()->isSelectable() || prevBl->element->getType().isEmpty())
                    {
                        block->getElement()->setSpaces(0);
                    }
                    else
                    {
                        block->getElement()->setSpaces(TAB_LENGTH);
                    }
                }
            }
            root->updateBlock();
            docScene->update();
            break;
        case Qt::Key_Delete :
            if (selected != 0)
            {
                selected->setVisible(false);
                Block *next = selected->removeBlock(true);

                if (next != 0)
                {
                    selectBlock(next);
                    next->getFirstLeaf()->textItem()->setTextCursorPos(0);
                }

                root->updateBlock();
            }
            break;
        }
    }
    QGraphicsRectItem::keyPressEvent(event);
}

void BlockGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        if ((event->modifiers() & Qt::AltModifier) == Qt::AltModifier)
        {
            changeMode(getTextGroup()->scene->getWindow()->getActionList());
            event->accept();
        }
    }

    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
    {
        event->ignore();
        return;
    }
    if (docScene->itemAt(event->scenePos()) == this) //! clicked directly on group
    {
        deselect();
        root->updateBlock();
        event->accept();
        QGraphicsRectItem::mousePressEvent(event); //! to ensure movement
    }
    else //! clicked on block/button inside group
    {
        event->ignore();
    }

    docScene->selectGroup(this);
}

void BlockGroup::dropEvent(QGraphicsSceneDragDropEvent *event) // todo refactor
{
    if (event->mimeData()->hasFormat(BLOCK_MIME))
    {
        showInsertLine(None, QPointF());

        Block *selected = docScene->selectedGroup()->selectedBlock();

        if (selected == 0)
        {
            event->accept();

            return;
        }

        qDebug("\nBlockGroup::dropEvent()");
        time.restart();

        QPointF scenePos = event->scenePos();
        bool shiftMod = (event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier;
        bool isRight = false;
        int lineNo = -1;
        Block *target = 0;

        // find drop target
        if (shiftMod)
        {
            QPair<Block*, bool> targetRight = root->findClosestLeaf(
                    root->mapFromParent(event->pos()));
            target = targetRight.first;
            isRight = targetRight.second;
        }
        else
        {
            int lineNo = lineAt(scenePos + QPointF(0, CHAR_HEIGHT/2.0));
            scenePos -= QPointF(0, CHAR_HEIGHT/2.0);
            target = getBlockIn(lineNo)->getFirstLeaf();
        }

        // test for possible recursion
        if (event->dropAction() != Qt::CopyAction &&
            (selected == target->parent || selected->isAncestorOf(target->parent)))
        {
            event->setDropAction(Qt::IgnoreAction);
            return;
        }

        setModified(true);

        // clone block
//        TreeElement *cloneEl = selected->element->clone();
        QString text = selected->element->getText();

        if (text.endsWith("\n")) text.chop(1);

        TreeElement *cloneEl = new TreeElement(text);
        // NOTE: deep clone is not needed here since it will be reanalyzed anyway

        qDebug("element cloned: %d", time.restart());
        Block *clone = new Block(cloneEl, 0, this);
        qDebug("cloned block created: %d", time.restart());

        // add clone to hierarchy
        if (shiftMod)
        {
            clone->element->setLineBreaking(false);
            Block* nextSibling = isRight ? target : target->nextSib;
            clone->setParentBlock(target->parent, nextSibling);
        }
        else
        {
            clone->element->setLineBreaking(true);
            Block* nextSibling = lineNo <= lastLine ? target : 0;
            clone->setParentBlock(target->parent, nextSibling);

            if (nextSibling == 0) clone->prevSib->getElement()->setLineBreaking(true);
        }

        // destroy original if needed (cannot remove before adding, layout would change)
        if (event->dropAction() != Qt::CopyAction)
        {
            selected->setVisible(false);
            BlockGroup *sourceGroup = selected->blockGroup();
            selected->removeBlock(true);

            if (sourceGroup != this)
            {
                sourceGroup->root->updateBlock();
            }
        }
        // reanalyze all
        reanalyze(clone, scenePos);

        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void BlockGroup::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(BLOCK_MIME))
    {
        event->acceptProposedAction();

        if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        {
            showInsertLine(Vertical, event->scenePos());
        }
        else
        {
            showInsertLine(Horizontal, event->scenePos());
        }
    }
    else
    {
        QGraphicsRectItem::dragEnterEvent(event);
    }
}

void BlockGroup::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(BLOCK_MIME))
    {
        event->acceptProposedAction();

        if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        {
            showInsertLine(Vertical, event->scenePos());
        }
        else
        {
            showInsertLine(Horizontal, event->scenePos());
        }
        // scroll while dragging
        ensureVisible(event->pos().x(), event->pos().y(), 1, 1, 30, 30);
    }
    else
    {
        QGraphicsRectItem::dragMoveEvent(event);
    }
}

void BlockGroup::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(BLOCK_MIME))
    {
        showInsertLine(None, QPointF());
    }
    else
    {
        QGraphicsRectItem::dragLeaveEvent(event);
    }
}

void BlockGroup::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
    {
        event->ignore();
        return;
    }

    if (docScene->itemAt(event->scenePos()) == this) //! clicked directly on group
    {
        setPos(30, 30);
        event->accept();
    }
    else //! clicked on block/button inside group
    {
        event->ignore();
    }
    QGraphicsRectItem::mouseDoubleClickEvent(event);

}

void BlockGroup::highlightON_OFF(){
     if(highlight){
        highlight = false;
     }else{
        highlight = true;
     }
}

void BlockGroup::highlightLines(QSet<int> lines)
{
    if(highlight){
    if (lines.isEmpty()) return;

//    foreach (QGraphicsRectItem *hRect, highlightingRects.values()) {
//        int key = highlightingRects.key(hRect);
//        if (!lines.contains(key)) {
//            highlightingRects.remove(key);
//            delete hRect;
//        }
//    }

    qreal offset = 4;
    QColor color;
    color.setNamedColor("yellow");
    color.setAlpha(100);

    foreach (int line, lines)
    {
        if (highlightingRects.value(line, 0) != 0) continue;

        Block *bl = getBlockIn(line);

        if (bl != 0)
        {
            QPointF pos = mapFromItem(bl, 0, 0);
            QGraphicsRectItem *hRect = new QGraphicsRectItem(
                    pos.x(), pos.y() + offset,
                    root->idealSize().width() - pos.x(), CHAR_HEIGHT - 2*offset,
                    this);
            hRect->setPen(QPen(color));
            hRect->setBrush(QBrush(color));
            highlightingRects.insert(line, hRect);
            searched = true;
        }
    }
    }else{

    }
}

bool BlockGroup::searchBlocks(QString searchStr, bool allowInner, bool exactMatch)
{
    if (searchStr.isEmpty()) return false;

    bool found = false;
    TreeElement *el = root->getElement();
    Block *bl;

    if (allowInner) searchStr.replace(" ", "_");

    while (el->hasNext())
    {
        if (allowInner || el->isLeaf())
        {
            if ((!exactMatch && el->getType().contains(searchStr)) ||
                el->getType() == searchStr)
            {
                TreeElement *foundEl = el;
                do
                {
                    bl = foundEl->getBlock();

                    if (foundEl->isLeaf()) break;

                    foundEl = (*foundEl)[0];
                }
                while (bl == 0);

                if (bl != 0)
                {
                    bl->setYellow(true);
                    found = true;
                }
            }
        }
        el = el->next();
    }
    if (found) searched = true;

    return found;
}

void BlockGroup::clearSearchResults()
{
    if (!searched) return;

    searched = false;

    TreeElement *el = root->getElement();
    Block *bl;

    while (el->hasNext())
    {
        bl = el->getBlock();

        if (bl != 0) bl->setYellow(false);

        el = el->next();
    }

    foreach (QGraphicsRectItem *hRect, highlightingRects.values())
    {
        delete hRect;
    }

    highlightingRects.clear();
    qDebug("Search cleared");
}
