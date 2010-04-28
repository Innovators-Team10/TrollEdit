#include "doc_block.h"
#include "arrow.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"
#include <QTextBrowser>
#include "block_group.h"

DocBlock::DocBlock(QPointF pos, BlockGroup *parentgroup)
    : Block(new TreeElement("", true, true), 0, parentgroup)
{
    setParentItem(0);
    element->setType("doc_comment");
    element->setFloating(true);
    myTextItem->adaptToFloating();  // disconnect all harmful signals
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptDrops(false);

    setPos(pos);
    updateBlock(false);

    arrow = 0;
    if (group->selectedBlock() != 0) {
//        relatedBlock->element->appendChild(commentBl->element);
        addArrow(this, group->selectedBlock(), group->docScene);
    }
}

DocBlock::~DocBlock()
{
}

void DocBlock::updateBlock(bool doAnimation)
{
    // update line
    line = -1;
    // update pos
    updatePos(!doAnimation);
    // update size
    updateSize(!doAnimation);
    // update fold button
    foldButton = 0;
    // animate
    if (doAnimation)
        animate();
}


void DocBlock::addImage(const QImage &image){
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    QTextCursor cursor = QTextCursor(myTextItem->document());
    cursor.insertImage(image);
}

void DocBlock::addFile(QUrl url){
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    myTextItem->setPlainText(url.toString());
}

void DocBlock::addText(QString text){
    myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    myTextItem->setPlainText(text);
    group->selectBlock(this);
    myTextItem->setTextCursorPosition(0);
}

void DocBlock::addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene){
    arrow = new Arrow(start, end, end, parentScene);
    arrow->setColor(Qt::blue);
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
