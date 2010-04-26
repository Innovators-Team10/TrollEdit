#include "doc_block.h"
#include "arrow.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"
#include <QTextBrowser>
#include "block_group.h"

DocBlock::DocBlock(QString text, QPointF pos, Block *relatedBlock, BlockGroup *parentgroup)
    : Block(new TreeElement("", false, true), 0, parentgroup)
{
    Block *commentBl = new Block(new TreeElement("doc_comment", true, true),
                                 0, parentgroup);
    element->setType(text);
    setParentBlock(commentBl);

    arrow = 0;
    if (relatedBlock != 0) {
//        relatedBlock->element->appendChild(commentBl->element);
        addArrow(this, relatedBlock, parentgroup->docScene);
    }

    commentBl->getElement()->setFloating(true);
    commentBl->setPos(pos);

//    if (docScene->getHighlightning().contains("multi_comment")) {
//        myTextItem->setFont(docScene->getHighlightning().value("multi_comment").first);
//    }

}

DocBlock::~DocBlock()
{
    if (arrow != 0) {
        arrow->setVisible(false);
        group->update();
        arrow->deleteLater();
    }
}

void DocBlock::addImage(const QImage &image){
    QTextCursor cursor = myTextItem->textCursor();
    this->image = image;
    if (!image.isNull())
        cursor.insertImage(image);
    myTextItem->setTextCursor(cursor);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
}

void DocBlock::addFile(QUrl url){
    //todo: pridanie linkov na subory ktore sa dragnu na scenu do dokumentacnych blokov
    //file = new QTextBrowser();
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
}

void DocBlock::addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene){
    arrow = new Arrow(start, end, end, parentScene);
    if(this->getElement()->getType() == "image")
        arrow->setColor(Qt::darkYellow);
    else
        arrow->setColor(Qt::blue);
}



void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
