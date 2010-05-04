#include "doc_block.h"
#include "arrow.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"
#include <QTextBrowser>
#include "block_group.h"

DocBlock::DocBlock(QPointF pos, BlockGroup *parentgroup)
    : Block(new TreeElement("doc_comment", true, true), 0, parentgroup)
{
    group->tempDocBlocks << this;

    setParentItem(0);
    type = "doc_comment";
    element->setFloating(true);

    myTextItem->adaptToFloating();  // disconnect all harmful signals
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptDrops(false);
    setPos(pos);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);

    Block *selected = group->selectedBlock();
    if (selected != 0) {
        addArrowTo(selected);
        selected->getElement()->appendChild(this->element);
    } else {
        arrow = 0;
    }
    updateBlock(false);
}

DocBlock::DocBlock(TreeElement* element, Block* parentBlock, BlockGroup *parentgroup)
    : Block(element, parentBlock, parentgroup)
{
    group->tempDocBlocks << this;

    arrow = 0;
    if (prevSib != 0)
        addArrowTo(prevSib);
    else if (parent != 0)
        addArrowTo(parent);

    type = element->getType();
    removeLinks();
    setParentItem(0);
    element->setFloating(true);
    myTextItem->adaptToFloating();  // disconnect all harmful signals
    setFlag(QGraphicsItem::ItemIsMovable);
    setAcceptDrops(false);

    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);

    setPos(-1,-1);
}

DocBlock::~DocBlock()
{
//    if (arrow != 0)
//        arrow->deleteLater();
}

void DocBlock::updateBlock(bool doAnimation)
{
    // update line
    line = -1;
    // update pos
    if (pos().x() < 0) {
        qreal x = group->pos().x() + group->boundingRect().width() + 50;
        if (arrow == 0) {
            setPos(x ,0);
        } else {
            Block *endItem = arrow->endItem();
            setPos(x, endItem->mapToScene(endItem->idealPos()).y());
        }
    }
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
    if (arrow != 0)
        arrow->setColor(Qt::blue);
}

void DocBlock::addFile(QUrl url){
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    myTextItem->setPlainText(url.toString());
    if (arrow != 0)
        arrow->setColor(Qt::blue);
}

void DocBlock::addText(QString text){
    myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    myTextItem->setPlainText(text);
    if (arrow != 0)
        arrow->setColor(Qt::darkGreen);
}

void DocBlock::addArrowTo(Block *end){
    arrow = new Arrow(this, end, group->docScene);
    connect(end, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
